#ifndef TOMATKERNEL_EXCEPT_H
#define TOMATKERNEL_EXCEPT_H

#include <interrupts/interrupts.h>
#include <graphics/term.h>
#include <kernel.h>

#include "global_except.h"
#include "buf.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common stuff
//
// These are common stuff for both global and error handling
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Error struct that contains everything needed for the error
 */
typedef struct error_info {
    error_frame_t* error_frames;
    int code;
} error_info_t;

typedef error_info_t* error_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Error handling
//
// This is for most of the error handling once the memory manager is available for the kernel
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IS_ERROR(err) ((err) == NULL ? 0 : (err)->code)

#define CHECK_ERROR(condition, error) \
    do { \
        if(!(condition)) { \
            if(IS_ERROR(err)) { \
                mm_free(&kernel_memory_manager, err); \
                err = NULL; \
            } \
            err = mm_allocate(&kernel_memory_manager, sizeof(error_info_t)); \
            memset(err, 0, sizeof(error_info_t)); \
            err->code = (error); \
            buf_push(err->error_frames, (error_frame_t){ __FILENAME__, __FUNCTION__, __LINE__ }); \
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
            buf_push(err->error_frames, (error_frame_t){ __FILENAME__, __FUNCTION__, __LINE__ }); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW(error) CHECK_AND_RETHROW_LABEL(error, cleanup)

#define KERNEL_STACK_TRACE(err) \
    do { \
        vmm_set(kernel_address_space); \
        term_set_background_color(COLOR_RED); \
        term_set_text_color(COLOR_WHITE); \
        term_print("\n[%s] Error %s (%d):\n", __FUNCTION__, except_strings[IS_ERROR(err)], IS_ERROR(err)); \
        term_set_background_color(COLOR_BLACK); \
        term_set_text_color(COLOR_WHITE); \
        for (size_t i = (buf_len((err)->error_frames)) - 1; i >= 1; i--) { \
            term_print("[%s] \trethrown at '%s' (%s:%d)\n", __FUNCTION__, (err)->error_frames[i].function, (err)->error_frames[i].file, (err)->error_frames[i].line); \
        } \
        term_print("[%s] \tthrown at '%s' (%s:%d)\n\n", __FUNCTION__, (err)->error_frames[0].function, (err)->error_frames[0].file, (err)->error_frames[0].line); \
    }while(0)

#define KERNEL_PANIC(err) \
    do { \
        cli(); \
        KERNEL_STACK_TRACE(err); \
        while (true) hlt(); \
    }while(0)

#define ERROR_FREE(err) \
    do { \
        buf_free((err)->error_frames); \
        mm_free(&kernel_memory_manager, (err)); \
        (err) = NULL; \
    } while(0)


#endif