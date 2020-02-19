#include "msr.h"
#include "intrin.h"

MSR_IA32_EFER __readmsr_efer(void) {
    return (MSR_IA32_EFER) { .raw = __readmsr(MSR_CORE_IA32_EFER) };
}

void __writemsr_efer(MSR_IA32_EFER efer) {
    __writemsr(MSR_CORE_IA32_EFER, efer.raw);
}

MSR_IA32_APIC_BASE __readmsr_apic_base() {
    return (MSR_IA32_APIC_BASE) { .raw = __readmsr(MSR_CODE_IA32_APIC_BASE) };
}
