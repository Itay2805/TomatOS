#ifndef TOMATKERNEL_STDINT_H
#define TOMATKERNEL_STDINT_H

typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

#define INT8_MIN            ((int8_t)0)
#define INT8_MAX            ((int8_t)0)
#define INT16_MIN           ((int16_t)0)
#define INT16_MAX           ((int16_t)0)
#define INT32_MIN           ((int32_t)0)
#define INT32_MAX           ((int32_t)0)
#define INT64_MIN           ((int64_t)0)
#define INT64_MAX           ((int64_t)0)

#define UINT8_MAX           ((uint8_t)0)
#define UINT16_MAX          ((uint16_t)0)
#define UINT32_MAX          ((uint32_t)0)
#define UINT64_MAX          ((uint64_t)0)

#endif //TOMATKERNEL_STDINT_H
