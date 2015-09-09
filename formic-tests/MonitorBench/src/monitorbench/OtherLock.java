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
		// System.out.println("My lock is :" + GC.getHashCode(lock) + "\n");

		for (int i=0; i<100; ++i) {
			lock.doWork(artificial_work);
		}

	}

}
