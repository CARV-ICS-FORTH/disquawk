/*
 * Copyright 2013-15, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

package java.lang;

import com.sun.squawk.Java5Marker;

/**
 * The Integer class wraps a value of the primitive type <code>int</code>
 * in an object. An object of type <code>Integer</code> contains a
 * single field whose type is <code>int</code>.
 * <p>
 * In addition, this class provides several methods for converting
 * an <code>int</code> to a <code>String</code> and a
 * <code>String</code> to an <code>int</code>, as well as other
 * constants and methods useful when dealing with an
 * <code>int</code>.
 *
 * @version 12/17/01 (CLDC 1.1)
 * @since   JDK1.0, CLDC 1.0
 */
@Java5Marker
public final class Integer {

    /**
     * The smallest value of type <code>int</code>. The constant
     * value of this field is <tt>-2147483648</tt>.
     */
    public static final int MIN_VALUE = 0x80000000;

    /**
     * The largest value of type <code>int</code>. The constant
     * value of this field is <tt>2147483647</tt>.
     */
    public static final int MAX_VALUE = 0x7fffffff;

    /**
     * All possible chars for representing a number as a String
     */
    final static char[] digits = {
        '0' , '1' , '2' , '3' , '4' , '5' ,
        '6' , '7' , '8' , '9' , 'a' , 'b' ,
        'c' , 'd' , 'e' , 'f' , 'g' , 'h' ,
        'i' , 'j' , 'k' , 'l' , 'm' , 'n' ,
        'o' , 'p' , 'q' , 'r' , 's' , 't' ,
        'u' , 'v' , 'w' , 'x' , 'y' , 'z'
    };

    /**
     * Creates a string representation of the first argument in the
     * radix specified by the second argument.
     * <p>
     * If the radix is smaller than <code>Character.MIN_RADIX</code> or
     * larger than <code>Character.MAX_RADIX</code>, then the radix
     * <code>10</code> is used instead.
     * <p>
     * If the first argument is negative, the first element of the
     * result is the ASCII minus character <code>'-'</code>
     * (<tt>'&#92;u002d'</tt>). If the first
     * argument is not negative, no sign character appears in the result.
     * <p>
     * The remaining characters of the result represent the magnitude of
     * the first argument. If the magnitude is zero, it is represented by
     * a single zero character <tt>'0'</tt> (<tt>'&#92;u0030'</tt>); otherwise,
     * the first character of the representation of the magnitude will
     * not be the zero character.
     * The following ASCII characters are used as digits:
     * <blockquote><pre>
     *   0123456789abcdefghijklmnopqrstuvwxyz
     * </pre></blockquote>
     * These are <tt>'&#92;u0030'</tt> through <tt>'&#92;u0039'</tt> and
     * <tt>'&#92;u0061'</tt> through <tt>'&#92;u007a'</tt>. If the
     * <tt>radix</tt> is <var>N</var>, then the first <var>N</var> of these
     * characters are used as radix-<var>N</var> digits in the order shown.
     * Thus, the digits for hexadecimal (radix 16) are
     * <blockquote><pre>
     * <tt>0123456789abcdef</tt>.
     * </pre></blockquote>
     *
     * @param   i       an integer.
     * @param   radix   the radix.
     * @return  a string representation of the argument in the specified radix.
     * @see     java.lang.Character#MAX_RADIX
     * @see     java.lang.Character#MIN_RADIX
     */
    public static String toString(int i, int radix) {

        if (radix < Character.MIN_RADIX || radix > Character.MAX_RADIX)
            radix = 10;

        /* Use the faster version */
        if (radix == 10) {
            return toString(i);
        }

        char buf[] = new char[33];
        boolean negative = (i < 0);
        int charPos = 32;

        if (!negative) {
            i = -i;
        }

        while (i <= -radix) {
            buf[charPos--] = digits[-(i % radix)];
            i = i / radix;
        }
        buf[charPos] = digits[-i];

        if (negative) {
            buf[--charPos] = '-';
        }

        return new String(buf, charPos, (33 - charPos));
    }

    /**
     * Append value directly into StringBuffer
     * @param buffer
     * @param i
     * @param radix
     * @return
     */
    static StringBuffer toStringBuffer(StringBuffer buffer, int i, int radix) {
        // This strategy first writes characters backwards, then reverses them.
        if (radix < Character.MIN_RADIX || radix > Character.MAX_RADIX)
            radix = 10;

        boolean negative = (i < 0);
        int start = buffer.length();

        if (!negative) {
            i = -i;
        }

        while (i <= -radix) {
            buffer.append(digits[-(i % radix)]);
            i = i / radix;
        }
         buffer.append(digits[-i]);

        if (negative) {
            buffer.append('-');
        }

        // now reverse in place:
        int end = buffer.length() - 1;
        int count = buffer.length() - start;

        for (int j = 0; j < count / 2; j++) {
            char c = buffer.charAt(start + j);
            buffer.setCharAt(start + j, buffer.charAt(end - j));
            buffer.setCharAt(end - j, c);
        }

        return buffer;
    }

    /**
     * Creates a string representation of the integer argument as an
     * unsigned integer in base&nbsp;16.
     * <p>
     * The unsigned integer value is the argument plus 2<sup>32</sup> if
     * the argument is negative; otherwise, it is equal to the argument.
     * This value is converted to a string of ASCII digits in hexadecimal
     * (base&nbsp;16) with no extra leading <code>0</code>s. If the
     * unsigned magnitude is zero, it is represented by a single zero
     * character <tt>'0'</tt> (<tt>'&#92;u0030'</tt>); otherwise, the first
     * character of the representation of the unsigned magnitude will
     * not be the zero character. The following characters are used as
     * hexadecimal digits:
     * <blockquote><pre>
     * 0123456789abcdef
     * </pre></blockquote>
     * These are the characters <tt>'&#92;u0030'</tt> through <tt>'&#92;u0039'</tt>
     * and <tt>'u\0039'</tt> through <tt>'&#92;u0066'</tt>.
     *
     * @param   i   an integer.
     * @return  the string representation of the unsigned integer value
     *          represented by the argument in hexadecimal (base&nbsp;16).
     * @since   JDK1.0.2
     */
    public static String toHexString(int i) {
        return toUnsignedString(i, 4);
    }

    /**
     * Creates a string representation of the integer argument as an
     * unsigned integer in base 8.
     * <p>
     * The unsigned integer value is the argument plus 2<sup>32</sup> if
     * the argument is negative; otherwise, it is equal to the argument.
     * This value is converted to a string of ASCII digits in octal
     * (base&nbsp;8) with no extra leading <code>0</code>s.
     * <p>
     * If the unsigned magnitude is zero, it is represented by a single
     * zero character <tt>'0'</tt> (<tt>'&#92;u0030'</tt>); otherwise, the
     * first character of the representation of the unsigned magnitude will
     * not be the zero character. The octal digits are:
     * <blockquote><pre>
     * 01234567
     * </pre></blockquote>
     * These are the characters <tt>'&#92;u0030'</tt> through <tt>'&#92;u0037'</tt>.
     *
     * @param   i   an integer
     * @return  the string representation of the unsigned integer value
     *          represented by the argument in octal (base&nbsp;8).
     * @since   JDK1.0.2
     */
    public static String toOctalString(int i) {
        return toUnsignedString(i, 3);
    }

    /**
     * Creates a string representation of the integer argument as an
     * unsigned integer in base&nbsp;2.
     * <p>
     * The unsigned integer value is the argument plus 2<sup>32</sup>if
     * the argument is negative; otherwise it is equal to the argument.
     * This value is converted to a string of ASCII digits in binary
     * (base&nbsp;2) with no extra leading <code>0</code>s.
     *
     * If the unsigned magnitude is zero, it is represented by a single
     * zero character <tt>'0'</tt> (<tt>'&#92;u0030'</tt>); otherwise, the
     * first character of the representation of the unsigned magnitude
     * will not be the zero character. The characters <tt>'0'</tt>
     * (<tt>'&#92;u0030'</tt>) and <tt>'1'</tt> (<tt>'&#92;u0031'</tt>) are used
     * as binary digits.
     *
     * @param   i   an integer.
     * @return  the string representation of the unsigned integer value
     *          represented by the argument in binary (base&nbsp;2).
     * @since   JDK1.0.2
     */
    public static String toBinaryString(int i) {
        return toUnsignedString(i, 1);
    }

    /**
     * Convert the integer to an unsigned number.
     */
    private static String toUnsignedString(int i, int shift) {
        char[] buf = new char[32];
        int charPos = 32;
        int radix = 1 << shift;
        int mask = radix - 1;
        do {
            buf[--charPos] = digits[i & mask];
            i >>>= shift;
        } while (i != 0);

        return new String(buf, charPos, (32 - charPos));
    }



    final static char [] DigitTens = {
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
        '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
        '2', '2', '2', '2', '2', '2', '2', '2', '2', '2',
        '3', '3', '3', '3', '3', '3', '3', '3', '3', '3',
        '4', '4', '4', '4', '4', '4', '4', '4', '4', '4',
        '5', '5', '5', '5', '5', '5', '5', '5', '5', '5',
        '6', '6', '6', '6', '6', '6', '6', '6', '6', '6',
        '7', '7', '7', '7', '7', '7', '7', '7', '7', '7',
        '8', '8', '8', '8', '8', '8', '8', '8', '8', '8',
        '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
        } ;

    final static char [] DigitOnes = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        } ;

        // I use the "invariant division by multiplication" trick to
        // accelerate Integer.toString.  In particular we want to
        // avoid division by 10.
        //
        // The "trick" has roughly the same performance characteristics
        // as the "classic" Integer.toString code on a non-JIT VM.
        // The trick avoids .rem and .div calls but has a longer code
        // path and is thus dominated by dispatch overhead.  In the
        // JIT case the dispatch overhead doesn't exist and the
        // "trick" is considerably faster than the classic code.
        //
        // TODO-FIXME: convert (x * 52429) into the equiv shift-add
        // sequence.
        //
        // RE:  Division by Invariant Integers using Multiplication
        //      T Gralund, P Montgomery
        //      ACM PLDI 1994
        //

    /**
     * Returns a new String object representing the specified integer. The
     * argument is converted to signed decimal representation and returned
     * as a string, exactly as if the argument and radix <tt>10</tt> were
     * given as arguments to the {@link #toString(int, int)} method.
     *
     * @param   i   an integer to be converted.
     * @return  a string representation of the argument in base&nbsp;10.
     */
    public static String toString(int i) {
        if (i == Integer.MIN_VALUE)
            return "-2147483648";
        int size = (i < 0) ? stringSize(-i) + 1 : stringSize(i);
        char[] buf = new char[size];
        getChars(i, size, buf);
        return new String(buf, 0, size);
    }

    /**
     * Places characters representing the integer i into the
     * character array buf. The characters are placed into
     * the buffer backwards starting with the least significant
     * digit at the specified index (exclusive), and working
     * backwards from there.
     *
     * Will fail if i == Integer.MIN_VALUE
     */
    static void getChars(int i, int index, char[] buf) {
        int q, r;
        int charPos = index;
        char sign = 0;

        if (i < 0) {
            sign = '-';
            i = -i;
        }

        // Generate two digits per iteration
        while (i >= 65536) {
            q = i / 100;
        // really: r = i - (q * 100);
            r = i - ((q << 6) + (q << 5) + (q << 2));
            i = q;
            buf [--charPos] = DigitOnes[r];
            buf [--charPos] = DigitTens[r];
        }

        // Fall thru to fast mode for smaller numbers
        // assert(i <= 65536, i);
        for (;;) {
            q = (i * 52429) >>> (16+3);
            r = i - ((q << 3) + (q << 1));  // r = i-(q*10) ...
            buf [--charPos] = digits [r];
            i = q;
            if (i == 0) break;
        }
        if (sign != 0) {
            buf [--charPos] = sign;
        }
    }

    final static int [] sizeTable = { 9, 99, 999, 9999, 99999, 999999, 9999999,
                                      99999999, 999999999, Integer.MAX_VALUE };

    // Requires positive x
    static int stringSize(int x) {
        for (int i=0; ; i++)
            if (x <= sizeTable[i])
                return i+1;
    }

    /**
     * Parses the string argument as a signed integer in the radix
     * specified by the second argument. The characters in the string
     * must all be digits of the specified radix (as determined by
     * whether {@link java.lang.Character#digit(char, int)} returns a
     * nonnegative value), except that the first character may be an
     * ASCII minus sign <code>'-'</code> (<code>'&#92;u002d'</code>) to
     * indicate a negative value. The resulting integer value is returned.
     * <p>
     * An exception of type <tt>NumberFormatException</tt> is thrown if any
     * of the following situations occurs:
     * <ul>
     * <li>The first argument is <tt>null</tt> or is a string of length zero.
     * <li>The radix is either smaller than
     * {@link java.lang.Character#MIN_RADIX} or
     * larger than {@link java.lang.Character#MAX_RADIX}.
     * <li>Any character of the string is not a digit of the specified radix,
     * except that the first character may be a minus sign <tt>'-'</tt>
     * (<tt>'&#92;u002d'</tt>) provided that the string is longer than length 1.
     * <li>The integer value represented by the string is not a value of type
     * <tt>int</tt>.
     * </ul><p>
     * Examples:
     * <blockquote><pre>
     * parseInt("0", 10) returns 0
     * parseInt("473", 10) returns 473
     * parseInt("-0", 10) returns 0
     * parseInt("-FF", 16) returns -255
     * parseInt("1100110", 2) returns 102
     * parseInt("2147483647", 10) returns 2147483647
     * parseInt("-2147483648", 10) returns -2147483648
     * parseInt("2147483648", 10) throws a NumberFormatException
     * parseInt("99", 8) throws a NumberFormatException
     * parseInt("Kona", 10) throws a NumberFormatException
     * parseInt("Kona", 27) returns 411787
     * </pre></blockquote>
     *
     * @param      s   the <code>String</code> containing the integer.
     * @param      radix   the radix to be used.
     * @return     the integer represented by the string argument in the
     *             specified radix.
     * @exception  NumberFormatException  if the string does not contain a
     *               parsable integer.

     */
    public static int parseInt(String s, int radix)
                throws NumberFormatException
    {
        if (s == null) {
            throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                       "null"
/* #endif */
            );
        }

        if (radix < Character.MIN_RADIX) {
            throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                       "radix " + radix +
/// skipped                       " less than Character.MIN_RADIX"
/* #endif */
            );
        }

        if (radix > Character.MAX_RADIX) {
            throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                       "radix " + radix +
/// skipped                       " greater than Character.MAX_RADIX"
/* #endif */
            );
        }

        int result = 0;
        boolean negative = false;
        int i = 0, len = s.length();
        int limit = -Integer.MAX_VALUE;
        int multmin;
        int digit;

        if (len > 0) {
            char firstChar = s.charAt(0);
            if (firstChar < '0') { // Possible leading "-"
                if (firstChar == '-') {
                    negative = true;
                    limit = Integer.MIN_VALUE;
                } else
                    throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                               s
/* #endif */
                    );

                if (len == 1) // Cannot have lone "-"
                    throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                               s
/* #endif */
                    );
                i++;
            }
            multmin = limit / radix;
            while (i < len) {
                // Accumulating negatively avoids surprises near MAX_VALUE
                digit = Character.digit(s.charAt(i++),radix);
                if (digit < 0) {
                    throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                               s
/* #endif */
                    );
                }
                if (result < multmin) {
                    throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                               s
/* #endif */
                    );
                }
                result *= radix;
                if (result < limit + digit) {
                    throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                               s
/* #endif */
                    );
                }
                result -= digit;
            }
        } else {
            throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                       s
/* #endif */
            );
        }
        if (negative) {
            if (i > 1) {
                return result;
            } else {    /* Only got "-" */
                throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                           s
/* #endif */
                );
            }
        } else {
            return -result;
        }
    }

    /**
     * Parses the string argument as a signed decimal integer. The
     * characters in the string must all be decimal digits, except that
     * the first character may be an ASCII minus sign <code>'-'</code>
     * (<tt>'&#92;u002d'</tt>) to indicate a negative value. The resulting
     * integer value is returned, exactly as if the argument and the radix
     * 10 were given as arguments to the
     * {@link #parseInt(java.lang.String, int)} method.
     *
     * @param      s   a string.
     * @return     the integer represented by the argument in decimal.
     * @exception  NumberFormatException  if the string does not contain a
     *               parsable integer.
     */
    public static int parseInt(String s) throws NumberFormatException {
        return parseInt(s,10);
    }

    /**
     * Returns a new Integer object initialized to the value of the
     * specified String. The first argument is interpreted as representing
     * a signed integer in the radix specified by the second argument,
     * exactly as if the arguments were given to the
     * {@link #parseInt(java.lang.String, int)} method. The result is an
     * <code>Integer</code> object that represents the integer value
     * specified by the string.
     * <p>
     * In other words, this method returns an <code>Integer</code> object
     * equal to the value of:
     * <blockquote><pre>
     * new Integer(Integer.parseInt(s, radix))
     * </pre></blockquote>
     *
     * @param      s   the string to be parsed.
     * @param      radix the radix of the integer represented by string
     *             <tt>s</tt>
     * @return     a newly constructed <code>Integer</code> initialized to the
     *             value represented by the string argument in the specified
     *             radix.
     * @exception  NumberFormatException  if the String cannot be
     *             parsed as an <code>int</code>.
     */
    public static Integer valueOf(String s, int radix) throws NumberFormatException {
        return new Integer(parseInt(s,radix));
    }

    /**
     * Returns a new Integer object initialized to the value of the
     * specified String. The argument is interpreted as representing a
     * signed decimal integer, exactly as if the argument were given to
     * the {@link #parseInt(java.lang.String)} method. The result is an
     * <tt>Integer</tt> object that represents the integer value specified
     * by the string.
     * <p>
     * In other words, this method returns an <tt>Integer</tt> object equal
     * to the value of:
     * <blockquote><pre>
     * new Integer(Integer.parseInt(s))
     * </pre></blockquote>
     *
     * @param      s   the string to be parsed.
     * @return     a newly constructed <code>Integer</code> initialized to the
     *             value represented by the string argument.
     * @exception  NumberFormatException  if the string cannot be parsed
     *             as an integer.
     */
    public static Integer valueOf(String s) throws NumberFormatException
    {
        return new Integer(parseInt(s, 10));
    }

/*if[INTEGER_CACHE_ENABLED]*/
    private static class IntegerCache {
        private IntegerCache(){}

        static final Integer cache[] = new Integer[-(-128) + 127 + 1];

        static {
            for(int i = 0; i < cache.length; i++)
                cache[i] = new Integer(i - 128);
        }
    }
/*end[INTEGER_CACHE_ENABLED]*/

    /**
     * Returns an {@code Integer} instance representing the specified
     * {@code int} value.  If a new {@code Integer} instance is not
     * required, this method should generally be used in preference to
     * the constructor {@link #Integer(int)}, as this method is likely
     * to yield significantly better space and time performance by
     * caching frequently requested values.
     *
     * @param  i an {@code int} value.
     * @return an {@code Integer} instance representing {@code i}.
     * @since  1.5
     */
    public static Integer valueOf(int i) {
/*if[INTEGER_CACHE_ENABLED]*/
        final int offset = 128;
        if (i >= -128 && i <= 127) { // must cache
            return IntegerCache.cache[i + offset];
        }
/*end[INTEGER_CACHE_ENABLED]*/
        return new Integer(i);
    }

    /**
     * The value of the Integer.
     *
     * @serial
     */
    private final int value;

    /**
     * Constructs a newly allocated <code>Integer</code> object that
     * represents the primitive <code>int</code> argument.
     *
     * @param   value   the value to be represented by the <code>Integer</code>.
     */
    public Integer(int value) {
        this.value = value;
    }

    /**
     * Constructs a newly allocated {@code Integer} object that
     * represents the {@code int} value indicated by the
     * {@code String} parameter. The string is converted to an
     * {@code int} value in exactly the manner used by the
     * {@code parseInt} method for radix 10.
     *
     * @param      s   the {@code String} to be converted to an
     *                 {@code Integer}.
     * @exception  NumberFormatException  if the {@code String} does not
     *               contain a parsable integer.
     * @see        java.lang.Integer#parseInt(java.lang.String, int)
     */
    public Integer(String s) throws NumberFormatException {
        this.value = parseInt(s, 10);
    }

    /**
     * Returns the value of this Integer as a byte.
     *
     * @return the value of this Integer as a byte.
     *
     * @since   JDK1.1
     */
    public byte byteValue() {
        return (byte)value;
    }

    /**
     * Returns the value of this Integer as a short.
     *
     * @return the value of this Integer as a short.
     *
     * @since   JDK1.1
     */
    public short shortValue() {
        return (short)value;
    }

    /**
     * Returns the value of this Integer as an int.
     *
     * @return  the <code>int</code> value represented by this object.
     */
    public int intValue() {
        return value;
    }

    /**
     * Returns the value of this Integer as a <tt>long</tt>.
     *
     * @return  the <code>int</code> value represented by this object that is
     *          converted to type <code>long</code> and the result of the
     *          conversion is returned.
     */
    public long longValue() {
        return (long)value;
    }

/*if[FLOATS]*/
    /**
     * Returns the value of this Integer as a <tt>float</tt>.
     *
     * @return  the <code>int</code> value represented by this object is
     *          converted to type <code>float</code> and the result of the
     *          conversion is returned.
     * @since   CLDC 1.1
     */
    public float floatValue() {
        return (float)value;
    }

    /**
     * Returns the value of this Integer as a <tt>double</tt>.
     *
     * @return  the <code>int</code> value represented by this object is
     *          converted to type <code>double</code> and the result of the
     *          conversion is returned.
     * @since   CLDC 1.1
     */
    public double doubleValue() {
        return (double)value;
    }
/*end[FLOATS]*/

    /**
     * Returns a String object representing this Integer's value. The
     * value is converted to signed decimal representation and returned
     * as a string, exactly as if the integer value were given as an
     * argument to the {@link java.lang.Integer#toString(int)} method.
     *
     * @return  a string representation of the value of this object in
     *          base&nbsp;10.
     */
    public String toString() {
        return String.valueOf(value);
    }

    /**
     * Returns a hashcode for this Integer.
     *
     * @return  a hash code value for this object, equal to the
     *          primitive <tt>int</tt> value represented by this
     *          <tt>Integer</tt> object.
     */
    public int hashCode() {
        return value;
    }

    /**
     * Compares this object to the specified object.
     * The result is <code>true</code> if and only if the argument is not
     * <code>null</code> and is an <code>Integer</code> object that contains
     * the same <code>int</code> value as this object.
     *
     * @param   obj   the object to compare with.
     * @return  <code>true</code> if the objects are the same;
     *          <code>false</code> otherwise.
     */
    public boolean equals(Object obj) {
        if (obj instanceof Integer) {
            return value == ((Integer)obj).intValue();
        }
        return false;
    }

    /**
     * Determines the integer value of the system property with the
     * specified name.
     *
     * <p>The first argument is treated as the name of a system property.
     * System properties are accessible through the
     * {@link java.lang.System#getProperty(java.lang.String)} method. The
     * string value of this property is then interpreted as an integer
     * value and an {@code Integer} object representing this value is
     * returned. Details of possible numeric formats can be found with
     * the definition of {@code getProperty}.
     *
     * <p>If there is no property with the specified name, if the specified name
     * is empty or {@code null}, or if the property does not have
     * the correct numeric format, then {@code null} is returned.
     *
     * <p>In other words, this method returns an {@code Integer}
     * object equal to the value of:
     *
     * <blockquote>
     *  {@code getInteger(nm, null)}
     * </blockquote>
     *
     * @param   nm   property name.
     * @return  the {@code Integer} value of the property.
     * @see     java.lang.System#getProperty(java.lang.String)
     * @see     java.lang.System#getProperty(java.lang.String, java.lang.String)
     */
    public static Integer getInteger(String nm) {
        return getInteger(nm, null);
    }

    /**
     * Determines the integer value of the system property with the
     * specified name.
     *
     * <p>The first argument is treated as the name of a system property.
     * System properties are accessible through the {@link
     * java.lang.System#getProperty(java.lang.String)} method. The
     * string value of this property is then interpreted as an integer
     * value and an {@code Integer} object representing this value is
     * returned. Details of possible numeric formats can be found with
     * the definition of {@code getProperty}.
     *
     * <p>The second argument is the default value. An {@code Integer} object
     * that represents the value of the second argument is returned if there
     * is no property of the specified name, if the property does not have
     * the correct numeric format, or if the specified name is empty or
     * {@code null}.
     *
     * <p>In other words, this method returns an {@code Integer} object
     * equal to the value of:
     *
     * <blockquote>
     *  {@code getInteger(nm, new Integer(val))}
     * </blockquote>
     *
     * but in practice it may be implemented in a manner such as:
     *
     * <blockquote><pre>
     * Integer result = getInteger(nm, null);
     * return (result == null) ? new Integer(val) : result;
     * </pre></blockquote>
     *
     * to avoid the unnecessary allocation of an {@code Integer}
     * object when the default value is not needed.
     *
     * @param   nm   property name.
     * @param   val   default value.
     * @return  the {@code Integer} value of the property.
     * @see     java.lang.System#getProperty(java.lang.String)
     * @see     java.lang.System#getProperty(java.lang.String, java.lang.String)
     */
    public static Integer getInteger(String nm, int val) {
        Integer result = getInteger(nm, null);
        return (result == null) ? new Integer(val) : result;
    }

    /**
     * Returns the integer value of the system property with the
     * specified name.  The first argument is treated as the name of a
     * system property.  System properties are accessible through the
     * {@link java.lang.System#getProperty(java.lang.String)} method.
     * The string value of this property is then interpreted as an
     * integer value, as per the {@code Integer.decode} method,
     * and an {@code Integer} object representing this value is
     * returned.
     *
     * <ul><li>If the property value begins with the two ASCII characters
     *         {@code 0x} or the ASCII character {@code #}, not
     *      followed by a minus sign, then the rest of it is parsed as a
     *      hexadecimal integer exactly as by the method
     *      {@link #valueOf(java.lang.String, int)} with radix 16.
     * <li>If the property value begins with the ASCII character
     *     {@code 0} followed by another character, it is parsed as an
     *     octal integer exactly as by the method
     *     {@link #valueOf(java.lang.String, int)} with radix 8.
     * <li>Otherwise, the property value is parsed as a decimal integer
     * exactly as by the method {@link #valueOf(java.lang.String, int)}
     * with radix 10.
     * </ul>
     *
     * <p>The second argument is the default value. The default value is
     * returned if there is no property of the specified name, if the
     * property does not have the correct numeric format, or if the
     * specified name is empty or {@code null}.
     *
     * @param   nm   property name.
     * @param   val   default value.
     * @return  the {@code Integer} value of the property.
     * @see     java.lang.System#getProperty(java.lang.String)
     * @see java.lang.System#getProperty(java.lang.String, java.lang.String)
     * @see java.lang.Integer#decode
     */
    public static Integer getInteger(String nm, Integer val) {
        String v = null;
        try {
            v = System.getProperty(nm);
        } catch (IllegalArgumentException e) {
        } catch (NullPointerException e) {
        }
        if (v != null) {
            try {
                return Integer.decode(v);
            } catch (NumberFormatException e) {
            }
        }
        return val;
    }

    /**
     * Decodes a {@code String} into an {@code Integer}.
     * Accepts decimal, hexadecimal, and octal numbers given
     * by the following grammar:
     *
     * <blockquote>
     * <dl>
     * <dt><i>DecodableString:</i>
     * <dd><i>Sign<sub>opt</sub> DecimalNumeral</i>
     * <dd><i>Sign<sub>opt</sub></i> {@code 0x} <i>HexDigits</i>
     * <dd><i>Sign<sub>opt</sub></i> {@code 0X} <i>HexDigits</i>
     * <dd><i>Sign<sub>opt</sub></i> {@code #} <i>HexDigits</i>
     * <dd><i>Sign<sub>opt</sub></i> {@code 0} <i>OctalDigits</i>
     * <p>
     * <dt><i>Sign:</i>
     * <dd>{@code -}
     * </dl>
     * </blockquote>
     *
     * <i>DecimalNumeral</i>, <i>HexDigits</i>, and <i>OctalDigits</i>
     * are defined in <a href="http://java.sun.com/docs/books/jls/second_edition/html/lexical.doc.html#48282">&sect;3.10.1</a>
     * of the <a href="http://java.sun.com/docs/books/jls/html/">Java
     * Language Specification</a>.
     *
     * <p>The sequence of characters following an (optional) negative
     * sign and/or radix specifier ("{@code 0x}", "{@code 0X}",
     * "{@code #}", or leading zero) is parsed as by the {@code
     * Integer.parseInt} method with the indicated radix (10, 16, or
     * 8).  This sequence of characters must represent a positive
     * value or a {@link NumberFormatException} will be thrown.  The
     * result is negated if first character of the specified {@code
     * String} is the minus sign.  No whitespace characters are
     * permitted in the {@code String}.
     *
     * @param     nm the {@code String} to decode.
     * @return    an {@code Integer} object holding the {@code int}
     *             value represented by {@code nm}
     * @exception NumberFormatException  if the {@code String} does not
     *            contain a parsable integer.
     * @see java.lang.Integer#parseInt(java.lang.String, int)
     */
    public static Integer decode(String nm) throws NumberFormatException {
        int radix = 10;
        int index = 0;
        boolean negative = false;
        Integer result;

        if (nm.length() == 0)
            throw new NumberFormatException("Zero length string");
        char firstChar = nm.charAt(0);
        // Handle sign, if present
        if (firstChar == '-') {
            negative = true;
            index++;
        }

        // Handle radix specifier, if present
        if (nm.startsWith("0x", index) || nm.startsWith("0X", index)) {
            index += 2;
            radix = 16;
        }
        else if (nm.startsWith("#", index)) {
            index ++;
            radix = 16;
        }
        else if (nm.startsWith("0", index) && nm.length() > 1 + index) {
            index ++;
            radix = 8;
        }

        if (nm.startsWith("-", index))
            throw new NumberFormatException("Sign character in wrong position");

        try {
            result = Integer.valueOf(nm.substring(index), radix);
            result = negative ? new Integer(-result.intValue()) : result;
        } catch (NumberFormatException e) {
            // If number is Integer.MIN_VALUE, we'll end up here. The next line
            // handles this case, and causes any genuine format error to be
            // rethrown.
            String constant = negative ? ("-" + nm.substring(index))
                                       : nm.substring(index);
            result = Integer.valueOf(constant, radix);
        }
        return result;
    }

    /**
     * Compares two {@code Integer} objects numerically.
     *
     * @param   anotherInteger   the {@code Integer} to be compared.
     * @return  the value {@code 0} if this {@code Integer} is
     *          equal to the argument {@code Integer}; a value less than
     *          {@code 0} if this {@code Integer} is numerically less
     *          than the argument {@code Integer}; and a value greater
     *          than {@code 0} if this {@code Integer} is numerically
     *           greater than the argument {@code Integer} (signed
     *           comparison).
     * @since   1.2
     */
    public int compareTo(Integer anotherInteger) {
        int thisVal = this.value;
        int anotherVal = anotherInteger.value;
        return (thisVal<anotherVal ? -1 : (thisVal==anotherVal ? 0 : 1));
    }


    // Bit twiddling

    /**
     * The number of bits used to represent an {@code int} value in two's
     * complement binary form.
     *
     * @since 1.5
     */
    public static final int SIZE = 32;

    /**
     * Returns an {@code int} value with at most a single one-bit, in the
     * position of the highest-order ("leftmost") one-bit in the specified
     * {@code int} value.  Returns zero if the specified value has no
     * one-bits in its two's complement binary representation, that is, if it
     * is equal to zero.
     *
     * @return an {@code int} value with a single one-bit, in the position
     *     of the highest-order one-bit in the specified value, or zero if
     *     the specified value is itself equal to zero.
     * @since 1.5
     */
    public static int highestOneBit(int i) {
        // HD, Figure 3-1
        i |= (i >>  1);
        i |= (i >>  2);
        i |= (i >>  4);
        i |= (i >>  8);
        i |= (i >> 16);
        return i - (i >>> 1);
    }

    /**
     * Returns an {@code int} value with at most a single one-bit, in the
     * position of the lowest-order ("rightmost") one-bit in the specified
     * {@code int} value.  Returns zero if the specified value has no
     * one-bits in its two's complement binary representation, that is, if it
     * is equal to zero.
     *
     * @return an {@code int} value with a single one-bit, in the position
     *     of the lowest-order one-bit in the specified value, or zero if
     *     the specified value is itself equal to zero.
     * @since 1.5
     */
    public static int lowestOneBit(int i) {
        // HD, Section 2-1
        return i & -i;
    }

    /**
     * Returns the number of zero bits preceding the highest-order
     * ("leftmost") one-bit in the two's complement binary representation
     * of the specified {@code int} value.  Returns 32 if the
     * specified value has no one-bits in its two's complement representation,
     * in other words if it is equal to zero.
     *
     * <p>Note that this method is closely related to the logarithm base 2.
     * For all positive {@code int} values x:
     * <ul>
     * <li>floor(log<sub>2</sub>(x)) = {@code 31 - numberOfLeadingZeros(x)}
     * <li>ceil(log<sub>2</sub>(x)) = {@code 32 - numberOfLeadingZeros(x - 1)}
     * </ul>
     *
     * @return the number of zero bits preceding the highest-order
     *     ("leftmost") one-bit in the two's complement binary representation
     *     of the specified {@code int} value, or 32 if the value
     *     is equal to zero.
     * @since 1.5
     */
    public static int numberOfLeadingZeros(int i) {
        // HD, Figure 5-6
        if (i == 0)
            return 32;
        int n = 1;
        if (i >>> 16 == 0) { n += 16; i <<= 16; }
        if (i >>> 24 == 0) { n +=  8; i <<=  8; }
        if (i >>> 28 == 0) { n +=  4; i <<=  4; }
        if (i >>> 30 == 0) { n +=  2; i <<=  2; }
        n -= i >>> 31;
        return n;
    }

    /**
     * Returns the number of zero bits following the lowest-order ("rightmost")
     * one-bit in the two's complement binary representation of the specified
     * {@code int} value.  Returns 32 if the specified value has no
     * one-bits in its two's complement representation, in other words if it is
     * equal to zero.
     *
     * @return the number of zero bits following the lowest-order ("rightmost")
     *     one-bit in the two's complement binary representation of the
     *     specified {@code int} value, or 32 if the value is equal
     *     to zero.
     * @since 1.5
     */
    public static int numberOfTrailingZeros(int i) {
        // HD, Figure 5-14
        int y;
        if (i == 0) return 32;
        int n = 31;
        y = i <<16; if (y != 0) { n = n -16; i = y; }
        y = i << 8; if (y != 0) { n = n - 8; i = y; }
        y = i << 4; if (y != 0) { n = n - 4; i = y; }
        y = i << 2; if (y != 0) { n = n - 2; i = y; }
        return n - ((i << 1) >>> 31);
    }

    /**
     * Returns the number of one-bits in the two's complement binary
     * representation of the specified {@code int} value.  This function is
     * sometimes referred to as the <i>population count</i>.
     *
     * @return the number of one-bits in the two's complement binary
     *     representation of the specified {@code int} value.
     * @since 1.5
     */
    public static int bitCount(int i) {
        // HD, Figure 5-2
        i = i - ((i >>> 1) & 0x55555555);
        i = (i & 0x33333333) + ((i >>> 2) & 0x33333333);
        i = (i + (i >>> 4)) & 0x0f0f0f0f;
        i = i + (i >>> 8);
        i = i + (i >>> 16);
        return i & 0x3f;
    }

    /**
     * Returns the value obtained by rotating the two's complement binary
     * representation of the specified {@code int} value left by the
     * specified number of bits.  (Bits shifted out of the left hand, or
     * high-order, side reenter on the right, or low-order.)
     *
     * <p>Note that left rotation with a negative distance is equivalent to
     * right rotation: {@code rotateLeft(val, -distance) == rotateRight(val,
     * distance)}.  Note also that rotation by any multiple of 32 is a
     * no-op, so all but the last five bits of the rotation distance can be
     * ignored, even if the distance is negative: {@code rotateLeft(val,
     * distance) == rotateLeft(val, distance & 0x1F)}.
     *
     * @return the value obtained by rotating the two's complement binary
     *     representation of the specified {@code int} value left by the
     *     specified number of bits.
     * @since 1.5
     */
    public static int rotateLeft(int i, int distance) {
        return (i << distance) | (i >>> -distance);
    }

    /**
     * Returns the value obtained by rotating the two's complement binary
     * representation of the specified {@code int} value right by the
     * specified number of bits.  (Bits shifted out of the right hand, or
     * low-order, side reenter on the left, or high-order.)
     *
     * <p>Note that right rotation with a negative distance is equivalent to
     * left rotation: {@code rotateRight(val, -distance) == rotateLeft(val,
     * distance)}.  Note also that rotation by any multiple of 32 is a
     * no-op, so all but the last five bits of the rotation distance can be
     * ignored, even if the distance is negative: {@code rotateRight(val,
     * distance) == rotateRight(val, distance & 0x1F)}.
     *
     * @return the value obtained by rotating the two's complement binary
     *     representation of the specified {@code int} value right by the
     *     specified number of bits.
     * @since 1.5
     */
    public static int rotateRight(int i, int distance) {
        return (i >>> distance) | (i << -distance);
    }

    /**
     * Returns the value obtained by reversing the order of the bits in the
     * two's complement binary representation of the specified {@code int}
     * value.
     *
     * @return the value obtained by reversing order of the bits in the
     *     specified {@code int} value.
     * @since 1.5
     */
    public static int reverse(int i) {
        // HD, Figure 7-1
        i = (i & 0x55555555) << 1 | (i >>> 1) & 0x55555555;
        i = (i & 0x33333333) << 2 | (i >>> 2) & 0x33333333;
        i = (i & 0x0f0f0f0f) << 4 | (i >>> 4) & 0x0f0f0f0f;
        i = (i << 24) | ((i & 0xff00) << 8) |
            ((i >>> 8) & 0xff00) | (i >>> 24);
        return i;
    }

    /**
     * Returns the signum function of the specified {@code int} value.  (The
     * return value is -1 if the specified value is negative; 0 if the
     * specified value is zero; and 1 if the specified value is positive.)
     *
     * @return the signum function of the specified {@code int} value.
     * @since 1.5
     */
    public static int signum(int i) {
        // HD, Section 2-7
        return (i >> 31) | (-i >>> 31);
    }

    /**
     * Returns the value obtained by reversing the order of the bytes in the
     * two's complement representation of the specified {@code int} value.
     *
     * @return the value obtained by reversing the bytes in the specified
     *     {@code int} value.
     * @since 1.5
     */
    public static int reverseBytes(int i) {
        return ((i >>> 24)           ) |
               ((i >>   8) &   0xFF00) |
               ((i <<   8) & 0xFF0000) |
               ((i << 24));
    }

}
