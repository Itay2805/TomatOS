//
// Created by Itay on 08/05/2019.
//

#ifndef TOMATKERNEL_GLOBAL_EXCEPT_H
#define TOMATKERNEL_GLOBAL_EXCEPT_H

#ifndef EXCEPT_MAX_FRAMES
#define EXCEPT_MAX_FRAMES 128u
#endif

#ifndef __FILENAME__
    #warning __FILENAME__ was not defined, defaulting to __FILE__
    #define __FILENAME__ __FILE__
#endif

#if EXCEPT_MAX_FRAMES >= 0xffffu || EXCEPT_MAX_FRAMES < 0u
    #error EXCEPT_MAX_FRAMES must be a positive 16bit value
#endif

#include "stdint.h"

/**
 * This represents a single frame in the error stack
 */
typedef struct error_frame {
    const char* file;
    const char* function;
    unsigned int line;
} error_frame_t;

/**
 * Global error, encoded as length:error_code
 */
typedef uint32_t global_error_t;

/**
 * Global error stack, used for the global error handling
 *
 * TODO: Make per CPU
 */
extern error_frame_t global_error_frames[EXCEPT_MAX_FRAMES];

/**
 * Error to string table for the error names
 */
extern const char* except_strings[];

/*
 * Error codes
 */

#define NO_ERROR                0u
#define ERROR_CHECK_FAILED      1u
#define ERROR_INVALID_ARGUMENT  2u
#define ERROR_INVALID_POINTER   3u
#define ERROR_ALREADY_FREED     4u
#define ERROR_OUT_OF_MEMORY     5u
#define ERROR_NOT_FOUND         6u
#define ERROR_INVALID_SYSCALL   7u
#define ERROR_NOT_IMPLEMENTED   8u
#define ERROR_INVALID_DOMAIN    9u
#define ERROR_INVALID_PORT      10u
#define ERROR_INVALID_RESOURCE  11u
#define ERROR_INVALID_PATH      12u
#define ERROR_NOT_READABLE      13u
#define ERROR_NOT_WRITEABLE     14u
#define ERROR_NOT_SEEKABLE      15u
#define ERROR_OUT_OF_RANGE      16u
#define ERROR_FINISHED          17u
#define ERROR_NOT_WAITABLE      18u

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global error handling
//
// This is for when the memory manager is not available, mostly needed for when the memory manager is not available
// (for example, the memory manager error handling)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GLOAL_ERROR_PUSH_FRAME \
    do { \
        int error_frames_length = (err >> 16u) & 0xFFFFu; \
        int error_code = err & 0xFFFFu; \
        if(error_frames_length < EXCEPT_MAX_FRAMES) { \
            global_error_frames[error_frames_length].file = __FILENAME__; \
            global_error_frames[error_frames_length].function = __FUNCTION__; \
            global_error_frames[error_frames_length].line = __LINE__; \
            error_frames_length++; \
            err = ((error_frames_length & 0xFFFFu) << 16u) | (error_code); \
        } \
    } while(0)

#define IS_GLOBAL_ERROR(err) (err & 0xFFFFu)

#define CHECK_GLOBAL_ERROR(condition, error) \
    do { \
        if(!(condition)) { \
            err = (error); \
            GLOAL_ERROR_PUSH_FRAME; \
            goto cleanup; \
        } \
    } while(0)

#define CHECK_GLOBAL(condition) CHECK_GLOBAL_ERROR(condition, ERROR_CHECK_FAILED)

#define CHECK_GLOBAL_FAIL_ERROR(error) CHECK_GLOBAL_ERROR(0, error)

#define CHECK_GLOBAL_FAIL() CHECK_GLOBAL_FAIL_ERROR(ERROR_CHECK_FAILED)

#define CHECK_GLOBAL_AND_RETHROW_LABEL(error, label)  \
    do { \
        err = (global_error_t)(error); \
        if(IS_GLOBAL_ERROR(err) != NO_ERROR) { \
            GLOAL_ERROR_PUSH_FRAME; \
            goto label; \
        } \
    } while(0)

#define CHECK_GLOBAL_AND_RETHROW(error) CHECK_GLOBAL_AND_RETHROW_LABEL(error, cleanup)

#define KERNEL_GLOBAL_STACK_TRACE() \
    do { \
        vmm_set(kernel_address_space); \
        term_set_background_color(COLOR_RED); \
        term_set_text_color(COLOR_WHITE); \
        term_print("\n\n[%s] Error %s (%d):\n", __FUNCTION__, except_strings[IS_GLOBAL_ERROR(err)], IS_GLOBAL_ERROR(err)); \
        term_set_background_color(COLOR_BLACK); \
        term_set_text_color(COLOR_WHITE); \
        for (size_t i = ((err) >> 16u) - 1; i >= 1; i--) { \
            term_print("[%s] \trethrown at '%s' (%s:%d)\n", __FUNCTION__, global_error_frames[i].function, global_error_frames[i].file, global_error_frames[i].line); \
        } \
        term_print("[%s] \tthrown at '%s' (%s:%d)\n", __FUNCTION__, global_error_frames[0].function, global_error_frames[0].file, global_error_frames[0].line); \
    }while(0)

#define KERNEL_GLOBAL_PANIC() \
    do { \
        cli(); \
        KERNEL_GLOBAL_STACK_TRACE(); \
        term_write("\n:(\n"); \
        while (true) hlt(); \
    }while(0)

#endif //TOMATKERNEL_GLOBAL_EXCEPT_H
