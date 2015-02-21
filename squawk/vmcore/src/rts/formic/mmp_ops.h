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
 * @file   mmp_ops.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * Header file for the Mailbox Message Protocol (MMP)
 */

#ifndef _MMP_OPS_H
#define _MMP_OPS_H

/* NOTE:
 * This enum can only take values up to 64, because it is getting
 * packed in the messages and there are only 6 bits reserved for it
 *
 * The values should much the ones in MMP.java
 */
typedef enum {
	// nop
	MMP_OPS_NOP=0,
	// Threads
	MMP_OPS_TH_SPAWN=1,
	// Monitors
	MMP_OPS_MNTR_ACK=2,
	MMP_OPS_MNTR_NOTIFICATION=3,
	MMP_OPS_MNTR_NOTIFICATION_ALL=4,
	MMP_OPS_AT_CAS_R=5,
	MMP_OPS_MNTR_NOTIFY,
	MMP_OPS_MNTR_NOTIFY_ALL,
	MMP_OPS_MNTR_ENTER,
	MMP_OPS_MNTR_EXIT,
	MMP_OPS_MNTR_WAIT,
	MMP_OPS_MNTR_WAITER_ADD,
	MMP_OPS_MNTR_WAITER_REMOVE,
	// Hash-Table
	MMP_OPS_HT_INSERT,
	MMP_OPS_HT_LOOKUP,
	MMP_OPS_HT_REMOVE,
	// Stack
	MMP_OPS_ST_PUSH,
	MMP_OPS_ST_POP,
	// Atomic Primitives
	MMP_OPS_AT_CAS,
	MMP_OPS_AT_FAD,
	MMP_OPS_AT_SWAP,
	MMP_OPS_AT_WRITE,
	// Synchronize
	MMP_OPS_SY_JOIN,
	MMP_OPS_SY_TERMINATE,
} mmpMsgOp_t;

#endif /* _MMP_OPS_H */
