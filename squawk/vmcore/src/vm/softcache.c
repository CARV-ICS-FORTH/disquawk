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
 * We can safely ignore the 6 LSBs since objects are cache line
 * aligned
 *
 * A simple mod after shifting to the right is still ignoring/dropping
 * the 9 MSBs that in our implementation denote the home board and
 * core id of the address. Since all VMs use the same garbage
 * collector we expect objects from different cores to have the "same"
 * address at their home node. To resolve this issue we need to put
 * those 9 MSBs in the equation
 *
 * @param key The key to hash
 */
INLINE int sc_dir_hash(UWord key) {
	key = (key >> 20) ^ (key >> 6);

	return key % SC_HASHTABLE_SIZE;
}

/**
 * The second hash function, used to resolve collisions
 *
 * On collisions we can use the home board and core id info (9MSBs)
 * this way Objects from different homes will follow different probes
 *
 * @param key The key to hash
 */
INLINE int sc_dir_hash2(UWord key) {
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

	for (i=0; i<SC_HASHTABLE_SIZE; ++i) {
		if (cacheDirectory_g[i].val < (UWord)cacheAllocTop_g) {
			cacheDirectory_g[i].key = NULL;
			cacheDirectory_g[i].val = NULL;
		}
	}

}

/**
 * Clears the read-only records
 */
INLINE void sc_dir_ro_clear() {
	int i;

	for (i=0; i<SC_HASHTABLE_SIZE; ++i) {
		if (cacheDirectory_g[i].val >= (UWord)cacheROAllocTop_g) {
			cacheDirectory_g[i].key = NULL;
			cacheDirectory_g[i].val = NULL;
		}
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
	cacheDirectory_g   = (sc_object_st*)roundUp((UWord)mm_scache_base(my_cid),
	                                            MM_CACHELINE_SIZE);
	cacheStart_g       =
		(Address)roundUp((UWord)cacheDirectory_g + SC_DIRECTORY_SIZE,
		                 MM_CACHELINE_SIZE);
	cacheEnd_g         =
		(Address)roundDown((UWord)cacheStart_g + SC_CACHE_SIZE,
		                   MM_CACHELINE_SIZE);
	cacheSize_g        = cacheEnd_g - cacheStart_g;
	cacheAllocTop_g    = cacheStart_g;
	cacheROAllocTop_g  = cacheEnd_g;
	cacheROThreshold_g =
		(Address)roundUp((UWord)(cacheEnd_g - (cacheSize_g / 2)),
		                 MM_CACHELINE_SIZE);
	cacheFlushes_g     = 0;
	cacheObjects_g     = 0;
	cachePendingWBs_g  = 0;

	sc_dir_clear();
	sc_dir_ro_clear();
#if 0
	fprintf(stderr, "+------------------ SOFTWARE-CACHE -------------------\n");
	printRange("| Directory", cacheDirectory_g,
	           (Address)cacheDirectory_g + SC_DIRECTORY_SIZE);
	printRange("| Cache", cacheStart_g, cacheEnd_g);
	fprintf(stderr, "| CacheSize = %d\n", cacheSize_g);
	fprintf(stderr, "+-----------------------------------------------------\n");
#endif
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
INLINE int sc_is_cacheable(Address obj) {
	// The object's home node board id
	int bid;

	// If the object is not in the HEAP
	if ( lt(obj, MM_MB_HEAP_BASE) )
		return 0;

	sysHomeOfAddress(obj, &bid);
	if ( bid != sysGetIsland() ) {
		printf("Obj 0x%p home=%d my home=%d\n", obj, bid, sysGetIsland());
	}
	return ( bid != sysGetIsland() );
}

/**
 * Takes an address and translates it to the local representation if
 * it is cacheable or masks it to remove the board id info if it is
 * local
 *
 * @param obj The object to check
 *
 * @return the translated or masked address
 */
inline Address sc_translate(Address obj){

//	printf("Trans: 0x%p\n", obj);
	/* Check if it is local */
	if (sc_is_cacheable(obj)) {
		printf("0x%p is cacheable\n", obj);
		/* If not, check if it is cached and if not, cache it */
		obj = sc_get(obj);
		/* Assert obj is in the cache */
		assume( hieq(obj, cacheStart_g) && lt(obj, cacheEnd_g) );
	} else if ((UWord)obj & (~0x7FFFFFF)) {
		/* If it is local, strip the tag */
		obj = (Address)((UWord)obj & 0x7FFFFFF);
		/* Assert obj is in the HEAP */
		assume( hieq(obj, (Address)MM_MB_HEAP_BASE) &&
		              lt(obj, (Address)(MM_MB_HEAP_BASE+MM_MB_HEAP_SIZE)) );
	}

	return obj;
}

/**
 * Fetch a remote object to the software cache.
 *
 * @param from The object to fetch
 * @param to   The address in the cache to store the copy
 * @param size The size of the object (must be less than 1MB)
 */
INLINE void sc_fetch(Address from, Address to, int size) {
	int cnt = 32;
	// The object's home node board id
	int from_bid;

	// Make sure we do not cache our own objects
	assume( sc_is_cacheable(from) );

	// make sure size <= 1MB, this is the upper limit for a DMA
	// transfer
	assume( (size > 0) && (size <= 0x100000) );

	// Find an available counter to use, using the bitmap
	while (cachePendingFEs_g == 0xFFFFFFFF) { // while all counters are used
		// go through them and check if there are any completed
		for (cnt = 0; cnt < 32; ++cnt) {
			// if the counter is zero the DMA finished and we can use
			// the counter
			if ( ar_cnt_get(my_cid, SC_DMA_FE_CNT_START + cnt) == 0 ) {
				// Mark the counter as available
				cachePendingFEs_g ^= 1<<cnt;
//				break;
			}
		}
	}

	// if we did not enter the previous loop find the first available
	// counter
	if (cnt == 32) {
		assume( cachePendingFEs_g != 0xFFFFFFFF );
		// Find the first available counter
		for (cnt=0; (cachePendingFEs_g & (1 << cnt)); ++cnt) {
			;
		}

		// Mark the counter as used
		cachePendingFEs_g |= 1<<cnt;
	}

	// Fetches are directly passed to the remote DRAM but have to be
	// pushed to the local DMA engine first.

	// Wait until our DMA engine can support at least one more DMA
	while ( !(ar_ni_status_get(my_cid) & 0xFF) ) {
		;
	}

	// Init ACK counter to -size
	ar_cnt_set(my_cid, cnt, -size);
	// Get the object's home node cid and bid
	sysHomeOfAddress(from, &from_bid);
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
	// The object's home node board id
	int to_bid;

	// Make sure we do not cache our own objects
	assume( sc_is_cacheable(to) );

	// make sure size <= 1MB, this is the upper limit for a DMA
	// transfer
	assume( (size > 0) && (size <= 0x100000) );

	// Find an available counter to use, using the bitmap
	while (cachePendingWBs_g == 0xFFFFFFFF) { // while all counters are used
		// go through them and check if there are any completed
		for (cnt = 0; cnt < 32; ++cnt) {
			// if the counter is zero the DMA finished and we can use
			// the counter
			if ( ar_cnt_get(my_cid, SC_DMA_WB_CNT_START + cnt) == 0 ) {
				// Mark the counter as available
				cachePendingWBs_g ^= 1<<cnt;
//				break;
			}
		}
	}

	// if we did not enter the previous loop find the first available
	// counter
	if (cnt == 32) {
		assume(cachePendingWBs_g != 0xFFFFFFFF);
		// Find the first available counter
		for (cnt=0; (cachePendingWBs_g & (1 << cnt)); ++cnt) {
			;
		}

		// Mark the counter as used
		cachePendingWBs_g |= 1<<cnt;
	}

	// Write-backs can be made from the HW cache or the DRAM.  In the
	// second case, they are directly passed to the local DRAM but
	// have to be pushed to the local DMA engine first.

	// Wait until our DMA engine can support at least one more DMA
	while ( !(ar_ni_status_get(my_cid) & 0xFF) ) {
		;
	}

	// Init counter to -size
	cnt += SC_DMA_WB_CNT_START;
	ar_cnt_set(my_cid, cnt, -size);
	// Get the object's home node cid and bid
	sysHomeOfAddress(to, &to_bid);
	// Issue the DMA
	ar_dma_with_ack(my_cid,    // my core id
	                my_bid,    // source board id
	                my_cid,    // source core id
	                (int)from, // source address
	                to_bid,    // destination board id
	                0xC,       // destination core id
	                (int)to,   // destination address
	                my_bid,    // ack board id
	                my_cid,    // ack core id
	                cnt,       // ack counter
	                size,      // data length
	                1,         // ignore dirty bit on source
	                0,         // force clean on dst
	                1);        // write through (doesn't really matter
                               // since we are writing directly on
                               // DRAM)
}

/**
 * Wait for all pending write backs to reach completion
 */
INLINE void sc_wait_pending_wb() {
	int cnt;

	// go through the counters and spin on non zero
	for (cnt = SC_DMA_WB_CNT_START; cnt < (SC_DMA_WB_CNT_START + 32); ++cnt) {
		while ( ar_cnt_get(my_cid, cnt) != 0 ) {
			;
		}
	}

	cachePendingWBs_g = 0;
}

/**
 * Wait for all pending fetches to reach completion
 */
INLINE void sc_wait_pending_fe() {
	int cnt;

	// go through the counters and spin on non zero
	for (cnt = SC_DMA_FE_CNT_START; cnt < SC_DMA_WB_CNT_START; ++cnt) {
		while ( ar_cnt_get(my_cid, cnt) != 0 ) {
			;
		}
	}

	cachePendingFEs_g = 0;
}

/**
 * Allocate a chunk of zeroed memory from the software cache
 *
 * @param   size        the length in bytes of the object and its header
 *                      (i.e. the total number of bytes to be allocated).
 * @return a pointer to the allocated memory or NULL if the allocation failed
 */
#ifdef SC_NATIVE
INLINE Address sc_alloc(int size) {
	Address ret       = cacheAllocTop_g;
	Offset  available = Address_diff(cacheROAllocTop_g, ret);

	assume(size >= 0 && size <= cacheSize_g);

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
#else /* SC_NATIVE */
INLINE Address sc_alloc(int size) {
	Address ret       = com_sun_squawk_SoftwareCache_allocTop;
	Offset  available = Address_diff(com_sun_squawk_SoftwareCache_cacheEnd,
	                                 ret);
	assume(size >= 0 && size <= cacheSize_g);

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
#endif /* SC_NATIVE*/

/**
 * Allocate a chunk of memory from the software cache for a read-only
 * object
 *
 * @return a pointer to the allocated memory or NULL if the allocation failed
 */
INLINE Address sc_ro_alloc() {
	Address ret       = Address_sub(cacheROAllocTop_g, SC_KLASS_SIZE);
	Offset  available;

	// if there are RW cache objects in the "RO section" of the cache
	if (unlikely(lt(cacheROThreshold_g, cacheAllocTop_g))) {
		// diff with cacheAllocTop_g
		available = Address_diff(cacheROAllocTop_g, cacheAllocTop_g);
	} else {
		// else diff with cacheROThreshold_g to avoid using "precious"
		// memory for RO objects
		available = Address_diff(cacheROAllocTop_g, cacheROThreshold_g);
	}

	// If there is not enough space return NULL and let the caller
	// handle it
	if (unlikely(lt(available, SC_KLASS_SIZE))) {
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

	// If there are pending write backs wait for them to complete
	/* if (unlikely(cachePendingWBs_g)) {
	 * 	sc_wait_pending_wb();
	 * } */

	/* Go through the dirty objects and issue write-back RDMAs */
	for (i=0; i< SC_HASHTABLE_SIZE; ++i) {
		if ( cacheDirectory_g[i].key & SC_DIRTY_MASK ) {
			Address oop    = (Address)cacheDirectory_g[i].val;
			Address cached = (Address)cacheDirectory_g[i].key;
			int     size   = com_sun_squawk_Klass_instanceSizeBytes(oop);
			// TODO: Make sure this size is correct
			sc_write_back(cached, (Address)oop, size);
		}
	}

	/* Check if we need to wait for completion */
	//if (likely(blocking)) {
		/* wait for the RDMAs to reach completion */
		sc_wait_pending_wb();
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
	if ( unlikely(ret == NULL) ) { // there is not enough space left
		// Write back any dirty objects
		sc_flush();
		cacheFlushes_g++;
		// clear the cache but keep the read-only objects
		sc_clear();
		// Try to allocate again
		ret = sc_alloc(sysGetCachelineSize());

		if ( unlikely(ret == NULL) ) { // there is still not enough space
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

	printf("Searching for 0x%p\n", obj);
	ar_panic("sc_get");

	// Check if it is cached
	ret = sc_dir_lookup((UWord)obj);
	if (ret == NULL) { // miss
		// Cache the object
		ret = sc_put(obj);
	}

	return ret;
}
