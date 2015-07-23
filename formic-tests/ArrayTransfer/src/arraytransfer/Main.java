/*
 * Copyright 2015 FORTH-ICS / CARV
 *                   (Foundation for Research & Technology -- Hellas,
 *                    Institute of Computer Science,
 *                    Computer Architecture & VLSI Systems Laboratory)
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
 * @file   Main.java
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief Create a new thread and try to allocate the maximum possible
 * size from both the master and the worker
 */

package arraytransfer;

import java.lang.Thread;
import java.lang.OutOfMemoryError;

public class Main {

	public static void main(String[] args) throws InterruptedException {

		int       tasks = 503;
		int       size  = 1024;
		Worker    task;
		long      start, end;
		Thread[]  t;
		int[]     array;
		boolean   ok = true;

		start = System.currentTimeMillis();
		end = System.currentTimeMillis();
		System.out.println("currentTimeMillis took me " + (end-start) + " ms");

		start = System.currentTimeMillis();
		array = new int[size];
		end = System.currentTimeMillis();

		System.out.println("It took me " + (end-start) +
		                   " ms to allocate " + (size*4)/1024 +
		                   "KBs");

		task = new Worker(size, array);
		t = new Thread[tasks];

		for (int i=0; i<tasks; i++) {
			t[i] = new Thread(task);
		}

		for (int i=0; i<size; i++) {
			array[i] = size-i;
		}

		for (int i=0; i<tasks; i++) {
			t[i].start();
		}

		for (int i=0; i<tasks; i++) {
			t[i].join();
		}

		for (int i=0; i<size; i++) {
			if (array[i] != size-i) {
				System.out.println("FAILED at ["+i+"]");
				ok = false;
			}
		}

		ok = ok && task.ok;

		if (ok)
			System.out.println("[1;32mPASS[0m");
		else
			System.out.println("[1;31mFAIL[0m");
	}

}
