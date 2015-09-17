package monitorbench;

import java.lang.Runnable;
import com.sun.squawk.*;

public class SameLock implements Runnable {

	private Lock lock;
	private int  artificial_work;

	public SameLock() {
		lock = new Lock();
		artificial_work = 100;
	}

	public SameLock(int work) {
		lock = new Lock();
		artificial_work = work;
	}

	public void run() {
		// System.out.print("The lock is :" + GC.getHashCode(lock) + "\n");
		long      start, end;

		start = System.currentTimeMillis();

		for (int i=0; i<100; ++i) {
			lock.doWork(artificial_work);
		}

		end = System.currentTimeMillis();

		System.out.print("W Total time on same lock " + (end-start) +
		                 " ms BID: " + VM.getIsland() +
		                 " CID: " + VM.getCore() + "\n");

	}

}
