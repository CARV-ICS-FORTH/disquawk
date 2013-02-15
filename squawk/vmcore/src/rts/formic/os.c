/*
 * Copyright 2013 FORTH-ICS. All Rights Reserved.
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
 * Please contact Foundation for Research and Technology - Hellas (FORTH)
 * Institute of Computer Science (ICS), N. Plastira 100, Vassilika Vouton,
 * GR-700 13 Heraklion, Crete, Greece or visit www.ics.forth.gr if you need
 * additional information or have any questions.
 */

#define TRUE 1
#define FALSE 0

#define IODOTC "formic_io.c"
#include "jni.h"

/**
 * Allocate a page-aligned chunk of memory of the given size.
 *
 * @param size size in bytes to allocate
 * @return pointer to allocated memory or null.
 */
INLINE void* sysValloc(size_t size) {
    return valloc(size);
}

/**
 * Free chunk of memory allocated by sysValloc
 *
 * @param ptr to to chunk allocated by sysValloc
 */
INLINE void sysVallocFree(void* ptr) {
    free(ptr);
}

#if 0

/**
 * Execute an IO operation for a Squawk isolate.
 */
static void ioExecute(void) {}

/**
 * Return the current time in milliseconds.
 */
volatile jlong sysTimeMillis(void) {}

/**
 * Return the current time in microseconds.
 */
jlong sysTimeMicros() {}

/**
 * Analogous to sysconf() in UNIX. The function allows Squawk to query the
 * current value of a configurable system variable. A system variable may be
 * device specific so it needs to be re-implemented for each MCU Squawk targets.
 *
 * The only parameter that must be implemented is _SC_PAGESIZE which informs the
 * caller of the size of a memory page of the underlying hardware. It is used by
 * Squawk to determine how much memory should be allocated when creating a byte
 * buffer which allocates buffers as a multiple of the memory page size. Both
 * the eSPOT and bSPOT ports of Squawk return a size of 4K. It cannot be
 * determined why 4K was chosen for the bSPOT port since the MCU used by the
 * bSPOT (AT91R40008) does not have an MMU.
 */
long sysconf(int code) {}

/**
 * Called by Squawk when a back branch occurs.
 */
INLINE void osbackbranch() {}

/**
 * Called while Squawk exits. For the eSPOT and bSPOT this function disables
 * interrupts and restarts Squawk.
 */
void osfinish() {}

/**
 * Cannot determine what the purpose of this function is. This function is not
 * implemented for the eSPOT or the bSPOT.
 */
INLINE void osloop() {}

#endif
