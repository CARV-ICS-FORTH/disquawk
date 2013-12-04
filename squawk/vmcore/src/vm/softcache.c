/*
 * Copyright (C) 2013-2014 FORTH-ICS / CARV
 *                         (Foundation for Research & Technology -- Hellas,
 *                          Institute of Computer Science,
 *                          Computer Architecture & VLSI Systems Laboratory)
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
 * @file   softcache.c
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * Native functions for the software cache implementation
 *
 * This is a software Object cache. The cache "block" size (minimum
 * granularity of a cache Object) is a cache line. Since objects are
 * allocated aligned to cache lines this is OK. The space overhead for
 * the cache is 8-bytes (2 pointers) per cached object. And the cache
 * can hold up to cache_memory/(cache_line+sizeof(2*(void*))) Objects.
 *
 * We implement the cache directory using double hashing
 *
 * For the write back DMA acknowledgments we use the hardware counters
 * 90-122
 */

#include "softcache.h"

/******************************************************************************\
|*                                                                            *|
|*                Implementation of the cache directory                       *|
|*                                                                            *|
\******************************************************************************/

/**
 * A node of the directory.  Maps original/remote addresses to local
 * cached object addresses
 */
struct sc_object {
	UWord key;   /**< The original/remote address of the object. Since
	              * objects are cache line aligned we can use the LSB
	              * as the dirty bit and the 1-5 bits to store the
	              * acknowledgment counter for the object if there is
	              * a pending DMA transfer for it */
	UWord val;   /**< The local address of the cached object. Again we
	              * can use the 6 LSBs for metadata */
};

/**
 * The first hash function
 *
 * @param key The key to hash
 */
INLINE int sc_dir_hash(UWord key) {
	// We can safely ignore the 6 LSBs since objects are cache line
	// aligned

	// A simple mod after shifting to the right is still
	// ignoring/dropping the 9 MSBs that in our implementation denote
	// the home board and core id of the address. Since all VMs use
	// the same garbage collector we expect objects from different
	// cores to have the "same" address at their home. To resolve this
	// issue we need to put those 9 MSBs in the equation
	key = (key >> 20) ^ (key >> 6);

	return key % SC_HASHTABLE_SIZE;
}

/**
 * The second hash function, used to resolve collisions
 *
 * @param key The key to hash
 */
INLINE int sc_dir_hash2(UWord key) {
	// On collisions we can use the home board and core id info
	// (9MSBs) this way Objects from different homes will follow
	// different probes
	return key >> 23;
}

/**
 * Looks up the cache directory to find the requested object.
 *
 * @param key The object to look up in the directory
 *
 * @return The address of the cached object or NULL if it was not
 *         found
 */
INLINE Address sc_dir_lookup(UWord key) {
	int i             = 1;
	int hash          = sc_dir_hash(key);
	int hash2;
	sc_object_st *ret = &cacheDirectory_g[hash];

	if ( ((ret->key ^ key) >> 6) == 0 )
		return (Address)ret->val;
	else if (ret->key) {
		hash2 = sc_dir_hash2(key);
		while (i < SC_HASHTABLE_SIZE) {
			hash = (hash + hash2) % SC_HASHTABLE_SIZE;
			ret  = &cacheDirectory_g[hash];
			if ( ((ret->key ^ key) >> 6) == 0 )
				return (Address)ret->val;
			else if ( ret->key == NULL)
				return NULL;
			++i;
		}
		// must never reach here
		assume(0);
		return NULL;
	} else
		return NULL;
}

/**
 * Inserts a key-value per in the directory
 *
 * @param key The key of the new node
 * @param val The value of the new node
 */
INLINE void sc_dir_insert(UWord key, UWord val) {
	int i              = 1;
	int hash           = sc_dir_hash(key);
	int hash2;
	sc_object_st *node = &cacheDirectory_g[hash];

	if (node->key == NULL) {
		node->key = key;
		node->val = val;
	} else {
		hash2 = sc_dir_hash2(key);
		while (i < SC_HASHTABLE_SIZE) {
			hash = (hash + hash2) % SC_HASHTABLE_SIZE;
			node = &cacheDirectory_g[hash];
			if (node->key == NULL) {
				node->key = key;
				node->val = val;
				break;
			}
			++i;
		}
		assume( i < SC_HASHTABLE_SIZE );
	}
}

/**
 * Clears all records except the read-only ones
 */
INLINE void sc_dir_clear() {
	int i;

	for (i=0; i<SC_HASHTABLE_SIZE; ++i)
		if (cacheDirectory_g[i].val < (UWord)cacheAllocTop_g) {
			cacheDirectory_g[i].key = 0;
			cacheDirectory_g[i].val = 0;
		}

}

/**
 * Clears the read-only records
 */
INLINE void sc_dir_ro_clear() {
	int i;

	for (i=0; i<SC_HASHTABLE_SIZE; ++i)
		if (cacheDirectory_g[i].val >= (UWord)cacheROAllocTop_g) {
			cacheDirectory_g[i].key = 0;
			cacheDirectory_g[i].val = 0;
		}
}

/******************************************************************************\
|*                                                                            *|
|*                     Implementation of the cache                            *|
|*                                                                            *|
\******************************************************************************/

/**
 * Initializes the software cache. MUST be invoked before any other
 * function in this file.
 */
void sc_initialize() {
	cacheDirectory_g  = (sc_object_st*)roundUp((UWord)mm_scache_base(my_cid),
	                                           MM_CACHELINE_SIZE);
	cacheStart_g      = (Address)roundUp((UWord)cacheDirectory_g + SC_DIRECTORY_SIZE,
	                                     MM_CACHELINE_SIZE);
	cacheEnd_g        = (Address)roundDown((UWord)cacheStart_g + SC_CACHE_SIZE,
	                                       MM_CACHELINE_SIZE);
	cacheSize_g       = cacheEnd_g - cacheStart_g;
	cacheAllocTop_g   = cacheStart_g;
	cacheROAllocTop_g = cacheEnd_g;
	cacheFlushes_g    = 0;
	cacheObjects_g    = 0;
	cachePendingWBs_g = 0;

	fprintf(stderr, "+------------------ SOFTWARE-CACHE -------------------\n");
	printRange("| Directory", cacheDirectory_g,
	           (Address)cacheDirectory_g + SC_DIRECTORY_SIZE);
	printRange("| Cache", cacheStart_g, cacheEnd_g);
	fprintf(stderr, "| CacheSize = %d\n", cacheSize_g);
	fprintf(stderr, "+-----------------------------------------------------\n");
}

/**
 * Fetch a remote object to the software cache.
 *
 * @param from The object to fetch
 * @param to   The address in the cache to store the copy
 * @param size The size of the object
 */
void sc_fetch(Address from, Address to, int size) {
	int cnt = 0;
	// Calculate the core id of the destination from the to address
	int from_cid;
	// Calculate the board id of the destination from the to address
	int from_bid;

	sysHomeOfAddress(from, &from_cid, &from_bid);
	assume(from_cid != sysGetCore() && from_bid != sysGetIsland());
	// make sure size <= 1MB
	assume( size > 0 && size <= 0x100000);

	// TODO: Find a counter to request the acknowledgment to
	cnt += SC_DMA_WB_CNT_START;

	// TODO: Check if we can start a new DMA

	// Init counter to -size
	ar_cnt_set(my_cid, cnt, -size);
	// Issue the DMA
	ar_dma_with_ack(my_cid,   // my core id
	                from_bid, // source board id
	                0xC,      // source core id
	                (int)from,// source address
	                my_bid,   // destination board id
	                my_cid,   // destination core id
	                (int)to,  // destination address
	                my_bid,   // ack board id
	                my_cid,   // ack core id
	                cnt,      // ack counter
	                size,     // data length
	                0,        // ignore dirty bit on source
	                0,        // force clean on dst
	                0);       // write through
}

/**
 * Write back a dirty Object
 *
 * @param from The object to write back
 * @param to   The home address of the object
 * @param size The size of the object
 */
void sc_write_back(Address from, Address to, int size) {
	int cnt = 32;
	// Calculate the core id of the destination from the to address
	int to_cid;
	// Calculate the board id of the destination from the to address
	int to_bid;

	sysHomeOfAddress(to, &to_cid, &to_bid);
	assume(to_cid != sysGetCore() && to_bid != sysGetIsland());
	// make sure size <= 1MB
	assume( size > 0 && size <= 0x100000);

	// Find an available counter to use, using the bitmap
	while (cachePendingWBs_g == 0xFFFFFFFF) { // while all counters are used
		// go through them and check if there are any completed
		for (cnt = 0; cnt < 32; ++cnt) {
			// if the counter is zero the DMA finished and we can use
			// the counter
			if ( ar_cnt_get(my_cid, SC_DMA_WB_CNT_START + cnt) == 0 ) {
				break;
			}
		}
	}

	// if we did not enter the previous loop find the available counter
	if (cnt == 32) {
		// Find the first available counter
		for (cnt=0;
		     (cachePendingWBs_g & (1 << cnt));
		     ++cnt);

		// Mark the counter as used
		cachePendingWBs_g |= 1<<cnt;
	}

	// TODO: Check if we can start a new DMA

	// Init counter to -size
	cnt += SC_DMA_WB_CNT_START;
	ar_cnt_set(my_cid, cnt, -size);
	// Issue the DMA
	ar_dma_with_ack(my_cid,   // my core id
	                my_bid,   // source board id
	                my_cid,   // source core id
	                (int)from,// source address
	                to_bid,   // destination board id
	                0xC,      // destination core id
	                (int)to,  // destination address
	                my_bid,   // ack board id
	                my_cid,   // ack core id
	                cnt,      // ack counter
	                size,     // data length
	                1,        // ignore dirty bit on source
	                0,        // force clean on dst
	                1);       // write through (doesn't really matter
                              // since we are writing directly on
                              // DRAM)
}

/**
 * Wait for all pending write backs to reach completion
 */
INLINE void sc_wait_pending() {
	int cnt;

	// go through the counters and spin on non zero
	for (cnt = 0; cnt < 32; ++cnt) {
		while ( ar_cnt_get(my_cid, SC_DMA_WB_CNT_START + cnt) != 0 );
	}

	cachePendingWBs_g = 0;
}

/**
 * Allocate a chunk of zeroed memory from the software cache
 *
 * @param   size        the length in bytes of the object and its header
 *                      (i.e. the total number of bytes to be allocated).
 * @return a pointer to the allocated memory or NULL if the allocation failed
 */
#ifdef NATIVE_SOFTWARE_CACHE
INLINE Address sc_alloc(int size) {
	Address ret       = cacheAllocTop_g;
	Offset  available = Address_diff(cacheROAllocTop_g,
	                                 ret);
	assume(size >= 0 && size <= com_sun_squawk_SoftwareCache_cacheSize);

#ifdef __MICROBLAZE__
	// make sure size is a multiple of the cache line size
	size = roundUp(size, sysGetCachelineSize());
#endif

	if (unlikely(lt(available, size))) {
		// If there is not enough space return NULL and let the caller
		// handle it
		return NULL;
	}

	cacheAllocTop_g = Address_add(ret, size);
#ifdef ASSUME
	// zero the memory
	zeroWords(ret, cacheAllocTop_g);
#endif
	cacheObjects_g++;

	return (Address)ret;
}
#else /* NATIVE_SOFTWARE_CACHE */
INLINE Address sc_alloc(int size) {
	Address ret       = com_sun_squawk_SoftwareCache_allocTop;
	Offset  available = Address_diff(com_sun_squawk_SoftwareCache_cacheEnd,
	                                 ret);
	assume(size >= 0 && size <= com_sun_squawk_SoftwareCache_cacheSize);

#ifdef __MICROBLAZE__
	// make sure size is a multiple of the cache line size
	size = roundUp(size, sysGetCachelineSize());
#endif

	if (unlikely(lt(available, size))) {
		// If there is not enough space return NULL and let the caller
		// handle it
		return NULL;
	}

	com_sun_squawk_SoftwareCache_allocTop = Address_add(ret, size);
#ifdef ASSUME
	// zero the memory
	zeroWords(ret, com_sun_squawk_GC_allocTop);
#endif
	com_sun_squawk_SoftwareCache_objectsCount++;

	return (Address)ret;
}
#endif /* NATIVE_SOFTWARE_CACHE*/

/**
 * Allocate a chunk of memory from the software cache for a read-only
 * object
 *
 * @return a pointer to the allocated memory or NULL if the allocation failed
 */
INLINE Address sc_ro_alloc() {
	Address ret       = Address_sub(cacheROAllocTop_g, SC_KLASS_SIZE);
	Offset  available = Address_diff(cacheROAllocTop_g, cacheAllocTop_g);

	if (unlikely(lt(available, SC_KLASS_SIZE))) {
		// If there is not enough space return NULL and let the caller
		// handle it
		return NULL;
	}

	cacheROAllocTop_g = ret;
#ifdef ASSUME
	// zero the memory
	zeroWords(ret, cacheAllocTop_g);
#endif
	cacheObjects_g++;

	return (Address)ret;
}

/**
 * Write-back any dirty objects in the software cache
 */
INLINE void sc_flush() {
	int i;
	int size = 0;

	// If there are pending write backs wait for them to complete
	/* if (unlikely(cachePendingWBs_g)) {
	 * 	sc_wait_pending();
	 * } */

	/* Go through the dirty objects and issue write-back rDMAs */
	for (i=0; i< SC_HASHTABLE_SIZE; ++i) {
		if ( cacheDirectory_g[i].key & SC_DIRTY_MASK ) {
			Address oop    = (Address)cacheDirectory_g[i].val;
			Address cached = (Address)cacheDirectory_g[i].key;
			// TODO: Find the size of the object
			sc_write_back(cached, (Address)oop, size);
		}
	}

	/* Check if we need to wait for completion */
	//if (likely(blocking)) {
		/* wait for the rDMAs to reach completion */
		sc_wait_pending();
	//} else {
	//	com_sun_squawk_SoftwareCache_pendingWriteBacks = 1;
	//}
}

/**
 * Clears all non read-only records and frees the memory
 */
INLINE void sc_clear() {
	// remove the records from the directory
	sc_dir_clear();
	// reset the allocation pointer
	cacheAllocTop_g = cacheStart_g;
}

/**
 * Clears all read-only records and frees the memory
 */
INLINE void sc_ro_clear() {
	// remove the records from the directory
	sc_dir_ro_clear();
	// reset the allocation pointer
	cacheROAllocTop_g = cacheEnd_g;
}

/**
 * Fetches an object to cache it.  Allocates the appropriate space and
 * updates the cache directory.
 *
 * @param obj The object to cache
 *
 * @return The cached object
 */
INLINE Address sc_put(Address obj) {
	Address   ret;

	// Allocate memory
	ret = sc_alloc(sysGetCachelineSize()); // use the cache line size
                                           // until we find a way to
                                           // know the object's size a
                                           // priori
	if (ret == NULL) { // there is not enough space left
		// Write back any dirty objects
		sc_flush();
		cacheFlushes_g++;
		// clear the cache but keep the read-only objects
		sc_clear();
		// Try to allocate again
		ret = sc_alloc(sysGetCachelineSize());
		if (ret == NULL) { // there is still not enough space
			// also clear the read-only cached objects
			sc_ro_clear();
			cacheClears_g++;
			// Try to allocate again
			ret = sc_alloc(sysGetCachelineSize());
			assume(ret!=NULL);
		}
	}

	// Fetch data
	sc_fetch(obj, ret, sysGetCachelineSize());

	// TODO: Check if the object's Klass is here

	// TODO: Check if we brought the whole object or not

	// Update the directory
	sc_dir_insert((UWord)obj, (UWord)ret);

	cacheObjects_g++;

	return ret;
}

/**
 * Looks up the cache to find the requested object. If it fails
 * (miss), it fetches it and adds it to the cache.
 *
 * @param obj The object to get from the cache
 *
 * @return The cached object
 */
Address sc_get(Address obj) {
	Address   ret;

	// Check if it is cached
	ret = sc_dir_lookup((UWord)obj);
	if (ret == NULL) { // miss
		// Cache the object
		ret = sc_put(obj);
	}

	return ret;
}
