#include "process.h"

map_t processes;

error_t process_create(process_t** process) {
    error_t err = NO_ERROR;

    process_t* new_process = calloc(1, sizeof(process_t));

    CHECK_AND_RETHROW(thread_create())

    *process = new_process;

cleanup:
    if(err == NO_ERROR) {

    }
    return err;
}

bool process_validate(process_t* process) {
    error_t err = NO_ERROR;
    void* addr;

    // make sure this thread actually exists in memory
    CHECK_AND_RETHROW(vmm_virt_to_phys(kernel_address_space, process, &addr));
    CHECK_AND_RETHROW(vmm_virt_to_phys(kernel_address_space, ((char*)process) + sizeof(process_t), &addr));

cleanup:
    return err == NO_ERROR;
}
