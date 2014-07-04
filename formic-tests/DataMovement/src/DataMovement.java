import com.sun.squawk.Address;
import com.sun.squawk.UWord;
import com.sun.squawk.VM;

/**
 * Compile with
 *
 * CFLAGS+=-DAR_CONFIGURE_THROUGH_MAKE=1 \
		-DAR_BOOT_MASTER_BID=0x00 \
		-DAR_ARM0_BID=-1 \
		-DAR_ARM1_BID=-1 \
		-DAR_XUP_BID=-1 \
		-DAR_FORMIC_CORES_PER_BOARD=1 \
		-DAR_FORMIC_MIN_X=0 \
		-DAR_FORMIC_MIN_Y=0 \
		-DAR_FORMIC_MIN_Z=0 \
		-DAR_FORMIC_MAX_X=0 \
		-DAR_FORMIC_MAX_Y=0 \
		-DAR_FORMIC_MAX_Z=1
 */

public class DataMovement {

	public static void main(String[] args) {

		// Check simple objects

		Integer temp  = new Integer(5120);
		int     addr  = Address.fromObject(temp).toUWord().toPrimitive();

		// Add some delay to make sure the write is performed on both
		// nodes
		for(int i=0; i<10000; ++i) {
		}

		int     bid   = ((addr >>> 27) == 1) ? 0 : 1;
		int     addr2 = (bid << 27) + (addr & 0x7FFFFFF);
		Integer temp2 = (Integer)Address.fromPrimitive(addr2).toObject();

		System.out.print("Object transfer...");
		if (temp2.intValue() == temp.intValue()) {
			System.out.println("[1;32mPASS[0m");
		} else {
			System.out.println("[1;31mFAIL[0m");
			System.out.println("local = "+temp+
			                   " &local = "+Integer.toHexString(addr));
			System.out.println("remote = "+temp2+
			                   " &remote = "+Integer.toHexString(addr2));
		}

		// Now check arrays

		Integer[] arr  = new Integer[4];
		arr[0] = new Integer(1024);
		arr[1] = new Integer(2048);
		arr[2] = new Integer(4096);
		arr[3] = new Integer(5192);
		addr   = Address.fromObject(arr).toUWord().toPrimitive();

		// Add some delay to make sure the write is performed on both
		// nodes
		for(int i=0; i<20000; ++i) {
		}

		bid   = ((addr >>> 27) == 1) ? 0 : 1;
		addr2 = (bid << 27) + (addr & 0x7FFFFFF);
		Integer[] arr2 = (Integer[])Address.fromPrimitive(addr2).toObject();

		System.out.print("Array transfer...");
		if (arr2[1].intValue() == arr[1].intValue()) {
			System.out.println("[1;32mPASS[0m");
		} else {
			System.out.println("[1;31mFAIL[0m");
			System.out.println("local = "+arr[1]+
			                   " &local = "+Integer.toHexString(addr));
			System.out.println("remote = "+arr2[1]+
			                   " &remote = "+Integer.toHexString(addr2));
		}

		// Now check large arrays

		Integer[] larr = new Integer[131072];
		larr[0]        = new Integer(1024);
		larr[1]        = new Integer(2048);
		larr[2]        = new Integer(4096);
		larr[3]        = new Integer(5192);
		larr[1024]     = new Integer(1234);
		addr           = Address.fromObject(larr).toUWord().toPrimitive();

		// Add some delay to make sure the write is performed on both
		// nodes
		for(int i=0; i<20000; ++i) {
		}

		bid   = ((addr >>> 27) == 1) ? 0 : 1;
		addr2 = (bid << 27) + (addr & 0x7FFFFFF);
		Integer[] larr2 = (Integer[])Address.fromPrimitive(addr2).toObject();

		System.out.print("Large array transfer...");
		if (larr2[1024].intValue() == larr[1024].intValue()) {
			System.out.println("[1;32mPASS[0m");
		} else {
			System.out.println("[1;31mFAIL[0m");
			System.out.println("local = "+larr[1024]+
			                   " &local = "+Integer.toHexString(addr));
			System.out.println("remote = "+larr2[1024]+
			                   " &remote = "+Integer.toHexString(addr2));
		}

		// Now arrays of Strings

		String[] arrS  = new String[4];
		arrS[0] = new String("1234");
		arrS[1] = new String("1234567");
		arrS[2] = new String("123456");
		arrS[3] = new String("123456789");
		addr   = Address.fromObject(arrS).toUWord().toPrimitive();

		// Add some delay to make sure the write is performed on both
		// nodes
		for(int i=0; i<20000; ++i) {
		}

		bid   = ((addr >>> 27) == 1) ? 0 : 1;
		addr2 = (bid << 27) + (addr & 0x7FFFFFF);
		String[] arrS2 = (String[])Address.fromPrimitive(addr2).toObject();

		System.out.print("String Array transfer...");
		if (arrS2[1].equals(arrS[1])) {
			System.out.println("[1;32mPASS[0m");
		} else {
			System.out.println("[1;31mFAIL[0m");
			System.out.println("local = "+arrS[1]+
			                   " &local = "+Integer.toHexString(addr));
			System.out.println("remote = "+arrS2[1]+
			                   " &remote = "+Integer.toHexString(addr2));
		}

		// Now check 2D arrays

		Integer[][] arrdd = new Integer[8][8];
		for (int i=0; i<8; ++i) {
			for (int j=0; j<8; ++j) {
				arrdd[i][j] = new Integer((i+1)*(j+1));
			}
		}
		addr = Address.fromObject(arrdd).toUWord().toPrimitive();

		// Add some delay to make sure the write is performed on both
		// nodes
		for(int i=0; i<20000; ++i) {
		}

		bid   = ((addr >>> 27) == 1) ? 0 : 1;
		addr2 = (bid << 27) + (addr & 0x7FFFFFF);
		Integer[][] arrdd2 = (Integer[][])Address.fromPrimitive(addr2).toObject();

		System.out.print("2D array transfer...");
		if (arrdd2[7][3].intValue() == arrdd[7][3].intValue()) {
			System.out.println("[1;32mPASS[0m");
			System.out.println("local = "+arrdd[7][3]+
			                   " &local = "+Integer.toHexString(addr));
			System.out.println("remote = "+arrdd2[7][3]+
			                   " &remote = "+Integer.toHexString(addr2));
		} else {
			System.out.println("[1;31mFAIL[0m");
			System.out.println("local = "+arrdd[7][3]+
			                   " &local = "+Integer.toHexString(addr));
			System.out.println("remote = "+arrdd2[7][3]+
			                   " &remote = "+Integer.toHexString(addr2));
		}

	}

}
