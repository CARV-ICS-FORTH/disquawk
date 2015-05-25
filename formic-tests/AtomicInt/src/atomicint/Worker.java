package atomicint;

import java.lang.Runnable;
import java.util.concurrent.atomic.*;
//import com.sun.squawk.*;

// On C level it goes up to 60-70 kops/sec with a single server

public class Worker implements Runnable {

	AtomicInteger   myint;

	public Worker() {
		myint  = new AtomicInteger();
	}

	public void run() {
		// int addr  = Address.fromObject(myint).toUWord().toPrimitive();
		// int addr2 = Address.fromObject(myint2).toUWord().toPrimitive();

		// Greetings
		// System.out.println("Hello World I am cid=" +
		//                    NativeUnsafe.getCore() +
		//                    " bid=" +
		//                    NativeUnsafe.getIsland());


		// int current = myint.get();
		// long start, end;


		// for (int i=0; i< 10; ++i) {
		// 	myint.getAndIncrement();
		// }

		// // Start timer
		// start = System.currentTimeMillis();

		// myint.getAndIncrement();
		// myint.getAndIncrement();
		// myint.getAndIncrement();
		// myint.getAndIncrement();
		// myint.getAndIncrement();
		// myint.getAndIncrement();
		// myint.getAndIncrement();
		// myint.getAndIncrement();
		// myint.getAndIncrement();
		// myint.getAndIncrement();

		// end = System.currentTimeMillis();

		// System.out.println("Maxi took me " + (end-start)/10 + " ms " +
		//                    NativeUnsafe.getIsland() + ":" + NativeUnsafe.getCore());
		// System.out.println("Maxi = " + myint.get() + " " +
		//                    NativeUnsafe.getIsland() + ":" + NativeUnsafe.getCore());

		// for (int i=0; i< 10; ++i) {
		// 	myint2.getAndIncrement();
		// }

		// // Start timer
		// start = System.currentTimeMillis();

		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();
		myint.getAndIncrement();

		// end = System.currentTimeMillis();

		// System.out.println("Mini took me " + (end-start)/10 + " ms " +
		//                    NativeUnsafe.getIsland() + ":" + NativeUnsafe.getCore());
		// System.out.println("Mini = " + myint2.get() + " " +
		//                    NativeUnsafe.getIsland() + ":" + NativeUnsafe.getCore());

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


		while(true);
	}

}
