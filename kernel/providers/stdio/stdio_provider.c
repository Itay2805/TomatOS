//
// Created by Itay on 08/05/2019.
//

#include <process/process.h>
#include <common/string.h>
#include <process/syscalls.h>
#include <common/klib.h>
#include <locks/critical_section.h>
#include <graphics/term.h>

#include "stdio_provider.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

#define STDOUT 0
#define STDIN 1

static resource_provider_t stdio_provider = {0};
static resource_t ps2_keyboard;

typedef struct resource_context {
    resource_t resource;
    thread_t* thread;
    int type;
    char* buf;
} resource_context_t;

resource_context_t** contexts = NULL;

static void add_context(resource_context_t* context) {
    resource_context_t** found = NULL;
    for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
        if(*it == NULL) {
            found = it;
            break;
        }
    }
    if(found != NULL) {
        *found = context;
    }else {
        buf_push(contexts, context);
    }
}

static error_t get_context(process_t* process, resource_t res, resource_context_t** context) {
    error_t err = NO_ERROR;
    resource_context_t* found = NULL;

    for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
        if(*it != NULL && (*it)->resource == res) {
            found = *it;
            break;
        }
    }

    CHECK_ERROR(found != NULL, ERROR_INVALID_RESOURCE);
    *context = found;

cleanup:
    return err;
}

static error_t remove_context(process_t* process, resource_t res) {
    error_t err = NO_ERROR;
    bool found = false;

    for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
        if(*it != NULL && (*it)->resource == res) {
            mm_free(&kernel_memory_manager, *it);
            *it = NULL;
            break;
        }
    }

    CHECK_ERROR(found, ERROR_INVALID_RESOURCE);

cleanup:
    return err;
}

// keycode mapping
#define KEY_RELEASE 0x80
static uint8_t scs_mapping[0x80];
static uint8_t shift_mapping[0x80];
static bool key_states[1024];

static char translate_char(char c) {
    if(c == 0xE0) {
        read(ps2_keyboard, &c, 0, NULL);
        // TODO:
        c = NULL;
    }else {
        if(c & KEY_RELEASE != 0) {
            // key release
            c &= ~KEY_RELEASE;
            if(scs_mapping[c] != NULL) {
                c = scs_mapping[c];
                key_states[c] = false;
            }
            c = NULL;
        }else {
            // key press
            if(scs_mapping[c] != NULL) {
                c = scs_mapping[c];
                if(key_states[KEYS_LEFT_SHIFT] || key_states[KEYS_RIGHT_SHIFT]) {
                    c = shift_mapping[c];
                }
                key_states[c] = true;
            }
        }
    }
    return c;
}

static void dispatch(char c) {
    for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
        if(*it != NULL && (*it)->type == STDIN) {
            CRITICAL_SECTION({
                buf_push((*it)->buf, c);
            });
            resource_manager_resource_ready((*it)->thread, (*it)->resource);
        }
    }
}

static void handle_ps2() {
    // TODO: detect which keyboards are available
    // uses ps2://keyboard/
    resource_descriptor_t descriptor;
    descriptor.scheme = "ps2";
    descriptor.domain = "keyboard";
    if(open(&descriptor, &ps2_keyboard)) {
        while(true) {
            wait(ps2_keyboard);
            while(poll(ps2_keyboard)) {
                char c = 0;
                read(ps2_keyboard, &c, 0, NULL);
                c = translate_char(c);
                if(c != NULL) {

                }
            }
        }
    }else {
        LOG_ERROR("Failed to open ps2://keyboard/, stdio://stdin/ will not work!");
        tkill(0);
    }
}

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource);

static error_t handle_open(process_t* process, thread_t* thread, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;
    resource_descriptor_t* desc = NULL;

    // get the desc
    CHECK_AND_RETHROW(copy_resource_descriptor_to_kernel(process, descriptor, &desc));
    CHECK_ERROR(desc->domain != NULL, ERROR_INVALID_ARGUMENT);

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &stdio_provider, &created_resource));

    // copy the resource to user space
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

    resource_context_t* context = mm_allocate(&kernel_memory_manager, sizeof(resource_context_t));
    context->resource = created_resource;
    context->thread = thread;
    context->buf = 0;

    if(strcmp(desc->domain, "stdout") == 0) {
        context->type = STDOUT;
    }else if(strcmp(desc->domain, "stdin") == 0) {
        context->type = STDIN;
    }else {
        CHECK_FAIL_ERROR(ERROR_INVALID_DOMAIN);
    }

cleanup:
    if(IS_ERROR(err)) {
        handle_close(process, thread, created_resource);
    }
    if(desc != NULL) {
        mm_free(&kernel_memory_manager, desc);
    }
    return err;
}

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(remove_context(process, resource));
    CHECK_AND_RETHROW(resource_remove(process, resource));

cleanup:
    return NO_ERROR;
}

static error_t handle_write(process_t* process, thread_t* thread, resource_t resource, char* buffer, size_t len) {
    error_t err = NO_ERROR;
    char* kbuffer = NULL;    
    resource_context_t* context = NULL;

    // get the context
    CHECK_AND_RETHROW(get_context(process, resource, &context));
    CHECK_ERROR(context->type == STDOUT, ERROR_NOT_WRITEABLE);

    kbuffer = mm_allocate(&kernel_memory_manager, len + 1);
    kbuffer[len] = 0;
    CHECK_AND_RETHROW(vmm_copy_to_kernel(process->address_space, buffer, kbuffer, len));

    term_write(kbuffer);

    // Does nothing
cleanup:
    mm_free(&kernel_memory_manager, kbuffer);
    return err;
}

static error_t handle_read(process_t* process, thread_t* thread, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;
    char* kbuffer = NULL;    
    resource_context_t* context = NULL;

    // get the context
    CHECK_AND_RETHROW(get_context(process, resource, &context));
    CHECK_ERROR(context->type == STDIN, ERROR_NOT_READABLE);

    CRITICAL_SECTION_LABEL(copy_to_user_failed, {
        len = MIN(buf_len(context->buf), len);
        CHECK_AND_RETHROW_LABEL(vmm_copy_to_user(process->address_space, context->buf, buffer, len), copy_to_user_failed);
        memcpy(context->buf, context->buf + len, buf_len(context->buf) - len);
        buf_resize(context->buf, buf_len(context->buf) - len);
    });

    if(read_size != NULL) CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &len, read_size, sizeof(size_t)));

cleanup:
    mm_free(&kernel_memory_manager, kbuffer);
    return err;
}

static error_t handle_poll(process_t* process, thread_t* thread, resource_t resource) {
    error_t err = NO_ERROR;
    char* kbuffer = NULL;    
    resource_context_t* context = NULL;

    // get the context
    CHECK_AND_RETHROW(get_context(process, resource, &context));

    if(context->type == STDOUT) {
        CHECK_FAIL_ERROR(ERROR_FINISHED);
    }else if(context->type == STDIN) {
        CHECK_ERROR(buf_len(context->buf) > 0, ERROR_FINISHED);
    }
cleanup:
    mm_free(&kernel_memory_manager, kbuffer);
    return err;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t term_provider_init() {
    error_t err = NO_ERROR;
    process_t* process = process_create(handle_ps2, true);
    process->threads[0]->cpu_state.rsp = mm_allocate(&kernel_memory_manager, KB(1));
    process->threads[0]->cpu_state.rbp = mm_allocate(&kernel_memory_manager, KB(1));

    // clear mapping
    memset(scs_mapping, 0, sizeof(scs_mapping));
    memset(shift_mapping, 0, sizeof(scs_mapping));

    // normal key mapping (scs 1)

    scs_mapping[0x0b] = '0';
    scs_mapping[0x02] = '1';
    scs_mapping[0x03] = '2';
    scs_mapping[0x04] = '3';
    scs_mapping[0x05] = '4';
    scs_mapping[0x06] = '5';
    scs_mapping[0x07] = '6';
    scs_mapping[0x08] = '7';
    scs_mapping[0x09] = '8';
    scs_mapping[0x0a] = '9';

    scs_mapping[0x29] = '`';
    scs_mapping[0xC] = '-';
    scs_mapping[0xD] = '=';
    scs_mapping[0x1A] = '[';
    scs_mapping[0x1B] = ']';
    scs_mapping[0x27] = ';';
    scs_mapping[0x28] = '\'';
    scs_mapping[0x2B] = '\\';
    scs_mapping[0x2B] = '\\';
    scs_mapping[0x33] = ',';
    scs_mapping[0x34] = '.';
    scs_mapping[0x35] = '/';

    scs_mapping[0x1e] = 'a';
    scs_mapping[0x30] = 'b';
    scs_mapping[0x2e] = 'c';
    scs_mapping[0x20] = 'd';
    scs_mapping[0x12] = 'e';
    scs_mapping[0x21] = 'f';
    scs_mapping[0x22] = 'g';
    scs_mapping[0x23] = 'h';
    scs_mapping[0x17] = 'i';
    scs_mapping[0x24] = 'j';
    scs_mapping[0x25] = 'k';
    scs_mapping[0x26] = 'l';
    scs_mapping[0x32] = 'm';
    scs_mapping[0x31] = 'n';
    scs_mapping[0x18] = 'o';
    scs_mapping[0x19] = 'p';
    scs_mapping[0x10] = 'q';
    scs_mapping[0x13] = 'r';
    scs_mapping[0x1f] = 's';
    scs_mapping[0x14] = 't';
    scs_mapping[0x16] = 'u';
    scs_mapping[0x2f] = 'v';
    scs_mapping[0x11] = 'w';
    scs_mapping[0x2d] = 'x';
    scs_mapping[0x15] = 'y';
    scs_mapping[0x2c] = 'z';

    scs_mapping[0x39] = ' ';

    scs_mapping[0x2A] = KEYS_LEFT_SHIFT;
    scs_mapping[0x36] = KEYS_RIGHT_SHIFT;

    // key mapping while pressed SHIFT (scs 1)

    shift_mapping['0'] = ')';
    shift_mapping['1'] = '!';
    shift_mapping['2'] = '@';
    shift_mapping['3'] = '#';
    shift_mapping['4'] = '$';
    shift_mapping['5'] = '%';
    shift_mapping['6'] = '^';
    shift_mapping['7'] = '&';
    shift_mapping['8'] = '*';
    shift_mapping['9'] = '(';

    shift_mapping['`'] = '~';
    shift_mapping['-'] = '_';
    shift_mapping['='] = '+';
    shift_mapping['['] = '{';
    shift_mapping[']'] = '}';
    shift_mapping[';'] = ':';
    shift_mapping['\''] = '"';
    shift_mapping['\\'] = '|';
    shift_mapping[','] = '<';
    shift_mapping['.'] = '>';
    shift_mapping['/'] = '?';

    shift_mapping['a'] = 'A';
    shift_mapping['b'] = 'B';
    shift_mapping['c'] = 'C';
    shift_mapping['d'] = 'D';
    shift_mapping['e'] = 'E';
    shift_mapping['f'] = 'F';
    shift_mapping['g'] = 'G';
    shift_mapping['h'] = 'H';
    shift_mapping['i'] = 'I';
    shift_mapping['j'] = 'J';
    shift_mapping['k'] = 'K';
    shift_mapping['l'] = 'L';
    shift_mapping['m'] = 'M';
    shift_mapping['n'] = 'N';
    shift_mapping['o'] = 'O';
    shift_mapping['p'] = 'P';
    shift_mapping['q'] = 'Q';
    shift_mapping['r'] = 'R';
    shift_mapping['s'] = 'S';
    shift_mapping['t'] = 'T';
    shift_mapping['u'] = 'U';
    shift_mapping['v'] = 'V';
    shift_mapping['w'] = 'W';
    shift_mapping['x'] = 'X';
    shift_mapping['y'] = 'Y';
    shift_mapping['z'] = 'Z';

    // even if shift is pressed we want to continue being able to use space
    shift_mapping[' '] = ' ';

    stdio_provider.scheme = "stdio";
    stdio_provider.pid = process->pid;
    stdio_provider.open = handle_open;
    stdio_provider.close = handle_close;
    stdio_provider.write = handle_write;
    stdio_provider.read = handle_read;
    stdio_provider.poll = handle_poll;

    CHECK_AND_RETHROW(resource_manager_register_provider(&stdio_provider));

cleanup:
    return err;
}
