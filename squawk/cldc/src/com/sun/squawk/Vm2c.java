package com.sun.squawk;

/**
 * Annotation to provide information to the vm2c module that will take the
 * Java source and generate a C executable form of it.
 *
 *
 */
public @interface Vm2c {
    String root() default "";
    String macro() default "";
    String code() default "";
    String proxy() default "";
    String implementers() default "";
}
 * Copyright     2015, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
