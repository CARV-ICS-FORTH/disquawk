package monitorbench;

import java.lang.Runnable;
import com.sun.squawk.*;

public class OtherLock implements Runnable {

	private int artificial_work;

	public OtherLock() {
		artificial_work = 100;
	}

	public OtherLock(int work) {
		artificial_work = work;
	}

	public void run() {

		Lock lock = new Lock();
		long start, end;
		// System.out.println("My lock is :" + GC.getHashCode(lock) + "\n");

		start = System.currentTimeMillis();

		for (int i=0; i<100; ++i) {
			lock.doWork(artificial_work);
		}

		end = System.currentTimeMillis();

		// int temp = 0;
		// for (int i=0; i<100000; ++i) {
		// 	temp += i;
		// }
		// System.out.print("= " + temp + "\n");

		System.out.print("W Total time on different lock " + (end-start) +
		                 " ms BID: " + VM.getIsland() +
		                 " CID: " + VM.getCore() + "\n");
	}

}
