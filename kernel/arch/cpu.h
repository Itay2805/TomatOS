#ifndef TOMATOS_CPU_H
#define TOMATOS_CPU_H

#include <util/defs.h>

#include <arch/intrin.h>

/**
 * The system context is saved into this struct on exceptions
 * so it can be nicely logged
 */
typedef struct system_context {
    uint64_t ds;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t int_num;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    IA32_RFLAGS rflags;
    uint64_t rsp;
    uint64_t ss;
} system_context_t;

/**
 * The page fault params are passed to the page fault handler function
 * for handling it or crashing on it
 */
typedef union page_fault_params {
    struct {
        uint32_t present : 1;
        uint32_t write : 1;
        uint32_t user : 1;
        uint32_t reserved_write : 1;
        uint32_t instruction_fetch : 1;
    };
    uint32_t raw;
} page_fault_params_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPU Local variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Declares a cpu local variable
 */
#define CPU_LOCAL __attribute__((address_space(256), section(".cpu_local_data")))

/**
 * Get the local variable of the given cpu
 */
#define CPU_LOCAL_OF(var, cpu) (*(typeof(var)*)(g_cpu_locals[cpu] + ((uintptr)&var)))

/**
 * List of pointers to the cpu locals of each cpu
 */
extern uintptr_t g_cpu_locals[256];

/**
 * The amount of cpus in the running system
 */
extern size_t g_cpu_count;

/**
 * The id of the current cpu
 */
extern size_t CPU_LOCAL g_cpu_id;

/**
 * The lapic id of the current cpu, this may not be the same
 * as the current cpu id
 */
extern size_t CPU_LOCAL g_lapic_id;

/**
 * Initialize the cpu locals for the given cpu
 *
 * @param cpuid     [IN] The id of the cpu
 * @param lapic_id  [IN] The id of the lapic
 */
void init_cpu_locals(size_t cpuid, size_t lapic_id);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other cpu related operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Pause the cpu for a bit, used around spinlocks
 * and alike
 */
void cpu_pause();

/**
 * Makes the cpu go to sleep until an interrupt
 * arrives
 */
void cpu_sleep();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory fences and barriers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Memory fence to serialize memory accesses
 */
void memory_barrier();

/**
 * TODO: This
 */
void store_fence();

/**
 * TODO: This
 */
void load_fence();

/**
 * TODO: This
 */
void memory_fence();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt management
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Disable interrupts on the current cpu
 */
void disable_interrupts();

/**
 * Enable interrupts on the current cpu
 */
void enable_interrupts();

/**
 * Checks if interrupts are enabled on the current cpu
 */
bool are_interrupts_enabled();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IPIs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum ipi {
    IPI_WAKEUP = 0xf0
} ipi_t;

void send_ipi();

#endif //TOMATOS_CPU_H
