/*
 * Copyright 2013-15, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
 * Copyright 2004-2008 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Sun Microsystems, Inc., 16 Network Circle, Menlo
 * Park, CA 94025 or visit www.sun.com if you need additional
 * information or have any questions.
 */

package java.lang;

import com.sun.squawk.*;
import com.sun.squawk.platform.MMP;

/**
 * A <i>thread</i> is a thread of execution in a program. The Java
 * Virtual Machine allows an application to have multiple threads of
 * execution running concurrently.
 * <p>
 * Every thread has a priority. Threads with higher priority are
 * executed in preference to threads with lower priority.
 * <p>
 * There are two ways to create a new thread of execution. One is to
 * declare a class to be a subclass of <code>Thread</code>. This
 * subclass should override the <code>run</code> method of class
 * <code>Thread</code>. An instance of the subclass can then be
 * allocated and started. For example, a thread that computes primes
 * larger than a stated value could be written as follows:
 * <p><hr><blockquote><pre>
 *     class PrimeThread extends Thread {
 *         long minPrime;
 *         PrimeThread(long minPrime) {
 *             this.minPrime = minPrime;
 *         }
 *
 *         public void run() {
 *             // compute primes larger than minPrime
 *             &nbsp;.&nbsp;.&nbsp;.
 *         }
 *     }
 * </pre></blockquote><hr>
 * <p>
 * The following code would then create a thread and start it running:
 * <p><blockquote><pre>
 *     PrimeThread p = new PrimeThread(143);
 *     p.start();
 * </pre></blockquote>
 * <p>
 * The other way to create a thread is to declare a class that
 * implements the <code>Runnable</code> interface. That class then
 * implements the <code>run</code> method. An instance of the class can
 * then be allocated, passed as an argument when creating
 * <code>Thread</code>, and started. The same example in this other
 * style looks like the following:
 * <p><hr><blockquote><pre>
 *     class PrimeRun implements Runnable {
 *         long minPrime;
 *         PrimeRun(long minPrime) {
 *             this.minPrime = minPrime;
 *         }
 *
 *         public void run() {
 *             // compute primes larger than minPrime
 *             &nbsp;.&nbsp;.&nbsp;.
 *         }
 *     }
 * </pre></blockquote><hr>
 * <p>
 * The following code would then create a thread and start it running:
 * <p><blockquote><pre>
 *     PrimeRun p = new PrimeRun(143);
 *     new Thread(p).start();
 * </pre></blockquote>
 * <p>
 *
 *
 * @see     java.lang.Runnable
 * @see     java.lang.Runtime#exit(int)
 * @see     java.lang.Thread#run()
 * @since   JDK1.0
 */
public class Thread implements Runnable {

    private VMThread vmThread;

    /**
     * The target to run (if run() is not overridden).
     */
    private Runnable target;

    /**
     * The name of the thread;
     */
    private String name;

    private boolean spawned;


    /**
     * The argument supplied to the current call to
     * java.util.concurrent.locks.LockSupport.park.
     * Set by (private) java.util.concurrent.locks.LockSupport.setBlocker
     * Accessed using java.util.concurrent.locks.LockSupport.getBlocker
     */
    public volatile Object parkBlocker = null;
    public boolean permit = false;

    // /* The object in which this thread is blocked in an interruptible I/O
    //  * operation, if any.  The blocker's interrupt method should be invoked
    //  * after setting this thread's interrupt status.
    //  */
    // private volatile Interruptible blocker;
    // private Object blockerLock = new Object();

    public void setBlocker(Object arg) {
        // Even though volatile, hotspot doesn't need a write barrier here.
        // unsafe.putObject(t, parkBlockerOffset, arg);
        parkBlocker = arg;
    }

    /**
     * Returns the blocker object supplied to the most recent
     * invocation of a park method that has not yet unblocked, or null
     * if not blocked.  The value returned is just a momentary
     * snapshot -- the thread may have since unblocked or blocked on a
     * different blocker object.
     *
     * @return the blocker
     * @since 1.6
     */
    public Object getBlocker() {
        // return unsafe.getObjectVolatile(t, parkBlockerOffset);
        return parkBlocker;
    }

    // HACK: Emulate park and unpark (not supporting spurious exits)
    // using wait/notify
    public synchronized void unpark() {
        if (!permit) {          // This if is not necessary
            // Make permit available
            permit = true;
            // Notify any potential waiter
            this.notify();
        }
    }

    public static void park(long millis) {
        Thread thread = Thread.currentThread();
        synchronized (thread) {
            // If the permit is not available wait to be notified or interrupted
            if (!thread.permit) {
                try {
                    thread.wait(millis);
                } catch (InterruptedException e) {
                    thread.interrupt();
                }
            }
            // Consume the permit since you got notified
            thread.permit = false;
        }
    }

    /**
     * The minimum priority that a thread can have.
     */
    public final static int MIN_PRIORITY = VMThread.MIN_PRIORITY;

   /**
     * The default priority that is assigned to a thread.
     */
    public final static int NORM_PRIORITY = VMThread.NORM_PRIORITY;

    /**
     * The maximum priority that a thread can have.
     */
    public final static int MAX_PRIORITY = VMThread.MAX_PRIORITY;

    /**
     * ThreadLocal values pertaining to this thread. This map is maintained
     * by the ThreadLocal class.
     */
    ThreadLocal.ThreadLocalMap threadLocals = null;

    /**
     * InheritableThreadLocal values pertaining to this thread. This map is
     * maintained by the InheritableThreadLocal class.
     */
    ThreadLocal.ThreadLocalMap inheritableThreadLocals = null;

    /**
     * Returns a reference to the currently executing thread object.
     *
     * @return  the currently executing thread.
     */
    public static Thread currentThread() {
        return VMThread.currentThread().getAPIThread();
    }

    /**
     * Causes the currently executing thread object to temporarily pause
     * and allow other threads to execute.
     */
    public static void yield() {
        VMThread.yield();
    }

    /**
     * Causes the currently executing thread to sleep (temporarily cease
     * execution) for the specified number of milliseconds. The thread
     * does not lose ownership of any monitors.
     *
     * @param      millis   the length of time to sleep in milliseconds.
     * @exception  InterruptedException if another thread has interrupted
     *             the current thread.  The <i>interrupted status</i> of the
     *             current thread is cleared when this exception is thrown.
     * @see        java.lang.Object#notify()
     */
    public static void sleep(long millis) throws InterruptedException {
        VMThread.sleep(millis);
    }

    /**
     * Causes the currently executing thread to sleep (cease execution)
     * for the specified number of milliseconds plus the specified number
     * of nanoseconds, subject to the precision and accuracy of system
     * timers and schedulers. The thread does not lose ownership of any
     * monitors.
     *
     * @param      millis   the length of time to sleep in milliseconds.
     * @param      nanos    0-999999 additional nanoseconds to sleep.
     * @exception  IllegalArgumentException  if the value of millis is
     *             negative or the value of nanos is not in the range
     *             0-999999.
     * @exception  InterruptedException if any thread has interrupted
     *             the current thread.  The <i>interrupted status</i> of the
     *             current thread is cleared when this exception is thrown.
     * @see        Object#notify()
     */
    public static void sleep(long millis, int nanos)
    throws InterruptedException {
        if (millis < 0) {
            throw new IllegalArgumentException("timeout value is negative");
        }

        if (nanos < 0 || nanos > 999999) {
            throw new IllegalArgumentException(
                                "nanosecond timeout value out of range");
        }

        if (nanos >= 500000 || (nanos != 0 && millis == 0)) {
            millis++;
        }

        sleep(millis);
    }

    /**
     * Allocates a new <code>Thread</code> object.
     * <p>
     * Threads created this way must have overridden their
     * <code>run()</code> method to actually do anything.
     *
     * @see     java.lang.Runnable
     */
    public Thread() {
        vmThread = new VMThread(this, null);
        this.threadLocals = null;
        this.inheritableThreadLocals = null;
        this.name = null;
    }

    /**
     * Allocates a new <code>Thread</code> object with a
     * specific target object whose <code>run</code> method
     * is called.
     *
     * @param   target   the object whose <code>run</code> method is called.
     */
    public Thread(Runnable target) {
        this.target = target;
        vmThread = new VMThread(this, null);
        this.threadLocals = null;
        this.inheritableThreadLocals = null;
        this.name = null;
    }

    /**
     * Allocates a new <code>Thread</code> object with the given
     * target and name.
     *
     * @param   target   the object whose <code>run</code> method is called.
     * @param   name     the name of the new thread.
     */
    public Thread(Runnable target, String name) {
        this.target = target;
        vmThread = new VMThread(this, name);
        this.threadLocals = null;
        this.inheritableThreadLocals = null;
        this.name = name;
    }

    /**
     * Allocates a new <code>Thread</code> object with the
     * given name.
     *
     * Threads created this way must have overridden their
     * <code>run()</code> method to actually do anything.
     *
     * @param   name   the name of the new thread.
     */
    public Thread(String name) {
        vmThread = new VMThread(this, name);
        this.threadLocals = null;
        this.inheritableThreadLocals = null;
        this.name = name;
    }

    /**
     * Causes this thread to begin execution; the Java Virtual Machine
     * calls the <code>run</code> method of this thread.
     * <p>
     * The result is that two threads are running concurrently: the
     * current thread (which returns from the call to the
     * <code>start</code> method) and the other thread (which executes its
     * <code>run</code> method).
     *
     * @exception  IllegalThreadStateException  if the thread was already
     *               started.
     * @see        java.lang.Thread#run()
     */
    public void start() {
        // FIXME: Mark all object references in the stack
        MMP.spawnThread(this);
    }

    /**
     * If this thread was constructed using a separate
     * <code>Runnable</code> run object, then that
     * <code>Runnable</code> object's <code>run</code> method is called;
     * otherwise, this method does nothing and returns.
     * <p>
     * Subclasses of <code>Thread</code> should override this method.
     *
     * @see     java.lang.Thread#start()
     * @see     java.lang.Runnable#run()
     */
    public void run() {
        if (target != null) {
            target.run();
        }
    }

    /**
     * Interrupts this thread.
     * <p>
     * This method does nothing if the current thread is interrupting itself.
     * <p>
     * If this thread is blocked in an invocation of the {@link Object#wait() wait()},
     * {@link Object#wait(long) wait(long)}, or {@link Object#wait(long, int) wait(long, int)}
     * methods of the {@link Object} class or of the {@link Thread#join()} or {@link Thread#sleep(long)}
     * methods of this class, then its interrupt
     * status will be cleared and it will receive an {@link InterruptedException}.
     * <p>
     * If none of the previous conditions hold then this thread's interrupt status will be set.
     * <p>
     * In an implementation conforming to the CLDC Specification, this operation
     * is not required to cancel or clean up any pending I/O operations that the
     * thread may be waiting for.
     *
     * @since JDK 1.0, CLDC 1.1
     */
    public synchronized void interrupt() {
        if (!spawned)
            VM.print("This thread is not yet spawned interrupt\n");
        else
            vmThread.interrupt();
    }

    /**
     * Tests whether the current thread has been interrupted.  The
     * <i>interrupted status</i> of the thread is cleared by this method.  In
     * other words, if this method were to be called twice in succession, the
     * second call would return false (unless the current thread were
     * interrupted again, after the first call had cleared its interrupted
     * status and before the second call had examined it).
     *
     * <p>A thread interruption ignored because a thread was not alive
     * at the time of the interrupt will be reflected by this method
     * returning false.
     *
     * @return  <code>true</code> if the current thread has been interrupted;
     *          <code>false</code> otherwise.
     * @see #isInterrupted()
     * @revised 6.0
     */
    public static boolean interrupted() {
        // return currentThread().isInterrupted(true);
        return false;           // HACK FIXME
    }

    /**
     * Tests if this thread is alive. A thread is alive if it has
     * been started and has not yet died.
     *
     * @return  <code>true</code> if this thread is alive;
     *          <code>false</code> otherwise.
     */
    public final boolean isAlive() {
        if (!spawned)
            return false;

        return vmThread.isAlive();
    }

    /**
     * Changes the priority of this thread.
     *
     * @param newPriority priority to set this thread to
     * @exception  IllegalArgumentException  If the priority is not in the
     *             range <code>MIN_PRIORITY</code> to
     *             <code>MAX_PRIORITY</code>.
     * @see        #getPriority
     * @see        java.lang.Thread#getPriority()
     * @see        java.lang.Thread#MAX_PRIORITY
     * @see        java.lang.Thread#MIN_PRIORITY
     */
    public final void setPriority(int newPriority) {
        // if (!spawned)
        //     VM.print("This thread is not yet spawned setPriority\n");
        // else
        if (spawned)
            vmThread.setPriority(newPriority);
    }

    public final void setVMThread(VMThread vmThread) {
        this.vmThread = vmThread;
        this.spawned = true;
    }

    /**
     * Returns this thread's priority.
     *
     * @return  this thread's name.
     * @see     #setPriority
     * @see     java.lang.Thread#setPriority(int)
     */
    public final int getPriority() {
        if (!spawned) {
            // VM.print("This thread is not yet spawned getPriority\n");
            return NORM_PRIORITY;
        }

        return vmThread.getPriority();
    }

    /**
     * Returns this thread's name.  Note that in CLDC the name
     * of the thread can only be set when creating the thread.
     *
     * @return  this thread's name.
     */
    public final String getName() {
        if (!spawned) {
            // VM.print("This thread is not yet spawned getName\n");
            return name;
        }

        return vmThread.getName();
    }

	public final String getTName() {
        return name;
    }

    /**
     * Returns the current number of active threads in the VM.
     *
     * @return  the current number of threads in this thread's thread group.
     */
    public static int activeCount() {
        return VMThread.activeCount();
    }

    /**
     * Waits at most <code>millis</code> milliseconds for this thread to
     * die. A timeout of <code>0</code> means to wait forever.
     *
     * @param      millis   the time to wait in milliseconds.
     * @exception  InterruptedException if any thread has interrupted
     *             the current thread.  The <i>interrupted status</i> of the
     *             current thread is cleared when this exception is thrown.
     */
    public final void join(long millis)
    throws InterruptedException {
        long base = System.currentTimeMillis();
        long now = 0;

        if (millis < 0) {
            throw new IllegalArgumentException("timeout value is negative");
        }


        while(true) {
            // VM.print("Check if not yet spawned join\n");
            synchronized (this) {
                if (spawned)
                    break;
            }
            // VM.print("This thread is not yet spawned join\n");

            int tmp = 0;
            for (int i=0; i<1000; i++) {
                tmp += i;
            }
        }

        // VM.print("This thread is spawned join\n");

        if (millis == 0) {
            while (true) {
                synchronized (vmThread) {
                    // VM.print("This thread is not yet spawned toString\n");
                    if (vmThread.isDead()) {
                        break;
                    }
                    // VM.print("WAIT\n");
                    vmThread.wait(0);
                }
            }
        } else {
            while (true) {
                synchronized (vmThread) {
                    long delay = millis - now;
                    if (delay <= 0 || vmThread.isDead()) {
                        break;
                    }
                    vmThread.wait(delay);
                }
                now = System.currentTimeMillis() - base;
            }
        }
    }

    /**
     * Waits at most <code>millis</code> milliseconds plus
     * <code>nanos</code> nanoseconds for this thread to die.
     *
     * @param      millis   the time to wait in milliseconds.
     * @param      nanos    0-999999 additional nanoseconds to wait.
     * @exception  IllegalArgumentException  if the value of millis is negative
     *               the value of nanos is not in the range 0-999999.
     * @exception  InterruptedException if any thread has interrupted
     *             the current thread.  The <i>interrupted status</i> of the
     *             current thread is cleared when this exception is thrown.
     */
    public final synchronized void join(long millis, int nanos)
    throws InterruptedException {

        if (millis < 0) {
            throw new IllegalArgumentException("timeout value is negative");
        }

        if (nanos < 0 || nanos > 999999) {
            throw new IllegalArgumentException(
                                "nanosecond timeout value out of range");
        }

        if (nanos >= 500000 || (nanos != 0 && millis == 0)) {
            millis++;
        }

        join(millis);
    }

    /**
     * Waits for this thread to die.
     *
     * @exception  InterruptedException if another thread has interrupted
     *             the current thread.  The <i>interrupted status</i> of the
     *             current thread is cleared when this exception is thrown.
     */
    public final void join() throws InterruptedException {
        join(0);
    }

    /**
     * Returns a string representation of this thread, including a unique number
     * that identifies the thread and the thread's priority.
     *
     * @return  a string representation of this thread.
     */
    public String toString() {
        if (!spawned) {
            // VM.print("This thread is not yet spawned toString\n");
            return name;
        }

        return vmThread.toString();
    }
}
