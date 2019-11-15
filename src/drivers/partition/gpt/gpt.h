#ifndef TOMATKERNEL_GPT_H
#define TOMATKERNEL_GPT_H

#include <drivers/storage/storage_object.h>

#include <stdbool.h>

/**
 * Check if the given device is gpt partitioned
 *
 */
bool check_gpt(storage_device_t* storage);

#endif //TOMATKERNEL_GPT_H
