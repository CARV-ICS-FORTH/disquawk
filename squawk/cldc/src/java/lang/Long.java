/*
 * Copyright     2015, FORTH-ICS / CARV
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
 * The Long class wraps a value of the primitive type <code>long</code>
 * in an object. An object of type <code>Long</code> contains a single
 * field whose type is <code>long</code>.
 * <p>
 * In addition, this class provides several methods for converting a
 * <code>long</code> to a <code>String</code> and a
 * <code>String</code> to a <code>long</code>, as well as other
 * constants and methods useful when dealing with a
 * <code>long</code>.
 *
 * @version 12/17/01 (CLDC 1.1)
 * @since   JDK1.0, CLDC 1.0
 */
@Java5Marker
public final class Long {
    /**
     * The smallest value of type <code>long</code>.
     */
    public static final long MIN_VALUE = 0x8000000000000000L;

    /**
     * The largest value of type <code>long</code>.
     */
    public static final long MAX_VALUE = 0x7fffffffffffffffL;

    /**
     * Creates a string representation of the first argument in the
     * radix specified by the second argument.
     * <p>
     * If the radix is smaller than <code>Character.MIN_RADIX</code> or
     * larger than <code>Character.MAX_RADIX</code>, then the radix
     * <code>10</code> is used instead.
     * <p>
     * If the first argument is negative, the first element of the
     * result is the ASCII minus sign <code>'-'</code>
     * (<code>'&#92;u002d'</code>. If the first argument is not negative,
     * no sign character appears in the result.
     * <p>
     * The remaining characters of the result represent the magnitude of
     * the first argument. If the magnitude is zero, it is represented by
     * a single zero character <code>'0'</code>
     * (<code>'&#92;u0030'</code>); otherwise, the first character of the
     * representation of the magnitude will not be the zero character.
     * The following ASCII characters are used as digits:
     * <blockquote><pre>
     *   0123456789abcdefghijklmnopqrstuvwxyz
     * </pre></blockquote>
     * These are <tt>'&#92;u0030'</tt> through <tt>'&#92;u0039'</tt>
     * and <tt>'&#92;u0061'</tt> through <tt>'&#92;u007a'</tt>. If the
     * radix is <var>N</var>, then the first <var>N</var> of these
     * characters are used as radix-<var>N</var> digits in the order
     * shown. Thus, the digits for hexadecimal (radix 16) are
     * <blockquote><pre>
     * <tt>0123456789abcdef</tt>.
     * </pre></blockquote>
     *
     * @param   i       a long.
     * @param   radix   the radix.
     * @return  a string representation of the argument in the specified radix.
     * @see     java.lang.Character#MAX_RADIX
     * @see     java.lang.Character#MIN_RADIX
     */
    public static String toString(long i, int radix) {
        if (radix < Character.MIN_RADIX || radix > Character.MAX_RADIX)
            radix = 10;

        char[] buf = new char[65];
        int charPos = 64;
        boolean negative = (i < 0);

        if (!negative) {
            i = -i;
        }

        while (i <= -radix) {
            buf[charPos--] = Integer.digits[(int)(-(i % radix))];
            i = i / radix;
        }
        buf[charPos] = Integer.digits[(int)(-i)];

        if (negative) {
            buf[--charPos] = '-';
        }

        return new String(buf, charPos, (65 - charPos));
    }

    /**
     * Returns a new String object representing the specified integer.
     * The argument is converted to signed decimal representation and
     * returned as a string, exactly as if the argument and the radix
     * 10 were given as arguments to the
     * {@link #toString(long, int)} method that takes two arguments.
     *
     * @param   i   a <code>long</code> to be converted.
     * @return  a string representation of the argument in base&nbsp;10.
     */
    public static String toString(long i) {
        return toString(i, 10);
    }

    /**
     * Parses the string argument as a signed <code>long</code> in the
     * radix specified by the second argument. The characters in the
     * string must all be digits of the specified radix (as determined by
     * whether <code>Character.digit</code> returns a
     * nonnegative value), except that the first character may be an
     * ASCII minus sign <code>'-'</code> (<tt>'&#92;u002d'</tt> to indicate
     * a negative value. The resulting <code>long</code> value is returned.
     * <p>
     * Note that neither <tt>L</tt> nor <tt>l</tt> is permitted to appear at
     * the end of the string as a type indicator, as would be permitted in
     * Java programming language source code - except that either <tt>L</tt>
     * or <tt>l</tt> may appear as a digit for a radix greater than 22.
     * <p>
     * An exception of type <tt>NumberFormatException</tt> is thrown if any of
     * the following situations occurs:
     * <ul>
     * <li>The first argument is <tt>null</tt> or is a string of length zero.
     * <li>The <tt>radix</tt> is either smaller than
     *     {@link java.lang.Character#MIN_RADIX} or larger than
     *     {@link java.lang.Character#MAX_RADIX}.
     * <li>The first character of the string is not a digit of the
     *     specified <tt>radix</tt> and is not a minus sign <tt>'-'</tt>
     *     (<tt>'&#92;u002d'</tt>).
     * <li>The first character of the string is a minus sign and the
     *     string is of length 1.
     * <li>Any character of the string after the first is not a digit of
     *     the specified <tt>radix</tt>.
     * <li>The integer value represented by the string cannot be
     *     represented as a value of type <tt>long</tt>.
     * </ul><p>
     * Examples:
     * <blockquote><pre>
     * parseLong("0", 10) returns 0L
     * parseLong("473", 10) returns 473L
     * parseLong("-0", 10) returns 0L
     * parseLong("-FF", 16) returns -255L
     * parseLong("1100110", 2) returns 102L
     * parseLong("99", 8) throws a NumberFormatException
     * parseLong("Hazelnut", 10) throws a NumberFormatException
     * parseLong("Hazelnut", 36) returns 1356099454469L
     * </pre></blockquote>
     *
     * @param      s       the <code>String</code> containing the
     *                     <code>long</code>.
     * @param      radix   the radix to be used.
     * @return     the <code>long</code> represented by the string argument in
     *             the specified radix.
     * @exception  NumberFormatException  if the string does not contain a
     *                                    parsable integer.
     */
    public static long parseLong(String s, int radix)
              throws NumberFormatException
    {
      if (s == null) {
          throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                     "null"
/* #endif */
          );
      }

      if (radix < Character.MIN_RADIX) {
          throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                     "radix " + radix +
/// skipped                     " less than Character.MIN_RADIX"
/* #endif */
          );
      }
      if (radix > Character.MAX_RADIX) {
          throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                     "radix " + radix +
/// skipped                     " greater than Character.MAX_RADIX"
/* #endif */
          );
      }

      long result = 0;
      boolean negative = false;
      int i = 0, max = s.length();
      long limit;
      long multmin;
      int digit;

      if (max > 0) {
          if (s.charAt(0) == '-') {
              negative = true;
              limit = Long.MIN_VALUE;
              i++;
          } else {
              limit = -Long.MAX_VALUE;
          }
          multmin = limit / radix;
            if (i < max) {
                digit = Character.digit(s.charAt(i++),radix);
              if (digit < 0) {
                  throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                             s
/* #endif */
                  );
              } else {
                  result = -digit;
              }
          }
          while (i < max) {
              // Accumulating negatively avoids surprises near MAX_VALUE
              digit = Character.digit(s.charAt(i++),radix);
              if (digit < 0) {
                  throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                             s
/* #endif */
                  );
              }
              if (result < multmin) {
                  throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                             s
/* #endif */
                  );
              }
              result *= radix;
              if (result < limit + digit) {
                  throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                             s
/* #endif */
                  );
              }
              result -= digit;
          }
      } else {
          throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                     s
/* #endif */
          );
      }
      if (negative) {
          if (i > 1) {
              return result;
          } else {    /* Only got "-" */
              throw new NumberFormatException(
/* #ifdef VERBOSE_EXCEPTIONS */
/// skipped                         s
/* #endif */
              );
          }
      } else {
          return -result;
      }
    }

    /**
     * Parses the string argument as a signed decimal <code>long</code>.
     * The characters in the string must all be decimal digits, except
     * that the first character may be an ASCII minus sign
     * <code>'-'</code> (<code>&#92;u002d'</code>) to indicate a negative
     * value. The resulting long value is returned, exactly as if the
     * argument and the radix <tt>10</tt> were given as arguments to the
     * {@link #parseLong(String, int)} method that takes two arguments.
     * <p>
     * Note that neither <tt>L</tt> nor <tt>l</tt> is permitted to appear
     * at the end of the string as a type indicator, as would be permitted
     * in Java programming language source code.
     *
     * @param      s   a string.
     * @return     the <code>long</code> represented by the argument in decimal.
     * @exception  NumberFormatException  if the string does not contain a
     *               parsable <code>long</code>.
     */
    public static long parseLong(String s) throws NumberFormatException {
      return parseLong(s, 10);
    }

    /**
     * The value of the Long.
     */
    private long value;

    /**
     * Constructs a newly allocated <code>Long</code> object that
     * represents the primitive <code>long</code> argument.
     *
     * @param   value   the value to be represented by the
     *          <code>Long</code> object.
     */
    public Long(long value) {
        this.value = value;
    }

    /**
     * Returns the value of this {@code Long} as a
     * {@code byte}.
     */
    public byte byteValue() {
        return (byte)value;
    }

    /**
     * Returns the value of this {@code Long} as a
     * {@code short}.
     */
    public short shortValue() {
        return (short)value;
    }

    /**
     * Returns the value of this {@code Long} as an
     * {@code int}.
     */
    public int intValue() {
        return (int)value;
    }

    /**
     * Returns the value of this Long as a long value.
     *
     * @return  the <code>long</code> value represented by this object.
     */
    public long longValue() {
        return (long)value;
    }

/*if[FLOATS]*/
    /**
     * Returns the value of this Long as a float.
     *
     * @return  the <code>long</code> value represented by this object is
     *          converted to type <code>float</code> and the result of
     *          the conversion is returned.
     * @since   CLDC 1.1
     */
    public float floatValue() {
        return (float)value;
    }

    /**
     * Returns the value of this Long as a double.
     *
     * @return  the <code>long</code> value represented by this object that
     *          is converted to type <code>double</code> and the result of
     *          the conversion is returned.
     * @since   CLDC 1.1
     */
    public double doubleValue() {
        return (double)value;
    }
/*end[FLOATS]*/

    /**
     * Returns a String object representing this Long's value.
     * The long integer value represented by this Long object is converted
     * to signed decimal representation and returned as a string, exactly
     * as if the long value were given as an argument to the
     * {@link #toString(long)} method that takes one argument.
     *
     * @return  a string representation of this object in base&nbsp;10.
     */
    public String toString() {
        return String.valueOf(value);
    }

    /**
     * Places characters representing the integer i into the
     * character array buf. The characters are placed into
     * the buffer backwards starting with the least significant
     * digit at the specified index (exclusive), and working
     * backwards from there.
     *
     * Will fail if i == Long.MIN_VALUE
     */
    static void getChars(long i, int index, char[] buf) {
        long q;
        int r;
        int charPos = index;
        char sign = 0;

        if (i < 0) {
            sign = '-';
            i = -i;
        }

        // Get 2 digits/iteration using longs until quotient fits into an int
        while (i > Integer.MAX_VALUE) {
            q = i / 100;
            // really: r = i - (q * 100);
            r = (int)(i - ((q << 6) + (q << 5) + (q << 2)));
            i = q;
            buf[--charPos] = Integer.DigitOnes[r];
            buf[--charPos] = Integer.DigitTens[r];
        }

        // Get 2 digits/iteration using ints
        int q2;
        int i2 = (int)i;
        while (i2 >= 65536) {
            q2 = i2 / 100;
            // really: r = i2 - (q * 100);
            r = i2 - ((q2 << 6) + (q2 << 5) + (q2 << 2));
            i2 = q2;
            buf[--charPos] = Integer.DigitOnes[r];
            buf[--charPos] = Integer.DigitTens[r];
        }

        // Fall thru to fast mode for smaller numbers
        // assert(i2 <= 65536, i2);
        for (;;) {
            q2 = (i2 * 52429) >>> (16+3);
            r = i2 - ((q2 << 3) + (q2 << 1));  // r = i2-(q2*10) ...
            buf[--charPos] = Integer.digits[r];
            i2 = q2;
            if (i2 == 0) break;
        }
        if (sign != 0) {
            buf[--charPos] = sign;
        }
    }

    // Requires positive x
    static int stringSize(long x) {
        long p = 10;
        for (int i=1; i<19; i++) {
            if (x < p)
                return i;
            p = 10*p;
        }
        return 19;
    }

    /**
     * Computes a hashcode for this Long. The result is the exclusive
     * OR of the two halves of the primitive <code>long</code> value
     * represented by this <code>Long</code> object. That is, the hashcode
     * is the value of the expression:
     * <blockquote><pre>
     * (int)(this.longValue()^(this.longValue()>>>32))
     * </pre></blockquote>
     *
     * @return  a hash code value for this object.
     */
    public int hashCode() {
        return (int)(value ^ (value >> 32));
    }

    /**
     * Compares this object against the specified object.
     * The result is <code>true</code> if and only if the argument is
     * not <code>null</code> and is a <code>Long</code> object that
     * contains the same <code>long</code> value as this object.
     *
     * @param   obj   the object to compare with.
     * @return  <code>true</code> if the objects are the same;
     *          <code>false</code> otherwise.
     */
    public boolean equals(Object obj) {
        if (obj instanceof Long) {
            return value == ((Long)obj).longValue();
        }
        return false;
    }

    @Java5Marker
    public static Long valueOf(final long val) {
        return new Long(val);
    }


    // Bit Twiddling

    /**
     * The number of bits used to represent a {@code long} value in two's
     * complement binary form.
     *
     * @since 1.5
     */
    public static final int SIZE = 64;

    /**
     * Returns a {@code long} value with at most a single one-bit, in the
     * position of the highest-order ("leftmost") one-bit in the specified
     * {@code long} value.  Returns zero if the specified value has no
     * one-bits in its two's complement binary representation, that is, if it
     * is equal to zero.
     *
     * @return a {@code long} value with a single one-bit, in the position
     *     of the highest-order one-bit in the specified value, or zero if
     *     the specified value is itself equal to zero.
     * @since 1.5
     */
    public static long highestOneBit(long i) {
        // HD, Figure 3-1
        i |= (i >>  1);
        i |= (i >>  2);
        i |= (i >>  4);
        i |= (i >>  8);
        i |= (i >> 16);
        i |= (i >> 32);
        return i - (i >>> 1);
    }

    /**
     * Returns a {@code long} value with at most a single one-bit, in the
     * position of the lowest-order ("rightmost") one-bit in the specified
     * {@code long} value.  Returns zero if the specified value has no
     * one-bits in its two's complement binary representation, that is, if it
     * is equal to zero.
     *
     * @return a {@code long} value with a single one-bit, in the position
     *     of the lowest-order one-bit in the specified value, or zero if
     *     the specified value is itself equal to zero.
     * @since 1.5
     */
    public static long lowestOneBit(long i) {
        // HD, Section 2-1
        return i & -i;
    }

    /**
     * Returns the number of zero bits preceding the highest-order
     * ("leftmost") one-bit in the two's complement binary representation
     * of the specified {@code long} value.  Returns 64 if the
     * specified value has no one-bits in its two's complement representation,
     * in other words if it is equal to zero.
     *
     * <p>Note that this method is closely related to the logarithm base 2.
     * For all positive {@code long} values x:
     * <ul>
     * <li>floor(log<sub>2</sub>(x)) = {@code 63 - numberOfLeadingZeros(x)}
     * <li>ceil(log<sub>2</sub>(x)) = {@code 64 - numberOfLeadingZeros(x - 1)}
     * </ul>
     *
     * @return the number of zero bits preceding the highest-order
     *     ("leftmost") one-bit in the two's complement binary representation
     *     of the specified {@code long} value, or 64 if the value
     *     is equal to zero.
     * @since 1.5
     */
    public static int numberOfLeadingZeros(long i) {
        // HD, Figure 5-6
         if (i == 0)
            return 64;
        int n = 1;
        int x = (int)(i >>> 32);
        if (x == 0) { n += 32; x = (int)i; }
        if (x >>> 16 == 0) { n += 16; x <<= 16; }
        if (x >>> 24 == 0) { n +=  8; x <<=  8; }
        if (x >>> 28 == 0) { n +=  4; x <<=  4; }
        if (x >>> 30 == 0) { n +=  2; x <<=  2; }
        n -= x >>> 31;
        return n;
    }

    /**
     * Returns the number of zero bits following the lowest-order ("rightmost")
     * one-bit in the two's complement binary representation of the specified
     * {@code long} value.  Returns 64 if the specified value has no
     * one-bits in its two's complement representation, in other words if it is
     * equal to zero.
     *
     * @return the number of zero bits following the lowest-order ("rightmost")
     *     one-bit in the two's complement binary representation of the
     *     specified {@code long} value, or 64 if the value is equal
     *     to zero.
     * @since 1.5
     */
    public static int numberOfTrailingZeros(long i) {
        // HD, Figure 5-14
        int x, y;
        if (i == 0) return 64;
        int n = 63;
        y = (int)i; if (y != 0) { n = n -32; x = y; } else x = (int)(i>>>32);
        y = x <<16; if (y != 0) { n = n -16; x = y; }
        y = x << 8; if (y != 0) { n = n - 8; x = y; }
        y = x << 4; if (y != 0) { n = n - 4; x = y; }
        y = x << 2; if (y != 0) { n = n - 2; x = y; }
        return n - ((x << 1) >>> 31);
    }

    /**
     * Returns the number of one-bits in the two's complement binary
     * representation of the specified {@code long} value.  This function is
     * sometimes referred to as the <i>population count</i>.
     *
     * @return the number of one-bits in the two's complement binary
     *     representation of the specified {@code long} value.
     * @since 1.5
     */
     public static int bitCount(long i) {
        // HD, Figure 5-14
        i = i - ((i >>> 1) & 0x5555555555555555L);
        i = (i & 0x3333333333333333L) + ((i >>> 2) & 0x3333333333333333L);
        i = (i + (i >>> 4)) & 0x0f0f0f0f0f0f0f0fL;
        i = i + (i >>> 8);
        i = i + (i >>> 16);
        i = i + (i >>> 32);
        return (int)i & 0x7f;
     }

    /**
     * Returns the value obtained by rotating the two's complement binary
     * representation of the specified {@code long} value left by the
     * specified number of bits.  (Bits shifted out of the left hand, or
     * high-order, side reenter on the right, or low-order.)
     *
     * <p>Note that left rotation with a negative distance is equivalent to
     * right rotation: {@code rotateLeft(val, -distance) == rotateRight(val,
     * distance)}.  Note also that rotation by any multiple of 64 is a
     * no-op, so all but the last six bits of the rotation distance can be
     * ignored, even if the distance is negative: {@code rotateLeft(val,
     * distance) == rotateLeft(val, distance & 0x3F)}.
     *
     * @return the value obtained by rotating the two's complement binary
     *     representation of the specified {@code long} value left by the
     *     specified number of bits.
     * @since 1.5
     */
    public static long rotateLeft(long i, int distance) {
        return (i << distance) | (i >>> -distance);
    }

    /**
     * Returns the value obtained by rotating the two's complement binary
     * representation of the specified {@code long} value right by the
     * specified number of bits.  (Bits shifted out of the right hand, or
     * low-order, side reenter on the left, or high-order.)
     *
     * <p>Note that right rotation with a negative distance is equivalent to
     * left rotation: {@code rotateRight(val, -distance) == rotateLeft(val,
     * distance)}.  Note also that rotation by any multiple of 64 is a
     * no-op, so all but the last six bits of the rotation distance can be
     * ignored, even if the distance is negative: {@code rotateRight(val,
     * distance) == rotateRight(val, distance & 0x3F)}.
     *
     * @return the value obtained by rotating the two's complement binary
     *     representation of the specified {@code long} value right by the
     *     specified number of bits.
     * @since 1.5
     */
    public static long rotateRight(long i, int distance) {
        return (i >>> distance) | (i << -distance);
    }

    /**
     * Returns the value obtained by reversing the order of the bits in the
     * two's complement binary representation of the specified {@code long}
     * value.
     *
     * @return the value obtained by reversing order of the bits in the
     *     specified {@code long} value.
     * @since 1.5
     */
    public static long reverse(long i) {
        // HD, Figure 7-1
        i = (i & 0x5555555555555555L) << 1 | (i >>> 1) & 0x5555555555555555L;
        i = (i & 0x3333333333333333L) << 2 | (i >>> 2) & 0x3333333333333333L;
        i = (i & 0x0f0f0f0f0f0f0f0fL) << 4 | (i >>> 4) & 0x0f0f0f0f0f0f0f0fL;
        i = (i & 0x00ff00ff00ff00ffL) << 8 | (i >>> 8) & 0x00ff00ff00ff00ffL;
        i = (i << 48) | ((i & 0xffff0000L) << 16) |
            ((i >>> 16) & 0xffff0000L) | (i >>> 48);
        return i;
    }

    /**
     * Returns the signum function of the specified {@code long} value.  (The
     * return value is -1 if the specified value is negative; 0 if the
     * specified value is zero; and 1 if the specified value is positive.)
     *
     * @return the signum function of the specified {@code long} value.
     * @since 1.5
     */
    public static int signum(long i) {
        // HD, Section 2-7
        return (int) ((i >> 63) | (-i >>> 63));
    }

    /**
     * Returns the value obtained by reversing the order of the bytes in the
     * two's complement representation of the specified {@code long} value.
     *
     * @return the value obtained by reversing the bytes in the specified
     *     {@code long} value.
     * @since 1.5
     */
    public static long reverseBytes(long i) {
        i = (i & 0x00ff00ff00ff00ffL) << 8 | (i >>> 8) & 0x00ff00ff00ff00ffL;
        return (i << 48) | ((i & 0xffff0000L) << 16) |
            ((i >>> 16) & 0xffff0000L) | (i >>> 48);
    }

}
