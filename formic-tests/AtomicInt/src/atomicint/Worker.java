package atomicint;

import java.lang.Runnable;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicInteger2;
import com.sun.squawk.*;

// On C level it goes up to 60-70 kops/sec with a single server

public class Worker implements Runnable {

	AtomicInteger   myint;
	AtomicInteger2  myint2;

	public Worker() {
		myint  = new AtomicInteger();
		myint2 = new AtomicInteger2();
	}

	public void run() {
		int addr  = Address.fromObject(myint).toUWord().toPrimitive();
		int addr2 = Address.fromObject(myint2).toUWord().toPrimitive();

		// Greetings
		System.out.println("Hello World I am cid=" +
		                   NativeUnsafe.getCore() +
		                   " bid=" +
		                   NativeUnsafe.getIsland() +
		                   " myint= " +
		                   Integer.toHexString(addr) +
		                   " myint2= " +
		                   Integer.toHexString(addr2) );


		int current = myint.get();
		long start, end;


		for (int i=0; i< 10; ++i) {
			myint.compareAndSet(current, current+1);
		}

		// Start timer
		start = System.currentTimeMillis();

		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);
		myint.compareAndSet(current, current+1);

		end = System.currentTimeMillis();

		System.out.println("Maxi took me " + (end-start)/100 + " ms " +
		                   NativeUnsafe.getIsland() + ":" + NativeUnsafe.getCore());

		// Start timer
		start = System.currentTimeMillis();

		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);
		myint2.compareAndSet(current, current+1);

		end = System.currentTimeMillis();

		System.out.println("Mini took me " + (end-start)/100 + " ms " +
		                   NativeUnsafe.getIsland() + ":" + NativeUnsafe.getCore());

		// if (myint.compareAndSet(current, current+1))

		// 	System.out.println("I increased it to " +
		// 	                   (current+1) +
		// 	                   " I am cid=" +
		// 	                   NativeUnsafe.getCore() +
		// 	                   " bid=" +
		// 	                   NativeUnsafe.getIsland() +
		// 	                   " myint= " +
		// 	                   Integer.toHexString(addr) +
		// 	                   " myint2= " +
		// 	                   Integer.toHexString(addr2) );

		// else

		// 	System.out.println("I failed I am cid=" +
		// 	                   NativeUnsafe.getCore() +
		// 	                   " bid=" +
		// 	                   NativeUnsafe.getIsland() +
		// 	                   " myint= " +
		// 	                   Integer.toHexString(addr) +
		// 	                   " myint2= " +
		// 	                   Integer.toHexString(addr2) );


	}

}
