package atomicint;

import java.lang.Runnable;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicInteger2;
import com.sun.squawk.*;

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

		System.out.println("Hello World I am cid=" +
		                   NativeUnsafe.getCore() +
		                   " bid=" +
		                   NativeUnsafe.getIsland() +
		                   " myint= " +
		                   Integer.toHexString(addr) +
		                   " myint2= " +
		                   Integer.toHexString(addr2) );

		int current = myint.get();
		if (myint.compareAndSet(current, current+1))

			System.out.println("I increased it to " +
			                   (current+1) +
			                   " I am cid=" +
			                   NativeUnsafe.getCore() +
			                   " bid=" +
			                   NativeUnsafe.getIsland() +
			                   " myint= " +
			                   Integer.toHexString(addr) +
			                   " myint2= " +
			                   Integer.toHexString(addr2) );

		else

			System.out.println("I failed I am cid=" +
			                   NativeUnsafe.getCore() +
			                   " bid=" +
			                   NativeUnsafe.getIsland() +
			                   " myint= " +
			                   Integer.toHexString(addr) +
			                   " myint2= " +
			                   Integer.toHexString(addr2) );


	}

}
