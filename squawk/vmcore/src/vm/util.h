/*
 * Copyright (C) 2013-2014 FORTH-ICS / CARV
 *                         (Foundation for Research & Technology -- Hellas,
 *                          Institute of Computer Science,
 *                          Computer Architecture & VLSI Systems Laboratory)
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

#ifndef VM_UTIL_H_
#define VM_UTIL_H_

#include <os.h>
#include <jni.h>
#include <rom.h>

/*---------------------------------------------------------------------------*\
 *                               Debugger                                    *
\*---------------------------------------------------------------------------*/

/*
 * Java level debugging is enabled only if the bootstrap image includes
 * the Squawk Debug Agent.
 */
#ifdef com_sun_squawk_debugger_sda_SDA
#define SDA_DEBUGGER true
#else
#define SDA_DEBUGGER false
#endif

#if SDA_DEBUGGER
#define DB_MAX_BPS 100
extern int sda_bp_set_or_stepping; // TRUE if we have any breakpoints set or are in a stepping mode
#define set_sda_bp_set_or_stepping(_isolate_) sda_bp_set_or_stepping = sda_updateBreakpointCache(_isolate_);

/* Null terminated array of instruction pointers for the breakpoints set in the current isolate. */
extern ByteAddress sda_breakpoints[DB_MAX_BPS + 1];

boolean sda_updateBreakpointCache(Address isolate);
boolean sda_isOperandStackEmpty(UWordAddress actual_fp, UWordAddress actual_sp);
void sda_clearStepState(Address thread);
Address getInterpreterInvokedFrame(UWordAddress beginFP, UWordAddress endFP);
void dumpSteppingInfo(Offset ipOffset, Offset fpOffset, Address stepInfo);
#else
#define set_sda_bp_set_or_stepping(_isolate_)
#ifndef MACROIZE
#define sda_bp_set_or_stepping FALSE
#endif  /* #ifndef MACROIZE */
#endif /* SDA_DEBUGGER */

#ifdef __GNUC__
#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif /* __GNUC__ */

/*---------------------------------------------------------------------------*\
 *                               mprotect                                    *
\*---------------------------------------------------------------------------*/

/* defined in os.c:
 *     sysGetPageSize()
 *     sysToggleMemoryProtection()
 */

/*---------------------------------------------------------------------------*\
 *                                  Errors                                   *
\*---------------------------------------------------------------------------*/

NORETURN void fatalInterpreterError0(char *msg, int opcode, int _iparm, ByteAddress _ip, UWordAddress _fp, UWordAddress _sp, char* file, int line);

NORETURN void fatalVMError0(char *msg, boolean cleanup);
NORETURN void fatalVMError(char *msg);

#define shouldNotReachHere() { fprintf(stderr, "shouldNotReachHere -- %s:%d\n", __FILE__, __LINE__); fatalVMError(""); }

#if defined(ASSUME) && ASSUME != 0
#define assume(x) if (!(x))  { fprintf(stderr, "Assertion failed: \"%s\", at %s:%d\n", #x, __FILE__, __LINE__); fatalVMError(""); }
#define assumeInterp(x) if (!(x))  { fprintf(stderr, "Assertion failed: \"%s\", at %s:%d\n", #x, __FILE__, __LINE__); fatalInterpreterError(""); }
#else
#define assume(x) /**/
#define assumeInterp(x) /**/
#endif /* ASSUME */

/**
 * version of assume that is always enabled.
 */
#define assumeAlways(x) if (!(x))  { fprintf(stderr, "Assertion failed: \"%s\", at %s:%d\n", #x, __FILE__, __LINE__); fatalVMError(""); }

/**
 * Exits the VM with an error message including various interpreter state.
 */
#ifdef MACROIZE
#define fatalInterpreterError(_msg) \
    fatalInterpreterError0(_msg, opcode, iparm_g, ip_g, fp_g, sp_g, __FILE__, __LINE__)
#else  /* #ifdef MACROIZE */
#define fatalInterpreterError(_msg) \
  fatalInterpreterError0(_msg,      0, iparm_g, ip_g, fp_g, sp_g, __FILE__, __LINE__)
#endif  /* #ifdef MACROIZE */

void printJavaStrSafely(Address value, char* valueName);

void printRange(char* label, void* start, void* end);

/*---------------------------------------------------------------------------*\
 *                               alignment                                   *
\*---------------------------------------------------------------------------*/

/**
 * Determines if a given value is a power of 2.
 *
 * @param value  the value to test
 * @return true if 'value' is a power of 2
 */
INLINE boolean isPowerOf2(UWord value) {
	return ((value) & (value - 1)) == 0;
}

/**
 * Rounds up a value based on a given alignment.
 *
 * @param value      the value to be rounded up
 * @param alignment  <code>value</value> is rounded up to be a multiple of this value
 * @return the aligned value
 */
INLINE UWord roundUp(UWord value, UWord alignment) {
	assume(isPowerOf2(alignment));
	return (value + (alignment - 1)) & ~(alignment - 1);
}

/**
 * Rounds up a value to the next word boundry.
 *
 * @param value  the value to round up
 * @return the result
 */
INLINE UWord roundUpToWord(UWord value) {
	return (value + (HDR_BYTES_PER_WORD - 1)) & ~(HDR_BYTES_PER_WORD - 1);
}

/**
 * Rounds down a value based on a given alignment.
 *
 * @param value      the value to be rounded down
 * @param alignment  <code>value</value> is rounded down to be a multiple of this value
 * @return the aligned value
 */
INLINE UWord roundDown(UWord value, UWord alignment) {
	assume(isPowerOf2(alignment));
	return value & ~(alignment - 1);
}

/**
 * Rounds down a value to the next word boundry.
 *
 * @param value  the value to round down
 * @return the result
 */
INLINE UWord roundDownToWord(UWord value) {
	return value & ~(HDR_BYTES_PER_WORD - 1);
}

/**
 * Determines if a given value is word aligned.
 *
 * @param value  the value to test
 * @return true if <code>value</code> is word aligned
 */
INLINE boolean isWordAligned(UWord value) {
	return value == roundDownToWord(value);
}


/**
 * Determines if a given value is aligned with respect to a given alignment.
 *
 * @param value      the value to test
 * @param alignment  the alignment
 * @return true if <code>value</code> is a mutliple of <code>alignment</code>
 */
INLINE boolean isAligned(UWord value, UWord alignment) {
	return value == roundDown(value, alignment);
}

/**
 * Converts two 32 bit ints into a Java long.
 *
 * @param high the high word
 * @param low  the low word
 * @return the resulting Java long
 */
INLINE jlong makeLong(int high, int low) {
	return (((jlong) high) << 32) | ((unsigned int)low);
}

/**
 * Return the high word of a Java long
 *
 * @param value 64-bit value
 * @return the high 32-bits of value
 */
INLINE int getHi(jlong value) {
	return (int) (value >> 32);
}

/**
 * Return the low word of a Java long
 *
 * @param value 64-bit value
 * @return the low 32-bits of value
 */
INLINE int getLo(jlong value) {
	return (int) (value & 0xFFFFFFFFL);
}

/*---------------------------------------------------------------------------*\
 *                            Low level operations                           *
\*---------------------------------------------------------------------------*/

INLINE jlong  slll(jlong a, int b)         { return a<<(b&63);                }
INLINE jlong  sral(jlong a, int b)         { return a>>(b&63);                }
INLINE jlong  srll(jlong a, int b)         { return ((ujlong)a)>>(b&63);      }
INLINE int    sll(int a, int b)            { return a<<(b&31);                }
INLINE int    sra(int a, int b)            { return a>>(b&31);                }
INLINE int    srl(int a, int b)            { return ((unsigned)a)>>(b&31);    }
INLINE int    i2b(int i)                   { return (byte)i;                  }
INLINE int    i2s(int i)                   { return (short)i;                 }
INLINE int    i2c(int i)                   { return (char)i;                  }
INLINE jlong  i2l(int i)                   { return (jlong)i;                 }
INLINE int    l2i(jlong l)                 { return (int)l;                   }

#ifdef FLOATS

#include "fp/global.h"
#ifdef FORMIC
#include "fp/fdlibm.h"
#endif  /* #ifdef FORMIC */

#ifndef C_FP_CONVERSIONS_OK // default to safe unless told otherwise
#define C_FP_CONVERSIONS_OK 0
#endif

#if C_FP_CONVERSIONS_OK
INLINE jlong float2ll(float f)              { return (jlong)f; }
INLINE jlong double2ll(double d)            { return (jlong)d; }
INLINE int double2l(double d)               { return (int)d; }
INLINE int float2l(float f)                 { return (int)f; }
#else
/* can't rely on C conversions. */
#define float2ll float2ll_slow
#define double2ll double2ll_slow
#define double2l double2l_slow
#define float2l float2l_slow

extern jlong float2ll_slow(float f);
extern jlong double2ll_slow(double d);
extern int double2l_slow(double d);
extern int float2l_slow(float f);
#endif

#if INLINE_JAVA_FP
INLINE int    addf(int l, int r)           { return f2ib(ib2f(l) + ib2f(r));           }
INLINE int    subf(int l, int r)           { return f2ib(ib2f(l) - ib2f(r));           }
INLINE int    mulf(int l, int r)           { return f2ib(ib2f(l) * ib2f(r));           }
INLINE int    divf(int l, int r)           { return f2ib(ib2f(l) / ib2f(r));           }
INLINE int    remf(int l, int r)           { return f2ib(JFP_lib_remf(ib2f(l), ib2f(r))); }
INLINE int    negf(int l)                  { return f2ib( - ib2f(l));        }
INLINE jlong  addd(jlong l, jlong r)       { return d2lb(lb2d(l) + lb2d(r));           }
INLINE jlong  subd(jlong l, jlong r)       { return d2lb(lb2d(l) - lb2d(r));           }
INLINE jlong  muld(jlong l, jlong r)       { return d2lb(JFP_lib_muld(lb2d(l), lb2d(r))); }
INLINE jlong  divd(jlong l, jlong r)       { return d2lb(JFP_lib_divd(lb2d(l), lb2d(r))); }
INLINE jlong  remd(jlong l, jlong r)       { return d2lb(JFP_lib_remd(lb2d(l), lb2d(r))); }
INLINE jlong  negd(jlong l)                { return d2lb(- lb2d(l));       }

INLINE int    i2f(int i)                   { return f2ib((float)i);                    }
INLINE jlong  i2d(int i)                   { return d2lb((double)i);                   }
INLINE int    l2f(jlong l)                 { return f2ib((float)l);                    }
INLINE jlong  l2d(jlong l)                 { return d2lb((double)l);                   }
INLINE int    f2i(int f)                   { return (int)float2l(ib2f(f));             }
INLINE jlong  f2l(int f)                   { return (jlong)float2ll(ib2f(f));          }
INLINE jlong  f2d(int f)                   { return d2lb((double)ib2f(f));             }
INLINE int    d2i(jlong l)                 { return (int)double2l(lb2d(l));            }
INLINE jlong  d2l(jlong l)                 { return (jlong)double2ll(lb2d(l));         }
INLINE int    d2f(jlong l)                 { return f2ib((float)lb2d(l));              }

#else /* !INLINE_JAVA_FP */

int    addf(int l, int r);
int    subf(int l, int r);
int    mulf(int l, int r);
int    divf(int l, int r);
int    remf(int l, int r);
int    negf(int l);
jlong  addd(jlong l, jlong r);
jlong  subd(jlong l, jlong r);
jlong  muld(jlong l, jlong r);
jlong  divd(jlong l, jlong r);
jlong  remd(jlong l, jlong r);
jlong  negd(jlong l);

int    i2f(int i);
jlong  i2d(int i);
int    l2f(jlong l);
jlong  l2d(jlong l);
int    f2i(int f);
jlong  f2l(int f);
jlong  f2d(int f);
int    d2i(jlong l);
jlong  d2l(jlong l);
int    d2f(jlong l);
#endif /* ! INLINE_JAVA_FP */

/*---------------------------------------------------------------------------*\
 *                                Math functions                             *
\*---------------------------------------------------------------------------*/

jlong math(int op, jlong rs1_l, jlong rs2_l);
#endif  /* #ifdef FLOATS */

#endif /* VM_UTIL_H_ */
