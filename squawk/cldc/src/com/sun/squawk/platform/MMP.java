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
 * @file   MMP.java
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief An interface for the Mailbox Message Protocol.
 *
 */
package com.sun.squawk.platform;

import com.sun.squawk.*;
import com.sun.squawk.util.*;
import com.sun.squawk.pragma.*;

public final class MMP {

	/* Must be the same as in mmp_ops.h */
	public static final int OPS_NOP                   = 0;
	public static final int OPS_TH_SPAWN              = 1;
	public static final int OPS_MNTR_ACK              = 2;
	public static final int OPS_MNTR_NOTIFICATION     = 3;
	public static final int OPS_MNTR_NOTIFICATION_ALL = 4;
	public static final int OPS_AT_CAS_ACK            = 18;
	public static final int OPS_AT_CAS_NACK           = 28;

	public static final int OPS_RW_WRITE              = 30;
	public static final int OPS_RW_WRITE_TRY          = 31;
	public static final int OPS_RW_WRITE_ACK          = 32;
	public static final int OPS_RW_WRITE_NACK         = 33;
	public static final int OPS_RW_WRITE_UNLOCK       = 34;
	public static final int OPS_RW_READ               = 35;
	public static final int OPS_RW_READ_TRY           = 36;
	public static final int OPS_RW_READ_ACK           = 37;
	public static final int OPS_RW_READ_NACK          = 38;
	public static final int OPS_RW_READ_UNLOCK        = 39;

	/**
	 * Query the mailbox for incoming messages and return a thread object
	 * if one of the messages was about scheduling a thread to this core.
	 *
	 * @param  type The type of the received message (return)
	 *
	 * @return a thread object if one of the messages was about scheduling
	 *         a thread to this core,
	 *         an object if one of the messages was a reply to a lock request
	 *         NULL otherwise
	 */
	public static Object checkMailbox(Integer type, Integer hash) throws NativePragma {
		throw Assert.shouldNotReachHere("unimplemented when hosted");
	}

	/**
	 * Peeks a core to schedule the given thread.  Then sends a
	 * message to this core with the thread object's address and the
	 * core and island of the hardware thread that spawned it.
	 */
	public static void spawnThread(Thread thread) throws NativePragma {
		throw Assert.shouldNotReachHere("unimplemented when hosted");
	}

	/**
	 * Reset the monitor manager statistics counters. Use with -DMMGR_STATS
	 */
	public static void mmgrResetStats(int id) throws NativePragma {
		throw Assert.shouldNotReachHere("unimplemented when hosted");
	}

	/**
	 * Print the monitor manager statistics counters. Use with -DMMGR_STATS
	 */
	public static void mmgrPrintStats(int id) throws NativePragma {
		throw Assert.shouldNotReachHere("unimplemented when hosted");
	}

}
