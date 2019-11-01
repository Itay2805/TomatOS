#ifndef TOMATKERNEL_VMM_H
#define TOMATKERNEL_VMM_H

#define DIRECT_MAPPING_BASE         0xFFFF800000000000ul
#define PHYSICAL_TO_DIRECT(addr)    ((typeof(addr))(((uintptr_t)(addr)) + DIRECT_MAPPING_BASE))
#define DIRECT_TO_PHYSICAL(addr)    ((typeof(addr))(((uintptr_t)(addr)) - DIRECT_MAPPING_BASE))
#define IS_KERNEL_PTR(ptr)          ((uintptr_t)(ptr) > DIRECT_MAPPING_BASE)

#endif //TOMATKERNEL_VMM_H
