package bmark;

import java.lang.Thread;

/**
 * Create new threads and start them. Then from the new threads
 * atomically increase a shared int.
 */

public class Main {

	public static void main(String[] args) throws InterruptedException {

		Worker   task  = new Worker();
		int      tasks = 1;
		long     start, end;
		Thread[] t;

		t = new Thread[tasks];

		for (int i = 0; i<tasks; i++) {
			t[i] = new Thread(task);
		}

		start = System.currentTimeMillis();

		for (int i = 0; i<tasks; i++) {
			t[i].start();
		}
		for (int i = 0; i<tasks; i++) {
			t[i].join();
		}
		end = System.currentTimeMillis();

		System.out.println("Thread Start-Join took me " + (end-start) + " ms");

		System.out.println("I am done");
	}

}
