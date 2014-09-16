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
 * @file   SoftwareCache.java
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

	/**
	 * Query the mailbox for incoming messages and return a thread object
	 * if one of the messages was about scheduling a thread to this core.
	 *
	 * @return a thread object if one of the messages was about scheduling
	 *         a thread to this core or NULL otherwise
	 */
	public static VMThread checkMailbox() throws NativePragma {
		throw Assert.shouldNotReachHere("unimplemented when hosted");
	}

	/**
	 * Peeks a core to schedule the given thread.  Then sends a
	 * message to this core with the thread object's address and the
	 * core and island of the hardware thread that spawned it.
	 */
	public static void spawnThread(VMThread thread) throws NativePragma {
		throw Assert.shouldNotReachHere("unimplemented when hosted");
	}

}
