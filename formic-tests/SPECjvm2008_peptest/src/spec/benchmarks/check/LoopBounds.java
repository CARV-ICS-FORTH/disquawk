/**
 * Copyright (c) 2008 Standard Performance Evaluation Corporation (SPEC)
 *               All rights reserved.
 *
 * Copyright (c) 1997,1998 Sun Microsystems, Inc. All rights reserved.
 *
 * This source code is provided as is, without any express or implied warranty.
 *
 * Check whether the JVM makes unwarranted assumptions about non-final
 * methods. This test case is derived from a hot spot in DeltaBlue
 * where it seems tempting to make the loop bounds constant, but
 * you cannot be certain that the class is not subclassed, etc.
 *
 * Walter Bays
 */

package spec.benchmarks.check;

import java.util.Vector;



class LoopBounds {

	protected Vector v;

	public static boolean gotError = false;

	public int size = 20;

	public LoopBounds() {
		v = new Vector();
		int f0 = 0;
		int f1 = 1;
		for (int i = 0; i < 20; i++) {
			v.addElement(Integer.valueOf(f1));
			int f = f0 + f1;
			f0 = f1;
			f1 = f;
		}
	}

	public int size() {
		return v.size();
	}

	public int constraintAt(int index) {
		Object o = v.elementAt(index);
		if (o instanceof Integer) {
			return ((Integer) o).intValue();
		} else {
			return 4711;
		}
	}

	public void execute() {
		int k = 0;
		for (int i = 0; i < size(); ++i) {
			// System.out.println ("v.size()=" + v.size() + " size()=" +
			// size());
//			System.out.print(constraintAt(i) + " ");
			k++; // Just do something
		}
		if (k != size)
			gotError = true;
	}

	public static void main(String[] args) {
		run();
	}

	public static void run() {
		(new LoopBounds()).execute();
		(new LoopBounds2()).execute();
	}

}

class LoopBounds2 extends LoopBounds {
	private int n = 0;

	public int size() {
		size = 12;
		if (n > 4 && n % 2 != 0)
			v.insertElementAt(new Double(1.0 / n), 6);
		return v.size() - n++;
	}
}
