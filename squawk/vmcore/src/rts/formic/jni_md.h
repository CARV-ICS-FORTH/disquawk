/*
 * Copyright 1996-2008 Sun Microsystems, Inc. All Rights Reserved.
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
 * Please contact Sun Microsystems, Inc., 16 Network Circle, Menlo
 * Park, CA 94025 or visit www.sun.com if you need additional
 * information or have any questions.
 */

#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#define JNIEXPORT
#define JNIIMPORT
#define JNICALL

/* Signed */
#ifndef __int8_t_defined
typedef signed char int8_t;
# define __int8_t_defined
#endif
typedef short int int16_t;
typedef int int32_t;
# if __WORDSIZE == 64
typedef long int int64_t;
# else
__extension__
typedef long long int int64_t;
# endif

/* Unsigned.  */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int uint32_t;
# define __uint32_t_defined
#endif

#if __WORDSIZE == 64
typedef unsigned long int uint64_t;
#else
__extension__
typedef unsigned long long int uint64_t;
#endif

#ifndef __int8_t
#define __int8_t  int8_t
#endif
#ifndef __int16_t
#define __int16_t int16_t
#endif
#ifndef __int32_t
#define __int32_t int32_t
#endif
#ifndef __int64_t
#define __int64_t int64_t
#endif
#ifndef __uint8_t
#define __uint8_t  uint8_t
#endif
#ifndef __uint16_t
#define __uint16_t uint16_t
#endif
#ifndef __uint32_t
#define __uint32_t uint32_t
#endif
#ifndef __uint64_t
#define __uint64_t uint64_t
#endif

typedef long jint;
typedef signed char jbyte;

#define jlong int64_t
/* #define jlong int32_t */

/* This is done in platform.h.spp */
/* #ifdef ujlong */
/* #undef ujlong */
/* #endif  /\* ujlong *\/ */
/* #define ujlong uint64_t */

#define JNI_TYPES_ALREADY_DEFINED_IN_JNI_MD_H

typedef unsigned char     jboolean;
typedef unsigned short    jchar;
typedef short     jshort;
typedef float     jfloat;
typedef ujlong    jdouble; /* HACK: Define doubles as long*/

typedef jint    jsize;

#define FILE void*

#endif /* !_JAVASOFT_JNI_MD_H_ */
