
#include <util/except.h>
#include <util/defs.h>
#include <stdint.h>
#include <mem/vmm.h>

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

typedef struct ubsan_type_mismatch_v1 {
    ubsan_source_location_t loc;
    ubsan_type_t* type;
    uint8_t log_alignment;
    uint8_t type_check_kind;
} ubsan_type_mismatch_v1_t;

typedef struct ubsan_unreachable {
    ubsan_source_location_t loc;
} ubsan_unreachable_t;

typedef struct ubsan_invalid_builtin {
    ubsan_source_location_t loc;
    unsigned char kind;
} ubsan_invalid_builtin_t;

typedef struct ubsan_alignment_assumption {
    ubsan_source_location_t loc;
    ubsan_source_location_t assumption_loc;
    ubsan_type_t* type;
} ubsan_alignment_assumption_t;

typedef struct ubsan_invalid_value {
    ubsan_source_location_t loc;
    ubsan_type_t* type;
} ubsan_invalid_value_t;

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
    "_Nonnull binding to"
};

#define WARN_PRINT(fmt, ...) PRINT("[!] " __MODULE__ ": " fmt "\n", ## __VA_ARGS__)

typedef struct frame {
    struct frame* rbp;
    uint64_t rip;
} frame_t;

static void trace_stack() {
#ifdef __TOMATOS_AMD64__
    frame_t* current = (frame_t*)__builtin_frame_address(0);
    for (size_t i = 0; i < UINT64_MAX; i++) {
        if(!current) {
            break;
        }

        WARN_PRINT("\t%02d: RIP [%p]", i, current->rip);

        current = current->rbp;
    }
#endif
}

void __ubsan_handle_add_overflow(ubsan_overflow_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
    trace_stack();
}

void __ubsan_handle_sub_overflow(ubsan_overflow_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
    trace_stack();
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
    trace_stack();
}

void __ubsan_handle_mul_overflow(ubsan_overflow_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
    trace_stack();
}

void __ubsan_handle_divrem_overflow(ubsan_overflow_t* data, uintptr_t lhs, uintptr_t rhs) {
    WARN_PRINT("%s", __FUNCTION__);
    trace_stack();
}

void __ubsan_handle_negate_overflow(ubsan_overflow_t* data, uintptr_t old) {
    WARN_PRINT("%s", __FUNCTION__);
    trace_stack();
}

void __ubsan_handle_shift_out_of_bounds(ubsan_shift_out_of_bounds_t* data, uintptr_t lhs, uintptr_t rhs) {
    if (((int64_t)rhs) < 0 && data->rhs->kind == 0 && data->rhs->info & 1) {
        WARN_PRINT("shift exponent %d is negative (%s:%d:%d)",
                rhs,
                data->loc.filename, data->loc.line, data->loc.column);
    } else if (rhs >= (1 << (data->lhs->info >> 1))) {
        WARN_PRINT("shift exponent %d is too large for %d-bit type %s (%s:%d:%d)",
                rhs,
                1 << (data->lhs->kind >> 1),
                data->lhs->name,
                data->loc.filename, data->loc.line, data->loc.column);
    } else if (((int64_t)rhs) < 0 && data->lhs->kind == 0 && data->lhs->info & 1) {
        WARN_PRINT("left shift of negative value %d (%s:%d:%d)",
                   lhs,
                   data->loc.filename, data->loc.line, data->loc.column);
    } else {
        WARN_PRINT("left shift of %d by %d places cannot be represented in type %s (%s:%d:%d)",
                   lhs,
                   rhs,
                   data->lhs->name,
                   data->loc.filename, data->loc.line, data->loc.column);
    }
    trace_stack();
}

void __ubsan_handle_out_of_bounds(ubsan_out_of_bounds_t* data, uintptr_t index) {
    WARN_PRINT("index %d out of bounds for type %s (%s:%d:%d)",
            index,
            data->array->name,
            data->loc.filename, data->loc.line, data->loc.column);
    trace_stack();
}

void __ubsan_handle_type_mismatch_v1(ubsan_type_mismatch_v1_t* data, uintptr_t ptr) {
    size_t alignment = 1 << data->log_alignment;
    if (ptr == 0) {
        WARN_PRINT("%s null pointer of type %s (%s:%d:%d)",
                g_type_check_names[data->type_check_kind],
                data->type->name,
                data->loc.filename, data->loc.line, data->loc.column);
    } else if (ptr & (alignment - 1)) {
        WARN_PRINT("%s misaligned address %p for type %s, which requires %d byte alignment (%s:%d:%d)",
                g_type_check_names[data->type_check_kind],
                (void*)ptr,
                data->type->name,
                alignment,
                data->loc.filename, data->loc.line, data->loc.column);
    } else {
        WARN_PRINT("%s address %p with insufficient space for an object of type %s (%s:%d:%d)",
               g_type_check_names[data->type_check_kind],
               (void*)ptr,
               data->type->name,
               data->loc.filename, data->loc.line, data->loc.column);
    }
    trace_stack();
}

void __ubsan_handle_builtin_unreachable(ubsan_unreachable_t* data) {
    WARN_PRINT("execution reached an unreachable program point (%s:%d:%d)", data->loc.filename, data->loc.line, data->loc.column);
    trace_stack();
}

void __ubsan_handle_invalid_builtin(ubsan_invalid_builtin_t* data) {
    WARN_PRINT("passing zero to %s, which is not a valid argument (%s:%d:%d)",
            data->kind ? "clz()" : "ctz()",
            data->loc.filename, data->loc.line, data->loc.column);
    trace_stack();
}

void __ubsan_handle_alignment_assumption(ubsan_alignment_assumption_t* data, uint64_t pointer, uint64_t alignment, uint64_t offset) {
//    uintptr_t real_pointer = pointer - offset;
    // TODO: lsb
    // TODO: actual_alignment

//    uint64_t mask = alignment - 1;
//    uint64_t mis_alignment_offset =

    if (!offset) {
        WARN_PRINT("assumption of %d byte alignment for pointer of type %s failed (%s:%d:%d)",
                alignment,
                data->type->name,
                data->loc.filename, data->loc.line, data->loc.column);
    } else {
        WARN_PRINT("assumption of %d byte alignment (with offset of %d byte) for pointer of type %s failed (%s:%d:%d)",
                alignment,
                offset,
                data->type->name,
                data->loc.filename, data->loc.line, data->loc.column);
    }
    WARN_PRINT("assumption was made specified here %s:%d:%d", data->assumption_loc.filename, data->assumption_loc.line, data->assumption_loc.column);
    trace_stack();
}

void __ubsan_handle_load_invalid_value(ubsan_invalid_value_t* data, uintptr_t val) {
    WARN_PRINT("load of value %p, which is not a valid value of type %s (%s:%d:%d)",
            val,
            data->type->name,
            data->loc.filename, data->loc.line, data->loc.column);
    trace_stack();
}
