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

package allocate;

import java.lang.Thread;
import java.lang.OutOfMemoryError;

public class Main {

	public static void main(String[] args) throws InterruptedException {

		int     size  = 1024*1024;
		Worker  task  = new Worker(size);
		long    start, end;
		Thread  t;
		int[]   array;

		start = System.currentTimeMillis();
		end = System.currentTimeMillis();
		System.out.println("currentTimeMillis took me " + (end-start) + " ms");

		t = new Thread(task);

		t.start();

		while(size > 1024) {    // Leave some space for prints
			try {
				start = System.currentTimeMillis();
				array = new int[size];
				end = System.currentTimeMillis();
				System.out.println("It took me " + (end-start) +
				                   " ms to allocate " + (size*4)/1024 +
				                   "KBs");
			} catch(OutOfMemoryError e) {
				size /= 2;
			}
		}

		t.join();

		System.out.println("I am done");
	}

}
