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

#include <scheduler.h>
#include <arch.h>
#include <util.h>

#include "os.h"
#include "mmp.h"
#include "mmgr.h"

/**
 * Send a single word mailbox message to the given core.
 *
 * @param to_bid The target core's board ID
 * @param to_cid The target core's core ID
 * @param msg    The word to send
 */
INLINE void mmpSend(int to_bid, int to_cid, unsigned int msg) {
	int cnt;
	int ret;

	cnt = 22; //TODO: which counter to use???

	do {
		/* kt_printf("Sending thread %p to 0x%02X/%d\n",
		 *           thread, target_bid, target_cid); */
		// Send to dst mailbox, keeping track with a local counter
		ar_cnt_set(sysGetCore(), cnt, -4);
		ar_mbox_send_ack(sysGetCore(),
		                 to_bid, to_cid,
		                 sysGetIsland(), sysGetCore(), cnt,
		                 msg);

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
 * Send a 2-word mailbox message to the given core.
 *
 * @param to_bid The target core's board ID
 * @param to_cid The target core's core ID
 * @param msg0   The first word to send
 * @param msg1   The second word to send
 */
INLINE void mmpSend2(int to_bid, int to_cid,
                     unsigned int msg0, unsigned int msg1) {
	int cnt;
	int ret;

	cnt = 22; //TODO: which counter to use???

	do {
		/* kt_printf("Sending thread %p to 0x%02X/%d\n",
		 *           thread, target_bid, target_cid); */
		// Send to dst mailbox, keeping track with a local counter
		ar_cnt_set(sysGetCore(), cnt, -8);
		ar_mbox_send2_ack(sysGetCore(),
		                  to_bid, to_cid,
		                  sysGetIsland(), sysGetCore(), cnt,
		                  msg0, msg1);

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
	int          target_cid;
	int          target_bid;
	int          cnt;

	cnt = 22; //TODO: which counter to use???

	// Pass your bid and cid with the opcode so that the other end
	// gets it straight from your cache. The bid can be inferred from
	// the global address but for consistency with all the messages we
	// provide it as well.
	msg0 = (sysGetIsland() << 19) | (sysGetCore() << 16) | MMP_OPS_TH_SPAWN;

	schdlrNext(&target_bid, &target_cid);

	mmpSend2(target_bid, target_cid, msg0, (unsigned int)thread);
}

/**
 * Query the mailbox for incoming messages and return a thread object
 * if one of the messages was about scheduling a thread to this core.
 *
 * @return a thread object if one of the messages was about scheduling
 *         a thread to this core or NULL otherwise
 */
INLINE Address mmpCheckMailbox(Address type) {
	unsigned int msg0;
	int          bid;
	int          cid;
	mmpMsgOp_t   msg_type;
	Address      result;
	Address      object;

	// First check to see if there are any messages (non blocking)
	if ( (ar_mbox_status_get(sysGetCore()) & 0xFFFF) == 0 ) {
		return NULL;
	}

	result   = NULL;

	// If there are, get the first
	msg0     = ar_mbox_get(sysGetCore());
	msg_type = (mmpMsgOp_t)(msg0 & 0xFFFF);
	cid      = (msg0 >> 16) & 0x7;
	bid      = (msg0 >> 19) & 0x3F;

	if (type != NULL)
		set_java_lang_Integer_value_local(type, msg_type);

	switch (msg_type) {
#ifndef ARCH_ARM
	// Thread specific messages
	case MMP_OPS_TH_SPAWN:
		/* kt_printf("I've got a message 0x%02X/%d\n",
		 *           sysGetIsland(), sysGetCore()); */
		// this is a two-words message
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("Some %d core pushed a thread (%p) for me\n",
		 *           cid, thread); */
		assume(object != NULL);
		// Fetch it now since we are going to need it soon. The
		// fetch must be explicit to be sure it will be fetched
		// from the proper cache.
		sc_put(object, cid);
		// TODO: After fetching it we should create an exact local copy!
		// FIXME: Now in case it gets out of our software cache it might
		// get refetched from the other board's main memory where it might
		// be outdated.
		/* kt_printf("Our new thread %p is in %p in cache\n",
		 *           thread, sc_put(thread, cid)); */
		result = object;
	// Handle Monitor Manager replies
	case MMP_OPS_MNTR_ACK:
		// The second word holds the object for which we requested
		// the monitor
		object = (Address)ar_mbox_get(sysGetCore());

		// If we do not own the monitor, retry
		if ( bid != my_bid || cid != my_cid ) {
			mmgrMonitorEnter((Address)object);
			set_java_lang_Integer_value_local(type, MMP_OPS_NOP);
		}
		else {
			result = (Address)object;
		}

		break;
#endif /* ARCH_ARM */
	// Monitor specific messages
	case MMP_OPS_MNTR_ENTER:
		// this is a two-words message
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("0x%02X/%d wants to enter monitor (%p)\n",
		 *           from_bid, from_cid, object); */
		assume(object != NULL);
		mmgrMonitorEnterHandler(bid, cid, object);
		break;
	case MMP_OPS_MNTR_EXIT:
		// this is a two-words message
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("0x%02X/%d wants to exit monitor (%p)\n",
		 *           from_bid, from_cid, object); */
		assume(object != NULL);
		mmgrMonitorExitHandler(bid, cid, object);
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

	return result;
}
