#ifndef TOMATKERNEL_SYSTEM_H
#define TOMATKERNEL_SYSTEM_H

#include <resource/resource.h>

#include <error.h>

typedef enum system_domain {
    SYS_KEYBOARD,
    SYS_SERIAL,
    SYS_FRAMEBUFFER,
} system_domain_t;

typedef struct system_provider {
    resource_descriptor_t* descriptor;
} system_provider_t;

/**
 * Register a new provider to the system
 *
 * @remark
 * The system provider is saved by pointer, so all the pointers in it has to stay valid
 * throughout the runtime of the kernel.
 *
 * @param domain    [IN] What domain is this provider providing
 * @param provider  [IN] The provider to register
 */
void system_provider_register(system_domain_t domain, system_provider_t* provider);

error_t system_provider_init();

#endif //TOMATKERNEL_SYSTEM_H
