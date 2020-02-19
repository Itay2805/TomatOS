#ifndef __SMP_SMP_H__
#define __SMP_SMP_H__

#include <stdatomic.h>

void smp_startup_all_aps(atomic_flag* flag);

#endif //__SMP_SMP_H__
