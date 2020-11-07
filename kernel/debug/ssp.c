
#include <util/except.h>
#include <stdnoreturn.h>

/**
 * This value will be switched to another value once
 * the kernel is initialized, so it will still be random
 */
__attribute__((used))
uintptr_t __stack_chk_guard = 0x8b549ac8df9f2e5;

noreturn void __stack_chk_fail() {
    ASSERT(false, "stack smashing detected");
}
