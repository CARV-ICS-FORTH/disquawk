/*
 * Copyright 2013-14 FORTH-ICS. All Rights Reserved.
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

/**
 * @file   os.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 */

#ifndef FORMIC_OS_H_
#define FORMIC_OS_H_

#define TRUE 1
#define FALSE 0

#define TOTAL_CORES \
	((AR_FORMIC_MAX_X - AR_FORMIC_MIN_X + 1) * \
	(AR_FORMIC_MAX_Y - AR_FORMIC_MIN_Y + 1) * \
	(AR_FORMIC_MAX_Z - AR_FORMIC_MIN_Z + 1) * \
	 AR_FORMIC_CORES_PER_BOARD)

/* Disable non volatile memory */
#define DEFAULT_NVM_SIZE   0

#define DEFAULT_RAM_SIZE   MM_MB_SLICE_SIZE

/* Try to use the normal C conversions */
#define C_FP_CONVERSIONS_OK 0

// Override setting from platform.h
#undef PLATFORM_UNALIGNED_LOADS
#define PLATFORM_UNALIGNED_LOADS false

#define IODOTC "formic_io.c"
#define sysPlatformName() "formic"

#include <arch.h>
#include <debug.h>
#include <types.h>
#include <myrmics.h>
#include <errno.h>
#include <address.h>

#include "jni.h"

/* map various functions to myrmics' ones */
#define fprintf(s, ...) printf(__VA_ARGS__)
#define fflush(s)       ar_uart_flush()
#define strlen          kt_strlen
#define strncpy         kt_strncpy
#define strncmp         kt_strncmp
#define free            kt_free
#define malloc          kt_malloc
#define atoi            kt_atoi

#define my_cid          (ar_get_core_id())
#define my_bid          (ar_get_board_id())

/* Myrmics uses the ar_float_sqrt and ar_float_pow as sqrt and pow
 * respectively. However to be consistent with libc those should be
 * sqrtf and powf.
 * Reference os_math.h and math_intrinsics/ for more info.
 */
#ifdef pow
# undef  pow
# define pow __ieee754_pow
#endif
#ifdef sqrt
# undef  sqrt
# define sqrt __ieee754_sqrt
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void sysFlushData(void * address, int size, int blocking);
void sysInvalidateCacheLine(void * address);
void exit(int status);

//extern int __linker_end_bss;
extern int __linker_squawk_suite_end;

/**
 * Allocate a page-aligned chunk of memory of the given size.
 * THIS IS INVOKED ONLY ONCE TO ALLOCATE THE WHOLE MEMORY
 *
 * @param size size in bytes to allocate
 * @return pointer to allocated memory or null.
 */
INLINE void* sysValloc(size_t size) {

	if (size>MM_MB_SLICE_SIZE) {
		fprintf(stderr, "Requesting %d Bytes but there are only %d available\n",
		        size, MM_MB_SLICE_SIZE);
		return NULL;
	}

	return (void*)mm_slice_base(my_cid);
}

#define sysVallocFree(ptr)
#define sysGetPageSize() MM_PAGE_SIZE // defined in memory_management.h
#define sysGetCachelineSize() MM_CACHELINE_SIZE // defined in memory_management.h

/**
 * Return the current time in microseconds.
 */
INLINE jlong sysTimeMicros() {
	// get the cycles, each cycle is 100ns
	jlong res = (jlong)ar_free_timer_get_cycles();
	res = res / 10;
	return res;
}

/**
 * @return the current time in milliseconds.
 */
INLINE jlong sysTimeMillis(void) {
	jlong res = sysTimeMicros();
	res = res / 1000;
	return res;
}

void sysCAS(void* ptr, unsigned int old, unsigned int new);
void sysToggleMemoryProtection(char* start, char* end, boolean readonly);
void sysGlobalMemoryProtection();

/**
 * Cannot determine what the purpose of this function is. This
 * function is not implemented for the eSPOT or the bSPOT. It gets
 * called in every bytecode execution.
 */
//INLINE void osloop() {}
#define osloop()

#define enableARMInterrupts enableInterrupts
#define enableInterrupts(x)
#define disableARMInterrupts disableInterrupts
#define disableInterrupts(x)

/**
 * Called while Squawk exits. For the eSPOT and bSPOT this function disables
 * interrupts and restarts Squawk.
 */
//void osfinish() {}
#define osfinish()

/**
 * @return the start address of the flash memory
 */
//unsigned int get_flash_base(void) { return 0; }
#define get_flash_base() 0

unsigned int get_flash_size(void);

/**
 * Called by Squawk when a back branch occurs.
 */
//INLINE void osbackbranch() {}
#define osbackbranch()

/**
 * Returns the core id running this VM instance.
 *
 * @return the core id
 */
#define sysGetCore ar_get_core_id

/**
 * Returns the island (numa/board) id running this VM instance.
 *
 * @return the island id
 */
#define sysGetIsland ar_get_board_id


/**
 * Calculatates the address's home core and island
 *
 * @param  addr   The address to find its home
 * @param  core   The home core (output if not NULL)
 * @param  island The home island (output)
 * @return Writes `core` and `island`
 */
INLINE void sysHomeOfAddress(Address addr, int* island, int* core) {
#ifdef __MICROBLAZE__
	*island = ((unsigned int)addr >> 26);
	if (core != NULL)
		*core   = ((unsigned int)addr >> 23) & 0x7;
#else
	exit(253);
#endif
}

#endif /* FORMIC_OS_H_ */
