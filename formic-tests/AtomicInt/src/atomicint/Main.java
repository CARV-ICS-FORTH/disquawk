package atomicint;

import java.lang.Thread;

/**
 * Create 8 new threads and start them. Then from the new threads
 * atomically increase a shared int.
 */

public class Main {

	public static void main(String[] args) throws InterruptedException {

		Worker task = new Worker();
		Thread t;
		int    tasks = 63;
		int    res;
		long   start, end;

		// Start timer
		start = System.currentTimeMillis();

		for (int i = 0; i<tasks; i++) {
			t = new Thread(task);
			t.start();
		}

		// FIXME: join is not ready yet
		// t1.join();
		// t2.join();
		// t3.join();
		// t4.join();
		// t5.join();
		// t6.join();
		// t7.join();
		// t8.join();

		while((res = task.myint2.get()) != tasks*10) {
			System.out.println("myint2 = " + res);
		}

		end = System.currentTimeMillis();

		System.out.println("It took me " + (end-start) + " ms");
		System.out.println("myint2 = " + task.myint2.get());
		System.out.println(
			"I am done (This should be printed after the Hello messages)");

		while(true) {
			;
		}
	}

}
