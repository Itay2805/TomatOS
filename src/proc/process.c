#include "process.h"

spinlock_t process_lock = SPINLOCK_INIT;
list_entry_t process_list = INIT_LIST_ENTRY(process_list);

/**
 * The kernel process is always pid 0
 */
process_t kernel_process = {
    .pid = 0,
    .threads_lock = SPINLOCK_INIT,
    .threads_list = INIT_LIST_ENTRY(kernel_process.threads_list)
};

void init_kernel_process() {
    insert_tail_list(&process_list, &kernel_process.link);
}
