/*
 * Copyright 2013-2014 FORTH-ICS / CARV
 *                     (Foundation for Research & Technology -- Hellas,
 *                      Institute of Computer Science,
 *                      Computer Architecture & VLSI Systems Laboratory)
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
 * @file   mmgr.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * Header file for the Monitors and the Monitors Manager implementations
 */

#ifndef MMGR_H_
#define MMGR_H_

#include <arch.h>
#include <address.h>
#include "mmp_ops.h"

#define MMGR_HT_SIZE           128
#define mmgrHT_g               mmgr_g->mmgrHT
#define mmgrMonitorFreeNodes_g mmgr_g->mmgrMonitorFreeNodes
#define mmgrWaiterFreeNodes_g  mmgr_g->mmgrWaiterFreeNodes
#define mmgrAllocTop_g         mmgr_g->mmgrAllocTop
#define mmgrAllocEnd_g         mmgr_g->mmgrAllocEnd

typedef struct wait_node waiter_t;

struct wait_node {
	unsigned int id; /**< The waiter's ID along with a bit denoting if
	                  * this struct is written on the first part of a
	                  * memmory chunk allocated by monitor_malloc()
	                  * packed as (board_ID << 3) | (core_ID) */
	waiter_t *next;  /**< Pointer to the next waiter in the
	                  * queue */
};

typedef struct wait_queue wait_queue_t;

struct wait_queue {
	waiter_t *head;
	waiter_t *tail;
};

typedef struct monitor monitor_t;

struct monitor {
	void *object;  /**< The object's reference coupled
	                * with this monitor */
	int  owner;    /**< The owner's ID packed as
	                * (board_ID << 3) | (core_ID) */
	union {
		wait_queue_t waiters; /**< Pointer to the list of waiters
		                       * (wait/notify) */
		wait_queue_t writers; /**< Pointer to the list of waiters
		                       * (wait/notify) */
	};
	wait_queue_t pending; /**< Pointer to the list of pending
	                       * requesters of this monitor */
	monitor_t    *lchild; /**< Pointer to the left child in the
	                       * Monitor Manager's monitor binary search
	                       * tree */
	monitor_t *rchild;    /**< Pointer to the right child in the
	                       * Monitor Manager's monitor binary
	                       * search tree */
#ifdef MMGR_STATS
	unsigned int times_acquired;  /**< Measuring the times this monitor was acquired */
	unsigned int times_requested; /**< Measuring the times this monitor was acquired */
#endif /* ifdef MMGR_STATS */
};

#ifdef MMGR_STATS
extern void mmgr_reset_stats();
extern void mmgr_print_stats();
#else
#define mmgr_reset_stats()
#define mmgr_print_stats()
#endif  /* MMGR_STATS */

typedef struct mmgrGlobalsStruct {
	monitor_t *mmgrHT[MMGR_HT_SIZE];
	monitor_t *mmgrMonitorFreeNodes;
	waiter_t  *mmgrWaiterFreeNodes;
	Address   mmgrAllocTop;
	Address   mmgrAllocEnd;
} mmgrGlobals
#ifdef __MICROBLAZE__
__attribute__((aligned(MM_CACHELINE_SIZE)))
#endif /* ifdef __MICROBLAZE__ */
;

void mmgrInitialize(mmgrGlobals *globals);

#ifdef ARCH_MB
void mmgrRequestHash(mmpMsgOp_t msg_op, unsigned int hash);
void mmgrMonitorEnter(Address object);
void mmgrMonitorExit(Address object);
void mmgrWaitMonitorExit(Address object);
void mmgrAddWaiter(Address object);
void mmgrRemoveWaiter(Address object);
void mmgrNotify(Address object, int all);

#endif /* ARCH_MB */
void mmgrMonitorEnterHandler(int bid, int cid, Address object);
void mmgrMonitorExitHandler(int bid, int cid, Address object, int iswait);
void mmgrRemoveWaiterHandler(int bid, int cid, Address object);
void mmgrAddWaiterHandler(int bid, int cid, Address object);
void mmgrNotifyHandler(int bid, int cid, Address object, int all);

void mmgrWriteLock(Address object, int istry);
void mmgrRWunlock(Address object, int istry);
void mmgrReadLock(Address object, int isread);
void mmgrWriteLockHandler(int bid, int cid, Address object, int istry);
void mmgrReadLockHandler(int bid, int cid, Address object, int istry);
void mmgrRWunlockHandler(int bid, int cid, Address object, int isread);

#endif /* MMGR_H_ */
