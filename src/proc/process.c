#include <mm/mm.h>
#include "process.h"
#include "elf.h"
#include "thread.h"

static pid_t g_pid_gen = 1;

spinlock_t process_lock = SPINLOCK_INIT;
list_entry_t process_list = INIT_LIST_ENTRY(process_list);

/**
 * The kernel process is always pid 0
 */
process_t kernel_process = {
    .pid = 0,
    .tid_gen = 0,
    .threads_lock = SPINLOCK_INIT,
    .threads_list = INIT_LIST_ENTRY(kernel_process.threads_list)
};

void init_kernel_process() {
    insert_tail_list(&process_list, &kernel_process.link);
}

err_t create_process(process_t** process) {
    err_t err = NO_ERROR;

    CHECK(process != NULL);

    // initialize a new process
    process_t* proc = mm_allocate(sizeof(process_t));
    vmm_create_address_space(&proc->vmm_handle);
    proc->pid = g_pid_gen++;
    proc->tid_gen = 0;
    proc->threads_list = INIT_LIST_ENTRY(proc->threads_list);
    proc->threads_lock = SPINLOCK_INIT;

    // insert the process to the process list
    spinlock_acquire(&process_lock);
    insert_tail_list(&process_list, &proc->link);
    spinlock_release(&process_lock);

    *process = proc;

cleanup:
    return err;
}

err_t spawn_process(file_t file, process_t** proc) {
    err_t err = NO_ERROR;
    vmm_handle_t* cur = NULL;
    process_t* new_proc = NULL;
    uintptr_t stack = -1;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(proc != NULL, ERROR_INVALID_PARAM);
    *proc = NULL;

    // create the process
    CHECK_AND_RETHROW(create_process(&new_proc));
    // switch to the new address space
    cur = vmm_get_handle();
    vmm_set_handle(&new_proc->vmm_handle);

    // load the elf
    elf_info_t info = {0};
    CHECK_AND_RETHROW(load_elf(file, &new_proc->vmm_handle, &info));

    // allocate a stack
    CHECK_AND_RETHROW(vmm_user_allocate(&new_proc->vmm_handle, ALLOCATE_ANY, &stack, 8, PAGE_READWRITE));

    // start the thread at the entry
    CHECK_AND_RETHROW(spawn_thread(new_proc, info.entry, stack + 4096 * 8, NULL));

    *proc = new_proc;

cleanup:
    // restore the state of the current process
    if (cur != NULL) {
        vmm_set_handle(cur);
    }

    // delete the process if we failed to create it
    if (IS_ERROR(err)) {
        // TODO: kill the thread if any

        if (new_proc != NULL) {
            if (new_proc->vmm_handle.pml4_physical != 0) {
                vmm_destroy_page_table(&new_proc->vmm_handle);
            }

            mm_free(new_proc);
        }
    }

    return err;
}
