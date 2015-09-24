package atomicint;

import java.lang.Thread;

/**
 * Create new threads and start them. Then from the new threads
 * atomically increase a shared int.
 */

public class Main {

	public static void main(String[] args) throws InterruptedException {

		GetAndIncr gstask = new GetAndIncr();
		Get        gtask  = new Get();
		Set        stask  = new Set();
		int        tasks = 503;
		int        res;
		long       start, end;
		Thread[]   t;

		t = new Thread[tasks];

		for (int i = 0; i<t.length; i++) {
			if (i%2 == 0)
				t[i] = new Thread(stask);
			else
				t[i] = new Thread(gtask);
		}

		start = System.currentTimeMillis();

		for (int i = 0; i<t.length; i++) {
			t[i].start();
		}

		for (int i = 0; i<t.length; i++) {
			t[i].join();
		}
		end = System.currentTimeMillis();

		System.out.println("Get and Set mix took me " + (end-start) + " ms");

		// ////////////////////////////////////////////////////////////////////////

		// for (int i = 0; i<t.length; i++) {
		// 	t[i] = new Thread(gtask);
		// }

		// start = System.currentTimeMillis();

		// for (int i = 0; i<t.length; i++) {
		// 	t[i].start();
		// }

		// for (int i = 0; i<t.length; i++) {
		// 	t[i].join();
		// }
		// end = System.currentTimeMillis();

		// System.out.println("Get took me " + (end-start) + " ms");

		// ////////////////////////////////////////////////////////////////////////

		// for (int i = 0; i<t.length; i++) {
		// 	t[i] = new Thread(gstask);
		// }

		// start = System.currentTimeMillis();

		// for (int i = 0; i<t.length; i++) {
		// 	t[i].start();
		// }

		// // while((res = task.myint.get()) < tasks*100) {
		// // 	System.out.println("myint = " + res);
		// // }

		// // end = System.currentTimeMillis();

		// // System.out.println("It took me " + (end-start) + " ms");
		// // System.out.println("myint = " + task.myint.get());

		// // start = System.currentTimeMillis();
		// for (int i = 0; i<t.length; i++) {
		// 	t[i].join();
		// }
		// end = System.currentTimeMillis();

		// System.out.println("GetAndIncrement took me " + (end-start) + " ms");

		// ////////////////////////////////////////////////////////////////////////

		// for (int i = 0; i<t.length; i++) {
		// 	t[i] = new Thread(stask);
		// }

		// start = System.currentTimeMillis();

		// for (int i = 0; i<t.length; i++) {
		// 	t[i].start();
		// }

		// for (int i = 0; i<t.length; i++) {
		// 	t[i].join();
		// }
		// end = System.currentTimeMillis();

		// System.out.println("Set took me " + (end-start) + " ms");

		// ////////////////////////////////////////////////////////////////////////

		System.out.println("I am done");
	}

}
