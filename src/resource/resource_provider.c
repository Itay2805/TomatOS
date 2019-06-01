#include <buf.h>
#include <string.h>
#include <map.h>
#include "resource_provider.h"

// TODO: We need the map and buf to get this working
static resource_provider_t** providers = 0;
static map_t kresource_to_provider = {0};
static uint64_t handler = 0;

kresource_t create_kresource(resource_provider_t* provider) {
    uint64_t khandler = 0;
    do {
        khandler = hash_uint64(handler++);
    } while(get_provider_by_kresource(khandler) == NULL);
    map_put_from_uint64(&kresource_to_provider, khandler, provider);
    return khandler;
}

void delete_kresource(kresource_t res) {
    map_put_from_uint64(&kresource_to_provider, res, 0);
}

error_t register_provider(resource_provider_t* provider) {
    error_t err = NO_ERROR;

    CHECK_ERROR_TRACE(get_provider_by_scheme(provider->scheme) == NULL, ERROR_SCHEME_EXISTS, "The scheme `%s` is already registered", provider->scheme);
    buf_push(providers, provider);

cleanup:
    return err;
}

resource_provider_t* get_provider_by_scheme(const char* scheme) {
    for(resource_provider_t** it = providers; it < buf_end(providers); it++) {
        // TODO: Use string interning
        if(strcmp((*it)->scheme, scheme) == 0) {
            return *it;
        }
    }
    return NULL;
}

resource_provider_t* get_provider_by_kresource(kresource_t res) {
    resource_provider_t* provider = map_get_from_uint64(&kresource_to_provider, res);
    return provider;
}

