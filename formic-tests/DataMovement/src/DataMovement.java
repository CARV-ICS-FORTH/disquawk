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

		Integer temp  = new Integer(5120);
		int     addr  = Address.fromObject(temp).toUWord().toPrimitive();

		// Add some delay to make sure the write is performed on both
		// nodes
		for(int i=0; i<10000; ++i) {
		}

		int     bid   = ((addr >>> 27) == 1) ? 0 : 1;
		int     addr2 = (bid << 27) + (addr & 0x7FFFFFF);
		Integer temp2 = (Integer)Address.fromPrimitive(addr2).toObject();

		if (temp2.intValue() == temp.intValue())
			System.out.println("[1;32mPASS[0m");
		else {
			System.out.println("[1;31mFAIL[0m");
			System.out.println("local = "+temp+
			                   "&local = "+Integer.toHexString(addr));
			System.out.println("remote = "+temp2+
			                   "&remote = "+Integer.toHexString(addr2));
		}
	}

}
