//
// Created by Itay on 11/05/2019.
//

#include <common/stdbool.h>
#include <resource/resource.h>
#include <common/string.h>
#include <common/klib.h>
#include <common/ctype.h>
#include <process/process.h>
#include <graphics/term.h>
#include <providers/stdio/stdio_provider.h>
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

static const char* mount_point;
static resource_descriptor_t* descriptor;
static resource_t working_dir;

static void safe_close(resource_t res) {
    if(res != NULL) {
        close(res);
    }
}

static void safe_delete(void* ptr) {
    if(ptr != NULL) {
        kfree(ptr);
    }
}

static void handle_command(const char* command_line) {
    if(buf_len(command_line) == 0 || strlen(command_line) == 0) return;
    if(command_line[0] == '!') {
        // Builtins, incase no mount is found
        if(strncmp(command_line, "!mount ", 7) == 0) {
            safe_close(working_dir);
            working_dir = NULL;
            if(descriptor) delete_resource_descriptor(descriptor);
            const char* uri_start = strchr(command_line, ' ') + 1;
            descriptor = resolve_uri(uri_start);
            if(descriptor == NULL) {
                LOG_ERROR("Failed to resolve path!");
            }else {
                if(open(descriptor, &working_dir)) {
                    LOG_INFO("Mounted `%s`", uri_start);
                }else {
                    LOG_ERROR("Failed to mount `%s`", uri_start);
                    delete_resource_descriptor(descriptor);
                    descriptor = NULL;
                }
            }
        }else {
            LOG_ERROR("No such builtin");
        }
    }else {
        // run a binary
        LOG_DEBUG("Implement running binaries from the command line");
    }
}

static void shell_start() {
    resource_t stdin;
    char* command_line = NULL;

    term_write("\n\n\n");
    LOG_NOTICE("TomatShell loaded successfully!");

    descriptor = resolve_uri(mount_point);
    if(!resolve_and_open("stdio://stdin/", &stdin)) {
        LOG_CRITICAL("shell failed to open stdin :(");
        tkill(0);
        // TODO: Exit process
    }

    if(mount_point != NULL && strlen(mount_point) > 0) {
        if(!resolve_and_open(mount_point, &working_dir)) {
            LOG_ERROR("Failed to mount `%s`, use `!mount <path>` to mount a resource", mount_point);
        }else {
            LOG_INFO("Loaded mount `%s`", mount_point);
        }
    }else {
        LOG_WARN("no mount specified, use `!mount <path>` to mount a resource");
    }

    while(true) {
        if(descriptor != NULL) {
            term_print("@:%s$ ", descriptor->path ? descriptor->path : "/");
        }else {
            term_print("@:<no mount>$ ");
        }

        char ch[] = { 0, 0 };
        while(ch[0] != '\n') {
            wait(stdin);
            while(poll(stdin)) {
                read(stdin, &ch[0], 1, NULL);
                if(buf_len(command_line) > 0 && ch[0] == KEYS_BACKSPACE) {
                    // delete char
                    term_set_cursor_pos(term_get_cursor_x() - 1, term_get_cursor_y());
                    term_write(" ");
                    term_set_cursor_pos(term_get_cursor_x() - 1, term_get_cursor_y());
                    buf_resize(command_line, buf_len(command_line) - 1);
                }else if(ch[0] != '\n') {
                    // add char
                    buf_push(command_line, ch[0]);
                    term_write(ch);
                }
            }
        }
        term_write("\n");
        buf_push(command_line, 0);
        handle_command(command_line);
        buf_clear(command_line);
    }
}

void start_shell(const char* mount) {
    process_t* process = process_create(shell_start, true);
    char* stack = kalloc(MB(1));
    process->threads[0]->cpu_state.rsp = (uint64_t)stack + MB(1);
    process->threads[0]->cpu_state.rbp = (uint64_t)stack + MB(1);

    mount_point = mount;
}
