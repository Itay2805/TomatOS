#include <libc/string.h>
#include <memory/mm.h>
#include "process.h"

process_t* kernel_process;

process_t* new_process() {
    return mm_allocate_pool(sizeof(process_t));
}
