#include "hpet.h"

// TODO: Have a driver that can support multiple comparators and HPETs for better
// TODO: accuracy

bool hpet_is_supported() {
    return false;
}

error_t hpet_init() {
    return NO_ERROR;
}

