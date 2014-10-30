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

#include "mmp_ops.h"
#include "mmgr.h"
#include "globals.h"
#include "myrmics.h"
#include "arch.h"

/******************************************************************************
 * Binary search tree hashtble chains
 ******************************************************************************/

static monitor_t *bst_lookup(monitor_t *bst, Address object) {

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

static monitor_t *bst_insert(monitor_t *bst, monitor_t *monitor) {
	monitor_t *tmp;

	if (bst == NULL)
		return monitor;

	tmp = bst;

	while (tmp) {
		ar_assert(tmp->object != monitor->object);
		if (tmp->object > monitor->object) {
			if (tmp->lchild)
				tmp = tmp->lchild;
			else
				tmp->lchild = monitor;
		}
		else {
			if (tmp->rchild)
				tmp = tmp->rchild;
			else
				tmp->rchild = monitor;
		}
	}

	return bst;
}

static inline monitor_t *bst_find_max(monitor_t *bst) {
	if (bst == NULL)
		return NULL;

	while (bst->rchild) {
		bst = bst->rchild;
	}

	return bst;
}

/* Forward declaration */
static monitor_t *bst_remove(monitor_t *bst, Address object);

static inline monitor_t *bst_remove_child(monitor_t *child) {
	monitor_t *tmp;

	if (child->lchild && child->rchild) {

		tmp = bst_find_max(child->lchild);

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

	// TODO: free it
	//free(tmp);

	return child;
}

static monitor_t *bst_remove(monitor_t *bst, Address object) {

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

/* Thomas Wang function that does it in 6 shifts (provided you use the
 * low bits, hash & (SIZE-1), rather than the high bits if you can't
 * use the whole value):
 */
static inline unsigned int hashint( unsigned int a) {
	a += ~(a<<15);
	a ^=  (a>>10);
	a +=  (a<<3);
	a ^=  (a>>6);
	a += ~(a<<11);
	a ^=  (a>>16);

	return a;
}

static unsigned int ht_hash(Address object) {
	unsigned int res;

	// Drop 6 LS bits, Objects are cache aligned
	// Keep the next 3 LS bits and drop the 3 after them (see mmgrGetManager)
	res = (unsigned int)object;
	res = ((res >> 12) << 3) | ((res >> 6) & 0x8);

	// Get the modulo
	res = hashint(res) & (MMGR_HT_SIZE - 1);

	return res;
}

static monitor_t *ht_lookup(Address object) {
	unsigned int index;
	Address      res;

	index = ht_hash(object);

	if (mmgrHT_g[index] != NULL)
		res = bst_lookup(mmgrHT_g[index], object);
	else
		res = NULL;

	return res;
}

static void ht_insert(monitor_t *monitor) {
	unsigned int index;

	index = ht_hash(monitor->object);

	if (mmgrHT_g[index] == NULL)
		mmgrHT_g[index] = monitor;
	else
		mmgrHT_g[index] = bst_insert(mmgrHT_g[index], monitor);

}

static int ht_remove(Address object) {
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
 */
void mmgrInitialize(monitor_t **mmgrHT) {
	mmgrHT_g = mmgrHT;

	kt_memset(mmgrHT_g, 0, sizeof(monitor_t*)*MMGR_HT_SIZE);
}

/**
 * Find the manager responsible for the given object.
 *
 * @param object The object
 * @param bid    The manager's bid (return)
 * @param cid    The manager's cid (return)
 */
INLINE void mmgrGetManager(Address object, int *bid, int *cid) {
	// We need only 3 bits since we have 2^3 monitor managers
	unsigned int id3;

	// Objects are cache line aligned so we can ignore the 6 LS bits
	// Then skip the next 3 bits so that neighbor objects don't end at the same
	// monitor manager
	id3 = (((unsigned int)object) >> 9) & 0x8;

#ifdef ARCH_MB
	*bid = 0x3F;
	*cid = id3;
#endif /* ARCH_MB */

#ifdef ARCH_ARM
	if (id3 & 1)
		*bid = 0x4B;
	else
		*bid = 0x6B;

	*cid = id3 >> 1;
#endif /* ARCH_ARM */

}

INLINE void mmpSend2(int to_bid, int to_cid,
                     unsigned int msg0, unsigned int msg1);

#ifdef ARCH_MB
/**
 * Send a msg_op request regarding the given object.  The responsible
 * manager is automatically calculated and the request is sent using a
 * 2-word mailbox message.
 *
 * @param msg_op The desired operation
 * @param object The object to operate on
 */
INLINE static void mmgrRequest(mmpMsgOp_t msg_op, Address object) {
	unsigned int msg0;
	int          target_cid;
	int          target_bid;

	// Pass your bid and cid with the opcode so that the other end
	// can check the owner.
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
INLINE void mmgrMonitorEnter(Address object) {
	mmgrRequest(MMP_OPS_MNTR_ENTER, object);

	// Remove me from the runnable threads and add me to the blocked
	// threads queue. This is done in VMThread.java
}

/**
 * Request to exit the given object's monitor.
 *
 * @param object The object to exit its monitor
 */
INLINE void mmgrMonitorExit(Address object) {
	mmgrRequest(MMP_OPS_MNTR_EXIT, object);

	// For monitor exits we don't need to wait for a reply, since the
	// request returns only when it gets an ACK we can safely assume
	// that the manager has the message in its queue and will
	// eventually process it.
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
INLINE monitor_t *mmgrGetMonitor(Address object) {
	monitor_t *res;

	// Look-up the object's monitor
	res = ht_lookup(object);

	// If not found create a new one and associate it with this object
	if (res == NULL) {
//FIXME
//		res         = new monitor;
		res->owner   = NULL;
		res->waiters = NULL;
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
INLINE void mmgrMonitorEnterHandler(int bid, int cid, Address object) {
	monitor_t    *monitor;

	monitor = mmgrGetMonitor(object);

	if (monitor->owner == NULL) {
		monitor->owner = (bid << 3) | cid;
	}

	// Reply back with the owner
	mmpSend2(bid, cid,
	         (unsigned int)( (monitor->owner << 16) | MMP_OPS_MNTR_ACK),
	         (unsigned int)object);
}

/**
 * Handle a monitor exit request.
 *
 * @param bid The board id we got the request from
 * @param cid The core id we got the request from
 * @param object The object on which we were requested to act
 */
INLINE void mmgrMonitorExitHandler(int bid, int cid, Address object) {
	monitor_t *monitor;

	monitor = mmgrGetMonitor(object);

	ar_assert( monitor->owner == ((bid<<3) | cid) );
	monitor->owner = NULL;

	// TODO What happens with the waiters?
}
