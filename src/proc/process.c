#include <mm/mm.h>
#include <stb_ds.h>
#include <util/def.h>
#include "process.h"
#include "elf.h"
#include "thread.h"
#include "sched.h"

static pid_t g_pid_gen = 1;

// TODO: Maybe switch to a hashmap
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

err_t get_process_by_pid(int pid, process_t** process) {
    err_t err = NO_ERROR;

    CHECK_AND_RETHROW(process != NULL);

    FOR_EACH_IN_LIST(process_list, link) {
        process_t* proc = CR(link, process_t, link);
        if (proc->pid == pid) {
            *process = proc;
            goto cleanup;
        }
    }

    CHECK_FAIL_ERROR(ERROR_NOT_FOUND);

cleanup:
    return err;
}

err_t create_process(process_t** process) {
    err_t err = NO_ERROR;

    CHECK(process != NULL);

    // initialize a new process
    process_t* proc = mm_allocate(sizeof(process_t));
    vmm_create_address_space(&proc->vmm_handle);
    proc->pid = g_pid_gen++;
    proc->tid_gen = 1;
    proc->threads_list = INIT_LIST_ENTRY(proc->threads_list);
    proc->threads_lock = SPINLOCK_INIT;
    proc->next_handle = 1;
    proc->handles = NULL;
    proc->handles_lock = SPINLOCK_INIT;

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

err_t add_handle(process_t* process, handle_t handle, int* out_handle) {
    err_t err = NO_ERROR;

    CHECK_ERROR(out_handle != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(process != NULL, ERROR_INVALID_PARAM);
    spinlock_acquire(&process->handles_lock);

    // add it
    *out_handle = process->next_handle++;
    hmput(process->handles, *out_handle, handle);

cleanup:
    if (process != NULL) {
        spinlock_release(&process->handles_lock);
    }

    return err;
}

err_t remove_handle(process_t* process, int handle) {
    err_t err = NO_ERROR;
    int i = 0;

    CHECK_ERROR(process != NULL, ERROR_INVALID_PARAM);
    spinlock_acquire(&process->handles_lock);

    // get the entry
    i = hmgeti(process->handles, handle);
    CHECK_ERROR(i != -1, ERROR_INVALID_HANDLE);

    // close the handle
    CHECK_AND_RETHROW(close_handle(process->handles[i].value));

cleanup:
    if (process != NULL) {
        // remove from the map
        if (i != -1) {
            hmdel(process->handles, handle);
        }

        spinlock_release(&process->handles_lock);
    }
    return err;
}

err_t get_handle(process_t* process, int handle, handle_t* out_handle) {
    err_t err = NO_ERROR;
    int i = -1;

    CHECK_ERROR(out_handle != NULL, ERROR_INVALID_HANDLE);
    CHECK_ERROR(process != NULL, ERROR_INVALID_PARAM);

    spinlock_acquire(&process->handles_lock);

    i = hmgeti(process->handles, handle);
    CHECK_ERROR(i != -1, ERROR_INVALID_HANDLE);

    // lock and increase count
    spinlock_acquire(&process->handles[i].value->lock);
    process->handles[i].value->refcount++;
    *out_handle = process->handles[i].value;

cleanup:
    if (i != 0) {
        spinlock_release(&process->handles[i].value->lock);
    }

    if (process != NULL) {
        spinlock_release(&process->handles_lock);
    }

    return err;
}