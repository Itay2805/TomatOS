#ifndef TOMATKERNEL_STDINT_H
#define TOMATKERNEL_STDINT_H

typedef __INT8_TYPE__       int8_t;
typedef __INT16_TYPE__      int16_t;
typedef __INT32_TYPE__      int32_t;
typedef __INT64_TYPE__      int64_t;

typedef __UINT8_TYPE__      uint8_t;
typedef __UINT16_TYPE__     uint16_t;
typedef __UINT32_TYPE__     uint32_t;
typedef __UINT64_TYPE__     uint64_t;

#define INT8_MIN            ((int8_t)-128)
#define INT8_MAX            ((int8_t)127)
#define INT16_MIN           ((int16_t)-32768)
#define INT16_MAX           ((int16_t)32767)
#define INT32_MIN           ((int32_t)-2147483648)
#define INT32_MAX           ((int32_t)2147483647)
#define INT64_MIN           ((int64_t)-9223372036854775808)
#define INT64_MAX           ((int64_t)9223372036854775807ll)

#define UINT8_MAX           ((uint8_t)255u)
#define UINT16_MAX          ((uint16_t)65535u)
#define UINT32_MAX          ((uint32_t)4294967295u)
#define UINT64_MAX          ((uint64_t)18446744073709551615ull)

#define UINT64_C(x)         (x##ull)

#endif //TOMATKERNEL_STDINT_H
