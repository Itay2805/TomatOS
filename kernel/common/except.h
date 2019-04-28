#ifndef TOMATKERNEL_EXCEPT_H
#define TOMATKERNEL_EXCEPT_H

#include "stdint.h"

#ifndef EXCEPT_MAX_FRAMES
    #define EXCEPT_MAX_FRAMES 128
#endif

#if EXCEPT_MAX_FRAMES >= 0xffff || EXCEPT_MAX_FRAMES < 0
    #error EXCEPT_MAX_FRAMES must be a positive 16bit value
#endif

typedef struct error_frame {
    const char* file;
    const char* function;
    const char* expression;
    const int line;
} error_frame_t;

#define ERROR_PUSH_FRAME(expression) \
    do { \
        if(((err >> 16) & 0xFFFF) < EXCEPT_MAX_FRAMES) { \
            error_frames[(err >> 16) & 0xFFFF] = (error_frame_t){ __FILE__, __FUNCTION__, #expression, __LINE__ }; \
            err = ((((err >> 16) & 0xFFFF) + 1) << 16) | (err & 0xFFFF); \
        } \
    } while(0)

typedef uint32_t error_t;

// TODO: Make per-CPU
extern error_frame_t error_frames[EXCEPT_MAX_FRAMES];

#define NO_ERROR                ((error_t)0)
#define ERROR_CHECK_FAILED      ((error_t)1)

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

#define CHECK_AND_RETHROW(error)  \
    do { \
        err = (error); \
        if(err != NO_ERROR) { \
            ERROR_PUSH_FRAME(error); \
            goto cleanup; \
        } \
    } while(0)

#define IS_ERROR(err) (err & 0xFFFF)

#endif