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

extern INLINE int  hwcnt_next_free ();
extern INLINE int  hwcnt_get_free (hwcnt_e usage);
extern INLINE void hwcnt_wait_pending (hwcnt_e reason);

#endif /* HWCNT_H_ */
