/*
 * Copyright    2015, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Oracle Corporation, 500 Oracle Parkway, Redwood
 * Shores, CA 94065 or visit www.oracle.com if you need additional
 * information or have any questions.
 */

package com.sun.squawk;

public class Debug {
	/**
	 * The debug level
	 *
	 * 0: no debug
	 * 1: info
	 * 2. debug prints (verbose)
	 * 3. more debug prints (very verbose)
	 */
	private static int level = 1;

	public static void setlevel (int level) {
		Debug.level = level;
	}

	/**
	 * Prints msg if invoked by the master core
	 */
	public static void pinfo (String msg) {
		if (level > 0 &&
		    VM.getCore() == 0 &&
		    VM.getIsland() == 0) {
			VM.print("[INFO] ");
			VM.println(msg);
		}
	}

	/**
	 * Prints msg depending on the debug level.
	 *
	 * On info level prints only messages from the master core (like
	 * pinfo).
	 *
	 * On verbose level prints messages from the first core of every
	 * board.
	 *
	 * On veryverbose level prints messages from all cores.
	 */
	public static void pdebug (String msg) {
		if ((level > 0 && VM.getCore() == 0 && VM.getIsland() == 0) ||
		    (level > 1 && VM.getCore() == 0) ||
		    (level > 2)) {
			VM.print("[DBG]  ");
			VM.println(msg);
		}
	}

	/**
	 * Prints msg depending on the debug level.
	 *
	 * On info level prints only messages from the master core (like
	 * pinfo).
	 *
	 * On verbose level prints messages from the first core of every
	 * board.
	 *
	 * On veryverbose level prints messages from all cores.
	 */
	public static void pdebug3 (String msg) {
		if ((level > 2)) {
			VM.print("[DBG3] ");
			VM.println(msg);
		}
	}

}
