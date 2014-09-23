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
 * @file   scheduler.c
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The scheduler
 *
 */

/**
 * Peeks a core to schedule the next thread.
 *
 * @param bid The board to schedule the next thread (output)
 * @param cid The core to schedule the next thread (output)
 */
void schdlr_next(int *bid, int *cid) {

	// FIXME: Make it work for various number of cores
	// Peek a core and a board (FIXME: do something smarter than RR)
	do {
		*cid = schedulerLastCore_g & 0x7;
		*bid = (schedulerLastCore_g >> 3) & 0x3F;
		schedulerLastCore_g++;
	} while ( (*bid >= 63) || // HACK disable board 64
	          ( (*cid == sysGetCore()) && (*bid == sysGetIsland()) ) );

	return;
}
