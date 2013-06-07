/*
 * Copyright (c) 2008 Standard Performance Evaluation Corporation (SPEC)
 *               All rights reserved.
 *
 * Copyright (c) 1997,1998 Sun Microsystems, Inc. All rights reserved.
 *
 * This source code is provided as is, without any express or implied warranty.
 *
 * Complain about Java errors that ought to be caught as
 * exceptions by the JVM. *NOT* an exhaustive conformance
 * test. This is just intended to catch some common errors
 * and omissions for the convenience of the benchmarker in
 * avoiding some run rule mistakes. This needs to be
 * expanded to test more.
 *
 * The timing of this "benchmark" is ignored in metric
 * calculation. It is here only in order to pass or
 * fail output verification.
 */

package spec.benchmarks.check;

public class Main {

    /**
     * Run this functional test.
     */
    // public static String testType() {
    //     return FUNCTIONAL;
    // }

    public static long runBenchmark() {
        boolean caughtIndex = false;
        boolean gotToFinally = false;

        try {
            int[] a = new int[10];
            for (int i = 0; i <= 10; i++)
                a[i] = i;
            System.out.println("Error: array bounds not checked");
        } catch (ArrayIndexOutOfBoundsException e) {
            caughtIndex = true;
        } finally {
            gotToFinally = true;
        }

        if (!caughtIndex) {
            System.out.println("1st bounds test error:\tindex exception not received");
        }
        if (!gotToFinally) {
            System.out.println("1st bounds test error:\tfinally clause not executed");
        }
        if (caughtIndex && gotToFinally) {
            System.out.println("1st bounds test:\tOK");
        }

        checkSubclassing();


        // LoopBounds mule = new LoopBounds();
        LoopBounds.run();

        if (LoopBounds.gotError) {
            System.out.println("2nd bounds test:\tfailed");
        } else {
            System.out.println("2nd bounds test:\tOK");
        }

        PepTest horse = new PepTest();
        horse.instanceMain();

        if (horse.gotError) {
            System.out.println("PepTest failed");
        }

        return 0;
    }

    private static void checkSubclassing() {
        Super sup = new Super(3);
        Sub sub = new Sub(3);
        System.out.println(sup.getName() + ": " + sup.toString());
        System.out.println(sub.getName() + ": " + sub.toString());
        System.out.println("Super: prot=" + sup.getProtected() + ", priv=" + sup.getPrivate());
        System.out.println("Sub:  prot=" + sub.getProtected() + ", priv=" + sub.getPrivate());
    }

    public void harnessMain() {
        runBenchmark();
    }

  public static void main(String[] args)
    {
      runBenchmark();
    }

}
