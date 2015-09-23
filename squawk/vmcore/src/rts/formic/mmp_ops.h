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
 * The values should match the ones in MMP.java
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
	MMP_OPS_MNTR_NOTIFY=5,
	MMP_OPS_MNTR_NOTIFY_ALL=6,
	MMP_OPS_MNTR_ENTER=7,
	MMP_OPS_MNTR_EXIT=8,
	MMP_OPS_MNTR_WAIT=9,
	MMP_OPS_MNTR_WAITER_ADD=10,
	MMP_OPS_MNTR_WAITER_REMOVE=11,
	// Hash-Table
	MMP_OPS_HT_INSERT=12,
	MMP_OPS_HT_LOOKUP=13,
	MMP_OPS_HT_REMOVE=14,
	// Stack
	MMP_OPS_ST_PUSH=15,
	MMP_OPS_ST_POP=16,
	// Atomic Primitives
	MMP_OPS_AT_CAS=17,
	MMP_OPS_AT_CAS_ACK=18,
	MMP_OPS_AT_CAS_NACK=28,
	MMP_OPS_AT_SET=19,
	MMP_OPS_AT_SET_R=20,
	MMP_OPS_AT_GET=21,
	MMP_OPS_AT_GET_R=22,
	MMP_OPS_AT_FAD=23,
	MMP_OPS_AT_SWAP=24,
	MMP_OPS_AT_WRITE=24,
	// Synchronize
	MMP_OPS_SY_JOIN=26,
	MMP_OPS_SY_TERMINATE=27,
	// Reader/Writer locks
	MMP_OPS_RW_WRITE=30,
	MMP_OPS_RW_WRITE_TRY=31,
	MMP_OPS_RW_WRITE_ACK=32,
	MMP_OPS_RW_WRITE_NACK=33,
	MMP_OPS_RW_WRITE_UNLOCK=34,
	MMP_OPS_RW_READ=35,
	MMP_OPS_RW_READ_TRY=36,
	MMP_OPS_RW_READ_ACK=37,
	MMP_OPS_RW_READ_NACK=38,
	MMP_OPS_RW_READ_UNLOCK=39,
	// Monitor manager specials
	MMP_OPS_MMGR_RESET_STATS=40,
	MMP_OPS_MMGR_PRINT_STATS=41
} mmpMsgOp_t;

#endif /* _MMP_OPS_H */
