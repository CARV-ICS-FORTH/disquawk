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
	/* The cache directory. */
	cacheDirectory_g   = (sc_object_st*)roundUp((UWord)mm_scache_base(sysGetCore()),
	                                            sysGetCachelineSize());
	/* Start address of the allocated memory to the software cache. */
	cacheStart_g       =
		(Address)roundUp((UWord)cacheDirectory_g + SC_DIRECTORY_SIZE,
		                 sysGetCachelineSize());
	/* End address of the allocated memory to the software cache. */
	cacheEnd_g         =
		(Address)roundDown((UWord)cacheStart_g + SC_CACHE_SIZE,
		                   sysGetCachelineSize());
	/* Size of the software cache in bytes. */
	cacheSize_g        = cacheEnd_g - cacheStart_g;
	/* The next allocation address. */
	cacheAllocTop_g    = cacheStart_g;
	/* Temp pointer to the last allocated cache-line if it was not used. */
	cacheAllocTemp_g   = NULL;
	/* The allocation address for read-only Objects. */
	cacheROAllocTop_g  = cacheEnd_g;
	/* The allocation threshold/limit address for read-only Objects. */
	cacheROThreshold_g =
		(Address)roundUp((UWord)(cacheEnd_g - (cacheSize_g / 2)),
		                 sysGetCachelineSize());
	/* Counter for the number of flushes due to full cache. */
	cacheFlushes_g     = 0;
	/* Counts how many times the cache was cleared. */
	cacheClears_g      = 0;
	/* Counter for the number of cached objects. */
	cacheObjects_g     = 0;
	/* Bitmap for pending write back DMAs. */
	cachePendingWBs_g  = 0;
	/* Bitmap for pending fetch DMAs. */
	cachePendingFEs_g  = 0;

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
 * Checks if an address is in the heap address space.  Heap addresses
 * are have at least on of their 6 MSBs set.
 *
 * @param addre The address to check
 *
 * @return >0 if the address is a heap address
 *         0 otherwise (true local)
 */
INLINE unsigned int sc_is_in_heap(Address addr) {
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
INLINE int sc_is_cacheable(Address obj) {
	// The object's home node board id
	int bid;

	// Assume that the object is in the HEAP
	assume(sc_is_in_heap(obj));

	sysHomeOfAddress(obj, &bid);
	// bid can't be zero
	assume(bid);
	/* if ( bid != (sysGetIsland() + 1) ) {
	 * 	printf("Obj %p home=%d my home=%d\n", obj, bid, sysGetIsland() + 1);
	 * } */

	return bid && ( bid != (sysGetIsland() + 1) );
}

/**
 * Takes an address and prefixes it with the caller's board id
 *
 * @param obj The object to prefix
 *
 * @return the prefixed address
 */
inline Address sc_prefix(Address obj) {

	/* printf("Pref: %p\n", obj);
	 * printf("Start: %p\n", MM_MB_HEAP_BASE);
	 * printf("End: %p\n", MM_MB_HEAP_BASE+MM_MB_HEAP_SIZE); */
	/* Assert obj is in the HEAP */
	assume( hieq(obj, (Address)MM_MB_HEAP_BASE) );
	assume( lt(obj, (Address)(MM_MB_HEAP_BASE+MM_MB_HEAP_SIZE)) );

	/* assert core 64 doesn't provide global addresses */
	assume( (sysGetIsland() + 1) < 64 );

	return (Address)(((UWord)obj & 0x3FFFFFF) | ((sysGetIsland() + 1) << 26));
}

/**
 * Takes an address and translates it to the local representation if
 * it is cacheable or masks it to remove the board id info if it is
 * local
 *
 * @param obj      The object to check
 *
 * @return the translated or masked address
 */
inline Address sc_translate(Address obj) {

	/* printf("Trans: %p\n", obj); */
	/* Check if it is local */
	if (!sc_is_in_heap(obj)) {
		/* printf("translating : %p\n", obj);
		 * ar_backtrace(); */
		return obj;
	} else if (unlikely(sc_is_cacheable(obj))) {
		/* printf("%p is cacheable\n", obj); */
		/* It is cacheable, get it from the cache */
		return sc_get(obj);
	} else {
		/* It is in the local heap slice, strip the tag */
		obj = (Address)(((UWord)obj & 0x3FFFFFF) | MM_MB_HEAP_BASE);
		/* Assert obj is in the HEAP */
		assume( hieq(obj, (Address)MM_MB_HEAP_BASE) );
		assume( lt(obj, (Address)(MM_MB_HEAP_BASE+MM_MB_HEAP_SIZE)) );
		return obj;
	}

}

/**
 * Fetch a remote object to the software cache.
 *
 * @param from The object to fetch
 * @param to   The address in the cache to store the copy
 * @param size The size of the object (must be less than 1MB)
 * @param cid  The core id from whose cache to fetch the data
 */
INLINE void sc_fetch(Address from, Address to, int size, int cid) {
	int cnt = 32;
	// The object's home node board id
	int from_bid;

	// Make sure we do not cache our own objects
	assume( sc_is_in_heap(from) );
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
			if ( ar_cnt_get(sysGetCore(), SC_DMA_FE_CNT_START + cnt) == 0 ) {
				// Mark the counter as available
				cachePendingFEs_g ^= 1<<cnt;
				// Break if we are fine with a single counter
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
	while ( (ar_ni_status_get(sysGetCore()) & 0xFF) == 0 ) {
		;
	}

	// Get the object's home node cid and bid
	sysHomeOfAddress(from, &from_bid);
	// from_bid can't be zero
	assume(from_bid);
	// DMAs are working on cache-line alignment and granularity
	// (64B). So we round up the size and mask the from address
	// accordingly to take the pure, cache-aligned address (offset) on
	// the remote node.
	size = roundUp(size, sysGetCachelineSize());
	from = (Address)(((UWord)from & 0x3FFFFC0) | MM_MB_HEAP_BASE);
	// Init ACK counter to -size
	ar_cnt_set(sysGetCore(), cnt, -size);
	// Issue the DMA
//	kt_printf("Issued DMA from %p of size %d\n", from, size);
	ar_dma_with_ack(sysGetCore(),         // my core id
	                from_bid - 1,         // source board id
	                (cid >= 0) ? cid : 0, // source core id, HACK: ATM fetch from the cache, untill the flush/invalidate mechanisms are ready.
	                (int)from,            // source address
	                sysGetIsland(),       // destination board id
	                sysGetCore(),         // destination core id
	                (int)to,              // destination address
	                sysGetIsland(),       // ack board id
	                sysGetCore(),         // ack core id
	                cnt,                  // ack counter
	                size,                 // data length
	                0,                    // ignore dirty bit on source
	                0,                    // force clean on dst
	                0);                   // write through

//	printf("Fetch from: %d-%p to %d-%p\n", from_bid, from, sysGetIsland(), to);

	// FIXME: We are not sure the DMA reached completion (probably
	// not), so for the moment let's just spin here. Note that this
	// must be fixed in order to make prefetching work efficiently
	// FIXME: Add check for Nack and retry if necessary
	while ( ar_cnt_get_triggered(sysGetCore(), cnt) == 0 ) {
		;
	}
	if ( ar_cnt_get_triggered(sysGetCore(), cnt) == 2 ) {
//		kt_printf("DMA Completed %d\n", ar_cnt_get(sysGetCore(), cnt));
	} else {
		kt_printf("DMA Nacked\n");
		ar_abort();
	}
	// Mark the counter as available
	cachePendingFEs_g ^= 1<<cnt;

	return;
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
	assume( sc_is_in_heap(to) );
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
			if ( ar_cnt_get(sysGetCore(), SC_DMA_WB_CNT_START + cnt) == 0 ) {
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
	while ( !(ar_ni_status_get(sysGetCore()) & 0xFF) ) {
		;
	}

	// Init counter to -size
	cnt += SC_DMA_WB_CNT_START;
	ar_cnt_set(sysGetCore(), cnt, -size);
	// Get the object's home node cid and bid
	sysHomeOfAddress(to, &to_bid);
	// to_bid can't be zero
	assume(to_bid);
	// Issue the DMA
	ar_dma_with_ack(sysGetCore(),   // my core id
	                sysGetIsland(), // source board id
	                sysGetCore(),   // source core id
	                (int)from,      // source address
	                to_bid - 1,     // destination board id
	                0xC,            // destination core id
	                (int)to,        // destination address
	                sysGetIsland(), // ack board id
	                sysGetCore(),   // ack core id
	                cnt,            // ack counter
	                size,           // data length
	                1,              // ignore dirty bit on source
	                0,              // force clean on dst
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
		while ( ar_cnt_get(sysGetCore(), cnt) != 0 ) {
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
		while ( ar_cnt_get(sysGetCore(), cnt) != 0 ) {
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
INLINE Address sc_alloc(unsigned int size) {
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
	if (unlikely(cachePendingWBs_g)) {
		sc_wait_pending_wb();
	}

	/* Go through the dirty objects and issue write-back RDMAs */
	for (i=0; i< SC_HASHTABLE_SIZE; ++i) {
		if ( cacheDirectory_g[i].key & SC_DIRTY_MASK ) {
			Address cached = (Address)cacheDirectory_g[i].val;
			Address oop    = (Address)cacheDirectory_g[i].key;
			int     size   = com_sun_squawk_Klass_instanceSizeBytes(oop);
			// TODO: Make sure this size is correct
			sc_write_back(cached, (Address)(((UWord)oop & 0x3FFFFC0) | MM_MB_HEAP_BASE), size);
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
 * Allocate a chunk of zeroed memory from the software cache
 *
 * @param   size        the length in bytes of the object and its header
 *                      (i.e. the total number of bytes to be allocated). If 0
 *                      it will allocate one cache-line.
 * @return  a pointer to the allocated memory
 */
INLINE Address sc_malloc(unsigned int size){
	Address ret;

	size = size ? size : sysGetCachelineSize();
	ret  = sc_alloc(size);

	if ( unlikely(ret == NULL) ) { // there is not enough space left
		// Write back any dirty objects
		sc_flush();
		cacheFlushes_g++;
		// clear the cache but keep the read-only objects
		sc_clear();
		// Try to allocate again
		ret = sc_alloc(size);

		if ( unlikely(ret == NULL) ) { // there is still not enough space
			// also clear the read-only cached objects
			sc_ro_clear();
			cacheClears_g++;
			// Try to allocate again
			ret = sc_alloc(size);
			assume(ret!=NULL);
		}
	}

	return ret;
}

/**
 * Get the address at which the body of an object starts given the
 * address, 'ret', of the block of memory allocated (cache-line
 * aligned) for the object.
 *
 * A cache-line containing an object looks like this:
 * +-----------------+---------------------------------------------+
 * | Klass reference | Object instance                             |
 * +-----------------+---------------------------------------------+
 *                   ^
 *                   | Offset = 4 B
 *
 * A cache-line containing an array looks like this:
 * +-----------------+-----------------+---------------------------+
 * | Array Length    | Klass reference | Object instances          |
 * +-----------------+-----------------+---------------------------+
 *                                     ^
 *                                     | Offset = 8 B
 *
 * A cache-line containing a method looks like this:
 * +---------------+----------------+--------------+-----------------+-------------+
 * | Method header | Defining Class | Array Length | Klass reference | Method Body |
 * +---------------+----------------+--------------+-----------------+-------------+
 *                                                                   ^
 *                                                                   | Offset = 12 B
 *
 * To find out the type of object stored in the cache-line we use
 * the header tag as defined in
 * cldc/src/com/sun/squawk/vm/HDR.java
 *
 * This is actually a clone of blockToOop from
 * cldc/src/com/sun/squawk/GC.java
 */
INLINE Address sc_block_to_oop(Address obj, Address oop, int cid) {
	int size, length;

	switch ((*(int*)oop) & HDR_headerTagMask) {

	case HDR_basicHeaderTag:  // Class Instance

		// Check if we brought the whole instance or not
		size = com_sun_squawk_Klass_instanceSizeBytes((Address)*(int*)oop);
		/* printf("Klass size = %d\n", size); */

		// If the instance size is larger than the cache-line we need
		// to fetch the rest
		if ( unlikely(size > sysGetCachelineSize()) ) {
			cacheAllocTemp_g = oop;
			oop = sc_malloc(size);
			// Fetch data
			sc_fetch(obj, oop, size, cid);
		}

		oop += 4;
		break;

	case HDR_arrayHeaderTag:  // Array

		// Check if we brought the whole array or not
		length = (*(int*)oop) >> 2;
		size   =
			length * getDataSize(
				com_sun_squawk_Klass_componentType((Address)*(int*)(oop+4)));
		/* printf("Array [%d] size = %d\n", length, size); */

		// If the instance size times the array elements is larger
		// than the cache-line we need to fetch the rest
		if ( unlikely(size > sysGetCachelineSize()) ) {
			// TODO: Do something smarter for arrays (don't bring the
			// whole array if we don't need it)
			cacheAllocTemp_g = oop;
			oop = sc_malloc(size);
			// Fetch data
			sc_fetch(obj, oop, size, cid);
		}

		oop += 8;
		break;

	case HDR_methodHeaderTag: // Method
		/* printf("%p is a method!\n", oop); */
		// TODO: What's the size of a method? Did we bring the whole method?
		oop += ((*(int*)oop) >> HDR_headerTagBits) * HDR_BYTES_PER_WORD;

		// HACK: crash on method header accesses, until we fully
		// understand their usage
		fatalVMError("Method header");

		break;

	default:
		fatalVMError("Wrong header tag");
		break;

	}

	return oop;
}

/**
 * Fetches an object to cache it.  Allocates the appropriate space and
 * updates the cache directory. If cid is a positive number then the
 * obj is fetched from that core's cache instead of the board's main
 * memory.
 *
 * @param obj The object to cache
 * @param cid  The core id to fetch it from
 *
 * @return The cached object
 */
INLINE Address sc_put(Address obj, int cid) {
	Address   ret;

	if ( unlikely(cacheAllocTemp_g != NULL) ) {
		// If the last allocated cache-line was not used, try using it
		// now
		ret = cacheAllocTemp_g;
		cacheAllocTemp_g = NULL;
	} else {
		// Allocate memory. We use the cache line size since we don't
		// know the object's size a priori
		ret = sc_malloc(sysGetCachelineSize());
	}

	// Fetch data
	sc_fetch(obj, ret, sysGetCachelineSize(), cid);

	// The object's Klass is always here and resides in the ROM so
	// there is no need for ext.ra fetches.

	/* int i;
	 * for (i=0; i<16; ++i) {
	 * 	printf("Dump %p: %p\n", (((int*)ret)+i), *(((int*)ret)+i));
	 * } */

	// Make sure the whole object is here and move the pointer to the
	// proper offset from the cache-line head
	ret = sc_block_to_oop(obj, ret, cid);

	// Update the directory
	sc_dir_insert((UWord)obj, (UWord)ret);

	cacheObjects_g++;

	/* printf("Put %p\n", ret); */

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

	/* printf("Searching for %p\n", obj); */

	// Check if it is cached
	ret = sc_dir_lookup((UWord)obj);
	if (ret == NULL) { // miss
		/* printf(" %p not found\n", obj); */
		// Cache the object
		ret = sc_put(obj, -1);
	}

	/* printf(" %p found at %p\n", obj, ret); */
	/* Assert ret is in the cache */
	assume( hieq(ret, cacheStart_g) );
	assume(   lt(ret, cacheEnd_g)   );

	return ret;
}
