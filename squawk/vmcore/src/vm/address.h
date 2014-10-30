/*
 * Copyright 2013-2014 FORTH-ICS / CARV
 *                     (Foundation for Research & Technology -- Hellas,
 *                      Institute of Computer Science,
 *                      Computer Architecture & VLSI Systems Laboratory)
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

/*
 * This file define types and functions to abstract the size of
 * machine words and addresses.  This eases portability between 32 and
 * 64 bit platforms.
 */

#ifndef VM_ADDRESS_H_
#define VM_ADDRESS_H_

typedef void*          Address;
typedef unsigned char* ByteAddress;
#if SQUAWK_64
typedef ujlong UWord;
typedef jlong Offset;
#define ADDRESS_MAX ((Address)0xFFFFFFFFFFFFFFFFL)
#define WORD_MAX    ((UWord)0xFFFFFFFFFFFFFFFFL)
#define DEADBEEF    (0xDEADBEEFDEADBEEFL)
#else
typedef unsigned int UWord;
typedef int Offset;
#define ADDRESS_MAX ((Address)0xFFFFFFFF)
#define WORD_MAX    ((UWord)0xFFFFFFFF)
#define DEADBEEF    (0xDEADBEEF)
#endif /* SQUAWK_64 */
typedef UWord* UWordAddress;

/**
 * Unsigned comparison between two machine word sized values.
 *
 * @param left  the first value
 * @param right the second value
 * @return true if 'left' is lower than 'right' in an unsigned comparison
 */
#define lo(left, right) ((UWord)left < (UWord)right)

/**
 * Unsigned comparison between two machine word sized values.
 *
 * @param left  the first value
 * @param right the second value
 * @return true if 'left' is lower than or equal to 'right' in an unsigned comparison
 */
#define loeq(left, right) ((UWord)left <= (UWord)right)

/**
 * Unsigned comparison between two machine word sized values.
 *
 * @param left  the first value
 * @param right the second value
 * @return true if 'left' is higher than 'right' in an unsigned comparison
 */
#define hi(left, right) ((UWord)left > (UWord)right)

/**
 * Unsigned comparison between two machine word sized values.
 *
 * @param left  the first value
 * @param right the second value
 * @return true if 'left' is higher than or equal to 'right' in an unsigned comparison
 */
#define hieq(left, right) ((UWord)left >= (UWord)right)

/**
 * Signed comparison between two machine word sized values.
 *
 * @param left  the first value
 * @param right the second value
 * @return true if 'left' is lower than 'right' in a signed comparison
 */
#define lt(left, right) ((Offset)left < (Offset)right)

/**
 * Signed comparison between two machine word sized values.
 *
 * @param left  the first value
 * @param right the second value
 * @return true if 'left' is lower than or equal to 'right' in a signed comparison
 */
#define le(left, right) ((Offset)left <= (Offset)right)

/**
 * Signed comparison between two machine word sized values.
 *
 * @param left  the first value
 * @param right the second value
 * @return true if 'left' is higher than 'right' in a signed comparison
 */
#define gt(left, right) ((Offset)left > (Offset)right)

/**
 * Signed comparison between two machine word sized values.
 *
 * @param left  the first value
 * @param right the second value
 * @return true if 'left' is higher than or equal to 'right' in a signed comparison
 */
#define ge(left, right) ((Offset)left >= (Offset)right)


/**
 * Adds a machine word sized signed offset to an address.
 *
 * @param address  the base address
 * @param offset   the signed offset (in bytes) to add
 * @return the result of adding 'offset' to 'address'
 */
#define Address_add(address, offset) ((Address)((UWord)address + ((Offset)offset)))

/**
 * Subtracts a machine word sized signed offset to an address.
 *
 * @param address  the base address
 * @param offset   the signed offset (in bytes) to subtract
 * @return the result of subtracting 'offset' from 'address'
 */
#define Address_sub(address, offset) ((Address)((UWord)address - (Offset)offset))

/**
 * Computes the signed distance between two addresses.
 *
 * @param address1  the first address
 * @param address2  the second address
 * @return the signed distance (in bytes) between 'address1' and 'address2'
 */
#define Address_diff(address1, address2) ((Offset)address1 - (Offset)address2)

#endif /* VM_ADDRESS_H_ */
