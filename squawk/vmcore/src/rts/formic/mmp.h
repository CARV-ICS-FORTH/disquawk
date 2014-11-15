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
 * @file   mmp.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The interface of the Mailbox Message Protocol (MMP)
 *
 */

#ifndef RTS_FORMIC_MMP_H
#define RTS_FORMIC_MMP_H

#include <platform.h>
#include <address.h>
#include "mmp_ops.h"
#include "hwcnt.h"

void    mmpSpawnThread (Address thread);
Address mmpCheckMailbox (Address type);

/**
 * Send a single word mailbox message to the given core.
 *
 * @param to_bid The target core's board ID
 * @param to_cid The target core's core ID
 * @param msg    The word to send
 */
INLINE void
mmpSend (int to_bid, int to_cid, unsigned int msg)
{
	int cnt;
	int ret;

	cnt           = hwcnt_get_free(HWCNT_MMP_SEND1);

	do {
		/* kt_printf("Sending thread %p to 0x%02X/%d\n",
		 *           thread, target_bid, target_cid); */
		/* Send to dst mailbox, keeping track with a local counter */
		ar_cnt_set(sysGetCore(), cnt, -4);
		ar_mbox_send_ack(sysGetCore(), to_bid, to_cid,
		                 sysGetIsland(), sysGetCore(), cnt, msg);

		/* Spin until counter notification arrives FIXME: make it non blocking
		**/
		while ( (ret = ar_cnt_get_triggered(sysGetCore(), cnt)) == 0) {
			;
		}
	} while (ret == 3); /* Retry on Nack */

	/* kt_printf("Sent\n"); */
	assume(ret == 2);   /* Ack */
	assume(ar_cnt_get(sysGetCore(), cnt) == 0);
	hwcnts_g[cnt] = HWCNT_FREE;
}
/**
 * Send a 2-word mailbox message to the given core.
 *
 * @param to_bid The target core's board ID
 * @param to_cid The target core's core ID
 * @param msg0   The first word to send
 * @param msg1   The second word to send
 */
INLINE void
mmpSend2 (int to_bid, int to_cid, unsigned int msg0, unsigned int msg1)
{
	int cnt;
	int ret;

	cnt           = hwcnt_get_free(HWCNT_MMP_SEND2);
	ret           = 0;

	do {
		/* Send to dst mailbox, keeping track with a local counter */
		ar_cnt_set(sysGetCore(), cnt, -8);
		ar_mbox_send2_ack(sysGetCore(), to_bid, to_cid,
		                  sysGetIsland(), sysGetCore(), cnt, msg0, msg1);

		/* Spin until counter notification arrives FIXME: make it non
		 * blocking */
		while ( (ret = ar_cnt_get_triggered(sysGetCore(), cnt)) == 0) {
			;
		}
	} while (ret == 3);

	assume(ret == 2); /* Ack */
	assume(ar_cnt_get(sysGetCore(), cnt) == 0);
	hwcnts_g[cnt] = HWCNT_FREE;
}
#endif /* RTS_FORMIC_MMP_H */
