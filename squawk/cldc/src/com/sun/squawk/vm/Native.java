/* **DO NOT EDIT THIS FILE** */
/*
 * Copyright 2004 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2014 Sun Microsystems, Inc. All Rights Reserved.
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

/**
 * This class defines the native method identifiers used in the Squawk system.
 * It was automatically generated as a part of the build process
 * and should not be edited by hand.
 *
 * @author   Nik Shaylor
 * @author   Doug Simon
 */
public final class Native {

    public final static int com_sun_squawk_Address$add                    = 0;
    public final static int com_sun_squawk_Address$addOffset              = 1;
    public final static int com_sun_squawk_Address$and                    = 2;
    public final static int com_sun_squawk_Address$diff                   = 3;
    public final static int com_sun_squawk_Address$eq                     = 4;
    public final static int com_sun_squawk_Address$fromObject             = 5;
    public final static int com_sun_squawk_Address$fromPrimitive          = 6;
    public final static int com_sun_squawk_Address$hi                     = 7;
    public final static int com_sun_squawk_Address$hieq                   = 8;
    public final static int com_sun_squawk_Address$isMax                  = 9;
    public final static int com_sun_squawk_Address$isZero                 = 10;
    public final static int com_sun_squawk_Address$lo                     = 11;
    public final static int com_sun_squawk_Address$loeq                   = 12;
    public final static int com_sun_squawk_Address$max                    = 13;
    public final static int com_sun_squawk_Address$ne                     = 14;
    public final static int com_sun_squawk_Address$or                     = 15;
    public final static int com_sun_squawk_Address$roundDown              = 16;
    public final static int com_sun_squawk_Address$roundDownToWord        = 17;
    public final static int com_sun_squawk_Address$roundUp                = 18;
    public final static int com_sun_squawk_Address$roundUpToWord          = 19;
    public final static int com_sun_squawk_Address$sub                    = 20;
    public final static int com_sun_squawk_Address$subOffset              = 21;
    public final static int com_sun_squawk_Address$toObject               = 22;
    public final static int com_sun_squawk_Address$toUWord                = 23;
    public final static int com_sun_squawk_Address$zero                   = 24;
    public final static int com_sun_squawk_UWord$and                      = 25;
    public final static int com_sun_squawk_UWord$eq                       = 26;
    public final static int com_sun_squawk_UWord$fromPrimitive            = 27;
    public final static int com_sun_squawk_UWord$hi                       = 28;
    public final static int com_sun_squawk_UWord$hieq                     = 29;
    public final static int com_sun_squawk_UWord$isMax                    = 30;
    public final static int com_sun_squawk_UWord$isZero                   = 31;
    public final static int com_sun_squawk_UWord$lo                       = 32;
    public final static int com_sun_squawk_UWord$loeq                     = 33;
    public final static int com_sun_squawk_UWord$max                      = 34;
    public final static int com_sun_squawk_UWord$ne                       = 35;
    public final static int com_sun_squawk_UWord$or                       = 36;
    public final static int com_sun_squawk_UWord$toInt                    = 37;
    public final static int com_sun_squawk_UWord$toOffset                 = 38;
    public final static int com_sun_squawk_UWord$toPrimitive              = 39;
    public final static int com_sun_squawk_UWord$zero                     = 40;
    public final static int com_sun_squawk_Offset$add                     = 41;
    public final static int com_sun_squawk_Offset$bytesToWords            = 42;
    public final static int com_sun_squawk_Offset$eq                      = 43;
    public final static int com_sun_squawk_Offset$fromPrimitive           = 44;
    public final static int com_sun_squawk_Offset$ge                      = 45;
    public final static int com_sun_squawk_Offset$gt                      = 46;
    public final static int com_sun_squawk_Offset$isZero                  = 47;
    public final static int com_sun_squawk_Offset$le                      = 48;
    public final static int com_sun_squawk_Offset$lt                      = 49;
    public final static int com_sun_squawk_Offset$ne                      = 50;
    public final static int com_sun_squawk_Offset$sub                     = 51;
    public final static int com_sun_squawk_Offset$toInt                   = 52;
    public final static int com_sun_squawk_Offset$toPrimitive             = 53;
    public final static int com_sun_squawk_Offset$toUWord                 = 54;
    public final static int com_sun_squawk_Offset$wordsToBytes            = 55;
    public final static int com_sun_squawk_Offset$zero                    = 56;

    /**
     * An encoded table of the native methods that can be linked to during class loading at runtime.
     * Each entry in the table is a length 'n' (encoded as a char relative to '0') followed by one or more characters 'chars' delimited
     * by a space character. The index of the entry is the native method identifier and the name of the native
     * method is constructed from the first 'n' characters of the name of the previous method concatenated with 'chars'.
     */
    public final static String LINKABLE_NATIVE_METHODS =

        /*   0 */"0" + "com.sun.squawk.Address.add " +
        /*   1 */"J" +                           "Offset " +
        /*   2 */"H" +                         "nd " +
        /*   3 */"G" +                        "diff " +
        /*   4 */"G" +                        "eq " +
        /*   5 */"G" +                        "fromObject " +
        /*   6 */"K" +                            "Primitive " +
        /*   7 */"G" +                        "hi " +
        /*   8 */"I" +                          "eq " +
        /*   9 */"G" +                        "isMax " +
        /*  10 */"I" +                          "Zero " +
        /*  11 */"G" +                        "lo " +
        /*  12 */"I" +                          "eq " +
        /*  13 */"G" +                        "max " +
        /*  14 */"G" +                        "ne " +
        /*  15 */"G" +                        "or " +
        /*  16 */"G" +                        "roundDown " +
        /*  17 */"P" +                                 "ToWord " +
        /*  18 */"L" +                             "Up " +
        /*  19 */"N" +                               "ToWord " +
        /*  20 */"G" +                        "sub " +
        /*  21 */"J" +                           "Offset " +
        /*  22 */"G" +                        "toObject " +
        /*  23 */"I" +                          "UWord " +
        /*  24 */"G" +                        "zero " +
        /*  25 */"?" +                "UWord.and " +
        /*  26 */"E" +                      "eq " +
        /*  27 */"E" +                      "fromPrimitive " +
        /*  28 */"E" +                      "hi " +
        /*  29 */"G" +                        "eq " +
        /*  30 */"E" +                      "isMax " +
        /*  31 */"G" +                        "Zero " +
        /*  32 */"E" +                      "lo " +
        /*  33 */"G" +                        "eq " +
        /*  34 */"E" +                      "max " +
        /*  35 */"E" +                      "ne " +
        /*  36 */"E" +                      "or " +
        /*  37 */"E" +                      "toInt " +
        /*  38 */"G" +                        "Offset " +
        /*  39 */"G" +                        "Primitive " +
        /*  40 */"E" +                      "zero " +
        /*  41 */"?" +                "Offset.add " +
        /*  42 */"F" +                       "bytesToWords " +
        /*  43 */"F" +                       "eq " +
        /*  44 */"F" +                       "fromPrimitive " +
        /*  45 */"F" +                       "ge " +
        /*  46 */"G" +                        "t " +
        /*  47 */"F" +                       "isZero " +
        /*  48 */"F" +                       "le " +
        /*  49 */"G" +                        "t " +
        /*  50 */"F" +                       "ne " +
        /*  51 */"F" +                       "sub " +
        /*  52 */"F" +                       "toInt " +
        /*  53 */"H" +                         "Primitive " +
        /*  54 */"H" +                         "UWord " +
        /*  55 */"F" +                       "wordsToBytes " +
        /*  56 */"F" +                       "zero " +
        "";

    public final static int com_sun_squawk_NativeUnsafe$call0             = 57;
    public final static int com_sun_squawk_NativeUnsafe$call1             = 58;
    public final static int com_sun_squawk_NativeUnsafe$call10            = 59;
    public final static int com_sun_squawk_NativeUnsafe$call2             = 60;
    public final static int com_sun_squawk_NativeUnsafe$call3             = 61;
    public final static int com_sun_squawk_NativeUnsafe$call4             = 62;
    public final static int com_sun_squawk_NativeUnsafe$call5             = 63;
    public final static int com_sun_squawk_NativeUnsafe$call6             = 64;
    public final static int com_sun_squawk_NativeUnsafe$cancelTaskExecutor = 65;
    public final static int com_sun_squawk_NativeUnsafe$charAt            = 66;
    public final static int com_sun_squawk_NativeUnsafe$compareAndSwapInt0 = 67;
    public final static int com_sun_squawk_NativeUnsafe$copyTypes         = 68;
    public final static int com_sun_squawk_NativeUnsafe$deleteNativeTask  = 69;
    public final static int com_sun_squawk_NativeUnsafe$deleteTaskExecutor = 70;
    public final static int com_sun_squawk_NativeUnsafe$free              = 71;
    public final static int com_sun_squawk_NativeUnsafe$getAddress        = 72;
    public final static int com_sun_squawk_NativeUnsafe$getAsByte         = 73;
    public final static int com_sun_squawk_NativeUnsafe$getAsInt          = 74;
    public final static int com_sun_squawk_NativeUnsafe$getAsShort        = 75;
    public final static int com_sun_squawk_NativeUnsafe$getAsUWord        = 76;
    public final static int com_sun_squawk_NativeUnsafe$getByte           = 77;
    public final static int com_sun_squawk_NativeUnsafe$getChar           = 78;
    public final static int com_sun_squawk_NativeUnsafe$getCore           = 79;
    public final static int com_sun_squawk_NativeUnsafe$getInt            = 80;
    public final static int com_sun_squawk_NativeUnsafe$getIsland         = 81;
    public final static int com_sun_squawk_NativeUnsafe$getLong           = 82;
    public final static int com_sun_squawk_NativeUnsafe$getLongAtWord     = 83;
    public final static int com_sun_squawk_NativeUnsafe$getObject         = 84;
    public final static int com_sun_squawk_NativeUnsafe$getShort          = 85;
    public final static int com_sun_squawk_NativeUnsafe$getType           = 86;
    public final static int com_sun_squawk_NativeUnsafe$getUWord          = 87;
    public final static int com_sun_squawk_NativeUnsafe$getUnalignedInt   = 88;
    public final static int com_sun_squawk_NativeUnsafe$getUnalignedLong  = 89;
    public final static int com_sun_squawk_NativeUnsafe$getUnalignedShort = 90;
    public final static int com_sun_squawk_NativeUnsafe$globalMemoryProtection = 91;
    public final static int com_sun_squawk_NativeUnsafe$malloc            = 92;
    public final static int com_sun_squawk_NativeUnsafe$runBlockingFunctionOn = 93;
    public final static int com_sun_squawk_NativeUnsafe$setAddress        = 94;
    public final static int com_sun_squawk_NativeUnsafe$setArrayTypes     = 95;
    public final static int com_sun_squawk_NativeUnsafe$setByte           = 96;
    public final static int com_sun_squawk_NativeUnsafe$setChar           = 97;
    public final static int com_sun_squawk_NativeUnsafe$setInt            = 98;
    public final static int com_sun_squawk_NativeUnsafe$setLong           = 99;
    public final static int com_sun_squawk_NativeUnsafe$setLongAtWord     = 100;
    public final static int com_sun_squawk_NativeUnsafe$setObject         = 101;
    public final static int com_sun_squawk_NativeUnsafe$setShort          = 102;
    public final static int com_sun_squawk_NativeUnsafe$setType           = 103;
    public final static int com_sun_squawk_NativeUnsafe$setUWord          = 104;
    public final static int com_sun_squawk_NativeUnsafe$setUnalignedInt   = 105;
    public final static int com_sun_squawk_NativeUnsafe$setUnalignedLong  = 106;
    public final static int com_sun_squawk_NativeUnsafe$setUnalignedShort = 107;
    public final static int com_sun_squawk_NativeUnsafe$swap              = 108;
    public final static int com_sun_squawk_NativeUnsafe$swap2             = 109;
    public final static int com_sun_squawk_NativeUnsafe$swap4             = 110;
    public final static int com_sun_squawk_NativeUnsafe$swap8             = 111;
    public final static int com_sun_squawk_VM$addToClassStateCache        = 112;
    public final static int com_sun_squawk_VM$addressResult               = 113;
    public final static int com_sun_squawk_VM$allocate                    = 114;
    public final static int com_sun_squawk_VM$asKlass                     = 115;
    public final static int com_sun_squawk_VM$callStaticNoParm            = 116;
    public final static int com_sun_squawk_VM$callStaticOneParm           = 117;
    public final static int com_sun_squawk_VM$copyBytes                   = 118;
    public final static int com_sun_squawk_VM$deadbeef                    = 119;
    public final static int com_sun_squawk_VM$doubleToLongBits            = 120;
    public final static int com_sun_squawk_VM$executeCIO                  = 121;
    public final static int com_sun_squawk_VM$executeCOG                  = 122;
    public final static int com_sun_squawk_VM$executeGC                   = 123;
    public final static int com_sun_squawk_VM$fatalVMError                = 124;
    public final static int com_sun_squawk_VM$finalize                    = 125;
    public final static int com_sun_squawk_VM$floatToIntBits              = 126;
    public final static int com_sun_squawk_VM$getBranchCount              = 127;
    public final static int com_sun_squawk_VM$getCore                     = 128;
    public final static int com_sun_squawk_VM$getFP                       = 129;
    public final static int com_sun_squawk_VM$getGlobalAddr               = 130;
    public final static int com_sun_squawk_VM$getGlobalInt                = 131;
    public final static int com_sun_squawk_VM$getGlobalOop                = 132;
    public final static int com_sun_squawk_VM$getGlobalOopCount           = 133;
    public final static int com_sun_squawk_VM$getGlobalOopTable           = 134;
    public final static int com_sun_squawk_VM$getIsland                   = 135;
    public final static int com_sun_squawk_VM$getMP                       = 136;
    public final static int com_sun_squawk_VM$getPreviousFP               = 137;
    public final static int com_sun_squawk_VM$getPreviousIP               = 138;
    public final static int com_sun_squawk_VM$hashcode                    = 139;
    public final static int com_sun_squawk_VM$initializeLiterals          = 140;
    public final static int com_sun_squawk_VM$intBitsToFloat              = 141;
    public final static int com_sun_squawk_VM$invalidateClassStateCache   = 142;
    public final static int com_sun_squawk_VM$isBigEndian                 = 143;
    public final static int com_sun_squawk_VM$longBitsToDouble            = 144;
    public final static int com_sun_squawk_VM$math                        = 145;
    public final static int com_sun_squawk_VM$serviceResult               = 146;
    public final static int com_sun_squawk_VM$setBytes                    = 147;
    public final static int com_sun_squawk_VM$setGlobalAddr               = 148;
    public final static int com_sun_squawk_VM$setGlobalInt                = 149;
    public final static int com_sun_squawk_VM$setGlobalOop                = 150;
    public final static int com_sun_squawk_VM$setPreviousFP               = 151;
    public final static int com_sun_squawk_VM$setPreviousIP               = 152;
    public final static int com_sun_squawk_VM$threadSwitch                = 153;
    public final static int com_sun_squawk_VM$zeroWords                   = 154;
    public final static int com_sun_squawk_CheneyCollector$memoryProtect  = 155;
    public final static int com_sun_squawk_ServiceOperation$cioExecute    = 156;
    public final static int com_sun_squawk_GarbageCollector$collectGarbageInC = 157;
    public final static int com_sun_squawk_GarbageCollector$hasNativeImplementation = 158;
    public final static int com_sun_squawk_Lisp2Bitmap$clearBitFor        = 159;
    public final static int com_sun_squawk_Lisp2Bitmap$clearBitsFor       = 160;
    public final static int com_sun_squawk_Lisp2Bitmap$getAddressForBitmapWord = 161;
    public final static int com_sun_squawk_Lisp2Bitmap$getAddressOfBitmapWordFor = 162;
    public final static int com_sun_squawk_Lisp2Bitmap$initialize         = 163;
    public final static int com_sun_squawk_Lisp2Bitmap$iterate            = 164;
    public final static int com_sun_squawk_Lisp2Bitmap$setBitFor          = 165;
    public final static int com_sun_squawk_Lisp2Bitmap$setBitsFor         = 166;
    public final static int com_sun_squawk_Lisp2Bitmap$testAndSetBitFor   = 167;
    public final static int com_sun_squawk_Lisp2Bitmap$testBitFor         = 168;
    public final static int com_sun_squawk_SoftwareCache$inHeap           = 169;
    public final static int com_sun_squawk_SoftwareCache$translate        = 170;
    public final static int com_sun_squawk_platform_MMP$checkMailbox      = 171;
    public final static int com_sun_squawk_platform_MMP$spawnThread       = 172;
    public final static int com_sun_squawk_platform_MMGR$addWaiter        = 173;
    public final static int com_sun_squawk_platform_MMGR$monitorEnter     = 174;
    public final static int com_sun_squawk_platform_MMGR$monitorExit      = 175;
    public final static int com_sun_squawk_platform_MMGR$notify           = 176;
    public final static int com_sun_squawk_platform_MMGR$removeWaiter     = 177;
    public final static int com_sun_squawk_platform_MMGR$waitMonitorExit  = 178;
    public final static int com_sun_squawk_RWlock$readLock0               = 179;
    public final static int com_sun_squawk_RWlock$unlock0                 = 180;
    public final static int com_sun_squawk_RWlock$writeLock0              = 181;
    public final static int com_sun_squawk_VM$lcmp                        = 182;
    public final static int ENTRY_COUNT                                   = 183;
}
