/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

/*
 * This file is available under and governed by the GNU General Public
 * License version 2 only, as published by the Free Software Foundation.
 * However, the following notice accompanied the original version of this
 * file:
 *
 * Written by Doug Lea with assistance from members of JCP JSR-166
 * Expert Group and released to the public domain, as explained at
 * http://creativecommons.org/licenses/publicdomain
 */

package java.util.concurrent.atomic;
import com.sun.squawk.*;

/**
 * An {@code int} value that may be updated atomically.  See the
 * {@link java.util.concurrent.atomic} package specification for
 * description of the properties of atomic variables. An
 * {@code AtomicInteger2} is used in applications such as atomically
 * incremented counters, and cannot be used as a replacement for an
 * {@link java.lang.Integer}. However, this class does extend
 * {@code Number} to allow uniform access by tools and utilities that
 * deal with numerically-based classes.
 *
 * @since 1.5
 * @author Doug Lea
 * @author Foivos S. Zakkak
*/
public class AtomicInteger2 extends Number {
    private int value;
    private RWlock lock;

    /**
     * Creates a new AtomicInteger2 with the given initial value.
     *
     * @param initialValue the initial value
     */
    public AtomicInteger2(int initialValue) {
        lock = new RWlock();
        value = initialValue;
        // TODO write-back after constructor
    }

    /**
     * Creates a new AtomicInteger2 with initial value {@code 0}.
     */
    public AtomicInteger2() {
        lock = new RWlock();
    }

    /**
     * Gets the current value.
     *
     * @return the current value
     */
    public final int get() {
        int ret;
        lock.readLock();
        ret = value;
        lock.readUnlock();
        return ret;
    }

    /**
     * Sets to the given value.
     *
     * @param newValue the new value
     */
    public final void set(int newValue) {
        lock.writeLock();
        value = newValue;
        lock.writeUnlock();
    }

    // /**
    //  * Eventually sets to the given value.
    //  *
    //  * @param newValue the new value
    //  * @since 1.6
    //  */
    // public final void lazySet(int newValue) {
    //     Unsafe.putOrderedInt(this, valueOffset, newValue);
    // }

    /**
     * Atomically sets to the given value and returns the old value.
     *
     * @param newValue the new value
     * @return the previous value
     */
    public final int getAndSet(int newValue) {
        int current;

        lock.writeLock();
        current = value;
        value   = newValue;
        lock.writeUnlock();

        return current;
    }

    /**
     * Atomically sets the value to the given updated value
     * if the current value {@code ==} the expected value.
     *
     * @param expect the expected value
     * @param update the new value
     * @return true if successful. False return indicates that
     * the actual value was not equal to the expected value.
     */
    public final boolean compareAndSet(int expect, int update) {
        if (lock.tryWriteLock()) {
            // TODO: Optimize by getting a read lock first and only
            // update to write if needed
            if (value != expect) {
                lock.writeUnlock();
                return false;
            }

            value = update;
            lock.writeUnlock();
            return true;
        }
        return false;
    }

    /**
     * Atomically sets the value to the given updated value
     * if the current value {@code ==} the expected value.
     *
     * <p>May <a href="package-summary.html#Spurious">fail spuriously</a>
     * and does not provide ordering guarantees, so is only rarely an
     * appropriate alternative to {@code compareAndSet}.
     *
     * @param expect the expected value
     * @param update the new value
     * @return true if successful.
     */
    public final boolean weakCompareAndSet(int expect, int update) {
        return compareAndSet(expect, update);
    }

    /**
     * Atomically increments by one the current value.
     *
     * @return the previous value
     */
    public final int getAndIncrement() {
        return getAndAdd(1);
    }

    /**
     * Atomically decrements by one the current value.
     *
     * @return the previous value
     */
    public final int getAndDecrement() {
        return getAndAdd(-1);
    }

    /**
     * Atomically adds the given value to the current value.
     *
     * @param delta the value to add
     * @return the previous value
     */
    public final int getAndAdd(int delta) {
        int current;

        lock.writeLock();
        current = value;
        value   = current + delta;
        lock.writeUnlock();

        return current;
    }

    /**
     * Atomically increments by one the current value.
     *
     * @return the updated value
     */
    public final int incrementAndGet() {
        return addAndGet(1);
    }

    /**
     * Atomically decrements by one the current value.
     *
     * @return the updated value
     */
    public final int decrementAndGet() {
        return addAndGet(-1);
    }

    /**
     * Atomically adds the given value to the current value.
     *
     * @param delta the value to add
     * @return the updated value
     */
    public final int addAndGet(int delta) {
        int next;

        lock.writeLock();
        next  = value + delta;
        value = next;
        lock.writeUnlock();

        return next;
    }

    /**
     * Returns the String representation of the current value.
     * @return the String representation of the current value.
     */
    public String toString() {
        return Integer.toString(get());
    }


    public int intValue() {
        return get();
    }

    public long longValue() {
        return (long)get();
    }

    public float floatValue() {
        return (float)get();
    }

    public double doubleValue() {
        return (double)get();
    }

}
