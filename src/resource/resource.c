//
// Created by Itay on 31/05/2019.
//

#include "resource.h"
#include "resource_provider.h"

error_t kopen(resource_descriptor_t* descriptor, kresource_t* out_res) {
    error_t err = NO_ERROR;

    CHECK_ERROR(descriptor, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(out_res, ERROR_INVALID_ARGUMENT);

    resource_provider_t* provider = get_provider_by_scheme(descriptor->scheme);
    CHECK_ERROR(provider, ERROR_INVALID_SCHEME);
    CHECK_AND_RETHROW(provider->open(descriptor, out_res));

cleanup:
    return err;
}

error_t kclose(kresource_t res) {
    error_t err = NO_ERROR;

    resource_provider_t* provider = get_provider_by_kresource(res);
    CHECK_ERROR(provider, ERROR_INVALID_RESOURCE);
    CHECK_AND_RETHROW(provider->close(res));

cleanup:
    return err;
}

error_t kseek(kresource_t res, size_t offset, int dir) {
    error_t err = NO_ERROR;

    resource_provider_t* provider = get_provider_by_kresource(res);
    CHECK_ERROR(provider, ERROR_INVALID_RESOURCE);
    CHECK_AND_RETHROW(provider->seek(res, offset, dir));

    cleanup:
    return err;
}

error_t ktell(kresource_t res, size_t* offset) {
    error_t err = NO_ERROR;

    resource_provider_t* provider = get_provider_by_kresource(res);
    CHECK_ERROR(provider, ERROR_INVALID_RESOURCE);
    CHECK_AND_RETHROW(provider->tell(res, offset));

cleanup:
    return err;
}

error_t kread(kresource_t res, void* buffer, size_t buffer_size, size_t read_size) {
    error_t err = NO_ERROR;

    resource_provider_t* provider = get_provider_by_kresource(res);
    CHECK_ERROR(provider, ERROR_INVALID_RESOURCE);
    CHECK_AND_RETHROW(provider->read(res, buffer, buffer_size, read_size));

cleanup:
    return err;
}

error_t kwrite(kresource_t res, const void* buffer, size_t buffer_size, size_t writen_size) {
    error_t err = NO_ERROR;

    resource_provider_t* provider = get_provider_by_kresource(res);
    CHECK_ERROR(provider, ERROR_INVALID_RESOURCE);
    CHECK_AND_RETHROW(provider->write(res, buffer, buffer_size, writen_size));

cleanup:
    return err;
}

error_t kstat(kresource_t res, resource_stat_t* stat) {
    error_t err = NO_ERROR;

    resource_provider_t* provider = get_provider_by_kresource(res);
    CHECK_ERROR(provider, ERROR_INVALID_RESOURCE);

    err = provider->close(res);

cleanup:
    return err;
}

error_t kpoll(kresource_t res, int* state) {
    error_t err = NO_ERROR;

    resource_provider_t* provider = get_provider_by_kresource(res);
    CHECK_ERROR(provider, ERROR_INVALID_RESOURCE);
    CHECK_AND_RETHROW(provider->poll(res, state));

cleanup:
    return err;
}

error_t kmkdir(resource_descriptor_t* descriptor) {
    error_t err = NO_ERROR;

    CHECK_ERROR(descriptor, ERROR_INVALID_ARGUMENT);

    resource_provider_t* provider = get_provider_by_scheme(descriptor->scheme);
    CHECK_ERROR(provider, ERROR_INVALID_SCHEME);
    CHECK_AND_RETHROW(provider->mkdir(descriptor));

cleanup:
    return err;
}

error_t kreaddir(kresource_t res, resource_stat_t* stat, int index) {
    error_t err = NO_ERROR;

    resource_provider_t* provider = get_provider_by_kresource(res);
    CHECK_ERROR(provider, ERROR_INVALID_RESOURCE);
    CHECK_AND_RETHROW(provider->readdir(res, stat, index));

cleanup:
    return err;
}
