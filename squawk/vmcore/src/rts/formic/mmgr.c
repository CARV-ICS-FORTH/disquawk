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

/* #define VERY_VERBOSE 1 */

#ifdef MMGR_STATS
#ifndef MMGR_STATS_PERIOD
#define MMGR_STATS_PERIOD 1000
#endif /* ifdef MMGR_STATS_PERIOD */
unsigned int read_locks;
unsigned int read_lock_reqs;
unsigned int write_locks;
unsigned int write_lock_reqs;
unsigned int monitor_enters;
unsigned int monitor_enter_reqs;
unsigned int start;

void mmgr_reset_stats() {
	read_locks = 0;
	read_lock_reqs = 0;
	write_locks = 0;
	write_lock_reqs = 0;
	monitor_enters = 0;
	monitor_enter_reqs = 0;
	start = sysGetTicks();
}

void mmgr_print_stats() {
	unsigned int time = sysGetTicks()-start;
	kt_printf("MMGR_STATS | %2d:%1d | %d/%d | %d/%d | %d/%d | %d |\n",
	          sysGetIsland(), sysGetCore(),
	          read_locks, read_lock_reqs,
	          write_locks, write_lock_reqs,
	          monitor_enters, monitor_enter_reqs,
	          time);
	mmgr_reset_stats();
}
#endif  /* MMGR_STATS */

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
monitor_alloc()
{
	monitor_t *new;

	/* If the freelist is empty */
	if (unlikely(mmgrMonitorFreeNodes_g == NULL)) {
		if ((mmgrAllocTop_g + sizeof(monitor_t)) < mmgrAllocEnd_g) {
			new             = mmgrAllocTop_g;
			mmgrAllocTop_g += sizeof(monitor_t);
		}
		else {
			return NULL;
		}
	}
	else {
		new                    = mmgrMonitorFreeNodes_g;
		mmgrMonitorFreeNodes_g = mmgrMonitorFreeNodes_g->rchild;
	}

	return new;
}

/**
 * Free up the memory allocated for monitor
 *
 * @param monitor The monitor whose memory to free up
 */
static inline void
monitor_free(monitor_t *monitor)
{
	monitor->rchild        = mmgrMonitorFreeNodes_g;
	mmgrMonitorFreeNodes_g = monitor;
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
bst_lookup(monitor_t *bst, Address object)
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
bst_insert(monitor_t *bst, monitor_t *monitor)
{
	monitor_t *tmp;

	if (bst == NULL)
		return monitor;

	tmp = bst;

	while (tmp) {
		assume(tmp->object != monitor->object);

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
bst_find_max(monitor_t *bst)
{
	if (bst == NULL)
		return NULL;

	while (bst->rchild) {
		bst = bst->rchild;
	}

	return bst;
}

/* Forward declaration */
static inline monitor_t* bst_remove(monitor_t *bst, Address object);

/**
 * Removes the root of the given subtree and returns a properly
 * modified tree.
 *
 * @note not tested
 * @param child The child to remove (root of a subtree)
 * @return The properly modified subtree
 */
static inline monitor_t*
bst_remove_child(monitor_t *child)
{
	monitor_t *tmp;

	if (child->lchild && child->rchild) {

		tmp            = bst_find_max(child->lchild);

		child->object  = tmp->object;
		child->owner   = tmp->owner;
		child->waiters = tmp->waiters;
#ifdef MMGR_QUEUE
		child->pending = tmp->pending;
#endif /* ifdef MMGR_QUEUE */
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
 * @note not tested
 * @param bst    The binary search tree root
 * @param object The object to remove
 * @return The resulting tree after the removal
 */
static inline monitor_t*
bst_remove(monitor_t *bst, Address object)
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
hashint(unsigned int a)
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
ht_hash(Address object)
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
ht_lookup(Address object)
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
ht_insert(monitor_t *monitor)
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
 * @note not tested
 * @param object The object to remove
 * @return 1 if it was found and removed or
 *         0 if not found
 */
static inline int
ht_remove(Address object)
{
	unsigned int index;

	index = ht_hash(object);

	if (mmgrHT_g[index] != NULL) {
		mmgrHT_g[index] = bst_remove(mmgrHT_g[index], object);

		return 1;
	}

	return 0;
}

/******************************************************************************
 * FIFO queue
 ******************************************************************************/

/**
 * q_dump
 *
 * @param queue The queue to dump
 */
static inline void
q_dump(wait_queue_t *queue)
{
	waiter_t *curr;

	curr = queue->head;

	kt_printf("-------------------- DUMP %p QUEUE --------------------\n",
	          queue);
	while (curr != NULL) {
		kt_printf("curr id %d:%d (%d)\n",
		          curr->id >> 3, curr->id & 7, curr->id);
		curr = curr->next;
	}
	kt_printf("-------------------- DUMP %p END   --------------------\n",
	          queue);
}

/**
 * q_enqueue
 *
 * @param queue The queue to enqueue to
 * @param id The id to enqueue
 * @return The updated queue
 */
static inline void
q_enqueue(wait_queue_t *queue, unsigned int id)
{
	waiter_t *waiter;
#if WAITER_REUSE
	int i;
	waiter_t *free;
#endif /* if WAITER_REUSE */

	ar_assert(queue);

	/* If there are available nodes in the free list use them */
	if (likely(mmgrWaiterFreeNodes_g != NULL)) {
		waiter                = mmgrWaiterFreeNodes_g;
		mmgrWaiterFreeNodes_g = mmgrWaiterFreeNodes_g->next;
	}
	/* Otherwise allocate some space */
	else {
		waiter = (waiter_t*)monitor_alloc();

#ifdef WAITER_REUSE
		/* HACK: Since a waiters queue node's size is 1/4 of the size of a
		 * monitor, we use the same allocation granularity and pack 4
		 * waiters in a single monitor */
		ar_assert(sizeof(monitor_t)/sizeof(waiter_t) >= 2);

		free = waiter+1;
		free->next = mmgrWaiterFreeNodes_g;

		/* The first waiter is going to be used and the last is directly
		 * assigned to the free-list, so we start from 2 */
		for (i = 2;
		     i < sizeof(monitor_t)/sizeof(waiter_t);
		     i++) {
			free++;
			free->next = free - 1;
		}
		mmgrWaiterFreeNodes_g = free;
#endif /* if WAITER_REUSE */
	}

	waiter->id   = id;
	waiter->next = NULL;

	/* Append to the end of the list */
	if (likely(queue->tail == NULL)) {
		ar_assert(queue->head == NULL);
		queue->head = waiter;
		queue->tail = waiter;
	} else {
		ar_assert(queue->head != NULL);
		queue->tail->next = waiter;
		queue->tail = waiter;
	}

	/* kt_printf("Enqueue\n");
	 * q_dump(queue); */
}

/**
 * q_remove
 *
 * @param queue The queue from which to remove the element with id
 * <code>id</code>
 * @param id    The id to remove
 */
static inline void
q_remove(wait_queue_t *queue, int id)
{
	waiter_t *prev;
	waiter_t *curr;

	curr = queue->head;
	prev = NULL;

	while (curr != NULL) {
		if (curr->id == id) {   /* Found it */

			if (queue->tail == curr) { /* It's on the tail */
				queue->tail = prev;
			}

			if (prev == NULL) { /* It's on the head */
				queue->head = curr->next;
			}

			curr->next            = mmgrWaiterFreeNodes_g;
			mmgrWaiterFreeNodes_g = curr;

			/* kt_printf("Remove\n");
			 * q_dump(queue); */
			return;
		}
		curr = curr->next;
	}
}

/**
 * q_dequeue
 *
 * @param queue The queue from which to dequeue an element
 * @return The id of the removed element or -1 if the queue is empty
 */
static inline int
q_dequeue(wait_queue_t *queue)
{
	waiter_t *to_free;
	int      ret;

	to_free = queue->head;

	if (!to_free)
		return -1;

	if (queue->tail == to_free)
		queue->tail = NULL;

	ret                   = to_free->id;
	queue->head           = queue->head->next;
	to_free->next         = mmgrWaiterFreeNodes_g;
	mmgrWaiterFreeNodes_g = to_free;

	/* kt_printf("Dequeue\n");
	 * q_dump(queue); */
	return ret;
}

/**
 * q_peek
 *
 * @param queue The queue from which to peek the top element
 * @return The id of the peeked element or -1 if the queue is empty
 */
static inline int
q_peek(wait_queue_t *queue)
{
	if (!queue->head)
		return -1;

	return queue->head->id;
}

/******************************************************************************
 * Interface Implementation (Client side)
 ******************************************************************************/

/**
 * Initialize the hashtable
 *
 * @param mmgrHT The allocated memory for the monitors' hashtable
 */
void
mmgrInitialize(mmgrGlobals *globals)
{
	*(unsigned int*)0x04400000 = 0;
	mmgr_g                     = globals;
	mmgrWaiterFreeNodes_g      = NULL;
	mmgrMonitorFreeNodes_g     = NULL;
#ifdef ARCH_MB
	mmgrAllocTop_g             = (Address) mm_slice_base(sysGetCore());
	mmgrAllocEnd_g             = mmgrAllocTop_g + MM_MB_SLICE_SIZE;
#else /* ifdef ARCH_MB */
	mmgrAllocTop_g             = (Address) mm_pa_kernel_base(sysGetCore());
	mmgrAllocEnd_g             = mmgrAllocTop_g + MM_PA_KERNEL_SIZE;
#endif /* ifdef ARCH_MB */

	kt_memset(mmgrHT_g, 0, sizeof(monitor_t*) * MMGR_HT_SIZE);
	mmgr_reset_stats();
}

/**
 * Find the manager responsible for the given object.
 *
 * @param hash   The object's hash
 * @param bid    The manager's bid (return)
 * @param cid    The manager's cid (return)
 */
static inline void
mmgrGetManager(unsigned int hash, int *bid, int *cid)
{
	/* We need only 3 bits since we have 2^3 monitor managers */
	unsigned int id3;

	/* TODO: Out-dated comment
	 * Objects are cache line aligned so we can ignore the 6 LS bits
	 * Then skip the next 3 bits so that neighbor objects don't end at the same
	 * monitor manager
	 */
	/* TODO: Use something that distributes objects from the same core as well */
	id3 = (hash >> 15) & 0x7;

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
mmgrRequest(mmpMsgOp_t msg_op, Address object)
{
	unsigned int msg0;
	int          target_cid;
	int          target_bid;
	unsigned int hash;

	hash = java_lang_Object_hashCode(object);

	/*
	 * Pass your bid and cid with the opcode so that the other end
	 * can check the owner.
	 */
	msg0 = (sysGetIsland() << 19) | (sysGetCore() << 16) | msg_op;

	mmgrGetManager(hash, &target_bid, &target_cid);

	mmpSend2(target_bid, target_cid, msg0, hash);
}

/**
 * Send a msg_op request regarding the object associated with the
 * given hash.  The responsible manager is automatically calculated
 * and the request is sent using a 2-word mailbox message.
 *
 * @param msg_op The desired operation
 * @param hash   The hash of the object to operate on
 */
void
mmgrRequestHash(mmpMsgOp_t msg_op, unsigned int hash)
{
	unsigned int msg0;
	int          target_cid;
	int          target_bid;

	/*
	 * Pass your bid and cid with the opcode so that the other end
	 * can check the owner.
	 */
	msg0 = (sysGetIsland() << 19) | (sysGetCore() << 16) | msg_op;

	mmgrGetManager(hash, &target_bid, &target_cid);

	mmpSend2(target_bid, target_cid, msg0, hash);
}

/**
 * Request to enter the given object's monitor.
 *
 * @param object The object to enter its monitor
 *
 */
void
mmgrMonitorEnter(Address object)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent an enter request for %p\n", object);
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

	assume(sc_in_heap(object));
	mmgrRequest(MMP_OPS_MNTR_ENTER, object);

	/* Empty our software cache.  There is no need to for writing back any
	dirty
	 * data, since they should be the result of a data race */
	/* sc_flush(); */
	sc_clear();

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
mmgrMonitorExit(Address object)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent an exit request\n");
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

	/* Write back any dirty data */
	sc_flush(SC_BLOCKING);

	/* assume(sc_in_heap(object)); */
	mmgrRequest(MMP_OPS_MNTR_EXIT, object);
}

/**
 * Request to exit the given object's monitor and notify the manager
 * we will be waiting on it.
 *
 * @param object The object to exit its monitor
 */
void
mmgrWaitMonitorExit(Address object)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent a wait exit request\n");
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

	/* Write back any dirty data */
	sc_flush(SC_BLOCKING);

/*	assume(sc_in_heap(object)); */
	mmgrRequest(MMP_OPS_MNTR_WAIT, object);
}

/**
 * Request to add me to the given object's waiters queue.
 *
 * @param object The object
 */
void
mmgrAddWaiter(Address object)
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
mmgrRemoveWaiter(Address object)
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
mmgrNotify(Address object, int all)
{
#  ifdef VERY_VERBOSE
	kt_printf("I sent a notify%s request\n", all ? " all" : "");
	ar_uart_flush();
#  endif /* ifdef VERY_VERBOSE */

	assume(sc_in_heap(object));
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
mmgrGetMonitor(Address object)
{
	monitor_t *res;

	/* Look-up the object's monitor */
	res = ht_lookup(object);

	/* If not found create a new one and associate it with this object */
	if (res == NULL) {

		res                  = monitor_alloc();
		assume(res != NULL);
		res->owner           = -1;
		res->waiters.head    = NULL;
		res->waiters.tail    = NULL;
#ifdef MMGR_QUEUE
		res->pending.head    = NULL;
		res->pending.tail    = NULL;
#endif /* ifdef MMGR_QUEUE */
		res->lchild          = NULL;
		res->rchild          = NULL;
#ifdef MMGR_STATS
		res->times_acquired  = 0;
		res->times_requested = 0;
#endif /* ifdef MMGR_STATS */
		res->object          = object;
		ht_insert(res);
	}

	return res;
}

/******************************************************************************
 * Interface Implementation (Server side)
 ******************************************************************************/

/**
 * Handle a monitor enter request.
 *
 * @param bid The board id we got the request from
 * @param cid The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
mmgrMonitorEnterHandler(int bid, int cid, Address object)
{
	monitor_t *monitor;

	/* kt_printf("I got an enter request for %p at %u from %d: %d\n",
	 *           object, sysGetTicks(), bid, cid); */

	monitor = mmgrGetMonitor(object);

#ifdef MMGR_STATS
	monitor_enter_reqs++;
	monitor->times_requested++;
	if (monitor_enter_reqs == MMGR_STATS_PERIOD) {
		mmgr_print_stats();
	}
#endif /* ifdef MMGR_STATS */

	/* If the monitor is available acquire it */
	if (monitor->owner == -1) {
		monitor->owner = (bid << 3) | cid;

#ifdef MMGR_STATS
		monitor_enters++;
#endif  /* MMGR_STATS */
		/* Reply back with the owner */
		mmpSend2(bid, cid,
		         (unsigned int)((monitor->owner << 16) | MMP_OPS_MNTR_ACK),
		         (unsigned int)object);

#ifdef MMGR_STATS
		monitor->times_acquired++;
		// Periodically print stats
		/* if ((monitor->times_acquired & 63) == 0) {
		 * 	printf("Monitor: %p Manager: %d Requested: %u Acquired: %u\n",
		 * 	       monitor->object, sysGetCore(),
		 * 	       monitor->times_requested, monitor->times_acquired);
		 * } */
#endif /* ifdef MMGR_STATS */
	}
	else {
		/* kt_printf("NOT Available %p at %u from %d: %d\n",
		 *           object, sysGetTicks(), bid, cid); */

#ifdef MMGR_QUEUE
		/* else add the requester to the queue holding the requesters
		 * waiting for this monitor */
		/* kt_printf("Queued enter request\n"); */
		q_enqueue(&monitor->pending, (bid << 3) | cid);
#else /* ifdef MMGR_QUEUE */
		/* Reply back with the owner */
		mmpSend2(bid, cid,
		         (unsigned int)((monitor->owner << 16) | MMP_OPS_MNTR_ACK),
		         (unsigned int)object);
#endif /* ifdef MMGR_QUEUE */
	}

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got an enter request for %p from %d:%d and the owner is %d:%d\n",
	    object, bid, cid, monitor->owner >> 3, monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */
}

/**
 * Handle a monitor exit request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 * @param iswait Whether this exit is a result of a wait() call
 */
void
mmgrMonitorExitHandler(int bid, int cid, Address object, int iswait)
{
	monitor_t *monitor;
	int       id;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got a%s exit request for %p from %d:%d and the owner is %d:%d\n",
	    iswait ? " wait" : "n", object, bid, cid, monitor->owner >> 3,
	    monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */

	ar_assert(monitor->owner == ((bid << 3) | cid));

	if (iswait) {
		mmgrAddWaiterHandler(bid, cid, object);
	}

#ifdef MMGR_QUEUE

	/* If there are pending threads give monitor to the next pending
	 * thread */
	if ((id = q_dequeue(&monitor->pending)) != -1) {
		/* kt_printf("There are pending monitors %p\n"); */
#ifdef MMGR_STATS
		monitor_enters++;
#endif /* ifdef MMGR_STATS */
		monitor->owner = id;
		bid            = id >> 3;
		cid            = id & 0x7;
		mmpSend2(bid, cid,
		         (unsigned int)((monitor->owner << 16) | MMP_OPS_MNTR_ACK),
		         (unsigned int)object);
	}
	else {
		monitor->owner = -1;
	}

#else /* ifdef MMGR_QUEUE */
	monitor->owner = -1;
#endif /* ifdef MMGR_QUEUE */

	/* TODO: Consider freeing the monitor if there are no pending or
	 * waiting threads */
}                  /* mmgrMonitorExitHandler */

/**
 * Handle a remove waiter request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
mmgrRemoveWaiterHandler(int bid, int cid, Address object)
{
	monitor_t *monitor;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got a remove waiter request for %p from %d:%d the owner is %d:%d\n",
	    object, bid, cid, monitor->owner >> 3, monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */

	q_remove(&monitor->waiters, (bid << 3) || cid);
}

/**
 * Handle a monitor exit request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
mmgrAddWaiterHandler(int bid, int cid, Address object)
{
	monitor_t *monitor;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got an add waiter request for %p from %d:%d the owner is %d:%d\n",
	    object, bid, cid, monitor->owner >> 3, monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */

	q_enqueue(&monitor->waiters, monitor->owner);

	return;
}                  /* mmgrAddWaiterHandler */

/**
 * Handle a monitor notify request.
 *
 * @param bid The board id we got the request from
 * @param cid The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
mmgrNotifyHandler(int bid, int cid, Address object, int all)
{
	monitor_t *monitor;
	int       waiter;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got a notify%s request for %p from %d:%d and the owner is %d:%d\n",
	    all ? " all" : "", object, bid, cid,
	    monitor->owner >> 3, monitor->owner & 7);
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
	ar_assert((monitor->owner == ((bid << 3) | cid)) || (monitor->owner == -1));

	/*
	 * If there is at least one waiter remove him from the waiters
	 * queue.
	 */
	do {
		waiter = q_dequeue(&monitor->waiters);

		if (waiter == -1) {
			break;
		}

		bid = waiter >> 3;
		cid = waiter & 7;

		/* kt_printf("I notify %d:%d\n", bid, cid); */
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
}

/**
 * Reader-Writer locks handling.  We keep 2 different queues per lock
 * (monitor_t): the readers queue (pending) and the writers queue
 * (writers).
 *
 * The readers queue holds the threads waiting to acquire the lock for
 * read.  The writers queue holds the threads waiting to acquire the
 * lock for write.
 *
 * As long as the writers' queue is empty the lock gets shared by
 * multiple readers.  When a write-lock request arrives, reader-lock
 * requests start being enqueued in the readers queue to give priority
 * to the write-lock request.  Subsequent write-locks get queued in
 * the writers' queue (if they are try locks a NACK is send back and
 * nothing is added to the queue).  When the writers' queue becomes
 * empty again the lock is passed to any waiting readers and so on.
 *
 * monitor->owner holds the write owner or the number of readers minus
 * one.  The monitor is completely free when its owner is set to -1.
 */


/**
 * Send a RW write (try) lock request.
 *
 * @param object The object on which we were requested to act
 * @param istry  Whether this is a try lock or not
 */
void
mmgrWriteLock(Address object, int istry)
{
#ifdef VERY_VERBOSE
	kt_printf("I sent a write%s lock request for %p\n", istry ? " try" : "",
	          object);
	ar_uart_flush();
#endif /* ifdef VERY_VERBOSE */

	mmgrRequest(istry ? MMP_OPS_RW_WRITE_TRY : MMP_OPS_RW_WRITE, object);

	/* If it is a try only empty our cache if we manage to acquire the lock
	 */
	/* if (!istry) { */
	 /* Empty our software cache.  There is no need to for writing back any
	  * dirty
	  * data, since they should be the result of a data race */
	 /* sc_flush(SC_BLOCKING); */
	 sc_clear();
	/* } */
}

/**
 * Send a RW read/write unlock request.
 *
 * @param object The object on which we were requested to act
 * @param isread Whether this was a read or a write lock
 */
void
mmgrReadLock(Address object, int istry)
{
#ifdef VERY_VERBOSE
	kt_printf("I sent a read%s lock request for %p\n", istry ? " try" : "",
	          object);
	ar_uart_flush();
#endif /* ifdef VERY_VERBOSE */

	mmgrRequest(istry ? MMP_OPS_RW_READ_TRY : MMP_OPS_RW_READ, object);

	/* If it is a try only empty our cache if we manage to acquire the lock
	*/
	/* if (!istry) { */
	 /* Empty our software cache.  There is no need to for writing back any
	  * dirty
	  * data, since they should be the result of a data race */
	 /* sc_flush(); */
	 sc_clear();
	/* } */
}

/**
 * Send a RW read (try) lock request.
 *
 * @param object The object on which we were requested to act
 * @param istry  Whether this is a try lock or not
 */
void
mmgrRWunlock(Address object, int isread)
{
#ifdef VERY_VERBOSE
	kt_printf("I sent a %s unlock request for %p\n", isread ? "read" : "write",
	          object);
	ar_uart_flush();
#endif /* ifdef VERY_VERBOSE */

	if (!isread) {
		/* Write back any dirty data */
		sc_flush(SC_BLOCKING);
	}

	mmgrRequest(isread ? MMP_OPS_RW_READ_UNLOCK : MMP_OPS_RW_WRITE_UNLOCK,
	            object);
}

/**
 * Handle a RW write try lock request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 * @param istry  Whether this is a try lock or not
 */
void
mmgrWriteLockHandler(int bid, int cid, Address object, int istry)
{
	monitor_t *monitor;

	monitor = mmgrGetMonitor(object);

#ifdef MMGR_STATS
	write_lock_reqs++;
	if (write_lock_reqs == MMGR_STATS_PERIOD) {
		mmgr_print_stats();
	}
#endif  /* ifdef MMGR_STATS */
#ifdef VERY_VERBOSE
	kt_printf(
	    "I got a write%s lock request for %p from %d:%d the owner is %d:%d (%d)\n",
	    istry ? " try" : "", object, bid, cid, monitor->owner >> 3,
		    monitor->owner & 7, monitor->owner);
#endif /* ifdef VERY_VERBOSE */

	/* If the monitor is available (neither read nor write acquired)
	 * acquire it */
	if (monitor->owner == -1) {
		ar_assert(monitor->pending.head == NULL);
		ar_assert(monitor->writers.head == NULL);
		monitor->owner = (bid << 3) | cid;

		/* Send ACK */
		mmpSend2(bid, cid,
		         (unsigned int)((monitor->owner << 16) | MMP_OPS_RW_WRITE_ACK),
		         (unsigned int)object);
	}
	/* Else if is a try lock send a NACK */
	else if (istry) {
		mmpSend2(bid, cid,
		         (unsigned int)((monitor->owner << 16) | MMP_OPS_RW_WRITE_NACK),
		         (unsigned int)object);

		return;
	}

#ifdef MMGR_STATS
	write_locks++;
#endif  /* MMGR_STATS */
	/* kt_printf("I enqueued a write%s lock request for %p from %d:%d the owner is %d:%d (%d)\n",
	 *           istry ? " try" : "", object, bid, cid, monitor->owner >> 3,
	 *           monitor->owner & 7, monitor->owner); */

	/* If it was not a failed try lock we must add it to the queue */
	q_enqueue(&monitor->writers, (bid << 3) | cid);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got the write%s lock request for %p from %d:%d the owner is %d:%d\n",
	    istry ? " try" : "", object, bid, cid, monitor->owner >> 3,
	    monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */
}

/**
 * Handle a monitor exit request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 * @param istry  Whether this is a try lock
 */
void
mmgrReadLockHandler(int bid, int cid, Address object, int istry)
{
	monitor_t *monitor;

	monitor = mmgrGetMonitor(object);

#ifdef MMGR_STATS
	read_lock_reqs++;
	if (read_lock_reqs == MMGR_STATS_PERIOD) {
		mmgr_print_stats();
	}
#endif  /* ifdef MMGR_STATS */

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got a read%s lock request for %p from %d:%d the owner is %d:%d\n",
	    istry ? " try" : "", object, bid, cid, monitor->owner >> 3,
	    monitor->owner & 7);
#endif /* ifdef VERY_VERBOSE */

	/* If the monitor is not write locked (or going to be), acquire it */
	if (monitor->writers.head == NULL) {
		monitor->owner++;

#ifdef MMGR_STATS
		read_locks++;
#endif  /* MMGR_STATS */

		/* Reply back with the owner */
		mmpSend2(bid, cid,
		         (unsigned int)((monitor->owner << 16) | MMP_OPS_RW_READ_ACK),
		         (unsigned int)object);
	}
	/* Else if is a try lock send a NACK */
	else if (istry) {
		mmpSend2(bid, cid,
		         (unsigned int)((monitor->owner << 16) | MMP_OPS_RW_READ_NACK),
		         (unsigned int)object);
	}
	/* Else add it to the queue */
	else {
		/* kt_printf("Queued enter request\n"); */
		q_enqueue(&monitor->pending, (bid << 3) | cid);
	}
}

/**
 * Handle a readers-writers unlock request.
 *
 * @param bid    The board id we got the request from
 * @param cid    The core id we got the request from
 * @param object The object on which we were requested to act
 * @param isread Whether it releases a reader or a writer lock
 */
void
mmgrRWunlockHandler(int bid, int cid, Address object, int isread)
{
	monitor_t *monitor;
	int       id;

	monitor = mmgrGetMonitor(object);

#ifdef VERY_VERBOSE
	kt_printf(
	    "I got a %s unlock request for %p from %d:%d the owner is %d:%d (%d)\n",
	    isread ? "read" : "write", object, bid, cid, monitor->owner >> 3,
	    monitor->owner & 7, monitor->owner);
#endif /* ifdef VERY_VERBOSE */

	ar_assert(monitor->owner > -1);

	/* If it was a read lock */
	if (isread) {
		/* Decrease readers' count and check if the monitor is now available */
		if (--monitor->owner == -1) {
			ar_assert(
			    monitor->pending.head == NULL || monitor->writers.head != NULL);

			/* If there are pending writer lock requests serve the first one */
			if ((id = q_peek(&monitor->writers)) != -1) {
				monitor->owner = id;
				bid            = id >> 3;
				cid            = id & 0x7;
				/* And notify the owner */
				mmpSend2(bid, cid,
				         (unsigned int)((id << 16) | MMP_OPS_RW_WRITE_ACK),
				         (unsigned int)object);
#ifdef VERY_VERBOSE
				kt_printf("%p is now owned by %d:%d (%d)\n",
				          object, monitor->owner >> 3,
				          monitor->owner & 7, monitor->owner);
#endif /* ifdef VERY_VERBOSE */
			}
		}
	}
	/* If it was a write lock */
	else {
		/* We must be the first in the writers queue */
		ar_assert(monitor->writers.head);
		ar_assert(monitor->writers.head->id == ((bid << 3) | cid));

		/* Remove ourself */
		q_dequeue(&monitor->writers);

		/* If there are pending writer lock requests serve the first one */
		if ((id = q_peek(&monitor->writers)) != -1) {
#ifdef MMGR_STATS
			write_locks++;
#endif /* ifdef MMGR_STATS */
			monitor->owner = id;
			/* And notify the owner */
			bid            = id >> 3;
			cid            = id & 0x7;
			mmpSend2(bid, cid,
			         (unsigned int)((id << 16) | MMP_OPS_RW_WRITE_ACK),
			         (unsigned int)object);
#ifdef VERY_VERBOSE
			kt_printf("%p is now owned by %d:%d (%d)\n",
			          object, monitor->owner >> 3,
			          monitor->owner & 7, monitor->owner);
#endif /* ifdef VERY_VERBOSE */
		}
		/* Else give the reader lock to the pending readers (if any) */
		else {
			monitor->owner = -1;

#ifdef VERY_VERBOSE
			kt_printf("No pending writers found giving %p to readers\n",
			          object);
#endif /* ifdef VERY_VERBOSE */
			while ((id = q_dequeue(&monitor->pending)) != -1) {
#ifdef MMGR_STATS
				read_locks++;
#endif /* ifdef MMGR_STATS */
				bid = id >> 3;
				cid = id & 0x7;
				monitor->owner++;
				/* Notify with the owner */
				mmpSend2(bid, cid,
				         (unsigned int)((id << 16) | MMP_OPS_RW_READ_ACK),
				         (unsigned int)object);
			}
		}
	}
}                  /* mmgrRWunlockHandler */
