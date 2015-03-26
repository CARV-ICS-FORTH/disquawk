/*
 * Copyright (c) 2015   , FORTH-ICS / CARV
 *                        (Foundation for Research & Technology -- Hellas,
 *                         Institute of Computer Science,
 *                         Computer Architecture & VLSI Systems Laboratory)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file   RWLock.java
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The implementation of a reader writers lock interface for VM usage.
 *
 *
 * TODO: Add MM synchronization
 */

package com.sun.squawk;

import com.sun.squawk.pragma.*;
import com.sun.squawk.util.*;

public class RWlock {
    private final void writeLock0(boolean istry) throws NativePragma {
        throw Assert.shouldNotReachHere("unimplemented when hosted");
    }

    public boolean tryWriteLock() {
        VMThread thread;
        writeLock0(true);
        VMThread.waitForEvent(GC.getHashCode(this));
        thread = VMThread.currentThread();
        // TODO: On success synchronize?
        return thread.getResult() != null;
    }

    public void writeLock() {
        // TODO: spin untill you get the lock
        readLock0(false);
        VMThread.waitForEvent(GC.getHashCode(this));
        // TODO: On success synchronize?
        return;
    }

    private final void readLock0(boolean istry) throws NativePragma {
        throw Assert.shouldNotReachHere("unimplemented when hosted");
    }

    public boolean tryReadLock() {
        VMThread thread;
        readLock0(true);
        VMThread.waitForEvent(GC.getHashCode(this));
        thread = VMThread.currentThread();
        // TODO: On success synchronize?
        return thread.getResult() != null;
    }

    public void readLock() {
        // TODO: spin untill you get the lock
        readLock0(false);
        VMThread.waitForEvent(GC.getHashCode(this));
        // TODO: On success synchronize?
        return;
    }

    private final void unlock0(boolean isread) throws NativePragma {
        throw Assert.shouldNotReachHere("unimplemented when hosted");
    }

    public void readUnlock() {
        // TODO: writeback and release
        unlock0(true);
    }

    public void writeUnlock() {
        // TODO: writeback and release
        unlock0(false);
    }
}
