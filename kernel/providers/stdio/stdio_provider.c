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

static resource_context_t** contexts = NULL;

static void add_context(resource_context_t* context) {
    resource_context_t** found = NULL;

    CRITICAL_SECTION({
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
    });
}

static error_t get_context(process_t* process, resource_t res, resource_context_t** context) {
    error_t err = NO_ERROR;
    resource_context_t* found = NULL;

    CRITICAL_SECTION({
         for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
             if(*it != NULL && (*it)->resource == res && (*it)->thread->parent == process) {
                 found = *it;
                 break;
             }
         }
    });

    CHECK_ERROR(found != NULL, ERROR_INVALID_RESOURCE);
    *context = found;

cleanup:
    return err;
}

static error_t remove_context(process_t* process, resource_t res) {
    error_t err = NO_ERROR;
    bool found = false;

    CRITICAL_SECTION({
         for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
             if(*it != NULL && (*it)->resource == res && (*it)->thread->parent == process) {
                 kfree(*it);
                 *it = NULL;
                 break;
             }
         }
     });

    CHECK_ERROR(found, ERROR_INVALID_RESOURCE);

cleanup:
    return err;
}

// keycode mapping
#define KEY_RELEASE 0x80u
static uint8_t scs_mapping[0x80] = {
        // 0x0
        NULL,
        NULL,   // KEYS_ESC
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        '0',
        '-',
        '=',
        NULL,   // KEYS_BACKSPACE
        '\t',
        'q',
        'w',
        'e',
        'r',
        't',
        'y',
        'u',
        'i',
        'o',
        'p',
        '[',
        ']',
        '\n',
        NULL,   // KEYS_LEFT_CONTROL
        'a',
        's',
        'd',
        'f',
        'g',
        'h',
        'j',
        'k',
        'l',
        ';',
        '\'',
        '`',
        NULL,   // KEYS_LEFT_SHIFT
        '\\',
        'z',
        'x',
        'c',
        'v',
        'b',
        'n',
        'm',
        ',',
        '.',
        '/',
        NULL,   // KEYS_RIGHT_SHIFT
        '*',    // keypad
        NULL,   // KEYS_LEFT_ALT
        ' ',
        NULL,   // KEYS_CAPS_LOCK
        NULL,   // KEYS_F1
        NULL,   // KEYS_F2
        NULL,   // KEYS_F3
        NULL,   // KEYS_F4
        NULL,   // KEYS_F5
        NULL,   // KEYS_F6
        NULL,   // KEYS_F7
        NULL,   // KEYS_F8
        NULL,   // KEYS_F9
        NULL,   // KEYS_F10
        NULL,   // KEYS_NUM_LOCK
        NULL,   // KEYS_SCROLL_LOCK
        '7',    // keypad
        '8',    // keypad
        '9',    // keypad
        '-',    // keypad
        '4',    // keypad
        '5',    // keypad
        '6',    // keypad
        '+',    // keypad
        '1',    // keypad
        '2',    // keypad
        '3',    // keypad
        '0',    // keypad
        '.',    // keypad
        NULL,
        NULL,
        NULL,
        NULL,   // KEYS_F11
        NULL,   // KEYS_F12
};

static uint8_t shift_mapping[0x80] = {
    ['q'] = 'Q',
    ['w'] = 'W',
    ['e'] = 'E',
    ['r'] = 'R',
    ['t'] = 'T',
    ['y'] = 'Y',
    ['u'] = 'U',
    ['i'] = 'I',
    ['o'] = 'O',
    ['p'] = 'P',
    ['['] = '{',
    [']'] = '}',
    ['\\'] = '|',
    ['a'] = 'A',
    ['s'] = 'S',
    ['d'] = 'D',
    ['f'] = 'F',
    ['g'] = 'G',
    ['h'] = 'H',
    ['j'] = 'J',
    ['k'] = 'K',
    ['l'] = 'L',
    [';'] = ':',
    ['\''] = '"',
    ['z'] = 'Z',
    ['x'] = 'X',
    ['c'] = 'C',
    ['v'] = 'V',
    ['b'] = 'B',
    ['n'] = 'N',
    ['m'] = 'M',
    [','] = ',',
    ['.'] = '.',
    ['/'] = '/',
    ['`'] = '~',
    ['1'] = '!',
    ['2'] = '@',
    ['3'] = '#',
    ['4'] = '$',
    ['5'] = '%',
    ['6'] = '^',
    ['7'] = '&',
    ['8'] = '*',
    ['9'] = '(',
    ['0'] = ')',
    ['-'] = '_',
    ['='] = '+',
};

static bool key_states[1024];

static uint8_t translate_char(uint8_t c) {
    if(c == 0xE0) {
        read(ps2_keyboard, &c, 0, NULL);
        // TODO:
        c = NULL;
    }else {
        if((c > KEY_RELEASE) != 0) {
            // key release
            c -= KEY_RELEASE;
            if(scs_mapping[c] != NULL) {
                c = scs_mapping[c];
                key_states[c] = false;
            }
            return NULL;
        }else {
            // key press
            if(scs_mapping[c] != NULL) {
                c = scs_mapping[c];
                // TODO: Transform according to CAPS or SHIFT
//                if(key_states[KEYS_LEFT_SHIFT] || key_states[KEYS_RIGHT_SHIFT]) {
//                    c = shift_mapping[c];
//                }
                key_states[c] = true;
                return c;
            }
        }
    }
    return NULL;
}

static void dispatch(uint8_t c) {
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
    resource_descriptor_t descriptor = {0};
    descriptor.scheme = "ps2";
    descriptor.domain = "keyboard";
    if(open(&descriptor, &ps2_keyboard)) {
        while(true) {
            wait(ps2_keyboard);
            while(poll(ps2_keyboard)) {
                uint8_t c = 0;
                read(ps2_keyboard, &c, 1, NULL);
                c = translate_char(c);
                if(c != NULL) {
                    dispatch(c);
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

    resource_context_t* context = kalloc(sizeof(resource_context_t));
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

    add_context(context);

cleanup:
    if(IS_ERROR(err)) {
        handle_close(process, thread, created_resource);
    }
    if(desc != NULL) {
        delete_resource_descriptor(desc);
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

    kbuffer = kalloc(len + 1);
    kbuffer[len] = 0;
    CHECK_AND_RETHROW(vmm_copy_to_kernel(process->address_space, buffer, kbuffer, len));

    term_write(kbuffer);

    // Does nothing
cleanup:
    kfree(kbuffer);
    return err;
}

static error_t handle_read(process_t* process, thread_t* thread, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;
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
    kfree(kbuffer);
    return err;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t stdio_provider_init() {
    error_t err = NO_ERROR;
    process_t* process = process_create(handle_ps2, true);
    uint64_t stack = (uint64_t) kalloc(KB(1));
    process->threads[0]->cpu_state.rsp = stack + KB(1);
    process->threads[0]->cpu_state.rbp = stack + KB(1);

    stdio_provider.scheme = "stdio";
    stdio_provider.pid = process->pid;

    // TODO: Need a better way to sign that we support wait, since for example on stdout we don't support it
    stdio_provider.wait_support = true;
    stdio_provider.open = handle_open;
    stdio_provider.close = handle_close;
    stdio_provider.write = handle_write;
    stdio_provider.read = handle_read;
    stdio_provider.poll = handle_poll;

    CHECK_AND_RETHROW(resource_manager_register_provider(&stdio_provider));

cleanup:
    return err;
}
