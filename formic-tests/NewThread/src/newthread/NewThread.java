package newthread;

import java.lang.Thread;

/**
 * Create 8 new threads and start them. Then from the new threads
 * print our ID, they should be running on a different core than the
 * one running the main.
 */

public class NewThread {

	public static void main(String[] args) {

		Hello task = new Hello();
		Thread t1 = new Thread(task);
		Thread t2 = new Thread(task);
		Thread t3 = new Thread(task);
		Thread t4 = new Thread(task);
		Thread t5 = new Thread(task);
		Thread t6 = new Thread(task);
		Thread t7 = new Thread(task);
		Thread t8 = new Thread(task);

		t1.start();
		t2.start();
		t3.start();
		t4.start();
		t5.start();
		t6.start();
		t7.start();
		t8.start();
		// FIXME: join is not ready yet
		//t1.join();
		//t2.join();
		//t3.join();
		//t4.join();
		//t5.join();
		//t6.join();
		//t7.join();
		//t8.join();
		while(true);
	}

}
