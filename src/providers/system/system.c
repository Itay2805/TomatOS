#include "system.h"

#include <resource/resource_provider.h>
#include <resource/resource.h>

#include <string.h>
#include <buf.h>

system_provider_t** keyboard_providers;
system_provider_t** serial_providers;
system_provider_t** framebuffer_providers;

static error_t domain_from_string(const char* str, system_domain_t* domain) {
    // TODO: Use str intern
    if(strcmp(str, "keyboard") == 0) {
        *domain = SYS_KEYBOARD;
    }else if(strcmp(str, "serial") == 0) {
        *domain = SYS_SERIAL;
    }else if(strcmp(str, "framebuffer") == 0) {
        *domain = SYS_FRAMEBUFFER;
    }else {
        return ERROR_INVALID_DOMAIN;
    }
}

static system_provider_t** get_providers_for_domain(system_domain_t domain) {
    switch(domain) {
        case SYS_KEYBOARD: return keyboard_providers;
        case SYS_SERIAL: return serial_providers;
        case SYS_FRAMEBUFFER: return framebuffer_providers;
        default: return NULL;
    }
}

static error_t open(resource_descriptor_t* descriptor, kresource_t* out_res) {
    error_t err = NO_ERROR;
    system_domain_t domain;
    system_provider_t** providers;

    CHECK_AND_RETHROW(domain_from_string(descriptor->domain, &domain));
    providers = get_providers_for_domain(domain);

    if(descriptor->port == 0) {
        // TODO: This should allow to get info about whatever
        CHECK_FAIL_ERROR(ERROR_NOT_IMPLEMENTED);
    }else {
        // any other port should just forward and open the correct provider
        CHECK_ERROR(descriptor->port >= 1 && descriptor->port <= buf_len(providers), ERROR_INVALID_PORT);
        CHECK_AND_RETHROW(kopen(providers[descriptor->port - 1]->descriptor, out_res));
    }

cleanup:
    return err;
}

static resource_provider_t provider = {
    .scheme = "system",
    .open = open,
};

void system_provider_register(system_domain_t domain, system_provider_t* provider) {
    switch(domain) {
        case SYS_KEYBOARD: {
            buf_push(keyboard_providers, provider);
            log_info("mounted %s to system://keyboard:%d/", provider->descriptor->scheme, buf_len(keyboard_providers));
        } break;
        case SYS_SERIAL: {
            buf_push(serial_providers, provider);
            log_info("mounted %s to system://serial:%d/", provider->descriptor->scheme, buf_len(keyboard_providers));
        } break;
        case SYS_FRAMEBUFFER: {
            buf_push(framebuffer_providers, provider);
            log_info("mounted %s to system://framebuffer:%d/", provider->descriptor->scheme, buf_len(keyboard_providers));
        } break;
        default: {
            log_warn("Attempted to register provider for invalid domain (scheme: %s)", provider->descriptor->scheme);
            return;
        }
    }
}


error_t system_provider_init() {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(register_provider(&provider));

cleanup:
    return err;
}
