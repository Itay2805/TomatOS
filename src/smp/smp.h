#ifndef TOMATKERNEL_SMP_H
#define TOMATKERNEL_SMP_H

#include <common/error.h>

/**
 * Will initialize and startup each of the cores.
 *
 * The cores will wait till the scheduler will kick in
 */
error_t smp_init();

#endif //TOMATKERNEL_SMP_H
