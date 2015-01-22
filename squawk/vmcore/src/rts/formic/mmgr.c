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
 * @file   mmgr.c
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The implementation of the Monitor Manager (MMGR)
 *
 */

#include <myrmics.h>
#include <util.h>
#include <softcache.h>
#include "mmgr.h"
#include "mmp.h"
#include "globals.h"
#include "arch.h"

/******************************************************************************
 * Custom allocator for the Monitor Manager
 ******************************************************************************/

/**
 * Allocates enough memory for a monitor_t object.
 *
 * @return The allocated memory or
 *         NULL if there is not enough space
 */
static inline monitor_t*
monitor_alloc ()
{
	monitor_t *new;

	/* If the freelist is empty */
	if (unlikely(mmgrFreeNodes_g == NULL)) {
		if ((mmgrAllocTop_g + sizeof(monitor_t)) < mmgrAllocEnd_g) {
			new             = mmgrAllocTop_g;
			mmgrAllocTop_g += sizeof(monitor_t);
		}
		else {
			return NULL;
		}
	}
	else {
		new             = mmgrFreeNodes_g;
		mmgrFreeNodes_g = mmgrFreeNodes_g->rchild;
	}

	return new;
}

/**
 * Free up the memory allocated for monitor
 *
 * @param monitor The monitor whose memory to free up
 */
static inline void
monitor_free (monitor_t *monitor)
{
	monitor->rchild = mmgrAllocTop_g;
	mmgrAllocTop_g  = monitor;
}

/******************************************************************************
 * Binary search tree hashtble chains
 ******************************************************************************/

/**
 * Looks for object in bst.
 *
 * @param bst    The binary search root
 * @param object The object to find
 * @return Returns the matching node or
 *         NULL if it was not found
 */
static inline monitor_t*
bst_lookup (monitor_t *bst, Address object)
{

	while (bst) {
		if (bst->object == object)
			return bst;
		else if (bst->object > object)
			bst = bst->lchild;
		else
			bst = bst->rchild;
	}

	return NULL;
}

/**
 * Inserts monitor to bst
 *
 * @param bst     The binary search root
 * @param monitor The monitor to insert
 * @return The head of the mofified binary search tree
 */
static inline monitor_t*
bst_insert (monitor_t *bst, monitor_t *monitor)
{
	monitor_t *tmp;

	if (bst == NULL)
		return monitor;

	tmp = bst;

	while (tmp) {
		ar_assert(tmp->object != monitor->object);

		if (tmp->object > monitor->object) {
			if (tmp->lchild) {
				tmp = tmp->lchild;
			}
			else {
				tmp->lchild = monitor;
				break;
			}
		}
		else {
			if (tmp->rchild) {
				tmp = tmp->rchild;
			}
			else {
				tmp->rchild = monitor;
				break;
			}
		}
	}

	return bst;
}

/**
 * Finds the right-most child of bst
 *
 * @param bst The binary search root
 * @return The right-most child or
 *         NULL if the tree is empty
 */
static inline monitor_t*
bst_find_max (monitor_t *bst)
{
	if (bst == NULL)
		return NULL;

	while (bst->rchild) {
		bst = bst->rchild;
	}

	return bst;
}

/* Forward declaration */
static inline monitor_t*bst_remove (monitor_t *bst, Address object);

/**
 * Removes the root of the given subtree and returns a properly
 * modified tree.
 *
 * @param child The child to remove (root of a subtree)
 * @return The properly modified subtree
 */
static inline monitor_t*
bst_remove_child (monitor_t *child)
{
	monitor_t *tmp;

	if (child->lchild && child->rchild) {

		tmp            = bst_find_max(child->lchild);

		child->object  = tmp->object;
		child->owner   = tmp->owner;
		child->waiters = tmp->waiters;
		child->lchild  = bst_remove(child->lchild, child->object);

		return child;
	}

	tmp = child;

	if (child->lchild)
		child = child->lchild;
	else if (child->rchild)
		child = child->rchild;
	else
		child = NULL;

	/*
	 * TODO: free it
	 * free(tmp);
	 */

	return child;
}

/**
 * Remove object from bst
 *
 * @param bst    The binary search tree root
 * @param object The object to remove
 * @return The resulting tree after the removal
 */
static inline monitor_t*
bst_remove (monitor_t *bst, Address object)
{

	while (bst) {
		if (bst->object == object) {
			return bst_remove_child(bst);
		}
		else if (bst->object > object) {
			bst->lchild = bst_remove(bst->lchild, object);
		}
		else {
			bst->rchild = bst_remove(bst->rchild, object);
		}
	}

	return NULL;
}

/******************************************************************************
 * Hashtable implementation
 ******************************************************************************/

/**
 * A hash function for integers
 *
 * Thomas Wang function that does it in 6 shifts (provided you use the
 * low bits, hash & (SIZE-1), rather than the high bits if you can't
 * use the whole value):
 *
 * @param a The integer to hash
 * @return The calculated hash
 */
static inline unsigned int
hashint (unsigned int a)
{
	a += ~(a << 15);
	a ^= (a >> 10);
	a += (a << 3);
	a ^= (a >> 6);
	a += ~(a << 11);
	a ^= (a >> 16);

	return a;
}

/**
 * Hash function for objects
 *
 * @param object The object's address to hash
 * @return The calculated hash
 */
static inline unsigned int
ht_hash (Address object)
{
	unsigned int res;

	/*
	 * Drop 6 LS bits, Objects are cache aligned
	 * Keep the next 3 LS bits and drop the 3 after them (see mmgrGetManager)
	 */
	res = (unsigned int)object;
	res = ((res >> 12) << 3) | ((res >> 6) & 0x8);

	/* Get the modulo */
	res = hashint(res) & (MMGR_HT_SIZE - 1);

	return res;
}

/**
 * Searches object in the monitors' hashtable
 *
 * @param object The object to search for
 * @return The found node or
 *         NULL if not found
 */
static inline monitor_t*
ht_lookup (Address object)
{
	unsigned int index;
	Address      res;

	index = ht_hash(object);

	if (mmgrHT_g[index] != NULL)
		res = bst_lookup(mmgrHT_g[index], object);
	else
		res = NULL;

	return res;
}

/**
 * Inserts monitor in the monitors' hashtable
 *
 * @param monitor The monitor to insert
 */
static inline void
ht_insert (monitor_t *monitor)
{
	unsigned int index;

	index = ht_hash(monitor->object);

	if (mmgrHT_g[index] == NULL)
		mmgrHT_g[index] = monitor;
	else
		mmgrHT_g[index] = bst_insert(mmgrHT_g[index], monitor);
}

/**
 * Removes object from the monitors' hashtable
 *
 * @param object The object to remove
 * @return 1 if it was found and removed or
 *         0 if not found
 */
static inline int
ht_remove (Address object)
{
	unsigned int index;

	index = ht_hash(object);

	if (mmgrHT_g[index] != NULL) {
		mmgrHT_g[index] = bst_remove(mmgrHT_g[index], object);

		return 1;
	}

	return 0;
}

/**
 * Initialize the hashtable
 *
 * @param mmgrHT The allocated memory for the monitors' hashtable
 */
void
mmgrInitialize (mmgrGlobals *globals)
{
	mmgr_g          = globals;
	mmgrFreeNodes_g = NULL;
#ifdef ARCH_MB
	mmgrAllocTop_g  = (Address) mm_slice_base(sysGetCore());
	mmgrAllocEnd_g  = mmgrAllocTop_g + MM_MB_SLICE_SIZE;
#else /* ifdef ARCH_MB */
	mmgrAllocTop_g  = (Address) mm_pa_kernel_base(sysGetCore());
	mmgrAllocEnd_g  = mmgrAllocTop_g + MM_PA_KERNEL_SIZE;
#endif /* ifdef ARCH_MB */

	kt_memset(mmgrHT_g, 0, sizeof(monitor_t*) * MMGR_HT_SIZE);
}

/**
 * Find the manager responsible for the given object.
 *
 * @param object The object
 * @param bid    The manager's bid (return)
 * @param cid    The manager's cid (return)
 */
static inline void
mmgrGetManager (Address object, int *bid, int *cid)
{
	/* We need only 3 bits since we have 2^3 monitor managers */
	unsigned int id3;

	/*
	 * Objects are cache line aligned so we can ignore the 6 LS bits
	 * Then skip the next 3 bits so that neighbor objects don't end at the same
	 * monitor manager
	 */
	id3 = (((unsigned int)object) >> 9) & 0x7;

#ifdef MMGR_ON_ARM

	if (id3 & 1)
		*bid = 0x4B;
	else
		*bid = 0x6B;

	*cid = id3 >> 1;
#else /* MMGR_ON_ARM */
	*bid = 0x3F;
	*cid = id3;
#endif /* MMGR_ON_ARM */
}

#ifdef ARCH_MB
/**
 * Send a msg_op request regarding the given object.  The responsible
 * manager is automatically calculated and the request is sent using a
 * 2-word mailbox message.
 *
 * @param msg_op The desired operation
 * @param object The object to operate on
 */
static inline void
mmgrRequest (mmpMsgOp_t msg_op, Address object)
{
	unsigned int msg0;
	int          target_cid;
	int          target_bid;

	/*
	 * Pass your bid and cid with the opcode so that the other end
	 * can check the owner.
	 */
	msg0 = (sysGetIsland() << 19) | (sysGetCore() << 16) | msg_op;

	mmgrGetManager(object, &target_bid, &target_cid);

	mmpSend2(target_bid, target_cid, msg0, (unsigned int)object);
}

/**
 * Request to enter the given object's monitor.
 *
 * @param object The object to enter its monitor
 *
 */
void
mmgrMonitorEnter (Address object)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent an enter request for %p\n", object);
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

/*	assume(sc_in_heap(object)); */
	mmgrRequest(MMP_OPS_MNTR_ENTER, object);

	/*
	 * Remove me from the runnable threads and add me to the blocked
	 * threads queue. This is done in VMThread.java
	 */
}

/**
 * Request to exit the given object's monitor.
 *
 * @param object The object to exit its monitor
 */
void
mmgrMonitorExit (Address object)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent an exit request\n");
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

/*	assume(sc_in_heap(object)); */
	mmgrRequest(MMP_OPS_MNTR_EXIT, object);
}

/**
 * Request to exit the given object's monitor and notify the manager
 * we will be waiting on it.
 *
 * @param object The object to exit its monitor
 */
void
mmgrWaitMonitorExit (Address object)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent a wait exit request\n");
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

/*	assume(sc_in_heap(object)); */
	mmgrRequest(MMP_OPS_MNTR_WAIT, object);
}

/**
 * Request to add me to the given object's waiters queue.
 *
 * @param object The object
 */
void
mmgrAddWaiter (Address object)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent an add waiter request\n");
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

/*	assume(sc_in_heap(object)); */
	mmgrRequest(MMP_OPS_MNTR_WAITER_ADD, object);
}

/**
 * Request to remove me from the given object's waiters queue.
 *
 * @param object The object
 */
void
mmgrRemoveWaiter (Address object)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent a remove waiter request\n");
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

/*	assume(sc_in_heap(object)); */
	mmgrRequest(MMP_OPS_MNTR_WAITER_REMOVE, object);
}

/**
 * Request to notify the waiters in the given object's waiters queue.
 *
 * @param object The object
 * @param all    Whether to notify all waiters or not
 */
void
mmgrNotify (Address object, int all)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent a notify%s request\n", all ? " all" : "");
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

/*	assume(sc_in_heap(object)); */
	mmgrRequest(all ? MMP_OPS_MNTR_NOTIFY_ALL : MMP_OPS_MNTR_NOTIFY, object);
}

#endif /* ARCH_MB */

/**
 * Get the monitor for the given object if it already exists or create
 * a new one if not
 *
 * @param object The object
 *
 * @return       The object's monitor
 */
static inline monitor_t*
mmgrGetMonitor (Address object)
{
	monitor_t *res;

	/* Look-up the object's monitor */
	res = ht_lookup(object);

	/* If not found create a new one and associate it with this object */
	if (res == NULL) {

		res          = monitor_alloc();
		assume(res != NULL);
		res->owner   = -1;
		res->waiters = NULL;
		res->lchild  = NULL;
		res->rchild  = NULL;
		res->object  = object;
		ht_insert(res);
	}

	return res;
}

/**
 * Handle a monitor enter request.
 *
 * @param bid The board id we got the request from
 * @param cid The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
mmgrMonitorEnterHandler (int bid, int cid, Address object)
{
	monitor_t *monitor;

	monitor = mmgrGetMonitor(object);

	if (monitor->owner == -1) {
		monitor->owner = (bid << 3) | cid;
	}

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got an enter request for %p from %d : %d and the owner is %d : %d\n",
	    object, bid, cid, monitor->owner >> 3, monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */

	/* Reply back with the owner */
	mmpSend2(bid, cid,
	         (unsigned int)((monitor->owner << 16) | MMP_OPS_MNTR_ACK),
	         (unsigned int)object);
}

/**
 * Handle a monitor exit request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
mmgrRemoveWaiterHandler (int bid, int cid, Address object)
{
	monitor_t      *monitor;
	waiter_queue_t *waiter;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf("I got a remove waiter request for %p from %d : %d \
and the owner is %d : %d\n", object, bid, cid, monitor->owner >> 3,
	          monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */

	waiter = monitor->waiters;

	if (waiter != NULL) {
		monitor->waiters = waiter->next;
	}
}

/**
 * Handle a monitor exit request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
mmgrAddWaiterHandler (int bid, int cid, Address object)
{
	monitor_t      *monitor;
	waiter_queue_t *waiter;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got an add waiter request for %p from %d : %d and the owner is %d : %d\n", object, bid, cid, monitor->owner >> 3, monitor->owner &
	    7);
#endif /* ifdef VERY_VERBOSE */

#if WAITER_REUSE

	/* HACK: Since a waiters queue node's size is roughly half of
	 * the size of a monitor, we use the same allocation
	 * granularity and mark the last waiter to know whether it is
	 * the first or the second queue node in the allocated
	 * chunk (see mmgr.h as well) */
	if (monitor->waiters && (monitor->waiters->id >> 16)) {
		waiter = monitor->waiters + sizeof(waiter_queue_t);
	}
	else {
		waiter     = (waiter_queue_t*)monitor_alloc();
		waiter->id = 1 << 16;
	}

	waiter->id |= monitor->owner;
#else /* if WAITER_REUSE */
	waiter      = (waiter_queue_t*)monitor_alloc();
	waiter->id  = monitor->owner;
#endif /* if WAITER_REUSE */

	waiter->next     = monitor->waiters;
	monitor->waiters = waiter;

	return;
}                  /* mmgrAddWaiterHandler */

/**
 * Handle a monitor exit request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 * @param iswait Whether this exit is a result of a wait() call
 */
void
mmgrMonitorExitHandler (int bid, int cid, Address object, int iswait)
{
	monitor_t *monitor;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got a%s exit request for %p from %d : %d and the owner is %d : %d\n",
	    iswait ? " wait" : "n", object, bid, cid, monitor->owner >> 3,
	    monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */

	assume( monitor->owner == ((bid << 3) | cid));

	if (iswait) {
		mmgrAddWaiterHandler(bid, cid, object);
	}

	monitor->owner = -1;
}                  /* mmgrMonitorExitHandler */

/**
 * Handle a monitor notify request.
 *
 * @param bid The board id we got the request from
 * @param cid The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
mmgrNotifyHandler (int bid, int cid, Address object, int all)
{
	monitor_t      *monitor;
	waiter_queue_t *waiter;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got a notify request for %p from %d : %d and the owner is %d : %d\n",
	    object, bid, cid, monitor->owner >> 3, monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */

	/*
	 * Sanity checks
	 *
	 * 1. The one that requested the notify is the owner of the
	 * monitor
	 * 2. The one that requests the notify is just a core that got
	 * notified but did not have any waiters in its queue (they timed
	 * out or got interrupted)
	 *
	 */
	assume((monitor->owner == ((bid << 3) | cid)) || (monitor->owner == -1));

	/*
	 * If there is at least one waiter remove him from the waiters
	 * queue.
	 */
	do {
		waiter = monitor->waiters;

		if (waiter == NULL) {
			break;
		}

		monitor->waiters = waiter->next;
		/*
		 * Send the notification
		 *
		 * FIXME: Do not send multiple messages to the same core
		 * identifier even if it appears multiple times in the waiters
		 * queue.
		 */
		mmpSend2(bid, cid,
		         (unsigned int)(monitor->owner << 16 |
		                        all ? MMP_OPS_MNTR_NOTIFICATION_ALL :
		                        MMP_OPS_MNTR_NOTIFICATION),
		         (unsigned int)object);
	} while (all);

	monitor->owner = -1;
}
