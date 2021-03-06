/*
 * Copyright (c) 2013-15, FORTH-ICS / CARV
 *                        (Foundation for Research & Technology -- Hellas,
 *                         Institute of Computer Science,
 *                         Computer Architecture & VLSI Systems Laboratory)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
#include "apmgr.h"

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
mmpSpawnThread(Address thread)
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
	/* Write back all dirty data (this is a release action) */
	sc_flush(SC_BLOCKING);

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
mmpCheckMailbox(Address type, Address hash)
{
	unsigned int msg0;
	int          bid;
	int          cid;
	int          tmp;
	int          new, expected, i;
	mmpMsgOp_t   msg_type;
	Address      result;
	Address      object;

#ifdef VERY_VERBOSE
	if (sysGetCore() == 0 && sysGetIsland() == 0) {
		kt_printf("IN mmpCheckMailbox\n");
		ar_uart_flush();
	}
#endif /* ifdef VERY_VERBOSE */

	/* First check to see if there are any messages (non blocking) */
	if ((ar_mbox_status_get(sysGetCore()) & 0xFFFF) == 0) {
		/* Set message type (OP) to NOP */
		if (type != NULL)
			set_java_lang_Integer_value(type, MMP_OPS_NOP);

		return NULL;
	}

	result = NULL;
	tmp    = 0;

	/* If there are, get the first */
	msg0     = ar_mbox_get(sysGetCore());
	msg_type = (mmpMsgOp_t)(msg0 & 0xFFFF);
	cid      = (msg0 >> 16) & 0x7;
	bid      = (msg0 >> 19) & 0x3F;

#ifdef VERY_VERBOSE
	if (sysGetCore() == 0 && sysGetIsland() == 0) {
		kt_printf("IN mmpCheckMailbox msg_type=%d\n", msg_type);
		ar_uart_flush();
	}
#endif /* ifdef VERY_VERBOSE */

	/* Set message type (OP) */
	if (type != NULL)
		set_java_lang_Integer_value(type, msg_type);

	switch (msg_type) {
#ifndef ARCH_ARM
	/* Thread specific messages */
	case MMP_OPS_TH_SPAWN:
		/* kt_printf("I've got a spawn message 0x%02X/%d\n",
		 *           sysGetIsland(), sysGetCore()); */
		/* kt_printf("The time is %u\n", ar_free_timer_get_ticks()); */
		/* ar_uart_flush(); */
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
		/* kt_printf("Our new thread %p is in cache\n", object);
		 * kt_printf("The time is %u\n", ar_free_timer_get_ticks()); */
		result = object;
		break;
	/* Handle Monitor Manager replies */
	case MMP_OPS_MNTR_ACK:
		/*
		 * The second word holds the hash of the object for which we
		 * requested the monitor
		 */
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("IN mmpCheckMailbox object=%p\n", object);
		 * ar_uart_flush(); */

#  ifdef MMGR_QUEUE
		/* Since we added queues at the lock manager we should always
		 * get the lock at acks */
		assume(bid == sysGetIsland() && cid == sysGetCore());

#    ifdef VERY_VERBOSE
		kt_printf("Monitor for %p acquired\n", object);
		ar_uart_flush();
#    endif /* ifdef VERY_VERBOSE */

		result = object;

#  else /* ifdef MMGR_QUEUE */

		/* If we do not own the monitor, retry */
		if (bid != sysGetIsland() || cid != sysGetCore()) {
#    ifdef VERY_VERBOSE
			kt_printf("Monitor owner is %d:%d\n", bid, cid);
			ar_uart_flush();
#    endif /* ifdef VERY_VERBOSE */

			/* TODO: Add backoff here... to avoid deadlock
			   (due to underlying hardware limitations) */
			/* Resend the enter request.  We do not use the mmgrMonitorEnter
			 * here to speed up things */
			mmgrRequestHash(MMP_OPS_MNTR_ENTER, (unsigned int)object);
			set_java_lang_Integer_value(type, MMP_OPS_NOP);
		}
		else {
#    ifdef VERY_VERBOSE
			kt_printf("Monitor for %p acquired\n", object);
			ar_uart_flush();
#    endif /* ifdef VERY_VERBOSE */

			result = object;
		}

#  endif /* ifdef MMGR_QUEUE */

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
		/* if (/\* sysGetCore() == 0 && *\/ sysGetIsland() == 63) {
		 * 	kt_printf("IN MNTR_ENTER object=%p queue_len=%d from=%d:%d\n",
		 * 	          object, ar_mbox_status_get(sysGetCore()) & 0xFFFF,
		 * 	          bid, cid);
		 * 	ar_uart_flush();
		 * } */
		/* kt_printf("0x%02X/%d wants to enter monitor (%p)\n",
		 *           from_bid, from_cid, object); */
		assume(object != NULL);
		mmgrMonitorEnterHandler(bid, cid, object);
		break;
	case MMP_OPS_MNTR_WAITER_ADD:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("0x%02X/%d wants to enter monitor (%p)\n",
		 *           from_bid, from_cid, object); */
		assume(object != NULL);
		mmgrAddWaiterHandler(bid, cid, object);
		break;
	case MMP_OPS_MNTR_WAITER_REMOVE:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		/* kt_printf("0x%02X/%d wants to enter monitor (%p)\n",
		 *           from_bid, from_cid, object); */
		assume(object != NULL);
		mmgrRemoveWaiterHandler(bid, cid, object);
		break;
	case MMP_OPS_MNTR_NOTIFY_ALL:
		tmp = 1;   /* Don't break here */
	case MMP_OPS_MNTR_NOTIFY:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		mmgrNotifyHandler(bid, cid, object, tmp);
		break;
	case MMP_OPS_MNTR_WAIT:
		tmp = 1;   /* Don't break here */
	case MMP_OPS_MNTR_EXIT:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		/* if (/\* sysGetCore() == 0 && *\/ sysGetIsland() == 63) {
		 * 	kt_printf("IN MNTR_EXIT object=%p queue_len=%d from=%d:%d\n",
		 * 	          object, ar_mbox_status_get(sysGetCore()) & 0xFFFF,
		 * 	          bid, cid);
		 * 	ar_uart_flush();
		 * } */
		/* kt_printf("0x%02X/%d wants to exit monitor (%p)\n",
		 *           from_bid, from_cid, object); */
		assume(object != NULL);
		mmgrMonitorExitHandler(bid, cid, object, tmp);
		break;
	/* Handle atomic primitives */
	case MMP_OPS_AT_CAS:
		/* this is a cache-line message */
		object   = (Address)ar_mbox_get(sysGetCore());
		assume(object != NULL);
		expected = (int)ar_mbox_get(sysGetCore());
		new      = (int)ar_mbox_get(sysGetCore());

		/* pop the empty words... */
		for (i = 0; i < 12; ++i) {
			(void)ar_mbox_get(sysGetCore());
		}

		apmgrCASHandler(bid, cid, object, expected, new);
		break;
	/* TODO: Add Christi's op-codes here */
	/*
	 * Hash-Table
	 * Stack
	 * Atomic Primitives
	 * Synchronize
	 */
	case MMP_OPS_RW_READ_TRY:
		tmp = 1;   /* Don't break here */
	case MMP_OPS_RW_READ:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		ar_assert(object != NULL);
		mmgrReadLockHandler(bid, cid, object, tmp);
		break;
	case MMP_OPS_RW_WRITE_TRY:
		tmp = 1;   /* Don't break here */
	case MMP_OPS_RW_WRITE:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		ar_assert(object != NULL);
		mmgrWriteLockHandler(bid, cid, object, tmp);
		break;
	case MMP_OPS_RW_READ_UNLOCK:
		tmp = 1;   /* Don't break here */
	case MMP_OPS_RW_WRITE_UNLOCK:
		/* this is a two-words message */
		object = (Address)ar_mbox_get(sysGetCore());
		ar_assert(object != NULL);
		mmgrRWunlockHandler(bid, cid, object, tmp);
		break;
	case MMP_OPS_RW_READ_ACK:
	case MMP_OPS_RW_WRITE_ACK:
		/* /\* Empty our software cache.  There is no need to for writing back any
		 *  * dirty
		 *  * data, since they should be the result of a data race *\/
		 * /\* sc_flush(); *\/
		 * sc_clear(); */
	case MMP_OPS_AT_CAS_ACK:
		/* For CAS we clear the cache at request */
	case MMP_OPS_AT_CAS_NACK:
	case MMP_OPS_RW_READ_NACK:
	case MMP_OPS_RW_WRITE_NACK:
		result = (Address)ar_mbox_get(sysGetCore());
		assume(result != NULL);
		break;
	case MMP_OPS_MMGR_RESET_STATS:
		mmgr_reset_stats();
		break;
	case MMP_OPS_MMGR_PRINT_STATS:
		mmgr_print_stats();
		break;
	default:
		kt_printf("Error: Unknown message operator\n");
		kt_printf("Error: OP = %2d\n", (int)msg_type);
		ar_abort();
		break;
	}              /* switch */

	if (hash != NULL)
		set_java_lang_Integer_value(hash, (int)result);

	return result;
}                  /* mmpCheckMailbox */
