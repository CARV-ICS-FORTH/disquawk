/*
 * Copyright (C) 2013-2014 FORTH-ICS / CARV
 *                         (Foundation for Research & Technology -- Hellas,
 *                          Institute of Computer Science,
 *                          Computer Architecture & VLSI Systems Laboratory)
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file   mmp.c
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The implementation of the Mailbox Message Protocol (MMP)
 *
 */

#include "mmp_ops.h"

/**
 * Peeks a core to schedule the given thread.  Then sends a message to
 * this core with the thread object's address and the core and island
 * of the hardware thread that spawned it.  The other end (receiving
 * thread) will add this thread to it's thread queue and implicitly
 * fetch it when needed (lazely).
 *
 * @param thread The thread object's address
 */
void mmpSpawnThread(Address thread) {
	unsigned int msg0;
	int          cnt;
	int          ret;

	cnt = 22; //TODO: which counter to use???

	// Pass your bid and cid with the opcode so that the other end
	// gets it straight from your cache
	msg0 = ( ((sysGetIsland() << 3) | sysGetCore()) << 16) | MMP_OPS_TH_SPAWN;

	// FIXME: Make it work for various number of cores
	// FIXME: Do it with a single counter and use mod/div to find the actual bid and cid
	// Peek a core and a board (FIXME: do something smarter than RR)
	while (schedulerLastCore_g == sysGetCore()) {
		schedulerLastCore_g   = (schedulerLastCore_g + 1) & 0x7;
	}
	while (schedulerLastIsland_g == sysGetIsland()) {
		schedulerLastIsland_g = (schedulerLastIsland_g + 1) & 0x3F;
	}

	do {
		// Send to dst mailbox, keeping track with a local counter
		ar_cnt_set(sysGetIsland(), sysGetCore(), -8);
		ar_mbox_send2_ack(sysGetCore(),
		                  schedulerLastIsland_g, schedulerLastCore_g,
		                  sysGetIsland(), sysGetCore(), cnt,
		                  msg0, (unsigned int)thread);

		// Spin until counter notification arrives FIXME: make it non blocking
		while ( (ret = ar_cnt_get_triggered(sysGetCore(), cnt)) == 0) {
			;
		}
	} while (ret == 3);

	/* ar_assert(ret == 1); // Ack
	 * ar_assert(ar_cnt_get(sysGetCore(), cnt) == 0); */

}

/**
 * Query the mailbox for incoming messages and return a thread object
 * if one of the messages was about scheduling a thread to this core.
 *
 * @return a thread object if one of the messages was about scheduling
 *         a thread to this core or NULL otherwise
 */
INLINE Address mmpCheckmailbox() {
	msg_op       msg;
	Address      thread;

	thread = NULL;

	// First check to see if there are any messages (non blocking)
	if ( (ar_mbox_status_get(sysGetCore()) & 0xFFFF) != 0 ) {
		// If there are, get the first
		msg = (msg_op) ar_mbox_get(sysGetCore());
		switch (msg) {
		// Thread specific messages
		case MMP_OPS_TH_SPAWN:
			kt_printf("Someone pushed a thread for me\n");
			// this is a two-words message
			thread = (Address)ar_mbox_get(sysGetCore());
			/* ar_assert(thread != NULL); */
			break;
		/* TODO: Add Christi's op-codes here */
		// Hash-Table
		// Stack
		// Atomic Primitives
		// Synchronize
		default:
			kt_printf("Error: Unknown message operator\n");
			/* ar_abort(); */
			break;
		}
	}

	return thread;
}
