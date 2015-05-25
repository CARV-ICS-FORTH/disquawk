/*
 * Copyright 2013-2014 FORTH-ICS / CARV
 *                     (Foundation for Research & Technology -- Hellas,
 *                      Institute of Computer Science,
 *                      Computer Architecture & VLSI Systems Laboratory)
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * @file   softcache.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * Header file for the software cache
 */

#ifndef __SOFTCACHE_H__
#define __SOFTCACHE_H__

#include <memory.h>
#include <os.h>

/**
 * A node of the directory.
 */
typedef struct sc_object sc_object_st;

#include <util.h>

/**
 * Enable statistics
 */
#define SC_STATS

/**
 * Flags marking whether a flush/write-back/fetch should be blocking
 * (synchronous/inorder) or not (asynchronous/inteleaved).
 */
#define SC_BLOCKING     1
#define SC_NON_BLOCKING 0

/**
 * Since objects are cache line aligned we use the LSB as the dirty
 * bit and the 1-5 bits to store the acknowledgment counter for the
 * object if there is a pending DMA transfer for it
 */
#define SC_DIRTY_MASK   0x01 /* 0000 0001 */
#define SC_CNT_MASK     0x3E /* 0011 1110 */
#define SC_ADDRESS_MASK ~((SC_CNT_MASK) | (SC_DIRTY_MASK))

/**
 * The hash-table size must be the closest prime to the cache-lines
 * that can fit in the cache. To calculate this value use the
 * cache_memory/(cache_line_size+4*sizeof(void*)) formula and pass
 * the result to http://easycalculation.com/prime-number.php to find
 * the next smaller (nearest) prime number
 */
#define SC_HASHTABLE_SIZE 85193
#define SC_DIRECTORY_SIZE (SC_HASHTABLE_SIZE * (4 * sizeof(void*)))
#define SC_CACHE_SIZE     (SC_HASHTABLE_SIZE * MM_CACHELINE_SIZE)

/* The size of a Klass object instance (72) rounded up to cache line size */
#define SC_KLASS_SIZE 128

void        sc_initialize();
Address     sc_get(Address obj, int is_write);
void        sc_mark_dirty(Address obj);
void        sc_write_back(Address object);
void        sc_flush(int blocking);
void        sc_clear();
void        sc_dump();
void        sc_stats();
sc_object_st* sc_put(Address obj, int cid);

/**
 * Checks if an address is in the heap address space.  Heap addresses
 * have at least one of their 6 MSBs set.
 *
 * @param addr The address to check
 *
 * @return >0 if the address is a heap address
 *         0 otherwise (true local)
 */
INLINE unsigned int
sc_in_heap(Address addr)
{
	/* HACK board 64 does not hold global addresses */
	assume(sysGetIsland() < 63);

	return (UWord)addr & (~0x3FFFFFF);
}

/**
 * Checks if an address is cacheable in this software cache (whether
 * it is a local object)
 *
 * @param obj The object to check
 *
 * @return 1 if the object is cacheable (remote object)
 *         0 otherwise (local object)
 */
INLINE int
sc_is_cacheable(Address obj)
{
	/* The object's home node board id */
	int bid;
	int cid;

	/* Assume that the object is in the HEAP */
	assume(sc_in_heap(obj));

	sysHomeOfAddress(obj, &bid, &cid);
	/* bid can't be zero */
	assume(bid);
	/* if ( bid != (sysGetIsland() + 1) ) {
	 *  printf("Obj %p home=%d my home=%d\n", obj, bid, sysGetIsland() + 1);
	 * } */
	/* if ( bid && (( bid != (sysGetIsland() + 1) ) || ( cid != sysGetCore() ))
	 * ) {
	 *  printf("Obj %p home=0x%02X/%d my home=0x%02X/%d\n",
	 *         obj, bid, cid, sysGetIsland() + 1, sysGetCore());
	 * } */

	return (bid != (sysGetIsland() + 1)) || (cid != sysGetCore());
}

/**
 * Takes an address and translates it to the local representation if
 * it is cacheable or masks it to remove the board id info if it is
 * local
 *
 * @param obj      The object to check
 * @param is_write Whether it is going to be written after translation
 *
 * @return the translated or masked address
 */
INLINE Address
sc_translate(Address obj, int is_write)
{
	assume(is_write >> 1 == 0);

	if (obj == NULL)
		return NULL;

	/*
	 * printf("Trans: %p\n", obj);
	 * Check if it is local
	 */
	if (!sc_in_heap(obj)) {
		/* printf("translating : %p\n", obj); */

		/* ar_backtrace(); */
		return obj;
	}
	else if (unlikely(sc_is_cacheable(obj))) {
		/* printf("%p is cacheable\n", obj); */

		/* It is cacheable, get it from the cache */

		/* TODO: Improve performance by marking as dirty only cachelines and
		 * not the whole object. */
		return sc_get(obj, is_write);
	}
	else {
		/* kt_printf("%p is in local heap\n", obj); */
		/* It is in the local heap slice, strip the tag */
		obj = (Address)(((UWord)obj & 0x3FFFFFF) | MM_MB_HEAP_BASE);
		/* Assert obj is in the HEAP */
		assume(hieq(obj, (Address)MM_MB_HEAP_BASE));
		assume(lt(obj, (Address)(MM_MB_HEAP_BASE + MM_MB_HEAP_SIZE)));

		return obj;
	}
}

/**
 * Takes an address and prefixes it with the caller's board id
 *
 * @param obj The object to prefix
 *
 * @return the prefixed address
 */
INLINE Address
sc_prefix(Address obj)
{

	/* printf("Pref: %p\n", obj);
	 * printf("Start: %p\n", MM_MB_HEAP_BASE);
	 * printf("End: %p\n", MM_MB_HEAP_BASE+MM_MB_HEAP_SIZE); */
	/* Assert obj is in the HEAP */
	assume(hieq(obj, (Address)MM_MB_HEAP_BASE));
	assume(lt(obj, (Address)(MM_MB_HEAP_BASE + MM_MB_HEAP_SIZE)));

	/* assert core 64 doesn't provide global addresses */
	assume((sysGetIsland() + 1) < 64);

	return (Address)(((UWord)obj & 0x3FFFFFF) | ((sysGetIsland() + 1) << 26));
}

#endif /* __SOFTCACHE_H__ */
