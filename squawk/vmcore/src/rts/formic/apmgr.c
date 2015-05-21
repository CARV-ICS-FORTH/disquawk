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
apmgrCASHandler (int bid, int cid, Address object,
                 unsigned int expected, unsigned int new)
{
	unsigned int tmp, res;
	Address tmpa;

	res = 0;
	tmpa = object;
	object = (Address)0x04400000; /* Test with an arbitrary address */
	tmp = *(unsigned int*)object;

	/* kt_printf("This is a CAS(%u, %u) from %d:%d ~ %u\n",
	 *           expected, new, bid, cid, tmp); */

	if ( tmp == expected ) {
		res = 1;
		*(unsigned int*)object = new;
	}
	/* Send back the reply */
	mmpSend2(bid, cid,
	         (unsigned int)(res ? MMP_OPS_AT_CAS_ACK : MMP_OPS_AT_CAS_NACK),
	         (unsigned int)tmpa);
}

/**
 * Handle an atomic set request.
 *
 * @param bid The board id we got the request from
 * @param cid The core id we got the request from
 * @param object The object on which we were requested to act
 * @param new The value we want to assign to the object
 */
void
apmgrSetHandler (int bid, int cid, Address object, unsigned int new)
{
	set_java_lang_Integer_value(object, new);

	/* Send back the reply (ACK) */
	mmpSend2(bid, cid,
	         (unsigned int)MMP_OPS_AT_SET_R,
	         (unsigned int)object);
}

/**
 * Handle an atomic get request.
 *
 * @param bid The board id we got the request from
 * @param cid The core id we got the request from
 * @param object The object on which we were requested to act
 */
void
apmgrGetHandler (int bid, int cid, Address object)
{
	unsigned int tmp, res;

	/* Send back the reply */
	mmpSend2(bid, cid,
	         (unsigned int)MMP_OPS_AT_GET_R,
	         (unsigned int)object);
}
