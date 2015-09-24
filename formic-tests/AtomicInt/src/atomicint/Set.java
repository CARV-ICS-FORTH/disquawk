package atomicint;

import java.lang.Runnable;
import java.util.concurrent.atomic.*;
import com.sun.squawk.VM;

// On C level it goes up to 60-70 kops/sec with a single server

public class Set implements Runnable {

	AtomicInteger   myint;

	public Set() {
		myint  = new AtomicInteger();
	}

	public void run() {

		long start, end;

		// Start timer
		start = System.currentTimeMillis();

		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);
		myint.set(50);

		end = System.currentTimeMillis();

		System.out.println("W contention on atomic " + (end-start) +
		                   " ms BID: " + VM.getIsland() +
		                   " CID: " + VM.getCore());
	}

}
