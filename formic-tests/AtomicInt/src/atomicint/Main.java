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

		for (int i = 0; i<500; i++) {
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

		// System.out.println(
		// 	"I am done (This should be printed after the Hello messages)");
		while(true);
	}

}
