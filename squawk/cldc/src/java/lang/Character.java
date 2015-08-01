/*
 *
 *
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
 * The Character class wraps a value of the primitive type <code>char</code>
 * in an object. An object of type <code>Character</code> contains a
 * single field whose type is <code>char</code>.
 * <p>
 * In addition, this class provides several methods for determining
 * the type of a character and converting characters from uppercase
 * to lowercase and vice versa.
 * <p>
 * Character information is based on the Unicode Standard, version 3.0.
 * However, in order to reduce footprint, by default the character
 * property and case conversion operations in CLDC are available
 * only for the ISO Latin-1 range of characters.  Other Unicode
 * character blocks can be supported as necessary.
 * <p>
 *
 * @version 12/17/01 (CLDC 1.1)
 * @since   JDK1.0, CLDC 1.0
 */

/*
 * Implementation note:
 *
 * The character property and case conversion facilities
 * provided by this CLDC implementation can be
 * extended by overriding an implementation class called
 * DefaultCaseConverter.  Refer to the end of this file
 * for details.
 */
public final class Character extends Object {

    /**
     * The minimum radix available for conversion to and from Strings.
     *
     * @see     java.lang.Integer#toString(int, int)
     * @see     java.lang.Integer#valueOf(java.lang.String)
     */
    public static final int MIN_RADIX = 2;

    /**
     * The maximum radix available for conversion to and from Strings.
     *
     * @see     java.lang.Integer#toString(int, int)
     * @see     java.lang.Integer#valueOf(java.lang.String)
     */
    public static final int MAX_RADIX = 36;

    /**
     * The constant value of this field is the smallest value of type
     * <code>char</code>.
     *
     * @since   JDK1.0.2
     */
    public static final char   MIN_VALUE = '\u0000';

    /**
     * The constant value of this field is the largest value of type
     * <code>char</code>.
     *
     * @since   JDK1.0.2
     */
    public static final char   MAX_VALUE = '\uffff';

    /**
     * The minimum value of a Unicode high-surrogate code unit in the
     * UTF-16 encoding. A high-surrogate is also known as a
     * <i>leading-surrogate</i>.
     *
     * @since 1.5
     */
    public static final char MIN_HIGH_SURROGATE = '\uD800';

    /**
     * The maximum value of a Unicode high-surrogate code unit in the
     * UTF-16 encoding. A high-surrogate is also known as a
     * <i>leading-surrogate</i>.
     *
     * @since 1.5
     */
    public static final char MAX_HIGH_SURROGATE = '\uDBFF';

    /**
     * The minimum value of a Unicode low-surrogate code unit in the
     * UTF-16 encoding. A low-surrogate is also known as a
     * <i>trailing-surrogate</i>.
     *
     * @since 1.5
     */
    public static final char MIN_LOW_SURROGATE  = '\uDC00';

    /**
     * The maximum value of a Unicode low-surrogate code unit in the
     * UTF-16 encoding. A low-surrogate is also known as a
     * <i>trailing-surrogate</i>.
     *
     * @since 1.5
     */
    public static final char MAX_LOW_SURROGATE  = '\uDFFF';

    /**
     * The minimum value of a Unicode surrogate code unit in the UTF-16 encoding.
     *
     * @since 1.5
     */
    public static final char MIN_SURROGATE = MIN_HIGH_SURROGATE;

    /**
     * The maximum value of a Unicode surrogate code unit in the UTF-16 encoding.
     *
     * @since 1.5
     */
    public static final char MAX_SURROGATE = MAX_LOW_SURROGATE;

    /**
     * The minimum value of a supplementary code point.
     *
     * @since 1.5
     */
    public static final int MIN_SUPPLEMENTARY_CODE_POINT = 0x010000;

    /**
     * The minimum value of a Unicode code point.
     *
     * @since 1.5
     */
    public static final int MIN_CODE_POINT = 0x000000;

    /**
     * The maximum value of a Unicode code point.
     *
     * @since 1.5
     */
    public static final int MAX_CODE_POINT = 0x10ffff;

	/**
     * The value of the Character.
     */
    private char value;

    /**
     * Constructs a <code>Character</code> object and initializes it so
     * that it represents the primitive <code>value</code> argument.
     *
     * @param  value   value for the new <code>Character</code> object.
     */
    public Character(char value) {
        this.value = value;
    }

    /**
     * Returns the value of this Character object.
     * @return  the primitive <code>char</code> value represented by
     *          this object.
     */
    public char charValue() {
        return value;
    }

    /**
     * Returns a hash code for this Character.
     * @return  a hash code value for this object.
     */
    public int hashCode() {
        return (int)value;
    }

    /**
     * Compares this object against the specified object.
     * The result is <code>true</code> if and only if the argument is not
     * <code>null</code> and is a <code>Character</code> object that
     * represents the same <code>char</code> value as this object.
     *
     * @param   obj   the object to compare with.
     * @return  <code>true</code> if the objects are the same;
     *          <code>false</code> otherwise.
     */
    public boolean equals(Object obj) {
        if (obj instanceof Character) {
            return value == ((Character)obj).charValue();
        }
        return false;
    }

    /**
     * Returns a String object representing this character's value.
     * Converts this <code>Character</code> object to a string. The
     * result is a string whose length is <code>1</code>. The string's
     * sole component is the primitive <code>char</code> value represented
     * by this object.
     *
     * @return  a string representation of this object.
     */
    public String toString() {
        char buf[] = {value};
        return String.valueOf(buf);
    }

    /**
     * Returns a <code>String</code> object representing the
     * specified <code>char</code>.  The result is a string of length
     * 1 consisting solely of the specified <code>char</code>.
     *
     * @param c the <code>char</code> to be converted
     * @return the string representation of the specified <code>char</code>
     * @since 1.4
     */
    public static String toString(char c) {
        return String.valueOf(c);
    }

    /**
     * Determines if the specified character is a lowercase character.
     * <p>
     * Note that by default CLDC only supports
     * the ISO Latin-1 range of characters.
     * <p>
     * Of the ISO Latin-1 characters (character codes 0x0000 through 0x00FF),
     * the following are lowercase:
     * <p>
     * a b c d e f g h i j k l m n o p q r s t u v w x y z
     * &#92;u00DF &#92;u00E0 &#92;u00E1 &#92;u00E2 &#92;u00E3 &#92;u00E4 &#92;u00E5 &#92;u00E6 &#92;u00E7
     * &#92;u00E8 &#92;u00E9 &#92;u00EA &#92;u00EB &#92;u00EC &#92;u00ED &#92;u00EE &#92;u00EF &#92;u00F0
     * &#92;u00F1 &#92;u00F2 &#92;u00F3 &#92;u00F4 &#92;u00F5 &#92;u00F6 &#92;u00F8 &#92;u00F9 &#92;u00FA
     * &#92;u00FB &#92;u00FC &#92;u00FD &#92;u00FE &#92;u00FF
     *
     * @param   ch   the character to be tested.
     * @return  <code>true</code> if the character is lowercase;
     *          <code>false</code> otherwise.
     * @since   JDK1.0
     */
    public static boolean isLowerCase(char ch) {
        return (ch >= 'a' && ch <= 'z')
                || (ch >= 0xDF && ch <= 0xF6)
                || (ch >= 0xF8 && ch <= 0xFF);
    }

   /**
     * Determines if the specified character is an uppercase character.
     * <p>
     * Note that by default CLDC only supports
     * the ISO Latin-1 range of characters.
     * <p>
     * Of the ISO Latin-1 characters (character codes 0x0000 through 0x00FF),
     * the following are uppercase:
     * <p>
     * A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
     * &#92;u00C0 &#92;u00C1 &#92;u00C2 &#92;u00C3 &#92;u00C4 &#92;u00C5 &#92;u00C6 &#92;u00C7
     * &#92;u00C8 &#92;u00C9 &#92;u00CA &#92;u00CB &#92;u00CC &#92;u00CD &#92;u00CE &#92;u00CF &#92;u00D0
     * &#92;u00D1 &#92;u00D2 &#92;u00D3 &#92;u00D4 &#92;u00D5 &#92;u00D6 &#92;u00D8 &#92;u00D9 &#92;u00DA
     * &#92;u00DB &#92;u00DC &#92;u00DD &#92;u00DE
     *
     * @param   ch   the character to be tested.
     * @return  <code>true</code> if the character is uppercase;
     *          <code>false</code> otherwise.
     * @see     java.lang.Character#isLowerCase(char)
     * @see     java.lang.Character#toUpperCase(char)
     * @since   1.0
     */
    public static boolean isUpperCase(char ch) {
        return (ch >= 'A'  && ch <= 'Z')
            || (ch >= 0xC0 && ch <= 0xD6)
            || (ch >= 0xD8 && ch <= 0xDE );
    }

    /**
     * Determines the number of <code>char</code> values needed to
     * represent the specified character (Unicode code point). If the
     * specified character is equal to or greater than 0x10000, then
     * the method returns 2. Otherwise, the method returns 1.
     *
     * <p>This method doesn't validate the specified character to be a
     * valid Unicode code point. The caller must validate the
     * character value using {@link #isValidCodePoint(int) isValidCodePoint}
     * if necessary.
     *
     * @param   codePoint the character (Unicode code point) to be tested.
     * @return  2 if the character is a valid supplementary character; 1 otherwise.
     * @see     #isSupplementaryCodePoint(int)
     * @since   1.5
     */
    public static int charCount(int codePoint)
    {
      return codePoint >= 0x10000 ? 2 : 1;
    }

    /**
     * Determines if the specified character is a digit.
     *
     * This is currently only supported for ISO Latin-1 digits: "0" through "9".
     *
     * @param   ch   the character to be tested.
     * @return  <code>true</code> if the character is a digit;
     *          <code>false</code> otherwise.
     * @since   JDK1.0
     */
    public static boolean isDigit(char ch) {
        return ch >= '0' && ch <= '9';
    }

    /**
     * The given character is mapped to its lowercase equivalent; if the
     * character has no lowercase equivalent, the character itself is
     * returned.
     * <p>
     * Note that by default CLDC only supports
     * the ISO Latin-1 range of characters.
     *
     * @param   ch   the character to be converted.
     * @return  the lowercase equivalent of the character, if any;
     *          otherwise the character itself.
     * @see     java.lang.Character#isLowerCase(char)
     * @see     java.lang.Character#isUpperCase(char)
     * @see     java.lang.Character#toUpperCase(char)
     * @since   JDK1.0
     */
    public static char toLowerCase(char ch) {
        if (isUpperCase(ch)) {
            if (ch <= 'Z') {
                return (char)(ch + ('a' - 'A'));
            } else {
                return (char)(ch + 0x20);
            }
        } else {
            return ch;
        }
    }

    // /**
    //  * Converts the character (Unicode code point) argument to
    //  * lowercase using case mapping information from the UnicodeData
    //  * file.
    //  *
    //  * <p> Note that
    //  * <code>Character.isLowerCase(Character.toLowerCase(codePoint))</code>
    //  * does not always return <code>true</code> for some ranges of
    //  * characters, particularly those that are symbols or ideographs.
    //  *
    //  * <p>In general, {@link java.lang.String#toLowerCase()} should be used to map
    //  * characters to lowercase. <code>String</code> case mapping methods
    //  * have several benefits over <code>Character</code> case mapping methods.
    //  * <code>String</code> case mapping methods can perform locale-sensitive
    //  * mappings, context-sensitive mappings, and 1:M character mappings, whereas
    //  * the <code>Character</code> case mapping methods cannot.
    //  *
    //  * @param   codePoint   the character (Unicode code point) to be converted.
    //  * @return  the lowercase equivalent of the character (Unicode code
    //  *          point), if any; otherwise, the character itself.
    //  * @see     java.lang.Character#isLowerCase(int)
    //  * @see     java.lang.String#toLowerCase()
    //  *
    //  * @since   1.5
    //  */
    // public static int toLowerCase(int codePoint) {
    //     return CharacterData.of(codePoint).toLowerCase(codePoint);
    // }

    /**
     * Converts the character argument to uppercase; if the
     * character has no uppercase equivalent, the character itself is
     * returned.
     * <p>
     * Note that by default CLDC only supports
     * the ISO Latin-1 range of characters.
     *
     * @param   ch   the character to be converted.
     * @return  the uppercase equivalent of the character, if any;
     *          otherwise the character itself.
     * @see     java.lang.Character#isLowerCase(char)
     * @see     java.lang.Character#isUpperCase(char)
     * @see     java.lang.Character#toLowerCase(char)
     * @since   JDK1.0
     */
    public static char toUpperCase(char ch) {
        if (isLowerCase(ch)) {
            if (ch <= 'z') {
                return (char)(ch - ('a' - 'A'));
            } else {
                return (char)(ch - 0x20);
            }
        } else {
            return ch;
        }
    }

    // /**
    //  * Converts the character (Unicode code point) argument to
    //  * uppercase using case mapping information from the UnicodeData
    //  * file.
    //  *
    //  * <p>Note that
    //  * <code>Character.isUpperCase(Character.toUpperCase(codePoint))</code>
    //  * does not always return <code>true</code> for some ranges of
    //  * characters, particularly those that are symbols or ideographs.
    //  *
    //  * <p>In general, {@link java.lang.String#toUpperCase()} should be used to map
    //  * characters to uppercase. <code>String</code> case mapping methods
    //  * have several benefits over <code>Character</code> case mapping methods.
    //  * <code>String</code> case mapping methods can perform locale-sensitive
    //  * mappings, context-sensitive mappings, and 1:M character mappings, whereas
    //  * the <code>Character</code> case mapping methods cannot.
    //  *
    //  * @param   codePoint   the character (Unicode code point) to be converted.
    //  * @return  the uppercase equivalent of the character, if any;
    //  *          otherwise, the character itself.
    //  * @see     java.lang.Character#isUpperCase(int)
    //  * @see     java.lang.String#toUpperCase()
    //  *
    //  * @since   1.5
    //  */
    // public static int toUpperCase(int codePoint) {
    //     return CharacterData.of(codePoint).toUpperCase(codePoint);
    // }

    /**
     * Returns the numeric value of the character <code>ch</code> in the
     * specified radix.
     *
     * This is only supported for ISO Latin-1 characters.
     *
     * @param   ch      the character to be converted.
     * @param   radix   the radix.
     * @return  the numeric value represented by the character in the
     *          specified radix.
     * @see     java.lang.Character#isDigit(char)
     * @since   JDK1.0
     */
    public static int digit(char ch, int radix) {
        int value = -1;
        if (radix >= Character.MIN_RADIX && radix <= Character.MAX_RADIX) {
          if (isDigit(ch)) {
              value = ch - '0';
          }
          else if (isUpperCase(ch) || isLowerCase(ch)) {
              // Java supradecimal digit
              value = (ch & 0x1F) + 9;
          }
        }
        return (value < radix) ? value : -1;
    }

    @Java5Marker
    public static Character valueOf(final char val) {
        return new Character(val);
    }


    /**
     * Determines whether the specified code point is a valid Unicode
     * code point value in the range of <code>0x0000</code> to
     * <code>0x10FFFF</code> inclusive. This method is equivalent to
     * the expression:
     *
     * <blockquote><pre>
     * codePoint >= 0x0000 && codePoint <= 0x10FFFF
     * </pre></blockquote>
     *
     * @param  codePoint the Unicode code point to be tested
     * @return <code>true</code> if the specified code point value
     * is a valid code point value;
     * <code>false</code> otherwise.
     * @since  1.5
     */
    public static boolean isValidCodePoint(int codePoint) {
        return codePoint >= MIN_CODE_POINT && codePoint <= MAX_CODE_POINT;
    }

    /**
     * Determines whether the specified character (Unicode code point)
     * is in the supplementary character range. The method call is
     * equivalent to the expression:
     * <blockquote><pre>
     * codePoint >= 0x10000 && codePoint <= 0x10FFFF
     * </pre></blockquote>
     *
     * @param  codePoint the character (Unicode code point) to be tested
     * @return <code>true</code> if the specified character is in the Unicode
     *         supplementary character range; <code>false</code> otherwise.
     * @since  1.5
     */
    public static boolean isSupplementaryCodePoint(int codePoint) {
        return codePoint >= MIN_SUPPLEMENTARY_CODE_POINT
            && codePoint <= MAX_CODE_POINT;
    }

    /**
     * Determines if the given <code>char</code> value is a
     * high-surrogate code unit (also known as <i>leading-surrogate
     * code unit</i>). Such values do not represent characters by
     * themselves, but are used in the representation of <a
     * href="#supplementary">supplementary characters</a> in the
     * UTF-16 encoding.
     *
     * <p>This method returns <code>true</code> if and only if
     * <blockquote><pre>ch >= '&#92;uD800' && ch <= '&#92;uDBFF'
     * </pre></blockquote>
     * is <code>true</code>.
     *
     * @param   ch   the <code>char</code> value to be tested.
     * @return  <code>true</code> if the <code>char</code> value
     *          is between '&#92;uD800' and '&#92;uDBFF' inclusive;
     *          <code>false</code> otherwise.
     * @see     java.lang.Character#isLowSurrogate(char)
     * @see     Character.UnicodeBlock#of(int)
     * @since   1.5
     */
    public static boolean isHighSurrogate(char ch) {
        return ch >= MIN_HIGH_SURROGATE && ch <= MAX_HIGH_SURROGATE;
    }

    /**
     * Determines if the given <code>char</code> value is a
     * low-surrogate code unit (also known as <i>trailing-surrogate code
     * unit</i>). Such values do not represent characters by themselves,
     * but are used in the representation of <a
     * href="#supplementary">supplementary characters</a> in the UTF-16 encoding.
     *
     * <p> This method returns <code>true</code> if and only if
     * <blockquote><pre>ch >= '&#92;uDC00' && ch <= '&#92;uDFFF'
     * </pre></blockquote> is <code>true</code>.
     *
     * @param   ch   the <code>char</code> value to be tested.
     * @return  <code>true</code> if the <code>char</code> value
     *          is between '&#92;uDC00' and '&#92;uDFFF' inclusive;
     *          <code>false</code> otherwise.
     * @see java.lang.Character#isHighSurrogate(char)
     * @since   1.5
     */
    public static boolean isLowSurrogate(char ch) {
        return ch >= MIN_LOW_SURROGATE && ch <= MAX_LOW_SURROGATE;
    }

    /**
     * Determines whether the specified pair of <code>char</code>
     * values is a valid surrogate pair. This method is equivalent to
     * the expression:
     * <blockquote><pre>
     * isHighSurrogate(high) && isLowSurrogate(low)
     * </pre></blockquote>
     *
     * @param  high the high-surrogate code value to be tested
     * @param  low the low-surrogate code value to be tested
     * @return <code>true</code> if the specified high and
     * low-surrogate code values represent a valid surrogate pair;
     * <code>false</code> otherwise.
     * @since  1.5
     */
    public static boolean isSurrogatePair(char high, char low) {
        return isHighSurrogate(high) && isLowSurrogate(low);
    }

    /**
     * Converts the specified surrogate pair to its supplementary code
     * point value. This method does not validate the specified
     * surrogate pair. The caller must validate it using {@link
     * #isSurrogatePair(char, char) isSurrogatePair} if necessary.
     *
     * @param  high the high-surrogate code unit
     * @param  low the low-surrogate code unit
     * @return the supplementary code point composed from the
     *         specified surrogate pair.
     * @since  1.5
     */
    public static int toCodePoint(char high, char low) {
        return ((high - MIN_HIGH_SURROGATE) << 10)
            + (low - MIN_LOW_SURROGATE) + MIN_SUPPLEMENTARY_CODE_POINT;
    }

    /**
     * Returns the code point at the given index of the
     * <code>CharSequence</code>. If the <code>char</code> value at
     * the given index in the <code>CharSequence</code> is in the
     * high-surrogate range, the following index is less than the
     * length of the <code>CharSequence</code>, and the
     * <code>char</code> value at the following index is in the
     * low-surrogate range, then the supplementary code point
     * corresponding to this surrogate pair is returned. Otherwise,
     * the <code>char</code> value at the given index is returned.
     *
     * @param seq a sequence of <code>char</code> values (Unicode code
     * units)
     * @param index the index to the <code>char</code> values (Unicode
     * code units) in <code>seq</code> to be converted
     * @return the Unicode code point at the given index
     * @exception NullPointerException if <code>seq</code> is null.
     * @exception IndexOutOfBoundsException if the value
     * <code>index</code> is negative or not less than
     * {@link CharSequence#length() seq.length()}.
     * @since  1.5
     */
    public static int codePointAt(CharSequence seq, int index) {
        char c1 = seq.charAt(index++);
        if (isHighSurrogate(c1)) {
            if (index < seq.length()) {
                char c2 = seq.charAt(index);
                if (isLowSurrogate(c2)) {
                    return toCodePoint(c1, c2);
                }
            }
        }
        return c1;
    }

    /**
     * Returns the code point at the given index of the
     * <code>char</code> array. If the <code>char</code> value at
     * the given index in the <code>char</code> array is in the
     * high-surrogate range, the following index is less than the
     * length of the <code>char</code> array, and the
     * <code>char</code> value at the following index is in the
     * low-surrogate range, then the supplementary code point
     * corresponding to this surrogate pair is returned. Otherwise,
     * the <code>char</code> value at the given index is returned.
     *
     * @param a the <code>char</code> array
     * @param index the index to the <code>char</code> values (Unicode
     * code units) in the <code>char</code> array to be converted
     * @return the Unicode code point at the given index
     * @exception NullPointerException if <code>a</code> is null.
     * @exception IndexOutOfBoundsException if the value
     * <code>index</code> is negative or not less than
     * the length of the <code>char</code> array.
     * @since  1.5
     */
    public static int codePointAt(char[] a, int index) {
        return codePointAtImpl(a, index, a.length);
    }

    /**
     * Returns the code point at the given index of the
     * <code>char</code> array, where only array elements with
     * <code>index</code> less than <code>limit</code> can be used. If
     * the <code>char</code> value at the given index in the
     * <code>char</code> array is in the high-surrogate range, the
     * following index is less than the <code>limit</code>, and the
     * <code>char</code> value at the following index is in the
     * low-surrogate range, then the supplementary code point
     * corresponding to this surrogate pair is returned. Otherwise,
     * the <code>char</code> value at the given index is returned.
     *
     * @param a the <code>char</code> array
     * @param index the index to the <code>char</code> values (Unicode
     * code units) in the <code>char</code> array to be converted
     * @param limit the index after the last array element that can be used in the
     * <code>char</code> array
     * @return the Unicode code point at the given index
     * @exception NullPointerException if <code>a</code> is null.
     * @exception IndexOutOfBoundsException if the <code>index</code>
     * argument is negative or not less than the <code>limit</code>
     * argument, or if the <code>limit</code> argument is negative or
     * greater than the length of the <code>char</code> array.
     * @since  1.5
     */
    public static int codePointAt(char[] a, int index, int limit) {
        if (index >= limit || limit < 0 || limit > a.length) {
            throw new IndexOutOfBoundsException();
        }
        return codePointAtImpl(a, index, limit);
    }

    static int codePointAtImpl(char[] a, int index, int limit) {
        char c1 = a[index++];
        if (isHighSurrogate(c1)) {
            if (index < limit) {
                char c2 = a[index];
                if (isLowSurrogate(c2)) {
                    return toCodePoint(c1, c2);
                }
            }
        }
        return c1;
    }

    /**
     * Returns the code point preceding the given index of the
     * <code>CharSequence</code>. If the <code>char</code> value at
     * <code>(index - 1)</code> in the <code>CharSequence</code> is in
     * the low-surrogate range, <code>(index - 2)</code> is not
     * negative, and the <code>char</code> value at <code>(index -
     * 2)</code> in the <code>CharSequence</code> is in the
     * high-surrogate range, then the supplementary code point
     * corresponding to this surrogate pair is returned. Otherwise,
     * the <code>char</code> value at <code>(index - 1)</code> is
     * returned.
     *
     * @param seq the <code>CharSequence</code> instance
     * @param index the index following the code point that should be returned
     * @return the Unicode code point value before the given index.
     * @exception NullPointerException if <code>seq</code> is null.
     * @exception IndexOutOfBoundsException if the <code>index</code>
     * argument is less than 1 or greater than {@link
     * CharSequence#length() seq.length()}.
     * @since  1.5
     */
    public static int codePointBefore(CharSequence seq, int index) {
        char c2 = seq.charAt(--index);
        if (isLowSurrogate(c2)) {
            if (index > 0) {
                char c1 = seq.charAt(--index);
                if (isHighSurrogate(c1)) {
                    return toCodePoint(c1, c2);
                }
            }
        }
        return c2;
    }

    /**
     * Returns the code point preceding the given index of the
     * <code>char</code> array. If the <code>char</code> value at
     * <code>(index - 1)</code> in the <code>char</code> array is in
     * the low-surrogate range, <code>(index - 2)</code> is not
     * negative, and the <code>char</code> value at <code>(index -
     * 2)</code> in the <code>char</code> array is in the
     * high-surrogate range, then the supplementary code point
     * corresponding to this surrogate pair is returned. Otherwise,
     * the <code>char</code> value at <code>(index - 1)</code> is
     * returned.
     *
     * @param a the <code>char</code> array
     * @param index the index following the code point that should be returned
     * @return the Unicode code point value before the given index.
     * @exception NullPointerException if <code>a</code> is null.
     * @exception IndexOutOfBoundsException if the <code>index</code>
     * argument is less than 1 or greater than the length of the
     * <code>char</code> array
     * @since  1.5
     */
    public static int codePointBefore(char[] a, int index) {
        return codePointBeforeImpl(a, index, 0);
    }

    /**
     * Returns the code point preceding the given index of the
     * <code>char</code> array, where only array elements with
     * <code>index</code> greater than or equal to <code>start</code>
     * can be used. If the <code>char</code> value at <code>(index -
     * 1)</code> in the <code>char</code> array is in the
     * low-surrogate range, <code>(index - 2)</code> is not less than
     * <code>start</code>, and the <code>char</code> value at
     * <code>(index - 2)</code> in the <code>char</code> array is in
     * the high-surrogate range, then the supplementary code point
     * corresponding to this surrogate pair is returned. Otherwise,
     * the <code>char</code> value at <code>(index - 1)</code> is
     * returned.
     *
     * @param a the <code>char</code> array
     * @param index the index following the code point that should be returned
     * @param start the index of the first array element in the
     * <code>char</code> array
     * @return the Unicode code point value before the given index.
     * @exception NullPointerException if <code>a</code> is null.
     * @exception IndexOutOfBoundsException if the <code>index</code>
     * argument is not greater than the <code>start</code> argument or
     * is greater than the length of the <code>char</code> array, or
     * if the <code>start</code> argument is negative or not less than
     * the length of the <code>char</code> array.
     * @since  1.5
     */
    public static int codePointBefore(char[] a, int index, int start) {
        if (index <= start || start < 0 || start >= a.length) {
            throw new IndexOutOfBoundsException();
        }
        return codePointBeforeImpl(a, index, start);
    }

    static int codePointBeforeImpl(char[] a, int index, int start) {
        char c2 = a[--index];
        if (isLowSurrogate(c2)) {
            if (index > start) {
                char c1 = a[--index];
                if (isHighSurrogate(c1)) {
                    return toCodePoint(c1, c2);
                }
            }
        }
        return c2;
    }


    /**
     * Converts the specified character (Unicode code point) to its
     * UTF-16 representation. If the specified code point is a BMP
     * (Basic Multilingual Plane or Plane 0) value, the same value is
     * stored in <code>dst[dstIndex]</code>, and 1 is returned. If the
     * specified code point is a supplementary character, its
     * surrogate values are stored in <code>dst[dstIndex]</code>
     * (high-surrogate) and <code>dst[dstIndex+1]</code>
     * (low-surrogate), and 2 is returned.
     *
     * @param  codePoint the character (Unicode code point) to be converted.
     * @param  dst an array of <code>char</code> in which the
     * <code>codePoint</code>'s UTF-16 value is stored.
     * @param dstIndex the start index into the <code>dst</code>
     * array where the converted value is stored.
     * @return 1 if the code point is a BMP code point, 2 if the
     * code point is a supplementary code point.
     * @exception IllegalArgumentException if the specified
     * <code>codePoint</code> is not a valid Unicode code point.
     * @exception NullPointerException if the specified <code>dst</code> is null.
     * @exception IndexOutOfBoundsException if <code>dstIndex</code>
     * is negative or not less than <code>dst.length</code>, or if
     * <code>dst</code> at <code>dstIndex</code> doesn't have enough
     * array element(s) to store the resulting <code>char</code>
     * value(s). (If <code>dstIndex</code> is equal to
     * <code>dst.length-1</code> and the specified
     * <code>codePoint</code> is a supplementary character, the
     * high-surrogate value is not stored in
     * <code>dst[dstIndex]</code>.)
     * @since  1.5
     */
    public static int toChars(int codePoint, char[] dst, int dstIndex) {
        if (codePoint < 0 || codePoint > MAX_CODE_POINT) {
            throw new IllegalArgumentException();
        }
        if (codePoint < MIN_SUPPLEMENTARY_CODE_POINT) {
            dst[dstIndex] = (char) codePoint;
            return 1;
        }
        toSurrogates(codePoint, dst, dstIndex);
        return 2;
    }

    /**
     * Converts the specified character (Unicode code point) to its
     * UTF-16 representation stored in a <code>char</code> array. If
     * the specified code point is a BMP (Basic Multilingual Plane or
     * Plane 0) value, the resulting <code>char</code> array has
     * the same value as <code>codePoint</code>. If the specified code
     * point is a supplementary code point, the resulting
     * <code>char</code> array has the corresponding surrogate pair.
     *
     * @param  codePoint a Unicode code point
     * @return a <code>char</code> array having
     *         <code>codePoint</code>'s UTF-16 representation.
     * @exception IllegalArgumentException if the specified
     * <code>codePoint</code> is not a valid Unicode code point.
     * @since  1.5
     */
    public static char[] toChars(int codePoint) {
        if (codePoint < 0 || codePoint > MAX_CODE_POINT) {
            throw new IllegalArgumentException();
        }
        if (codePoint < MIN_SUPPLEMENTARY_CODE_POINT) {
                return new char[] { (char) codePoint };
        }
        char[] result = new char[2];
        toSurrogates(codePoint, result, 0);
        return result;
    }

    static void toSurrogates(int codePoint, char[] dst, int index) {
        int offset = codePoint - MIN_SUPPLEMENTARY_CODE_POINT;
        dst[index+1] = (char)((offset & 0x3ff) + MIN_LOW_SURROGATE);
        dst[index] = (char)((offset >>> 10) + MIN_HIGH_SURROGATE);
    }

}
