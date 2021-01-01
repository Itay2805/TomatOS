#include <util/defs.h>
#include <util/except.h>
#include <util/string.h>
#include "debug.h"

typedef struct source_location {
    const char* filename;
    uint32_t line;
    uint32_t column;
} source_location_t;

#define SOURCE_LOCATION(s) " at ", (s).filename, ":", (s).line, ":", (s).column

typedef enum type_kind {
    TK_INTEGER  = 0x0000,
    TK_FLOAT    = 0x0001,
    TK_UNKNOWN  = 0xFFFF,
} type_kind_t;

typedef struct type_descriptor {
    uint16_t type_kind;
    uint16_t type_info;
    char type_name[];
} type_descriptor_t;

static bool is_integer(type_descriptor_t* ty) {
    return ty->type_kind == TK_INTEGER;
}

static bool is_signed_integer(type_descriptor_t* ty) {
    return is_integer(ty) && (ty->type_info & 1);
}

static bool is_unsigned_integer(type_descriptor_t* ty) {
    return is_integer(ty) && !(ty->type_info & 1);
}

static unsigned get_integer_bit_width(type_descriptor_t* ty) {
    ASSERT(is_integer(ty));
    return 1 << (ty->type_info >> 1);
}

typedef uintptr_t value_handle_t;
typedef int64_t sint_max_t;
typedef uint64_t uint_max_t;

static bool is_inline_int(type_descriptor_t* ty, value_handle_t value) {
    ASSERT(is_integer(ty));
    unsigned inline_bits = sizeof(value) * 8;
    unsigned bits = get_integer_bit_width(ty);
    return bits <= inline_bits;
}

static sint_max_t get_sint_value(type_descriptor_t* ty, value_handle_t value) {
    ASSERT(is_signed_integer(ty))
    if (is_inline_int(ty, value)) {
        unsigned extra_bits = sizeof(sint_max_t) * 8 - get_integer_bit_width(ty);
        return (sint_max_t)value << extra_bits >> extra_bits;
    } else if (get_integer_bit_width(ty) == 64) {
        return (sint_max_t)(int64_t)value;
    } else {
        ASSERT(false, "unexpected bit width ", get_integer_bit_width(ty));
    }
}

static sint_max_t get_uint_value(type_descriptor_t* ty, value_handle_t value) {
    ASSERT(is_unsigned_integer(ty))
    if (is_inline_int(ty, value)) {
        return value;
    } else if (get_integer_bit_width(ty) == 64) {
        return (sint_max_t)(int64_t)value;
    } else {
        ASSERT(false, "unexpected bit width ", get_integer_bit_width(ty));
    }
}

static uint_max_t get_positive_int_value(type_descriptor_t* ty, value_handle_t value) {
    if (is_unsigned_integer(ty)) {
        return get_uint_value(ty, value);
    }

    sint_max_t val = get_sint_value(ty, value);
    ASSERT(val >= 0);
    return val;
}

static bool is_negative(type_descriptor_t* ty, value_handle_t value) {
    return is_signed_integer(ty) && get_sint_value(ty, value) < 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct type_mismatch_data {
    source_location_t loc;
    type_descriptor_t* type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
} type_mismatch_data_t;

static const char* g_type_check_kinds[] = {
    "load of", "store to", "reference binding to", "member access within",
    "member call on", "constructor call on", "downcast of", "downcast of",
    "upcast of", "cast to virtual base of", "_Nonnull binding to"
};

void __ubsan_handle_type_mismatch_v1(type_mismatch_data_t* data, value_handle_t pointer) {

    size_t alignment = 1 << data->log_alignment;

    if (!pointer) {
        WARN(
            g_type_check_kinds[data->type_check_kind],
            " null pointer of type ", data->type->type_name,
            SOURCE_LOCATION(data->loc)
        );
    } else if (pointer & (alignment - 1)) {
        WARN(
            g_type_check_kinds[data->type_check_kind],
            " misaligned address ", (void*)pointer,
            " for type ", data->type->type_name,
            " which requires ", alignment, " byte alignment"
            SOURCE_LOCATION(data->loc)
        );
    } else {
        WARN(
            g_type_check_kinds[data->type_check_kind],
            " address ", (void*)pointer, " with insufficient space ",
            " for an object of type ", data->type->type_name,
            SOURCE_LOCATION(data->loc)
        );
    }
    debug_trace_own_stack();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct alignment_assumption_data {
    source_location_t loc;
    source_location_t assumption_loc;
    type_descriptor_t* type;
} alignment_assumption_data_t;

void __ubsan_handle_alignment_assumption(alignment_assumption_data_t* data, value_handle_t pointer, value_handle_t alignment, size_t offset) {
    size_t real_pointer = pointer - offset;
    size_t lsb = __builtin_ffsl(real_pointer);
    size_t actual_alignment = 1 << lsb;
    size_t mask = alignment - 1;
    size_t mis_alignment_offset = real_pointer & mask;

    if (offset == 0) {
        WARN(
            "assumption of ", alignment,
            " byte alignment for pointer of type ", data->type->type_name, " failed"
            SOURCE_LOCATION(data->loc)
        );
    } else {
        WARN(
            "assumption of ", alignment,
            " byte alignment (with offset of ", offset,
            " byte) for pointer of type ", data->type->type_name, " failed"
            SOURCE_LOCATION(data->loc)
        );
    }

    WARN(
        "alignment assumption was specified here ",
         data->assumption_loc.filename, ":", data->assumption_loc.line, ":", data->assumption_loc.column
     );

    WARN(
        (offset == 0) ? "offset " : "", "address is ", actual_alignment,
        " aligned, misalignment offset is ", mis_alignment_offset, " bytes"
    );
    debug_trace_own_stack();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct overflow_data {
    source_location_t loc;
    type_descriptor_t* type;
} overflow_data_t;

#define HANDLE_OVERFLOW(func, sign) \
    void func(overflow_data_t* data, size_t lhs, size_t rhs) { \
        WARN( \
            is_signed_integer(data->type) ? "signed" : "unsigned", \
            " integer overflow: ", lhs, sign, rhs, \
            " cannot be represented in type ", \
            data->type->type_name,  \
            SOURCE_LOCATION(data->loc) \
        ); \
        debug_trace_own_stack(); \
    }

HANDLE_OVERFLOW(__ubsan_handle_add_overflow, "+")
HANDLE_OVERFLOW(__ubsan_handle_sub_overflow, "-")
HANDLE_OVERFLOW(__ubsan_handle_mul_overflow, "*")

// TODO: negate_overflow
// TODO: divrem_overflow

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct shift_out_of_bounds_data {
    source_location_t loc;
    type_descriptor_t* lhs_type;
    type_descriptor_t* rhs_type;
} shift_out_of_bounds_data_t;

void __ubsan_handle_shift_out_of_bounds(shift_out_of_bounds_data_t* data, value_handle_t lhs, value_handle_t rhs) {
    if (is_negative(data->rhs_type, rhs) || get_positive_int_value(data->rhs_type, rhs) >= get_integer_bit_width(data->lhs_type)) {
        if (is_negative(data->rhs_type, rhs)) {
            WARN("shift exponent ", rhs, " is negative", SOURCE_LOCATION(data->loc));
        } else {
            WARN("shift exponent ", rhs, " is too large for ",
                 get_integer_bit_width(data->lhs_type), "-bit type ", data->lhs_type->type_name,
                 SOURCE_LOCATION(data->loc));
        }
    } else {
        if (is_negative(data->lhs_type, lhs)) {
            WARN("left shift of negative value ", lhs, SOURCE_LOCATION(data->loc));
        } else {
            WARN("left shift of ", lhs, " by ", rhs, " places cannot be represented in type ", data->lhs_type->type_name,
                 SOURCE_LOCATION(data->loc));
        }
    }
    debug_trace_own_stack();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct out_of_bounds_data {
    source_location_t loc;
    type_descriptor_t* array_type;
    type_descriptor_t* index_type;
} out_of_bounds_data_t;

void __ubsan_handle_out_of_bounds(out_of_bounds_data_t* data, value_handle_t index) {
    WARN("index ", index, " out of bounds for type ", data->array_type->type_name, SOURCE_LOCATION(data->loc));
    debug_trace_own_stack();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: builtin_unreachable
// TODO: missing_return

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: vla_bound_not_positive

////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct invalid_value_data {
    source_location_t loc;
    type_descriptor_t* type;
} invalid_value_data_t;

void __ubsan_handle_load_invalid_value(invalid_value_data_t* data, value_handle_t val) {
    WARN("load of value ", val, ", which is not a valid value for type ", data->type->type_name, SOURCE_LOCATION(data->loc));
    debug_trace_own_stack();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct invalid_builtin_data {
    source_location_t loc;
    unsigned char kind;
} invalid_builtin_data_t;

void __ubsan_handle_invalid_builtin(invalid_builtin_data_t* data) {
    WARN("passing zero to ", data->kind ? "ctz()" : "clz()", ", which is not a valid argument", SOURCE_LOCATION(data->loc));
    debug_trace_own_stack();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: function_type_mismatch

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: nonnull_return_v1
// TODO: nullability_return_v1

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: nonnull_arg
// TODO: nullability_arg

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct pointer_overflow_data {
    source_location_t loc;
} pointer_overflow_data_t;

void __ubsan_handle_pointer_overflow(pointer_overflow_data_t* data, value_handle_t base, value_handle_t result) {
    if (base >= 0 && result >= 0) {
        if (base > result) {
            WARN(
                "addition of unsigned offset to ", (void*)base,
                " overflowed to ", (void*)result,
                SOURCE_LOCATION(data->loc)
            );
        } else {
            WARN(
                "subtraction of unsigned offset from ", (void*)base,
                " overflowed to ", (void*)result,
                SOURCE_LOCATION(data->loc)
            );
        }
    } else {
        WARN(
            "pointer index expression with base ", (void*)base,
            " overflowed to ", (void*)result,
            SOURCE_LOCATION(data->loc)
        );
    }
    debug_trace_own_stack();
}
