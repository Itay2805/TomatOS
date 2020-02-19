/*===---- stdint.h - Standard header for sized integer types --------------===*\
  *
  * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
  * See https://llvm.org/LICENSE.txt for license information.
  * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
  *
 \*===----------------------------------------------------------------------===*/

#ifndef __CLANG_STDINT_H
#define __CLANG_STDINT_H

/* C99 7.18.1.1 Exact-width integer types.
 * C99 7.18.1.2 Minimum-width integer types.
 * C99 7.18.1.3 Fastest minimum-width integer types.
 *
 * The standard requires that exact-width type be defined for 8-, 16-, 32-, and
 * 64-bit types if they are implemented. Other exact width types are optional.
 * This implementation defines an exact-width types for every integer width
 * that is represented in the standard integer types.
 *
 * The standard also requires minimum-width types be defined for 8-, 16-, 32-,
 * and 64-bit widths regardless of whether there are corresponding exact-width
 * types.
 *
 * To accommodate targets that are missing types that are exactly 8, 16, 32, or
 * 64 bits wide, this implementation takes an approach of cascading
 * redefinitions, redefining __int_leastN_t to successively smaller exact-width
 * types. It is therefore important that the types are defined in order of
 * descending widths.
 *
 * We currently assume that the minimum-width types and the fastest
 * minimum-width types are the same. This is allowed by the standard, but is
 * suboptimal.
 *
 * In violation of the standard, some targets do not implement a type that is
 * wide enough to represent all of the required widths (8-, 16-, 32-, 64-bit).
 * To accommodate these targets, a required minimum-width type is only
 * defined if there exists an exact-width type of equal or greater width.
 */

typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;

typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;

typedef __INT16_TYPE__ int16_t;
typedef __UINT16_TYPE__ uint16_t;

typedef __INT8_TYPE__ int8_t;
typedef __UINT8_TYPE__ uint8_t;

/* C99 7.18.1.4 Integer types capable of holding object pointers.
 */
#define __stdint_join3(a,b,c) a ## b ## c

typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;

/* C99 7.18.1.5 Greatest-width integer types.
 */
typedef __INTMAX_TYPE__  intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

/* C99 7.18.4 Macros for minimum-width integer constants.
 *
 * The standard requires that integer constant macros be defined for all the
 * minimum-width types defined above. As 8-, 16-, 32-, and 64-bit minimum-width
 * types are required, the corresponding integer constant macros are defined
 * here. This implementation also defines minimum-width types for every other
 * integer width that the target implements, so corresponding macros are
 * defined below, too.
 *
 * These macros are defined using the same successive-shrinking approach as
 * the type definitions above. It is likewise important that macros are defined
 * in order of decending width.
 *
 * Note that C++ should not check __STDC_CONSTANT_MACROS here, contrary to the
 * claims of the C standard (see C++ 18.3.1p2, [cstdint.syn]).
 */

#define __int_c_join(a, b) a ## b
#define __int_c(v, suffix) __int_c_join(v, suffix)
#define __uint_c(v, suffix) __int_c_join(v##U, suffix)

#define INT64_C(v) __int_c(v, __INT64_C_SUFFIX__)
#define UINT64_C(v) __uint_c(v, __INT64_C_SUFFIX__)

#define INT32_C(v) __int_c(v, __INT32_C_SUFFIX__)
#define UINT32_C(v) __uint_c(v, __INT32_C_SUFFIX__)

#define INT16_C(v) __int_c(v, __INT16_C_SUFFIX__)
#define UINT16_C(v) __uint_c(v, __INT16_C_SUFFIX__)

#define INT8_C(v) __int_c(v, __INT8_C_SUFFIX__)
#define UINT8_C(v) __uint_c(v, __INT8_C_SUFFIX__)

/* C99 7.18.2.1 Limits of exact-width integer types.
 * C99 7.18.2.2 Limits of minimum-width integer types.
 * C99 7.18.2.3 Limits of fastest minimum-width integer types.
 *
 * The presence of limit macros are completely optional in C99.  This
 * implementation defines limits for all of the types (exact- and
 * minimum-width) that it defines above, using the limits of the minimum-width
 * type for any types that do not have exact-width representations.
 *
 * As in the type definitions, this section takes an approach of
 * successive-shrinking to determine which limits to use for the standard (8,
 * 16, 32, 64) bit widths when they don't have exact representations. It is
 * therefore important that the definitions be kept in order of decending
 * widths.
 *
 * Note that C++ should not check __STDC_LIMIT_MACROS here, contrary to the
 * claims of the C standard (see C++ 18.3.1p2, [cstdint.syn]).
 */

#define INT64_MAX           INT64_C( 9223372036854775807)
#define INT64_MIN         (-INT64_C( 9223372036854775807)-1)
#define UINT64_MAX         UINT64_C(18446744073709551615)


#define INT32_MAX           INT32_C(2147483647)
#define INT32_MIN         (-INT32_C(2147483647)-1)
#define UINT32_MAX         UINT32_C(4294967295)

#define INT16_MAX            INT16_C(32767)
#define INT16_MIN          (-INT16_C(32767)-1)
#define UINT16_MAX          UINT16_C(65535)


#define INT8_MAX            INT8_C(127)
#define INT8_MIN          (-INT8_C(127)-1)
#define UINT8_MAX          UINT8_C(255)

/* C99 7.18.2.4 Limits of integer types capable of holding object pointers. */
/* C99 7.18.3 Limits of other integer types. */

#define  INTPTR_MIN  (-__INTPTR_MAX__-1)
#define  INTPTR_MAX    __INTPTR_MAX__
#define UINTPTR_MAX   __UINTPTR_MAX__
#define PTRDIFF_MIN (-__PTRDIFF_MAX__-1)
#define PTRDIFF_MAX   __PTRDIFF_MAX__
#define    SIZE_MAX      __SIZE_MAX__

/* C99 7.18.2.5 Limits of greatest-width integer types. */
#define  INTMAX_MIN (-__INTMAX_MAX__-1)
#define  INTMAX_MAX   __INTMAX_MAX__
#define UINTMAX_MAX  __UINTMAX_MAX__

/* 7.18.4.2 Macros for greatest-width integer constants. */
#define  INTMAX_C(v) __int_c(v,  __INTMAX_C_SUFFIX__)
#define UINTMAX_C(v) __int_c(v, __UINTMAX_C_SUFFIX__)

#endif /* __CLANG_STDINT_H */