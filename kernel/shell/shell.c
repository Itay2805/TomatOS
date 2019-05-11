//
// Created by Itay on 11/05/2019.
//

#include <common/stdbool.h>
#include <resource/resource.h>
#include <common/string.h>
#include <common/klib.h>
#include <common/ctype.h>
#include "shell.h"

static resource_descriptor_t* resolve_uri(const char* str) {
    bool err = true;
    char* scheme = NULL;
    char* domain = NULL;
    char* path = NULL;
    char* sub_str = NULL;
    resource_descriptor_t* sub = NULL;
    int port = 0;

    // the scheme
    while(isalnum(*str)) {
        buf_push(scheme, *str++);
    }
    buf_push(scheme, 0);

    // constant
    if(*str++ != ':') goto cleanup;
    if(*str++ != '/') goto cleanup;
    if(*str++ != '/') goto cleanup;

    // get the subresource
    if(*str == '[') {
        str++;
        int depth = 1;
        while(depth > 0) {
            if(*str == ']') {
                depth--;
            }else if(*str == '[') {
                depth++;
            }else if(*str == 0) {
                goto cleanup;
            }

            if(depth > 0) {
                buf_push(sub_str, *str);
            }
            str++;
        }

        buf_push(sub_str, '\0');
    }

    // the domain
    if(isalnum(*str)) {
        while(isalnum(*str)) {
            buf_push(domain, *str++);
        }
        buf_push(domain, '\0');
    }

    // the port
    if(*str == ':') {
        str++;
        while(isdigit(*str)) {
            port *= 10;
            port += *str++ - '0';
        }
    }

    // constant
    if(*str++ != '/') goto cleanup;

    // path
    if(isalnum(*str)) {
        while (isalnum(*str) || *str == '/' || *str == '.' || *str == '_') {
            buf_push(path, *str++);
        }
        buf_push(path, '\0');
    }

    err = false;

    if(sub_str != NULL) {
        sub = resolve_uri(sub_str);
        err = sub == NULL ? true : false;
    }

cleanup:
    if(sub_str != NULL) {
        buf_free(sub_str);
    }

    resource_descriptor_t* desc = NULL;

    if(err) {
        if(sub) delete_resource_descriptor(sub);
    }else {
        desc = kalloc(sizeof(resource_descriptor_t));
        desc->port = port;
        desc->sub = sub;

        desc->scheme = kalloc(buf_len(scheme));
        memcpy(desc->scheme, scheme, buf_len(scheme));

        if(domain) {
            desc->domain = kalloc(buf_len(domain));
            memcpy(desc->domain, domain, buf_len(domain));
        }

        if(path) {
            desc->path = kalloc(buf_len(path));
            memcpy(desc->path, path, buf_len(path));
        }
    }

    buf_free(scheme);
    buf_free(domain);
    buf_free(path);

    return desc;
}

bool resolve_and_open(const char* str, resource_t* resource) {
    resource_descriptor_t* desc = resolve_uri(str);
    bool succ = open(desc, resource);
    delete_resource_descriptor(desc);
    return succ;
}
