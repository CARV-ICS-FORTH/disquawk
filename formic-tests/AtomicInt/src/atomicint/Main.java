package atomicint;

import java.lang.Thread;

/**
 * Create new threads and start them. Then from the new threads
 * atomically increase a shared int.
 */

public class Main {

	public static void main(String[] args) throws InterruptedException {

		Worker   task  = new Worker();
		int      tasks = 503;
		int      res;
		long     start, end;
		Thread[] t;

		t = new Thread[tasks];

		for (int i = 0; i<t.length; i++) {
			t[i] = new Thread(task);
		}

		// Start timer
		// start = System.currentTimeMillis();
		// end = System.currentTimeMillis();
		// System.out.println("currentTimeMillis took me " + (end-start) + " ms");

		start = System.currentTimeMillis();

		for (int i = 0; i<t.length; i++) {
			// start = System.currentTimeMillis();
			t[i].start();
			// end = System.currentTimeMillis();
			// System.out.println("start took me " + (end-start) + " ms");
		}

		while((res = task.myint.get()) < tasks*100) {
			System.out.println("myint = " + res);
		}

		end = System.currentTimeMillis();

		System.out.println("It took me " + (end-start) + " ms");
		System.out.println("myint = " + task.myint.get());

		start = System.currentTimeMillis();
		for (int i = 0; i<t.length; i++) {
			t[i].join();
		}
		end = System.currentTimeMillis();

		System.out.println("Join took me " + (end-start) + " ms");

		System.out.println("I am done");
	}

}
