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

/**
 * @file   os.c.spp
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 * @date   Thu Oct 24 17:58:30 2013
 *
 * @brief  Contains
 *
 *
 */
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
#include "arch.h"
#include "debug.h"
#include "types.h"
#include "myrmics.h"
#include "jni.h"
#include "errno.h"

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

/**
 * @brief Write back data from the local cache.
 * Calculates the _home_ board of the address and issues a DMA transfer to its
 * DRAM erasing the dirty bit on the local cache.
 *
 * @param address   The address to b written back.
 * @param size      The size of the data to write back.
 * @param blocking  Whether to block and wait for an acknowledgment
 *
 * @warning Not tested
 */
/* void sysFlushData(void * address, int size, int blocking) {
 * 	int dst_bid;
 *
 * 	// Align to cache line size
 * 	address &= ~(AR_DMA_ALIGN-1);
 *
 * 	dst_bid = 0;
 * 	ar_dma_with_ack(my_cid,
 * 	                my_bid, my_cid, address,  // Source
 * 	                dst_bid, 0xC, address & , // Destination
 * 	                my_bid, my_cid, ack_cnt,  // Acknowledgment
 * 	                size, 1, 0, 0);
 *
 * 	// wait t
 * } */

void sysInvalidateCacheLine(void * address) {

}

/**
 * @brief Terminate process.
 * Terminates the process and shuts formic off.
 *
 * @param status The exit status
 */
void exit(int status) {
	kt_printf("Board=%d Core=%d : Exited with %d\n",
	          my_bid, my_cid, status);

	// Instruct server to kill the power
	if ((my_bid == AR_BOOT_MASTER_BID) && (!my_cid)) {
		ar_uart_flush();
		ar_timer_busy_wait_msec(200);
		kt_printf("%s", DBG_KILL_CONNECTION);
	}

	// Stall until power off
	while (1);
}


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

/**
 * Free chunk of memory allocated by sysValloc
 *
 * @param ptr to to chunk allocated by sysValloc
 */
#define sysVallocFree(ptr)

/**
 * @return the page size
 */
#define sysGetPageSize() MM_PAGE_SIZE // defined in memory_management.h

/**
 * @return the cache line size
 */
#define sysGetCachelineSize() MM_CACHELINE_SIZE // defined in memory_management.h

/**
 * Return the current time in microseconds.
 */
jlong sysTimeMicros() {
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

/**
 * Compare and swap.
 *
 * @todo This is dummy, need to implement it right using the JVM design.
 *
 * @return >0 if succeeded, 0 otherwise
 */
INLINE int sysCAS(void* ptr, int old, int new) {
	int ret;

	ret  = (*(int*)ptr == old);
	*(int*)ptr = new;

	return ret;
}

/**
 * Sets a region of memory read-only or reverts it to read & write.
 *
 * @param start    the start of the memory region
 * @param end      one byte past the end of the region
 * @param readonly specifies if read-only protection is to be enabled or disabled
 */
INLINE void sysToggleMemoryProtection(char* start, char* end, boolean readonly) {}

/**
 * Sets code and bss sections as read-only.
 */
INLINE void sysGlobalMemoryProtection() {
  // Install ART code region and the heap
  ar_art_install_region(1,                         // region ID
                        MM_MB_VA_CODE_PAGE,        // base
                        // size (Till the start of the stack)
                        MM_MB_VA_STACK_BASE-MM_MB_VA_CODE_PAGE,
                        1,                         // cacheable
                        1,                         // read only
                        1,                         // executable
                        1,                         // user accessible
                        1);                        // privileged accessible
}

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

/**
 * @return the size of the flash memory
 */
unsigned int get_flash_size(void) {
	return (unsigned int)&__linker_squawk_suite_end;
}

/**
 * Called by Squawk when a back branch occurs.
 */
//INLINE void osbackbranch() {}
#define osbackbranch()

#ifdef ATOMIC_BARRIER
/**
 * @brief Global barrier.
 * Dissemination barrier (from: Two Algorithms for Barrier
 * Synchronization) implementation using bitfields and atomic
 * increment.
 *
 * NOTE: NOT WORKING for sparse cores (i.e. 1 core per board).
 *
 * Performance:
 *
 * |  Cores  |  Ticks  |
 * |--------:|--------:|
 * |      1  |     11  |
 * |      8  |    368  |
 * |     32  |    674  |
 * |    512  |   1248  |
 */
// FIXME: Add parity
void sysBarrier() {
	register int i, tmp, log2_cores;
	register int partner_cid, partner_bid;

	if(TOTAL_CORES==1)
		return;

	// This could also be calculated at initialization
	log2_cores = int_log2(TOTAL_CORES) + TOTAL_CORES%2;

	for (i=0; i<=log2_cores; ++i) {
		// core + 2^i % P
		tmp = ((my_bid<<3 | my_cid)+(1<<i))%TOTAL_CORES;
		partner_bid = tmp >> 3;
		partner_cid = tmp & 0x7;
		ar_cnt_incr(my_cid, partner_bid, partner_cid, NOC_COUNTER_WAKEUP3, 1<<i);
		while (!(ar_cnt_get(my_cid, NOC_COUNTER_WAKEUP3) & 1<<i));
	}

	// reset the counter
	ar_cnt_set(my_cid, NOC_COUNTER_WAKEUP3, 0);
}
#else
/**
 * @brief Global barrier.
 * Implements a simple, centralized barrier using the counters
 *
 * Performance:
 *
 * |  Cores  |  Ticks  |
 * |--------:|--------:|
 * |      8  |    509  |
 * |    512  |  22000  |
 */
void sysBarrier() {
	int i, x, y, z, slave_bid;

	// Compute how many cores should be activated

	// The master/coordinator
	if((my_bid == AR_BOOT_MASTER_BID) && !my_cid) {

		while (ar_cnt_get(my_cid, NOC_COUNTER_WAKEUP3));

		// Re-Initialize the barrier counter
		ar_cnt_set(my_cid, NOC_COUNTER_WAKEUP3, 1+8-TOTAL_CORES);  // HACK exclude bid 63

		// Make Formic slaves reach their boot barrier
		for (x = AR_FORMIC_MIN_X; x <= AR_FORMIC_MAX_X; x++)
			for (y = AR_FORMIC_MIN_Y; y <= AR_FORMIC_MAX_Y; y++)
				for (z = AR_FORMIC_MIN_Z; z <= AR_FORMIC_MAX_Z; z++)
					for (i = 0; i < AR_FORMIC_CORES_PER_BOARD; i++) {
						slave_bid = (x << 4) | (y << 2) | z;

						if (!( ( (slave_bid == my_bid) && (i == my_cid) ) ||
						       ( (slave_bid >= 63) ) )) // HACK exclude bid 63
							ar_cnt_incr(my_cid, slave_bid, i, NOC_COUNTER_WAKEUP3, 1);
					}
	} else { // the slaves
		// Initialize the local barrier counter
		ar_cnt_set(my_cid, NOC_COUNTER_WAKEUP3, -1);
		// Notify master we are here
		ar_cnt_incr(my_cid, AR_BOOT_MASTER_BID, 0, NOC_COUNTER_WAKEUP3, 1);
		// Block until master indicates all cores have reached the barrier
		while (ar_cnt_get(my_cid, NOC_COUNTER_WAKEUP3));
	}
}
#endif

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
