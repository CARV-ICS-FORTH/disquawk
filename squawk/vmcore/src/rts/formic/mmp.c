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
	int          target_cid;
	int          target_bid;

	cnt = 22; //TODO: which counter to use???

	// Pass your cid with the opcode so that the other end gets it
	// straight from your cache. The bid will be inferred from the
	// global address.
	msg0 = ( (sysGetCore()) << 16) | MMP_OPS_TH_SPAWN;

	schdlr_next(&target_bid, &target_cid);

	do {
		/* kt_printf("Sending thread %p to 0x%02X/%d\n",
		 *           thread, target_bid, target_cid); */
		// Send to dst mailbox, keeping track with a local counter
		ar_cnt_set(sysGetCore(), cnt, -8);
		ar_mbox_send2_ack(sysGetCore(),
		                  target_bid, target_cid,
		                  sysGetIsland(), sysGetCore(), cnt,
		                  msg0, (unsigned int)thread);

		// Spin until counter notification arrives FIXME: make it non blocking
		while ( (ret = ar_cnt_get_triggered(sysGetCore(), cnt)) == 0) {
			;
		}

	} while (ret == 3);

	/* kt_printf("Sent\n"); */
	assume(ret == 2); // Ack
	assume(ar_cnt_get(sysGetCore(), cnt) == 0);

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
	int          cid;

	thread = NULL;

	// First check to see if there are any messages (non blocking)
	if ( (ar_mbox_status_get(sysGetCore()) & 0xFFFF) != 0 ) {
		// If there are, get the first
		msg = (msg_op) ar_mbox_get(sysGetCore()) & 0xFFFF;
		switch (msg) {
		// Thread specific messages
		case MMP_OPS_TH_SPAWN:
			/* kt_printf("I've got a message 0x%02X/%d\n",
			 *           sysGetIsland(), sysGetCore()); */
			cid = (msg >> 16) & 0x7;
			// this is a two-words message
			thread = (Address)ar_mbox_get(sysGetCore());
			/* kt_printf("Some %d core pushed a thread (%p) for me\n",
			 *           cid, thread); */
			assume(thread != NULL);
			// Fetch it now since we are going to need it soon. The
			// fetch must be explicit to be sure it will be fetched
			// from the proper cache.
			sc_put(thread, cid);
			// TODO: After fetching it we should create an exact local copy!
			// FIXME: Now in case it gets out of our software cache it might
			// get refetched from the other board's main memory where it might
			// be outdated.
			/* kt_printf("Our new thread %p is in %p in cache\n",
			 *           thread, sc_put(thread, cid)); */
			break;
		/* TODO: Add Christi's op-codes here */
		// Hash-Table
		// Stack
		// Atomic Primitives
		// Synchronize
		default:
			kt_printf("Error: Unknown message operator\n");
			ar_abort();
			break;
		}
	}

	return thread;
}
