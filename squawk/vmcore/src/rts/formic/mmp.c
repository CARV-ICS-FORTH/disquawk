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
#include <softcache.h>

#include "os.h"
#include "mmp.h"
#include "mmgr.h"

/**
 * Peeks a core to schedule the given thread.  Then sends a message to
 * this core with the thread object's address and the core and island
 * of the hardware thread that spawned it.  The other end (receiving
 * thread) will add this thread to it's thread queue and implicitly
 * fetch it when needed (lazely).
 *
 * @param thread The thread object's address
 */
void
mmpSpawnThread (Address thread)
{
	unsigned int msg0;
	int          target_cid;
	int          target_bid;

	/*
	 * Pass your bid and cid with the opcode so that the other end
	 * gets it straight from your cache. The bid can be inferred from
	 * the global address but for consistency with all the messages we
	 * provide it as well.
	 */
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
Address
mmpCheckMailbox (Address type)
{
	unsigned int msg0;
	int          bid;
	int          cid;
	int          wait;
	mmpMsgOp_t   msg_type;
	Address      result;
	Address      object;

	/* First check to see if there are any messages (non blocking) */
	if ( (ar_mbox_status_get(sysGetCore()) & 0xFFFF) == 0) {
		return NULL;
	}

	result = NULL;
	wait   = 0;

	/* If there are, get the first */
	msg0     = ar_mbox_get(sysGetCore());
	msg_type = (mmpMsgOp_t)(msg0 & 0xFFFF);
	cid      = (msg0 >> 16) & 0x7;
	bid      = (msg0 >> 19) & 0x3F;

	if (type != NULL)
		set_java_lang_Integer_value(type, msg_type);

	switch (msg_type) {
#ifndef ARCH_ARM
	/* Thread specific messages */
	case MMP_OPS_TH_SPAWN:
		/* kt_printf("I've got a spawn message 0x%02X/%d\n",
		 *           sysGetIsland(), sysGetCore()); */
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("Some %d core pushed a thread (%p) for me\n",
		 *           cid, object); */
		assume(object != NULL);
		/*
		 * Fetch it now since we are going to need it soon. The
		 * fetch must be explicit to be sure it will be fetched
		 * from the proper cache.
		 */
		sc_put(object, cid);
		/*
		 * TODO: After fetching it we should create an exact local copy!
		 * FIXME: Now in case it gets out of our software cache it might
		 * get refetched from the other board's main memory where it might
		 * be outdated.
		 */
/*		kt_printf("Our new thread %p is in cache\n", object); */
		result = object;
		break;
	/* Handle Monitor Manager replies */
	case MMP_OPS_MNTR_ACK:
		/*
		 * The second word holds the object for which we requested
		 * the monitor
		 */
		object = (Address)ar_mbox_get(sysGetCore());

		/* If we do not own the monitor, retry */
		if (bid != my_bid || cid != my_cid) {
#  ifdef VERY_VERBOSE
			kt_printf("Monitor owner is %d:%d\n", bid, cid);
			ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

			/* Resend a request */
			mmgrMonitorEnter(object);
			set_java_lang_Integer_value(type, MMP_OPS_NOP);
		}
		else {
#  ifdef VERY_VERBOSE
			kt_printf("Monitor for %p acquired\n", object);
			ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

			result = object;
		}

		break;
	case MMP_OPS_MNTR_NOTIFICATION:
	case MMP_OPS_MNTR_NOTIFICATION_ALL:
		/* this is a two-words message */
		result = (Address)ar_mbox_get(sysGetCore());
		break;
#endif /* ARCH_ARM */
	/* Monitor specific messages */
	case MMP_OPS_MNTR_ENTER:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("0x%02X/%d wants to enter monitor (%p)\n",
		 *           from_bid, from_cid, object); */
		assume(object != NULL);
		mmgrMonitorEnterHandler(bid, cid, object);
		break;
	case MMP_OPS_MNTR_WAITER_ADD:
		mmgrAddWaiterHandler(bid, cid, object);
		break;
	case MMP_OPS_MNTR_WAITER_REMOVE:
		mmgrRemoveWaiterHandler(bid, cid, object);
		break;
	case MMP_OPS_MNTR_WAIT:
		wait = 1;  /* Don't break here */
	case MMP_OPS_MNTR_EXIT:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("0x%02X/%d wants to exit monitor (%p)\n",
		 *           from_bid, from_cid, object); */
		assume(object != NULL);
		mmgrMonitorExitHandler(bid, cid, object, wait);
		break;
	/* TODO: Add Christi's op-codes here */
	/*
	 * Hash-Table
	 * Stack
	 * Atomic Primitives
	 * Synchronize
	 */
	default:
		kt_printf("Error: Unknown message operator\n");
		ar_abort();
		break;
	}              /* switch */

	return result;
}                  /* mmpCheckMailbox */
