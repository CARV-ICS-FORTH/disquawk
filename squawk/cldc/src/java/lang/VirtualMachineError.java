/*
 * @(#)VirtualMachineError.java	1.13 03/01/23
 *
 * Copyright     2015, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
 * Copyright 2003 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

package java.lang;

/**
 * Thrown to indicate that the Java Virtual Machine is broken or has 
 * run out of resources necessary for it to continue operating. 
 *
 *
 * @author  Frank Yellin
 * @version 1.13, 01/23/03
 * @since   JDK1.0
 */
abstract public
class VirtualMachineError extends Error {
    /**
     * Constructs a <code>VirtualMachineError</code> with no detail message.
     */
    public VirtualMachineError() {
	super();
    }

    /**
     * Constructs a <code>VirtualMachineError</code> with the specified 
     * detail message. 
     *
     * @param   s   the detail message.
     */
    public VirtualMachineError(String s) {
	super(s);
    }
}
