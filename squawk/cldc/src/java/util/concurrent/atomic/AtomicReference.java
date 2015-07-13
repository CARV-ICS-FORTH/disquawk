/*
 * Copyright     2015, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
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
 * An object reference that may be updated atomically. See the {@link
 * java.util.concurrent.atomic} package specification for description
 * of the properties of atomic variables.
 * @since 1.5
 * @author Doug Lea
 * @param <V> The type of object referred to by this reference
 */
public class AtomicReference<V>  // implements java.io.Serializable
{
    // private static final long serialVersionUID = -1848883965231344442L;

    // private static final Unsafe unsafe = Unsafe.getUnsafe();
    // private static final long valueOffset;

    // static {
    //   try {
    //     valueOffset = unsafe.objectFieldOffset
    //         (AtomicReference.class.getDeclaredField("value"));
    //   } catch (Exception ex) { throw new Error(ex); }
    // }

    private volatile V value;
    private RWlock lock;

    /**
     * Creates a new AtomicReference with the given initial value.
     *
     * @param initialValue the initial value
     */
    public AtomicReference(V initialValue) {
        lock = new RWlock();
        value = initialValue;
        // TODO write-back after constructor
    }

    /**
     * Creates a new AtomicReference with null initial value.
     */
    public AtomicReference() {
        lock = new RWlock();
    }

    /**
     * Gets the current value.
     *
     * @return the current value
     */
    public final V get() {
        V ret;
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
    public final void set(V newValue) {
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
    // public final void lazySet(V newValue) {
    //     unsafe.putOrderedObject(this, valueOffset, newValue);
    // }

    /**
     * Atomically sets the value to the given updated value
     * if the current value {@code ==} the expected value.
     * @param expect the expected value
     * @param update the new value
     * @return true if successful. False return indicates that
     * the actual value was not equal to the expected value.
     */
    public final boolean compareAndSet(V expect, V update) {
        lock.writeLock();
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
    public final boolean weakCompareAndSet(V expect, V update) {
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
     * Atomically sets to the given value and returns the old value.
     *
     * @param newValue the new value
     * @return the previous value
     */
    public final V getAndSet(V newValue) {
        while (true) {
            V x = get();
            if (compareAndSet(x, newValue))
                return x;
        }
    }

    /**
     * Returns the String representation of the current value.
     * @return the String representation of the current value.
     */
    public String toString() {
        return String.valueOf(get());
    }

}
