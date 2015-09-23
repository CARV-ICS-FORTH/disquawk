package monitorbench;

import java.lang.Thread;

/**
 * Create new threads and start them. Then from the new threads
 * atomically increase a shared int.
 */

public class Main {

	public static void main(String[] args) throws InterruptedException {

		int       artificial_work = 0;
		SameLock  stask = new SameLock(artificial_work);
		OtherLock otask = new OtherLock(artificial_work);
		int       tasks = 503;
		int       res;
		long      start, end;
		Thread[]  t;

		System.out.println("Running monitorBench with artificial_work set to "
		                   + artificial_work + " and " + tasks + " tasks");

		t = new Thread[tasks];

		// First content on the same lock
		for (int i = 0; i<t.length; i++) {
			t[i] = new Thread(stask);
		}

		start = System.currentTimeMillis();

		for (int i = 0; i<t.length; i++) {
			t[i].start();
		}
		for (int i = 0; i<t.length; i++) {
			t[i].join();
		}
		end = System.currentTimeMillis();

		System.out.println("Total time on the same lock " + (end-start) + " ms");

		// Then try locking a different lock
		for (int i = 0; i<t.length; i++) {
			t[i] = new Thread(otask);
		}

		start = System.currentTimeMillis();

		for (int i = 0; i<t.length; i++) {
			t[i].start();
		}

		for (int i = 0; i<8; i++) {
			MMP.mmgrResetStats(i);
		}

		for (int i = 0; i<t.length; i++) {
			t[i].join();
		}
		end = System.currentTimeMillis();

		System.out.println("Total time on different locks " + (end-start) + " ms");

		System.out.println("I am done");
	}

}
