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
 * @file   os.c
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 * @date   Thu Oct 24 17:58:30 2013
 *
 * @brief  Contains
 *
 */

#include "os.h"
#include "mmp.h"

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

/**
 * Compare and swap.  An asynchronous implementation, sends a request
 * to the atomic primitives manager.  The thread is then added to an
 * event queue waiting for the request to be processed.
 */
void sysCAS(void* ptr, unsigned int old, unsigned int new) {
	unsigned int msg[16];
	int dst_bid, dst_cid;

	/* FIXME: Choose dst_bid and dst_cid somehow */
	dst_bid = 0x3F;
	dst_cid = 0;

	kt_memset(msg, 0, sizeof(msg));
	msg[0] = ((my_bid << 3) | my_cid) << 16 | MMP_OPS_AT_CAS; /* The opcode */
	msg[1] = (unsigned int)ptr; /* The object address */
	msg[2] = old;               /* The expected value */
	msg[3] = new;               /* The new value */
	mmpSend16(dst_bid, dst_cid, msg);
}

/**
 * Sets a region of memory read-only or reverts it to read & write.
 *
 * @param start    the start of the memory region
 * @param end      one byte past the end of the region
 * @param readonly specifies if read-only protection is to be enabled or disabled
 */
void sysToggleMemoryProtection(char* start, char* end, boolean readonly) {}

/**
 * Sets code and bss sections as read-only.
 */
void sysGlobalMemoryProtection() {
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
 * @return the size of the flash memory
 */
unsigned int get_flash_size(void) {
	return (unsigned int)&__linker_squawk_suite_end;
}
