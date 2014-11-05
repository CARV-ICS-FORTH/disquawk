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
 * @file   hwcnt.c
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief Helper functions for the formic hardware counters
 */

#include <globals.h>
#include <arch.h>

#include "hwcnt.h"

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

	cnt           = hwcnt_next_free();

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
 * hwcnt_wait_pending
 *
 * @param reason TODO
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