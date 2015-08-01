package java.lang.annotation;

import com.sun.squawk.Java5Marker;

@Java5Marker
public @Documented @Retention( RetentionPolicy.RUNTIME) @Target( ElementType.ANNOTATION_TYPE) @interface Retention {
  public RetentionPolicy value();
}
 * Copyright     2015, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
