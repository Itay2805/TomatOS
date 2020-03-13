#include "msr.h"
#include "intrin.h"

IA32_EFER __read_efer(void) {
    return (IA32_EFER) { .raw = __readmsr(MSR_IA32_EFER) };
}

void __write_efer(IA32_EFER efer) {
    __writemsr(MSR_IA32_EFER, efer.raw);
}

IA32_APIC_BASE __read_apic_base() {
    return (IA32_APIC_BASE) { .raw = __readmsr(MSR_CODE_IA32_APIC_BASE) };
}
