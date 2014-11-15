/**** Created by Squawk builder from "vmcore/src/vm/platform.h.spp.preprocessed"
 * ****/ /*
 * Copyright 2004-2010 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2011 Oracle Corporation. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Oracle Corporation, 500 Oracle Parkway, Redwood
 * Shores, CA 94065 or visit www.oracle.com if you need additional
 * information or have any questions.
 */

#ifndef _VMCORE_VM_PLATFORM_H_
#define _VMCORE_VM_PLATFORM_H_

#ifndef FORMIC
#  include <stddef.h>
#  include <stdio.h>
#  include <stdlib.h>
#endif /* FORMIC */

#if defined(__i386__) || defined(__i386) || defined(_M_IX86) || \
    defined(_M_X64) || defined(_M_AMD64)
#  define PROCESSOR_ARCHITECTURE_X86 1
#  define ARM_FPA                    0
#elif defined(__ppc__) || defined(__ppc)
#  define PROCESSOR_ARCHITECTURE_X86 0
#  define ARM_FPA                    0
#elif defined(__sparc__) || defined(__sparc)
#  define PROCESSOR_ARCHITECTURE_X86 0
#  define ARM_FPA                    0
#elif defined(__arm__)
#  define PROCESSOR_ARCHITECTURE_X86 0
#  ifndef ARM_FPA
#    ifdef __VFP_FP__
#      define ARM_FPA                0
#    else /*__VFP_FP__*/
#      define ARM_FPA                1
#    endif /*__VFP_FP__*/
#  endif /* ifndef ARM_FPA */
#elif defined(__MICROBLAZE__)
#  define PROCESSOR_ARCHITECTURE_X86 0
#  define ARM_FPA                    0
#else              /* if defined(__i386__) || defined(__i386) ||
                    * defined(_M_IX86) ||
                    * defined(_M_X64) || defined(_M_AMD64) */
#  warning "unsupported processor"
#endif  /* if defined(__i386__) || defined(__i386) || defined(_M_IX86) ||
         * defined(_M_X64) || defined(_M_AMD64) */

#ifdef __APPLE__
#  include <CoreFoundation/CoreFoundation.h>
#  include <pthread.h>
#  ifdef KERNEL_SQUAWK
#    include <dlfcn.h>
#  endif /* KERNEL_SQUAWK */
#  include <sys/resource.h>
#  include <sys/time.h>
/*
 * *#else
 *#    include <malloc.h>
 */
#endif /* __APPLE__ */

#define true                         1
#define false                        0
#define boolean                      int
#define null                         ((void*)0)
#define byte                         signed char
#define ujlong                       unsigned jlong

#ifndef FORMIC
#  include <string.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <errno.h>
#  include <string.h>
#  include <signal.h>
#  include <math.h>
#endif /* FORMIC */

#ifdef _MSC_VER
#  include <io.h>
#  undef  ujlong
#  define ujlong                     unsigned __int64
#  define JLONG_CONSTANT(c)          c ## i64
#  define WIN32_LEAN_AND_MEAN
#  ifdef MAXINLINE
#    define INLINE                   __forceinline
#  else /* ifdef MAXINLINE */
#    define INLINE                   __inline
#  endif /* ifdef MAXINLINE */
#  include <windows.h>
#  define pathSeparatorChar          ';'
#  define fileSeparatorChar          '\\'
#  define FORCE_USED
#else /* ifdef _MSC_VER */
#  define JLONG_CONSTANT(c)          c ## LL
#  define pathSeparatorChar          ':'
#  define fileSeparatorChar          '/'
#  define FORCE_USED                 __attribute__((used))
#endif /* _MSC_VER */

#ifndef _MSC_VER
#  ifndef O_BINARY
#    define O_BINARY                 0 /* for compatibility with open() and
	                                    * close() on Windows */
#  endif /* O_BINARY */
#  ifndef FLASH_MEMORY
#    include <sys/mman.h>
#  endif /* FLASH_MEMORY */
#  if defined __GNUC__ && !defined SQUAWK
#    include <unistd.h>
#    undef  ujlong
#    ifdef sun
#      define ujlong                 uint64_t
#    else /* sun */
#      define ujlong                 u_int64_t
#    endif /* sun */
#  elif defined SQUAWK
#    undef  ujlong
#    define ujlong                   uint64_t
#  else /* assume CC */
#  endif /* __GNUC__ */
#  ifdef KERNEL_SQUAWK
#    ifndef FLASH_MEMORY
#      include <pthread.h>
#    endif /* FLASH_MEMORY */
#  endif /* KERNEL_SQUAWK */
#endif /* _MSC_VER */

#ifdef FORMIC
#  ifdef MAXINLINE
#    define INLINE                   static __attribute__((always_inline))
#  else /* ifdef MAXINLINE */
#    define INLINE                   static inline
#  endif /* ifdef MAXINLINE */
#endif /* __MICROBLAZE__ */

#ifndef INLINE
#  define INLINE                     static inline
#endif /* ifndef INLINE */

#ifdef __GNUC__
#  define NOINLINE                   __attribute__ ((noinline))
#  define NORETURN                   __attribute__ ((noreturn))
#else /* ifdef __GNUC__ */
#  ifdef _MSC_VER
#    define NOINLINE                 __declspec(noinline)
#    define NORETURN                 __declspec(noreturn)
#  else /* ifdef _MSC_VER */
#    define NOINLINE
#    define NORETURN
#  endif /* ifdef _MSC_VER */
#endif /* */

#define ENABLE_FLOATS                1

/* It actualy never seems to pay off to inline this FP code.
 *  But the switch is here to play with.
 */
#define INLINE_JAVA_FP               0

#ifndef INLINE_JAVA_FP
#  if ARM_FPA
#    define INLINE_JAVA_FP           0
#  else /* if ARM_FPA */
#    define INLINE_JAVA_FP           1
#  endif /* if ARM_FPA */
#endif /* ifndef INLINE_JAVA_FP */

#ifdef KERNEL_SQUAWK
#  ifndef MAXSIG
#    define MAXSIG                   32
#  endif /* ifndef MAXSIG */
#endif /* KERNEL_SQUAWK */

/**
 * These two conditional compilation macros are also used as values in certain
 * parts and
 * as such must be given a value if they are not defined. This also means that
 * they must
 * used with the '#if' as opposed to '#ifdef' preprocessor directive when
 * surrounding
 * conditional code.
 */
#if defined(ASSUME) && ASSUME != 0
#  undef ASSUME
#  define ASSUME                     true
#else /* if defined(ASSUME) && ASSUME != 0 */
#  define ASSUME                     false
#endif /* if defined(ASSUME) && ASSUME != 0 */

#if defined(SQUAWK_64) && SQUAWK_64 != 0
#  undef SQUAWK_64
#  define SQUAWK_64                  true
#else /* if defined(SQUAWK_64) && SQUAWK_64 != 0 */
#  define SQUAWK_64                  false
#endif /* if defined(SQUAWK_64) && SQUAWK_64 != 0 */

/**
 * Set all PLATFORM_TYPES based on the one defined.
 */
#if defined(PLATFORM_TYPE_DELEGATING)
#  if PLATFORM_TYPE_DELEGATING == 0
#    error PLATFORM_TYPE_DELEGATING must be 1 or undefined
#  endif /* if PLATFORM_TYPE_DELEGATING == 0 */
#  define PLATFORM_TYPE_NATIVE       0
#  define PLATFORM_TYPE_SOCKET       0
#  define PLATFORM_TYPE_BARE_METAL   0
#elif defined(PLATFORM_TYPE_BARE_METAL)
#  if PLATFORM_TYPE_BARE_METAL == 0
#    error PLATFORM_TYPE_BARE_METAL must be 1 or undefined
#  endif /* if PLATFORM_TYPE_BARE_METAL == 0 */
#  define PLATFORM_TYPE_NATIVE       0
#  define PLATFORM_TYPE_SOCKET       0
#  define PLATFORM_TYPE_DELEGATING   0
#elif defined(PLATFORM_TYPE_NATIVE)
#  if PLATFORM_TYPE_NATIVE == 0
#    error PLATFORM_TYPE_NATIVE must be 1 or undefined
#  endif /* if PLATFORM_TYPE_NATIVE == 0 */
#  define PLATFORM_TYPE_DELEGATING   0
#  define PLATFORM_TYPE_SOCKET       0
#  define PLATFORM_TYPE_BARE_METAL   0
#elif defined(PLATFORM_TYPE_SOCKET)
#  if PLATFORM_TYPE_SOCKET == 0
#    error PLATFORM_TYPE_SOCKET must be 1 or undefined
#  endif /* if PLATFORM_TYPE_SOCKET == 0 */
#  define PLATFORM_TYPE_NATIVE       0
#  define PLATFORM_TYPE_DELEGATING   0
#  define PLATFORM_TYPE_BARE_METAL   0
#else /* if defined(PLATFORM_TYPE_DELEGATING) */
#  error "unspecified platform type"
#endif /* if defined(PLATFORM_TYPE_DELEGATING) */

#if PLATFORM_TYPE_DELEGATING
#  include <setjmp.h>
#endif /* if PLATFORM_TYPE_DELEGATING */

#ifndef max
#  define max(a, b)                  ((a) < (b) ? (b) : (a))
#  define min(a, b)                  ((a) > (b) ? (b) : (a))
#  define abs(x)                     ((x) >= 0 ? (x) : -(x))
#endif /* ifndef max */

#endif /* _VMCORE_VM_PLATFORM_H_ */
