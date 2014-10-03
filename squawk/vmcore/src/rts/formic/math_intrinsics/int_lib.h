/* ===-- int_lib.h - configuration header for compiler-rt  -----------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses. See LICENSE.TXT for details.
 *
 * ===----------------------------------------------------------------------===
 *
 * This file is a configuration header for compiler-rt.
 * This file is not part of the interface of this library.
 *
 * ===----------------------------------------------------------------------===
 */

#ifndef INT_LIB_H
#define INT_LIB_H

#ifdef ARCH_MB
#include "../jni.h"
#endif /* */

//zakkak:
/* This is needed by the compiler-rt to understand that this is a
 * little endian platform */
#define _YUGA_LITTLE_ENDIAN 1
#define _YUGA_BIG_ENDIAN    0

/* Limits for integral types. We have to redefine it because we don't
 * use stdint.h */
#undef INT64_C
#undef UINT32_C
#undef UINT64_C
#define INT64_C(c)  c ## LL
#define UINT32_C(c) c
#define UINT64_C(c) c ## ULL
#undef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#undef INT32_MAX
#define INT32_MAX              (2147483647)
#undef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#undef INT64_MIN
#define INT64_MIN              (-__INT64_C(9223372036854775807)-1)
#undef INT64_MAX
#define INT64_MAX              (__INT64_C(9223372036854775807))
#undef UINT64_MAX
#define UINT64_MAX             (__UINT64_C(18446744073709551615))

/* Assumption: Signed integral is 2's complement. */
/* Assumption: Right shift of signed negative is arithmetic shift. */
/* Assumption: Endianness is little or big (not mixed). */

/* ABI macro definitions */

#if __ARM_EABI__
# define ARM_EABI_FNALIAS(aeabi_name, name)         \
  void __aeabi_##aeabi_name() __attribute__((alias("__" #name)));
# define COMPILER_RT_ABI __attribute__((pcs("aapcs")))
#else
# define ARM_EABI_FNALIAS(aeabi_name, name)
# define COMPILER_RT_ABI
#endif

/* Include the standard compiler builtin headers we use functionality from. */
#include <limits.h>
//#include <stdint.h>
#include <stdbool.h>
#include <float.h>

/* Include the commonly used internal type definitions. */
#include "int_types.h"

/* Include internal utility function declarations. */
#include "int_util.h"

#endif /* INT_LIB_H */
