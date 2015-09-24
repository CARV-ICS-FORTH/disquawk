package atomicint;

import java.lang.Runnable;
import java.util.concurrent.atomic.*;
import com.sun.squawk.VM;

// On C level it goes up to 60-70 kops/sec with a single server

public class Get implements Runnable {

	AtomicInteger   myint;

	public Get() {
		myint  = new AtomicInteger();
		myint.set(50);
	}

	public void run() {

		long start, end;

		// Start timer
		start = System.currentTimeMillis();

		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();
		myint.get();

		end = System.currentTimeMillis();

		System.out.println("R contention on atomic " + (end-start) +
		                   " ms BID: " + VM.getIsland() +
		                   " CID: " + VM.getCore());
	}

}
