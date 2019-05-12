#ifndef TOMATKERNEL_EXCEPT_H
#define TOMATKERNEL_EXCEPT_H

#include <common/logging.h>
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
                kfree(err); \
                err = NULL; \
            } \
            err = kalloc(sizeof(error_info_t)); \
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
        LOG_ERROR("Error %s (%d):", except_strings[IS_ERROR(err)], IS_ERROR(err)); \
        for (size_t i = (buf_len((err)->error_frames)) - 1; i >= 1; i--) { \
            LOG_ERROR("\trethrown at '%s' (%s:%d)", (err)->error_frames[i].function, (err)->error_frames[i].file, (err)->error_frames[i].line); \
        } \
        LOG_ERROR("\tthrown at '%s' (%s:%d)", (err)->error_frames[0].function, (err)->error_frames[0].file, (err)->error_frames[0].line); \
    }while(0)

#define KERNEL_PANIC(err) \
    do { \
        cli(); \
        LOG_CRITICAL("kernel panic :("); \
        KERNEL_STACK_TRACE(err); \
        while (true) hlt(); \
    }while(0)

#define ERROR_FREE(err) \
    do { \
        buf_free((err)->error_frames); \
        kfree((err)); \
        (err) = NULL; \
    } while(0)


#endif