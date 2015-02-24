/*
 * Copyright      (C) 2015 FORTH-ICS / CARV
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
 * @file   apmgr.c
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The implementation of the Atomic Primitives Manager (APMGR)
 *
 */
#include <myrmics.h>
#include <util.h>
#include "mmgr.h"
#include "mmp.h"
#include "globals.h"
#include "arch.h"

/**
 * Handle a compare-and-swap request.
 *
 * @param bid The board id we got the request from
 * @param cid The core id we got the request from
 * @param object The object on which we were requested to act
 * @param expected The value we expect the object to have
 * @param new The value we want to assign to the object
 */
void
apmgrCASHandler (int bid, int cid, Address object, int expected, int new)
{
	int tmp, res;

	res = 0;
	tmp = java_lang_Integer_value(object);

	if ( tmp == expected ) {
		set_java_lang_Integer_value(object, new);
	}
	/* Send back the reply */
	mmpSend2(bid, cid,
	         (unsigned int)(res << 16 | MMP_OPS_AT_CAS_R),
	         (unsigned int)object);
}
