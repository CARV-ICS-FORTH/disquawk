package monitorbench;

public class Lock {

	public synchronized void doWork(int workload) {
		// System.out.print("Do work for " + workload + "\n");
		for (int j=0; j<workload; ++j) {
		}
	}

}
