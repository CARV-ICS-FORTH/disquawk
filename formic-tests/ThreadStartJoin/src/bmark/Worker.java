package bmark;

import java.lang.Runnable;

public class Worker implements Runnable {

	public void run() {
		// Do nothing in order to measure the thread start-join overhead
		// System.out.println("I am in The time is " + System.currentTimeMillis() + " ms");
	}

}
