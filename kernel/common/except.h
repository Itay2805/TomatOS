#ifndef TOMATKERNEL_EXCEPT_H
#define TOMATKERNEL_EXCEPT_H

#include <graphics/term.h>
#include <interrupts/interrupts.h>
#include "stdint.h"

#ifndef EXCEPT_MAX_FRAMES
    #define EXCEPT_MAX_FRAMES 128u
#endif

#ifndef __FILENAME__
    #warning __FILENAME__ was not defined, defaulting to unknown
    #define __FILENAME__ "unknown file"
#endif

#if EXCEPT_MAX_FRAMES >= 0xffffu || EXCEPT_MAX_FRAMES < 0u
    #error EXCEPT_MAX_FRAMES must be a positive 16bit value
#endif

typedef struct error_frame {
    const char* file;
    const char* function;
    unsigned int line;
} error_frame_t;

#define ERROR_PUSH_FRAME(expression) \
    do { \
        if(((err >> 16u) & 0xFFFFu) < EXCEPT_MAX_FRAMES) { \
            error_frames[(err >> 16u) & 0xFFFFu] = (error_frame_t){ __FILENAME__, __FUNCTION__, __LINE__ }; \
            err = ((((err >> 16u) & 0xFFFFu) + 1u) << 16u) | (err & 0xFFFFu); \
        } \
    } while(0)

typedef uint32_t error_t;

// TODO: Make per-CPU
extern error_frame_t error_frames[EXCEPT_MAX_FRAMES];

#define NO_ERROR                ((error_t)0u)
#define ERROR_CHECK_FAILED      ((error_t)1u)
#define ERROR_INVALID_ARGUMENT  ((error_t)2u)
#define ERROR_INVALID_POINTER   ((error_t)3u)
#define ERROR_ALREADY_FREED     ((error_t)4u)
#define ERROR_OUT_OF_MEMORY     ((error_t)5u)
#define ERROR_INVALID_SCHEME    ((error_t)6u)
#define ERROR_NOT_FOUND         ((error_t)7u)
#define ERROR_INVALID_SYSCALL   ((error_t)8u)

#define IS_ERROR(err) (err & 0xFFFFu)

#define CHECK_ERROR(condition, error) \
    do { \
        if(!(condition)) { \
            err = (error); \
            ERROR_PUSH_FRAME(condition); \
            goto cleanup; \
        } \
    } while(0)

#define CHECK(condition) CHECK_ERROR(condition, ERROR_CHECK_FAILED)

#define CHECK_FAIL_ERROR(error) CHECK_ERROR(0, error)

#define CHECK_FAIL() CHECK_FAIL_ERROR(ERROR_CHECK_FAILED)

#define CHECK_AND_RETHROW_LABEL(error, label)  \
    do { \
        err = (error); \
        if(IS_ERROR(err) != NO_ERROR) { \
            ERROR_PUSH_FRAME(error); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW(error) CHECK_AND_RETHROW_LABEL(error, cleanup)

extern const char* except_strings[];

#define KERNEL_STACK_TRACE() \
    do { \
        vmm_set(kernel_address_space); \
        term_set_background_color(COLOR_RED); \
        term_set_text_color(COLOR_WHITE); \
        term_print("\n\n[%s] Error %s (%d):\n", __FUNCTION__, except_strings[IS_ERROR(err)], IS_ERROR(err)); \
        term_set_background_color(COLOR_BLACK); \
        term_set_text_color(COLOR_WHITE); \
        for (size_t i = ((err) >> 16u) - 1; i >= 1; i--) { \
            term_print("[%s] \trethrown at '%s' (%s:%d)\n", __FUNCTION__, error_frames[i].function, error_frames[i].file, error_frames[i].line); \
        } \
        term_print("[%s] \tthrown at '%s' (%s:%d)\n", __FUNCTION__, error_frames[0].function, error_frames[0].file, error_frames[0].line); \
    }while(0)

#define KERNEL_PANIC() \
    do { \
        cli(); \
        KERNEL_STACK_TRACE(); \
        while (true) hlt(); \
    }while(0)

#endif