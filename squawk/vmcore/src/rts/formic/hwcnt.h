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
 * @file   hwcnt.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The interface for the formic hardware counters
 *
 * Each core has 128 hardware counters that support remove increase/decrease
 *
 * These counters are mostly used for DMA acknowledgements.
 */

#ifndef HWCNT_H_
#define HWCNT_H_

#include <arch.h>
#include <noc.h>
#include <platform.h>

/* We use counter 126 as the centralized barrier counter */
#define HWCNT_BARRIER_COUNTER 126
#define HWCNT_MAX_COUNTERS    126

typedef enum {
	HWCNT_FREE = 0,    /**< The counter is free/available */
	HWCNT_SC_FETCH,    /**< The counter is used to acknowledge a
	                    * software cache fetch */
	HWCNT_SC_PREFETCH, /**< The counter is used to acknowledge a
	                    * software cache fetch */
	HWCNT_SC_WB,       /**< The counter is used to acknowledge a
	                    * software cache write back */
	HWCNT_MMP_SEND1,   /**< The counter is used to acknowledge a
	                    * single word mailbox write */
	HWCNT_MMP_SEND2,   /**< The counter is used to acknowledge a
	                    * two-word mailbox write */
	HWCNT_RESERVED,    /**< The counter is reserved by the VM, i.e.,
	                    * for the barrier implementation */
} hwcnt_e;

#include <globals.h>

/* FIXME: OPTIMIZE (consider using lists) */

/**
 * hwcnt_next_free detects the next available hardware counter and
 * returns it.
 *
 * @return The first available counter
 *         or -1 if there is no available counter
 */
INLINE int
hwcnt_next_free ()
{
	int i;

	for (i = 0; i < HWCNT_MAX_COUNTERS; ++i) {
		if (hwcnts_g[i] == HWCNT_FREE)
			return i;
	}

	return -1;
}

/**
 * hwcnt_get_free returns an available hardware counter.  Caution,
 * this is a blocking operation since if there is no available
 * hardware counter it will wait for at least one pending transfer to
 * complete.
 *
 * @return An available hardware counter
 */
INLINE int
hwcnt_get_free (hwcnt_e usage)
{
	int cnt, i;

	cnt = hwcnt_next_free();

	/*
	 * If there is no available counter, go through them and check if
	 * there are any completed transfers
	 */
	while (cnt == -1) {
		for (i = 0; i < HWCNT_MAX_COUNTERS; ++i) {
			/*
			 * if the counter is zero the DMA finished and we can use
			 * the counter
			 */
			if (ar_cnt_get(sysGetCore(), i) == 0) {
				/* Mark the counter as available */
				hwcnts_g[i] = HWCNT_FREE;
				cnt         = i;
			}
		}
	}

	hwcnts_g[cnt] = usage;

	return cnt;
}

/**
 * hwcnt_wait_pending goes through all allocated, for the given
 * reason, counters and spins on them until they all become zero
 *
 * @param reason The given reason that the counters we want to spin on
 * were allocated for
 */
INLINE void
hwcnt_wait_pending (hwcnt_e reason)
{
	int cnt;

	/* go through the counters and spin on non zero */
	for (cnt = 0; cnt < HWCNT_MAX_COUNTERS; ++cnt) {
		if (hwcnts_g[cnt] == reason) {
			while (ar_cnt_get(sysGetCore(), cnt) != 0) {
				;
			}

			hwcnts_g[cnt] = HWCNT_FREE;
		}
	}
}

#endif /* HWCNT_H_ */
