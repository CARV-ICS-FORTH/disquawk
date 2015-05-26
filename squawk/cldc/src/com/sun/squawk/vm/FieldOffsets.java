/*
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

package com.sun.squawk.vm;

import com.sun.squawk.*;

/**
 * This class contains the offsets and types for fields that must be directly accessed
 * by the VM or other Squawk tools such as the mapper. The romizer ensures that these
 * offsets and types are correct when it creates the image for the bootstrap suite.
 * The value of the offset depends on whether the field is static or not - see
 * {@link Member#getOffset} for more detail.
 * <p>
 * A simple version of the field's type is also encoded into the high 32 bits of each entry
 * all of which are longs. The types are restricted to byte, char, short, int, long,
 * float, double, and Object and are encoded using the corresponding {@link CID} value
 * placed in the high 32 bits of the long.
 * <p>
 * This unfortunate encoding is needed if we want to keep the type and offset in one
 * place because these values are needed in parts of the VM that need to be executed
 * before normal object construction can take place. Two routines are provided to decode
 * the offset and type of a field, and the offset can also be obtained by casting the
 * field into an int.
 * <p>
 * The name of the constant must be composed of the name of the class that defines the
 * field (with '.'s replaced by '_'s) and the name of the field with a '$' separating them.
 */
public class FieldOffsets {

    private FieldOffsets() {}

    private final static int  CIDSHIFT = 32;
    private final static long OOP   = ((long)CID.OBJECT) << CIDSHIFT;
    private final static long INT   = ((long)CID.INT)    << CIDSHIFT;
    private final static long SHORT = ((long)CID.SHORT)  << CIDSHIFT;
    private final static long BYTE = ((long)CID.BYTE)  << CIDSHIFT;

    /**
     * The offset of the 'virtualMethods' field in com.sun.squawk.Klass
     */
    public final static long com_sun_squawk_Klass$virtualMethods = 2 | OOP;

    /**
     * The offset of the 'staticMethods' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$staticMethods = 3 | OOP;

    /**
     * The offset of the 'name' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$name = 4 | OOP;

    /**
     * The offset of the 'componentType' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$componentType = 5 | OOP;

    /**
     * The offset of the 'superType' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$superType = 6 | OOP;

    /**
     * The offset of the 'superType' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$interfaces = 7 | OOP;

    /**
     * The offset of the 'objects' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$objects = 9 | OOP;

    /**
     * The offset of the 'oopMap' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$oopMap = 10 | OOP;

    /**
     * The offset of the 'oopMapWord' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$oopMapWord = 11 | OOP;

    /**
     * The offset of the 'dataMap' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$dataMap = 12 | OOP;

    /**
     * The offset of the 'dataMapWord' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$dataMapWord = 13 | OOP;

    /**
     * The offset of the 'modifiers' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$modifiers = (/*VAL*/false/*SQUAWK_64*/ ? 28 : 14) | INT;

    /**
     * The offset of the 'dataMapLength' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$dataMapLength = (/*VAL*/false/*SQUAWK_64*/ ? 27 : 30) | SHORT;

    /**
     * The offset of the 'id' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$id = (/*VAL*/false/*SQUAWK_64*/ ? 58 : 31) | SHORT;

    /**
     * The offset of the 'instanceSizeBytes' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$instanceSizeBytes = 32 | SHORT;

    /**
     * The offset of the 'staticFieldsSize' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$staticFieldsSize = 33 | SHORT;

    /**
     * The offset of the 'refStaticFieldsSize' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$refStaticFieldsSize = 34 | SHORT;

    /**
     * The offset of the 'state' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$state = 70 | BYTE;

    /**
     * The offset of the 'initModifiers' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$onlyFinalStatics = 71 | BYTE;

    /**
     * The offset of the 'initModifiers' field in com.sun.squawk.Klass.
     */
    public final static long com_sun_squawk_Klass$initModifiers = 72 | BYTE;

    /**
     * The offset of the 'entryTable' field in com.sun.squawk.util.SquawkHashtable.
     */
    public final static long com_sun_squawk_util_SquawkHashtable$entryTable = 2 | OOP;

    /**
     * The offset of the 'vmThread' field in java.lang.Thread.
     */
    public final static long java_lang_Thread$vmThread = 2 | OOP;

    /**
     * The offset of the 'target' field in java.lang.Thread.
     */
    public final static long java_lang_Thread$target = 3 | OOP;

    /**
     * The offset of the 'klass' field in java.lang.Class.
     */
    public final static long java_lang_Class$klass = 2 | OOP;

    /**
     * The offset of the 'isolate' field in com.sun.squawk.VMThread.
     */
    public final static long com_sun_squawk_VMThread$isolate = (/*VAL*/false/*SQUAWK_64*/ ? 2 : 4) | OOP;

    /**
     * The offset of the 'stack' field in com.sun.squawk.VMThread.
     */
    public final static long com_sun_squawk_VMThread$stack = (/*VAL*/false/*SQUAWK_64*/ ? 3 : 5) | OOP;

    /**
     * The offset of the 'stackSize' field in com.sun.squawk.VMThread.
     */
    public final static long com_sun_squawk_VMThread$stackSize = (/*VAL*/false/*SQUAWK_64*/ ? 4 : 6) | INT;

/*if[!ENABLE_ISOLATE_MIGRATION]*/
/*else[ENABLE_ISOLATE_MIGRATION]*/
//    /**
//     * The offset of the 'savedStackChunks' field in com.sun.squawk.Isolate.
//     */
//    public final static long com_sun_squawk_Isolate$savedStackChunks = (/*VAL*/false/*SQUAWK_64*/ ? 0 : 0) | OOP;
/*end[ENABLE_ISOLATE_MIGRATION]*/

    /**
     * The offset of the 'trace' field in java.lang.Throwable. Used by VM.printVMStackTrace()..
     */
    public final static long java_lang_Throwable$trace = 3 | OOP;

    /**
     * The offset of the 'classes' field in com.sun.squawk.Suite. Used by com.sun.squawk.ObjectGraphLoader of the Romizer.
     */
    public final static long com_sun_squawk_Suite$classes = 2 | OOP;

    /**
     * The offset of the 'name' field in com.sun.squawk.Suite. Used by com.sun.squawk.ObjectGraphLoader of the Romizer.
     */
    public final static long com_sun_squawk_Suite$name = 3 | OOP;

    /**
     * The offset of the 'metadatas' field in com.sun.squawk.Suite. Used by com.sun.squawk.ObjectGraphLoader of the Romizer.
     */
    public final static long com_sun_squawk_Suite$metadatas = 4 | OOP;

    /**
     * The offset of the 'type' field in com.sun.squawk.Suite. Used by com.sun.squawk.ObjectGraphLoader of the Romizer.
     */
    public final static long com_sun_squawk_Suite$type = 5 | INT;

    /**
     * The offset of the 'definedClass' field in com.sun.squawk.KlassMetadata. Used by com.sun.squawk.ObjectGraphLoader of the Romizer.
     */
    public final static long com_sun_squawk_KlassMetadata$definedClass = 2 | OOP;

    /**
     * The offset of the 'definedClass' field in com.sun.squawk.KlassMetadata. Used by com.sun.squawk.ObjectGraphLoader of the Romizer.
     */
    public final static long com_sun_squawk_KlassMetadata$symbols = 3 | OOP;

    /**
     * The offset of the 'classTable' field in com.sun.squawk.KlassMetadata. Used by com.sun.squawk.ObjectGraphLoader of the Romizer.
     */
    public final static long com_sun_squawk_KlassMetadata$classTable = 4 | OOP;

    /**
     * Decodes a field's type from a given field descriptor.
     *
     * @param fieldDesc   an encoded field descriptor
     * @return the system ID of the type encoded in <code>fieldDesc</code>
     */
    public static int decodeSystemID(long fieldDesc) {
        return (int)(fieldDesc >> CIDSHIFT);
    }

    /**
     * Decodes a field's offset from a given field descriptor.
     *
     * @param fieldDesc   an encoded field descriptor
     * @return the offset encoded in <code>fieldDesc</code>. This is equivalent to <code>(int)field</code>
     */
    public static int decodeOffset(long fieldDesc) {
        return (int)fieldDesc;
    }
}
