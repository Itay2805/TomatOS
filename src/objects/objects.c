#include "objects.h"

static handle_t handle_gen = 0;

handle_t generate_object_handle() {
    return ++handle_gen;
}
