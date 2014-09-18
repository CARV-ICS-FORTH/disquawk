package newthread;

import java.lang.Runnable;
import com.sun.squawk.NativeUnsafe;

public class Hello implements Runnable {

	public void run() {
		System.out.println("Hello World I am cid=" +
		                   NativeUnsafe.getCore() +
		                   " bid=" +
		                   NativeUnsafe.getIsland());
	}

}
