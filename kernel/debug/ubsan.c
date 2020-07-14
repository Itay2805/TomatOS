
#include <util/except.h>
#include <util/defs.h>
#include <stdint.h>

typedef struct ubsan_type {
    uint16_t kind;
    uint16_t info;
    char name[];
} ubsan_type_t;

typedef struct ubsan_source_location {
    const char* filename;
    uint32_t line;
    uint32_t column;
} ubsan_source_location_t;

typedef struct ubsan_overflow {
    ubsan_source_location_t loc;
    ubsan_type_t* type;
} ubsan_overflow_t;

typedef struct ubsan_shift_out_of_bounds {
    ubsan_source_location_t loc;
    ubsan_type_t* lhs;
    ubsan_type_t* rhs;
} ubsan_shift_out_of_bounds_t;

typedef struct ubsan_out_of_bounds {
    ubsan_source_location_t loc;
    ubsan_type_t* array;
    ubsan_type_t* index;
} ubsan_out_of_bounds_t;

typedef struct ubsan_non_null_return {
    ubsan_source_location_t loc;
} ubsan_non_null_return_t;

typedef struct ubsan_type_mismatch_v1 {
    ubsan_source_location_t loc;
    ubsan_type_t* type;
    uint8_t log_alignment;
    uint8_t type_check_kind;
} ubsan_type_mismatch_v1_t;

typedef struct ubsan_type_mismatch {
    ubsan_source_location_t loc;
    ubsan_type_t* type;
    uint64_t alignment;
    uint8_t type_check_kind;
} ubsan_type_mismatch_t;

typedef struct ubsan_vla_bound {
    ubsan_source_location_t loc;
    ubsan_type_t* type;
} ubsan_vla_bound_t;

typedef struct ubsan_invalid_value {
    ubsan_source_location_t loc;
    ubsan_type_t* type;
} ubsan_invalid_value_t;

typedef struct ubsan_unreachable {
    ubsan_source_location_t loc;
} ubsan_unreachable_t;

typedef struct ubsan_nonnull_arg {
    ubsan_source_location_t loc;
} ubsan_nonnull_arg_t;

const char* g_type_check_names[] = {
    "load of",
    "store to",
    "reference binding to",
    "member access within",
    "member call on",
    "constructor call on",
    "downcast of",
    "downcast of",
    "upcast of",
    "cast to virtual base of",
    "_Nonnull binding to",
    "dynamic operation on"
};

#define WARN_PRINT(fmt, ...) PRINT("[!] " __MODULE__ ": " fmt "\n", ## __VA_ARGS__)

void __ubsan_handle_add_overflow(ubsan_overflow_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_sub_overflow(ubsan_overflow_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_pointer_overflow(ubsan_overflow_t* data, uintptr_t base, uintptr_t result) {
    if (base == 0 && result == 0) {
        WARN_PRINT("applying zero offset to null pointer (%s:%d:%d)", data->loc.filename, data->loc.line, data->loc.column);
    } else if (base == 0 && result != 0) {
        WARN_PRINT("applying non-zero offset %p to null pointer (%s:%d:%d)", result, data->loc.filename, data->loc.line, data->loc.column);
    } else if (base != 0 && result == 0) {
        WARN_PRINT("applying non-zero offset to non-null pointer %p produced null pointer (%s:%d:%d)", base, data->loc.filename, data->loc.line, data->loc.column);
    } else if (((ptrdiff_t)base) >= 0 == ((ptrdiff_t)result) >= 0) {
        if (base > result) {
            WARN_PRINT("addition of unsigned offset to %p overflowed to %p (%s:%d:%d)", base, result, data->loc.filename, data->loc.line, data->loc.column);
        } else {
            WARN_PRINT("subtraction of unsigned offset to %p overflowed to %p (%s:%d:%d)", base, result, data->loc.filename, data->loc.line, data->loc.column);
        }
    } else {
        WARN_PRINT("pointer index expression with base %p overflowed to %p (%s:%d:%d)", base, result, data->loc.filename, data->loc.line, data->loc.column);
    }
}

void __ubsan_handle_mul_overflow(ubsan_overflow_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_divrem_overflow(ubsan_overflow_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_negate_overflow(ubsan_overflow_t* data, uintptr_t old) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_shift_out_of_bounds(ubsan_shift_out_of_bounds_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_out_of_bounds(ubsan_out_of_bounds_t* data, uintptr_t index) {
    WARN_PRINT("index %d out of bounds for type %s (%s:%d:%d)", index, data->array->name, data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_nonnull_return(ubsan_non_null_return_t* data, ubsan_source_location_t* loc) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_type_mismatch_v1(ubsan_type_mismatch_v1_t* data, uintptr_t ptr) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_vla_bound_not_positive(ubsan_vla_bound_t* data, uintptr_t bound) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_load_invalid_value(ubsan_invalid_value_t* data, uintptr_t val) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_builtin_unreachable(ubsan_unreachable_t* data) {
    WARN_PRINT("execution reached an unreachable program point (%s:%d:%d)", data->loc.filename, data->loc.line, data->loc.column);
}

void __ubsan_handle_nonnull_arg(ubsan_nonnull_arg_t* data) {
    WARN_PRINT("%s", __FUNCTION__);
}

void __ubsan_handle_type_mismatch(ubsan_type_mismatch_t* data, uintptr_t ptr) {
    WARN_PRINT("%s", __FUNCTION__);
}
