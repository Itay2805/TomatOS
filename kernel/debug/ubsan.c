#include <util/defs.h>
#include <util/except.h>
#include <util/string.h>
#include "debug.h"

typedef struct source_location {
    const char* filename;
    uint32_t line;
    uint32_t column;
} source_location_t;

#define SOURCE(s) (s).filename, (s).line, (s).column
#define SOURCE_FMT " at %s:%d:%d"

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

static bool is_minus_one(type_descriptor_t* ty, value_handle_t value) {
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
        UNLOCKED_WARN(
            "%s null pointer of type %s" SOURCE_FMT,
            g_type_check_kinds[data->type_check_kind], data->type->type_name, SOURCE(data->loc)
        );
    } else if (pointer & (alignment - 1)) {
        UNLOCKED_WARN(
            "%s misaligned address 0x%p for type %s which requires %d byte alignment" SOURCE_FMT,
            g_type_check_kinds[data->type_check_kind], pointer, data->type->type_name, alignment, SOURCE(data->loc)
        );
    } else {
        UNLOCKED_WARN(
            "%s address %p with insufficient space for an object of type %s" SOURCE_FMT,
            g_type_check_kinds[data->type_check_kind], pointer, data->type->type_name, SOURCE(data->loc)
        );
    }
    debug_trace_me();
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
        UNLOCKED_WARN(
            "assumption of %d byte alignment for pointer of type %s failed" SOURCE_FMT,
            alignment, data->type->type_name, SOURCE(data->loc)
        );
    } else {
        UNLOCKED_WARN(
            "assumption of %d byte alignment (with offset of %d byte) for pointer of type %s failed" SOURCE_FMT,
            alignment, offset, data->type->type_name, SOURCE(data->loc)
        );
    }

    UNLOCKED_WARN(
        "alignment assumption was specified here %s:%d:%d",
         data->assumption_loc.filename, data->assumption_loc.line, data->assumption_loc.column
     );

    UNLOCKED_WARN(
        "%saddress is %d aligned, misalignment offset is %d bytes",
        (offset == 0) ? "offset " : "", actual_alignment, mis_alignment_offset
    );
    debug_trace_me();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct overflow_data {
    source_location_t loc;
    type_descriptor_t* type;
} overflow_data_t;

#define HANDLE_OVERFLOW(func, sign) \
    void func(overflow_data_t* data, size_t lhs, size_t rhs) { \
        UNLOCKED_WARN( \
            "%s integer overflow: %ld %c %ld cannot be represented in type %s" SOURCE_FMT, \
            is_signed_integer(data->type) ? "signed" : "unsigned", \
            lhs, sign, rhs, data->type->type_name,  \
            SOURCE(data->loc) \
        ); \
        debug_trace_me(); \
    }

HANDLE_OVERFLOW(__ubsan_handle_add_overflow, '+')
HANDLE_OVERFLOW(__ubsan_handle_sub_overflow, '-')
HANDLE_OVERFLOW(__ubsan_handle_mul_overflow, '*')

void __ubsan_handle_negate_overflow(overflow_data_t* data, value_handle_t old_val) {
    if (is_signed_integer(data->type)) {
        UNLOCKED_WARN(
            "negation of %d cannot be represented in type %s; cast to an unsigned type to negate this value to itself" SOURCE_FMT,
            old_val, data->type->type_name, SOURCE(data->loc)
        );
    } else {
        UNLOCKED_WARN(
            "negation of %d cannot be represented in type %s" SOURCE_FMT,
            old_val, data->type->type_name, SOURCE(data->loc)
        );
    }
}

void __ubsan_handle_divrem_overflow(overflow_data_t* data, value_handle_t lhs, value_handle_t rhs) {
    if (is_minus_one(data->type, rhs)) {
        UNLOCKED_WARN(
            "division of %d by -1 cannot be represented in type %s" SOURCE_FMT,
            lhs, data->type->type_name
        );
    } else {
        UNLOCKED_WARN("division by zero" SOURCE_FMT, SOURCE(data->loc));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct shift_out_of_bounds_data {
    source_location_t loc;
    type_descriptor_t* lhs_type;
    type_descriptor_t* rhs_type;
} shift_out_of_bounds_data_t;

void __ubsan_handle_shift_out_of_bounds(shift_out_of_bounds_data_t* data, value_handle_t lhs, value_handle_t rhs) {
    if (is_negative(data->rhs_type, rhs) || get_positive_int_value(data->rhs_type, rhs) >= get_integer_bit_width(data->lhs_type)) {
        if (is_negative(data->rhs_type, rhs)) {
            UNLOCKED_WARN(
                "shift exponent %d is negative" SOURCE_FMT,
                rhs, SOURCE(data->loc)
            );
        } else {
            UNLOCKED_WARN(
                "shift exponent %d is too large for %d-bit type %s" SOURCE_FMT,
                 rhs, get_integer_bit_width(data->lhs_type), data->lhs_type->type_name, SOURCE(data->loc)
             );
        }
    } else {
        if (is_negative(data->lhs_type, lhs)) {
            UNLOCKED_WARN(
                "left shift of negative value %d" SOURCE_FMT,
                lhs, SOURCE(data->loc)
            );
        } else {
            UNLOCKED_WARN(
                "left shift of %d by %d places cannot be represented in type %s" SOURCE_FMT,
                lhs, rhs, data->lhs_type->type_name, SOURCE(data->loc)
            );
        }
    }
    debug_trace_me();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct out_of_bounds_data {
    source_location_t loc;
    type_descriptor_t* array_type;
    type_descriptor_t* index_type;
} out_of_bounds_data_t;

void __ubsan_handle_out_of_bounds(out_of_bounds_data_t* data, value_handle_t index) {
    UNLOCKED_WARN(
        "index %d out of bounds for type %s" SOURCE_FMT,
        index, data->array_type->type_name, SOURCE(data->loc));
    debug_trace_me();
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
    UNLOCKED_WARN(
        "load of value %ld, which is not a valid value for type %s" SOURCE_FMT,
        val, data->type->type_name, SOURCE(data->loc)
    );
    debug_trace_me();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct invalid_builtin_data {
    source_location_t loc;
    unsigned char kind;
} invalid_builtin_data_t;

void __ubsan_handle_invalid_builtin(invalid_builtin_data_t* data) {
    UNLOCKED_WARN(
        "passing zero to %s, which is not a valid argument" SOURCE_FMT,
         data->kind ? "ctz()" : "clz()", SOURCE(data->loc)
     );
    debug_trace_me();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: function_type_mismatch

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: nonnull_return_v1
// TODO: nullability_return_v1

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct nonnull_arg_data {
    source_location_t loc;
    source_location_t attr_loc;
    int arg_index;
} nonnull_arg_data_t;

void __ubsan_handle_nonnull_arg(nonnull_arg_data_t* data) {
    UNLOCKED_WARN(
        "null pointer passed as argument, %d, which is declared to never be null" SOURCE_FMT,
        data->arg_index, SOURCE(data->loc)
    );

    if (data->attr_loc.filename != NULL) {
        UNLOCKED_WARN("_Nonnull type annotation specified here" SOURCE_FMT, SOURCE(data->attr_loc));
    }
}

// TODO: nullability_arg

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct pointer_overflow_data {
    source_location_t loc;
} pointer_overflow_data_t;

void __ubsan_handle_pointer_overflow(pointer_overflow_data_t* data, value_handle_t base, value_handle_t result) {
    if (base >= 0 && result >= 0) {
        if (base > result) {
            UNLOCKED_WARN(
                "addition of unsigned offset to 0x%p overflowed to 0x%p" SOURCE_FMT,
                base, result, SOURCE(data->loc)
            );
        } else {
            UNLOCKED_WARN(
                "subtraction of unsigned offset from 0x%p overflowed to 0x%p" SOURCE_FMT,
                base, result, SOURCE(data->loc)
            );
        }
    } else {
        UNLOCKED_WARN(
            "pointer index expression with base 0x%p overflowed to 0x%p" SOURCE_FMT,
            base, result, SOURCE(data->loc)
        );
    }
    debug_trace_me();
}
