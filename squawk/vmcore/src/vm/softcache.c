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
#include "hwcnt.h"
#include "hwcache.h"

#ifdef ASSUME
void zeroWords(UWordAddress start, UWordAddress end);

#endif /* ifdef ASSUME */

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
	UWord key;                 /**< The original/remote address of the
	                            * object.  Since objects are cache
	                            * line aligned we can use the LSB as
	                            * the dirty bit and the 1-5 bits to
	                            * store the acknowledgment counter for
	                            * the object if there is a pending DMA
	                            * transfer for it */
	UWord val;                 /**< The local address of the cached
	                            * object.  Again we can use the 6 LSBs
	                            * for metadata */
	sc_object_st *next_dirty;  /**< Pointer to the next dirty element */
	sc_object_st *next_cached; /**< Pointer to the next cached object */
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
#define dir_hash(key) (((key >> 20) ^ (key >> 6)) % SC_HASHTABLE_SIZE)

/**
 * The second hash function, used to resolve collisions
 *
 * On collisions we can use the home board and core id info (9MSBs)
 * this way Objects from different homes will follow different probes
 *
 * @param key The key to hash
 */
#define dir_hash2(key) (key >> 23)

/**
 * Looks up the cache directory to find the requested object.
 *
 * @param key The object to look up in the directory
 *
 * @return The address of the cached object or NULL if it was not
 *         found
 */
static inline sc_object_st*
dir_lookup(UWord key)
{
	int          i    = 1;
	int          hash = dir_hash(key);
	int          hash2;
	sc_object_st *ret = &cacheDirectory_g[hash];

	if (((ret->key ^ key) >> 6) == 0)
		return ret;
	else if (ret->key) {
		hash2 = dir_hash2(key);

		while (i < SC_HASHTABLE_SIZE) {
			hash = (hash + hash2) % SC_HASHTABLE_SIZE;
			ret  = &cacheDirectory_g[hash];

			if (((ret->key ^ key) >> 6) == 0)
				return ret;
			else if (ret->key == NULL)
				return NULL;

			++i;
		}

		/* must never reach here */
		assume(0);

		return NULL;
	}
	else
		return NULL;
}

/**
 * Inserts a key-value pair in the directory
 *
 * @param key The key of the new node
 * @param val The value of the new node
 */
static inline sc_object_st*
dir_insert(UWord key, UWord val)
{
	int          i     = 1;
	int          hash  = dir_hash(key);
	int          hash2;
	sc_object_st *node = &cacheDirectory_g[hash];

	assume(!(key & ~SC_ADDRESS_MASK));

	if (node->key == NULL) {
		node->key = key;
		node->val = val;
	}
	else {
		hash2 = dir_hash2(key);

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

		assume(i < SC_HASHTABLE_SIZE);
	}

	node->next_dirty  = NULL;
	node->next_cached = cachedObjects_g;
	cachedObjects_g   = node;

	return node;
}

/**
 * Write back a dirty Object
 *
 * @param from The object to write back
 * @param to   The home address of the object
 * @param size The size of the object
 *
 * @return The used counter for the ack
 */
static inline int
write_back(Address from, Address to)
{
	int     cnt;
	/* The object's home node board id */
	int     to_bid;
	int     size, length;
	Address klass;

	switch ((*(int*)to) & HDR_headerTagMask) {

	case HDR_basicHeaderTag: /* Class Instance */
		klass = (Address) * (int*)from;
		size  = com_sun_squawk_Klass_instanceSizeBytes(klass) +
		        HDR_basicHeaderSize;

		/* printf("klass = %p\n", klass);
		 * printf("Size = %d HDR = %d name = %s\n", size, HDR_basicHeaderSize,
		 * com_sun_squawk_Klass_name(
		 *            klass)); */
		assume(size != HDR_basicHeaderSize);

		break;

	case HDR_arrayHeaderTag: /* Array */
		klass = (Address) * (int*)(from + 4);

		/* Check if we brought the whole array or not */
		length = (*(int*)from) >> 2;
		size   = length *
		         getDataSize(com_sun_squawk_Klass_componentType(klass)) +
		         HDR_arrayHeaderSize;
		break;
	case HDR_methodHeaderTag: /* Method */
		printf("%p is a method!\n", to);

		/*
		 * HACK: crash on method header accesses, until we fully
		 * understand their usage
		 */
		fatalVMError("Method header");
		break;
		break;
	default:
		fatalVMError("Wrong header tag");
		break;
	}

	/* Make sure we do not cache our own objects */
	assume(sc_in_heap(to));
	assume(sc_is_cacheable(to));

	/*
	 * make sure size <= 1MB, this is the upper limit for a DMA
	 * transfer
	 */
	assume((size > 0) && (size <= 0x100000));

	/* Find an available counter to use */
	cnt = hwcnt_get_free(HWCNT_SC_WB);
	assume(cnt >= 0);

	/* kt_printf("Write-back %3d %p to %p size = %d\n", cnt, from, to, size); */
	/* int i;
	 * for (i=0; i<size; i+=4) {
	 *  kt_printf("\t %p\t=%p --> %p\t=%p\n",
	 *            (UWord)from+i, *(int*)((UWord)from+i),
	 *            (UWord)to+i, *(int*)((UWord)to+i));
	 * } */
	/* sc_dump(); */

	/*
	 * DMAs are working on cache-line alignment and granularity
	 * (64B). So we round up the size and mask the from address
	 * accordingly to take the pure, cache-aligned address (offset) on
	 * the remote node.
	 */
	size = roundUp(size, sysGetCachelineSize());
	assume(!((UWord)from & ~SC_ADDRESS_MASK) &&
	       !((UWord)to & ~SC_ADDRESS_MASK));

	/*
	 * Write-backs can be made from the HW cache or the DRAM.  In the
	 * second case, they are directly passed to the local DRAM but
	 * have to be pushed to the local DMA engine first.
	 */

	/* Wait until our DMA engine can support at least one more DMA */
	while (!(ar_ni_status_get(sysGetCore()) & 0xFF)) {
		;
	}

	/* Init counter to -size */
	ar_cnt_set(sysGetCore(), cnt, -size);
	/* Get the object's home node cid and bid */
	sysHomeOfAddress(to, &to_bid, NULL);
	/* to_bid can't be zero */
	assume(to_bid);
	/* Issue the DMA */
	ar_dma_with_ack(sysGetCore(),   /* my core id */
	                sysGetIsland(), /* source board id */
	                sysGetCore(),   /* source core id */
	                (int)from,      /* source address */
	                to_bid - 1,     /* destination board id */
	                0xC,            /* destination core id */
	                (int)to,        /* destination address */
	                sysGetIsland(), /* ack board id */
	                sysGetCore(),   /* ack core id */
	                cnt,            /* ack counter */
	                size,           /* data length */
	                0,              /* Do not ignore dirty bit on
	                                 *  source on write-backs, this
	                                 *  might result in losing writes
	                                 *  to the software cache */
	                0,              /* force clean on dst */
	                1);             /*
	                                 * write through (doesn't really matter
	                                 * since we are writing directly on
	                                 * DRAM)
	                                 */

	/* while (ar_cnt_get(sysGetCore(), cnt) != 0) {
	 *  ;
	 * } */

	/* sc_dump(); */

	return cnt;
}                  /* write_back */

/**
 * Wait for all pending write backs to reach completion
 */
#define wait_pending_wb() hwcnt_wait_pending(HWCNT_SC_WB)

/**
 * Wait for all pending fetches to reach completion
 */
#define wait_pending_fe() hwcnt_wait_pending(HWCNT_SC_FETCH)

/**
 * Clears all records
 */
static inline void
dir_clear()
{
	sc_object_st *tmp;
	UWord        cached;

	while ((tmp = cachedObjects_g)) {

		/* NOTE: need to write-back to be safe in case of nested monitor
		 * acquisition */
		if (tmp->key & SC_DIRTY_MASK) {
			cached = tmp->val & SC_ADDRESS_MASK;
			write_back((Address)cached,
			           (Address)(((UWord)tmp->key &
			                      0x3FFFFC0) | MM_MB_HEAP_BASE));
		}

		cachedObjects_g  = tmp->next_cached;
		tmp->next_cached = NULL;
		tmp->key         = NULL;
		tmp->val         = NULL;
	}

	wait_pending_wb();

	cacheDirty_g = NULL;
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
void
sc_initialize()
{
	/* The cache directory. */
	cacheDirectory_g = (sc_object_st*)roundUp((UWord)mm_scache_base(
	                                              sysGetCore()),
	                                          sysGetCachelineSize());
	/* Start address of the allocated memory to the software cache. */
	cacheStart_g = (Address)roundUp((UWord)cacheDirectory_g + SC_DIRECTORY_SIZE,
	                                sysGetCachelineSize());
	/* End address of the allocated memory to the software cache. */
	cacheEnd_g = (Address)roundDown((UWord)cacheStart_g + SC_CACHE_SIZE,
	                                sysGetCachelineSize());
	/* Size of the software cache in bytes. */
	cacheSize_g      = cacheEnd_g - cacheStart_g;
	/* The next allocation address. */
	cacheAllocTop_g  = cacheStart_g;
	/* Temp pointer to the last allocated cache-line if it was not used. */
	cacheAllocTemp_g = NULL;
	/* Counter for the number of flushes due to full cache. */
	cacheFlushes_g   = 0;
	/* Counts how many times the cache was cleared. */
	cacheClears_g    = 0;
	/* Counter for the number of cached objects. */
	cacheObjects_g   = 0;
	/* List of dirty entries */
	cacheDirty_g     = NULL;
	/* List of cached entries */
	cachedObjects_g  = NULL;

	/* Make sure the cache is empty */
	memset(cacheDirectory_g, 0, SC_DIRECTORY_SIZE);
#if 0
	fprintf(stderr, "+------------------ SOFTWARE-CACHE -------------------\n");
	printRange("| Directory", cacheDirectory_g,
	           (Address)cacheDirectory_g + SC_DIRECTORY_SIZE);
	printRange("| Cache", cacheStart_g, cacheEnd_g);
	fprintf(stderr, "| CacheSize = %d\n", cacheSize_g);
	fprintf(stderr, "+-----------------------------------------------------\n");
#endif /* if 0 */

#if 0
	int start, end;

	if (sysGetCore() == 0 && sysGetIsland() == 0) {

		printf("-------------------- CACHE DUMP STATS --------------------\n");

		start = sysGetTicks();
		sc_flush(SC_BLOCKING);
		sc_flush(SC_BLOCKING);
		sc_flush(SC_BLOCKING);
		sc_flush(SC_BLOCKING);
		sc_flush(SC_BLOCKING);
		end = sysGetTicks();
		printf(" SW Flush takes:      %10u cc\n", (end - start) / 5);

		start = sysGetTicks();
		sc_clear();
		sc_clear();
		sc_clear();
		sc_clear();
		sc_clear();
		end = sysGetTicks();
		printf(" SW Clear takes:      %10u cc\n", (end - start) / 5);

		start = sysGetTicks();
		hwcache_flush();
		hwcache_flush();
		hwcache_flush();
		hwcache_flush();
		hwcache_flush();
		end = sysGetTicks();
		printf(" HW Flush takes:      %10u cc\n", (end - start) / 5);

		start = sysGetTicks();
		hwcache_flush_clear();
		hwcache_flush_clear();
		hwcache_flush_clear();
		hwcache_flush_clear();
		hwcache_flush_clear();
		end = sysGetTicks();
		printf(" HW FlushClear takes: %10u cc\n", (end - start) / 5);

		printf("----------------------------------------------------------\n");
	}

#endif /* if 0 */
}                  /* sc_initialize */

/**
 * Allocate a chunk of memory from the software cache
 *
 * @param   size        the length in bytes of the object and its header
 *                      (i.e. the total number of bytes to be allocated). If 0
 *                      it will allocate one cache-line.
 * @return a pointer to the allocated memory or NULL if the allocation failed
 */
static inline Address
challoc(int size)
{
	Address ret       = cacheAllocTop_g;
	Offset  available = Address_diff(cacheEnd_g, ret);

	size = size ? size : sysGetCachelineSize();
	assume(size >= 0 && size <= cacheSize_g);

#ifdef __MICROBLAZE__
	/* make sure size is a multiple of the cache line size */
	size = roundUp(size, sysGetCachelineSize());
#endif /* ifdef __MICROBLAZE__ */

	if (unlikely(lt(available, size))) {
		/* If there is not enough space write back any dirty objects */
		sc_flush(SC_BLOCKING);
		/* and clear the cache */
		sc_clear();
		/* Try to allocate again */
		ret = challoc(size);

		assume(ret != NULL);
	}
	else {
		cacheAllocTop_g = Address_add(ret, size);
#ifdef ASSUME
		/* zero the memory */
		zeroWords(ret, cacheAllocTop_g);
#endif /* ifdef ASSUME */
	}

	return (Address)ret;
}                  /* challoc */

/**
 * Clears all non read-only records and frees the memory
 */
void
sc_clear()
{
	/* printf("SC_CLEAR\n"); */
	/* remove the records from the directory */
	dir_clear();
	/* reset the allocation pointer */
	cacheAllocTop_g = cacheStart_g;
	/* flush and clear the hardware cache.
	 * NOTE: Here we need to flush to make sure the software cache
	 * changes will persist. */
	hwcache_flush_clear();
#ifdef SC_STATS
	cacheClears_g++;
#endif /* ifdef SC_STATS */
}

/**
 * Fetch a remote object to the software cache.
 *
 * @param from The object to fetch
 * @param to   The address in the cache to store the copy
 * @param size The size of the object (must be less than 1MB)
 * @param cid  The core id from whose cache to fetch the data
 */
static void
fetch(Address from, Address to, int size, int cid)
{
	int cnt;
	/* The object's home node board id */
	int from_bid;

	/* Make sure we do not cache our own objects */
	assume(sc_in_heap(from));
	assume(sc_is_cacheable(from));

	/*
	 * make sure size <= 1MB, this is the upper limit for a DMA
	 * transfer
	 */
	assume((size > 0) && (size <= 0x100000));

	/* Get an available counter to use */
	cnt = hwcnt_get_free(HWCNT_SC_FETCH);

	/*
	 * Fetches are directly passed to the remote DRAM but have to be
	 * pushed to the local DMA engine first.
	 */

	/* Wait until our DMA engine can support at least one more DMA */
	while ((ar_ni_status_get(sysGetCore()) & 0xFF) == 0) {
		;
	}

	/* Get the object's home node cid and bid */
	sysHomeOfAddress(from, &from_bid, NULL);
	/* from_bid can't be zero */
	assume(from_bid);
	/*
	 * DMAs are working on cache-line alignment and granularity
	 * (64B). So we round up the size and mask the from address
	 * accordingly to take the pure, cache-aligned address (offset) on
	 * the remote node.
	 */
	size = roundUp(size, sysGetCachelineSize());
	from = (Address)(((UWord)from & 0x3FFFFC0) | MM_MB_HEAP_BASE);
	/* Init ACK counter to -size */
	ar_cnt_set(sysGetCore(), cnt, -size);
	/* Issue the DMA */
	/* kt_printf("Issued DMA from %p (%d) of size %d\n", from,
	 *           (cid == -1) ? 0xC : cid, size); */
	ar_dma_with_ack(sysGetCore(),            /* my core id */
	                from_bid - 1,            /* source board id */
	                (cid == -1) ? 0xC : cid, /* source core id */
	                (int)from,               /* source address */
	                sysGetIsland(),          /* destination board id */
	                sysGetCore(),            /* destination core id */
	                (int)to,                 /* destination address */
	                sysGetIsland(),          /* ack board id */
	                sysGetCore(),            /* ack core id */
	                cnt,                     /* ack counter */
	                size,                    /* data length */
	                0,                       /* ignore dirty bit on source */
	                0,                       /* force clean on dst */
	                0);                      /* write through */

/*	printf("Fetch from: %d-%p to %d-%p\n", from_bid, from, sysGetIsland(), to);
**/

	/*
	 * FIXME: We are not sure the DMA reached completion (probably
	 * not), so for the moment let's just spin here. Note that this
	 * must be fixed in order to make prefetching work efficiently
	 * FIXME: Add check for Nack and retry if necessary
	 */
	while (ar_cnt_get_triggered(sysGetCore(), cnt) == 0) {
		;
	}

	if (ar_cnt_get_triggered(sysGetCore(), cnt) == 2) {
/*		kt_printf("DMA Completed %d\n", ar_cnt_get(sysGetCore(), cnt)); */
	}
	else {
		kt_printf("DMA Nacked\n");
		ar_abort();
	}

	/* kt_printf("Fetched %3d %p from %p size = %d\n", cnt, to, from, size);
	 * sc_dump(); */
	/* kt_printf("Fetched klass = %p to = %p name = %s\n", *(int*)to, to,
	 * com_sun_squawk_Klass_name(
	 *               *(int*)to)); */

	/* Mark the counter as available */
	hwcnts_g[cnt] = HWCNT_FREE;
}                  /* fetch */

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
 * +----------------+--------------+-----------------+-------------+
 * | Defining Class | Array Length | Klass reference | Method Body |
 * +----------------+--------------+-----------------+-------------+
 *                                                   ^
 *                                                   | Offset = 12 B
 *
 * To find out the type of object stored in the cache-line we use
 * the header tag as defined in
 * cldc/src/com/sun/squawk/vm/HDR.java
 *
 * This is actually a clone of blockToOop from
 * cldc/src/com/sun/squawk/GC.java
 */
static inline Address
block_to_oop(Address obj, Address oop, int cid)
{
	int     size, length;
	Address klass;

	/* int     start, end; */

	switch ((*(int*)oop) & HDR_headerTagMask) {

	case HDR_basicHeaderTag: /* Class Instance */
		/* start              = sysGetTicks(); */
		klass = (Address) * (int*)oop;
		/* end                = sysGetTicks();
		 * printf("Find klass took %10u cc \n", end - start); */

		/* Check if we brought the whole instance or not */
		/* start = sysGetTicks(); */
		size = com_sun_squawk_Klass_instanceSizeBytes(klass);
		/* end   = sysGetTicks();
		 * printf("Find size took %10u cc \n", end - start); */
		/* printf("Klass size = %d\n", size); */

		/*
		 * If the instance size is larger than the cache-line we need
		 * to fetch the rest
		 */
		if (unlikely(size > sysGetCachelineSize())) {
			/* start            = sysGetTicks(); */
			cacheAllocTemp_g = oop;
			oop              = challoc(size + HDR_basicHeaderSize);
			/* Fetch data */
			/* printf("Fetch again = %d (%p -- %p) Name=%s\n", size, (Address) *
			 *        (int*)cacheAllocTemp_g, obj,
			 *        com_sun_squawk_Klass_name(klass)); */
			fetch(obj, oop, size, cid);
			/* end = sysGetTicks();
			 * printf("refetch took %10u cc \n", end - start); */
		}

		oop += HDR_basicHeaderSize;
		break;

	case HDR_arrayHeaderTag: /* Array */
		klass = (Address) * (int*)(oop + 4);

		/* Check if we brought the whole array or not */
		length = (*(int*)oop) >> 2;
		size   = length *
		         getDataSize(com_sun_squawk_Klass_componentType(klass));
		/* printf("Array [%d] size = %d\n", length, size); */

		/*
		 * If the instance size times the array elements is larger
		 * than the cache-line we need to fetch the rest
		 */
		if (unlikely(size > sysGetCachelineSize())) {
			/*
			 * TODO: Do something smarter for arrays (don't bring the
			 * whole array if we don't need it)
			 */
			cacheAllocTemp_g = oop;
			oop              = challoc(size + HDR_arrayHeaderSize);
			/* Fetch data */
			fetch(obj, oop, size, cid);
		}

		oop += HDR_arrayHeaderSize;
		break;

	case HDR_methodHeaderTag: /* Method */
		printf("%p is a method!\n", oop);
		/* TODO: What's the size of a method? Did we bring the whole method? */
		oop += ((*(int*)oop) >> HDR_headerTagBits) * HDR_BYTES_PER_WORD;

		/*
		 * HACK: crash on method header accesses, until we fully
		 * understand their usage
		 */
		fatalVMError("Method header");

		break;

	default:
		fatalVMError("Wrong header tag");
		break;
	}              /* switch */

	return oop;
}                  /* block_to_oop */

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
sc_object_st*
sc_put(Address obj, int cid)
{
	Address      ret;
	sc_object_st *node;

	if (unlikely(cacheAllocTemp_g != NULL)) {
		/*
		 * If the last allocated cache-line was not used, try using it
		 * now
		 */
		ret              = cacheAllocTemp_g;
		cacheAllocTemp_g = NULL;
	}
	else {
		/*
		 * Allocate memory. We use the cache line size since we don't
		 * know the object's size a priori
		 */
		ret = challoc(0);
	}

	/* Fetch data */
	fetch(obj, ret, sysGetCachelineSize(), cid);

	/*
	 * The object's Klass is always here and resides in the ROM so
	 * there is no need for ext.ra fetches.
	 */

	/* int i;
	 * for (i=0; i<16; ++i) {
	 *  printf("Dump %p: %p\n", (((int*)ret)+i), *(((int*)ret)+i));
	 * } */

	/*
	 * Make sure the whole object is here and move the pointer to the
	 * proper offset from the cache-line head
	 */
	/* int start, end;
	 * start = sysGetTicks(); */
	ret = block_to_oop(obj, ret, cid);
	/* end   = sysGetTicks();
	 * printf("block_to_oop takes %10u cc\n", end - start); */

	/* Update the directory */
	/* start = sysGetTicks(); */
	node = dir_insert((UWord)obj & SC_ADDRESS_MASK, (UWord)ret);
	/* end   = sysGetTicks();
	 * printf("dir_insert takes %10u cc\n", end - start); */

	cacheObjects_g++;

	/* printf("Put %p\n", ret); */
	/* sc_dump(); */

	return node;
}                  /* sc_put */

/**
 * Looks up the cache to find the requested object. If it fails
 * (miss), it fetches it and adds it to the cache.
 *
 * @param obj      The object to get from the cache
 * @param is_write Whether it is going to be written after translation
 *
 * @return The cached object
 */
Address
sc_get(Address obj, int is_write)
{
	sc_object_st *ret;
	Address      retval;

	assume(is_write >> 1 == 0);

	/* printf("Searching for %p\n", obj); */

	/* int start, end;
	 * start = sysGetTicks(); */
	/* Check if it is cached */
	ret = dir_lookup((UWord)obj);
	/* end   = sysGetTicks();
	 * printf("dir_lookup takes %10u cc\n", end - start); */

	if (ret == NULL) { /* miss */
		/* printf(" %p not found\n", obj); */
		/* Cache the object */
		/* start = sysGetTicks(); */
		ret = sc_put(obj, -1);
		/* end   = sysGetTicks();
		 * printf("sc_put takes %10u cc\n", end - start); */
	}

	/* Address klass;
	 * klass = (Address) * (int*)(ret->val & SC_ADDRESS_MASK);
	 */

	/* If it is requested to be written and is not already marked as dirty */
	if (is_write && !(ret->key & SC_DIRTY_MASK)) {
		/* printf("Marked %p name = %s\n", obj,
		 * com_sun_squawk_Klass_name(klass)); */
		ret->key       |= is_write;
		ret->next_dirty = cacheDirty_g;
		cacheDirty_g    = ret;
	}

	/* else if (is_write && (ret->key & SC_DIRTY_MASK)) {
	 *  printf("Did not mark because it is dirty %p name = %s\n", obj,
	 * com_sun_squawk_Klass_name(
	 *             klass));
	 * } */

	retval = (Address)ret->val;

	/*
	 * printf(" %p found at %p\n", obj, ret);
	 * Assert ret is in the cache
	 */
	assume(hieq(retval, cacheStart_g));
	assume(lt(retval, cacheEnd_g));

	return retval;
}                  /* sc_get */

/**
 * Looks up the cache to find the requested object and marks it as
 * dirty.  If it fails (miss), it first fetches it and adds it to the
 * cache.
 *
 * @param obj The object to mark as dirty in the cache
 */
void
sc_mark_dirty(Address obj)
{
	UWord        key  = (UWord)obj;
	int          i    = 1;
	int          hash = dir_hash(key);
	int          hash2;
	sc_object_st *ret = &cacheDirectory_g[hash];

	/* printf("Marking %p\n", obj); */

	/* If the first cell is not empty and id doesn't match */
	if ((((ret->key ^ key) >> 6) != 0) && (ret->key)) {
		hash2 = dir_hash2(key);

		while (i < SC_HASHTABLE_SIZE) {
			hash = (hash + hash2) % SC_HASHTABLE_SIZE;
			ret  = &cacheDirectory_g[hash];

			/* Break if we find a match or an empty cell */
			if ((((ret->key ^ key) >> 6) == 0) || (ret->key == NULL))
				break;

			++i;
		}
	}

	/* printf("i=%d key=%p\n", i, ret->key); */
	assume(i < SC_HASHTABLE_SIZE && ret->key);

	/* If we find it and it is not marked already, we mark it */
	if (  /* (i < SC_HASHTABLE_SIZE) &&  */ !(ret->key & SC_DIRTY_MASK)) {
		/* printf("Marked %p\n", obj); */
		ret->key       |= 1;
		assume(ret->next_dirty == NULL);
		ret->next_dirty = cacheDirty_g;
		cacheDirty_g    = ret;
	}
}

/**
 * Write-back object
 *
 * @param object the object to write back
 */
void
sc_write_back(Address object)
{
	sc_object_st *entry;

	entry = dir_lookup((UWord)object);

	kt_printf("to=%p from=%p\n", object, entry->val);
	write_back((Address)entry->val, object);

	wait_pending_wb();
}

/**
 * Write-back any dirty objects in the software cache
 *
 * @param blocking Whether we should wait for the DMAs to reach completion or
 *                 not
 */
void
sc_flush(int blocking)
{
	int          cnt;
	sc_object_st *tmp;

	/* printf("SC_FLUSH\n"); */

	/*
	 * Wait for pending write backs to complete in order to avoid
	 * double write backs
	 */
	wait_pending_wb();

	/* Go through the dirty objects and issue write-back RDMAs */
	while (cacheDirty_g) {
		assume(cacheDirty_g->key & SC_DIRTY_MASK);
		UWord   cached = cacheDirty_g->val & SC_ADDRESS_MASK;
		Address oop    = (Address)(cacheDirty_g->key & SC_ADDRESS_MASK);

		/* TODO: Write-back only dirty cache lines not the whole object */
		cnt =
		    write_back((Address)cached,
		               (Address)(((UWord)oop & 0x3FFFFC0) | MM_MB_HEAP_BASE));

		/* FIXME cnt might be larger than 32 */
		assume(cnt < 32);
		/* Remove dirty bit and add cnt */
		/* TODO: Should dirty bit be removed after completion? */
		cacheDirty_g->key = (cacheDirty_g->key & SC_ADDRESS_MASK) | (cnt << 1);
		tmp               = cacheDirty_g;
		cacheDirty_g      = cacheDirty_g->next_dirty;
		tmp->next_dirty   = NULL;
	}

	/* Check if we need to wait for completion */
	if (likely(blocking)) {
		/* wait for the RDMAs to reach completion */
		wait_pending_wb();
	}

	/* Flush the hardware cache as well */
	hwcache_flush();
#ifdef SC_STATS
	cacheFlushes_g++;
#endif /* ifdef SC_STATS */
}                  /* sc_flush */

/**
 * Checks the object type and calculates its size and returns its
 * type.
 *
 * To find out the type of object stored in the cache-line we use
 * the header tag as defined in
 * cldc/src/com/sun/squawk/vm/HDR.java
 *
 * @param oop            The object to get its size
 * @param size_in_bytes  A pointer to an integer to write the object size
 *
 * @return 2 if the object is a method
 *         1 if it is an array
 *         0 if it is a class instance
 */
static inline int
object_size_and_type(Address oop, int *size_in_bytes)
{
	int length;

	switch ((*(int*)oop) & HDR_headerTagMask) {

	case HDR_basicHeaderTag: /* Class Instance */

		/* Check if we brought the whole instance or not */
		*size_in_bytes =
		    com_sun_squawk_Klass_instanceSizeBytes((Address) * (int*)oop);
		/* printf("Klass size = %d\n", size); */

		return 0;

	case HDR_arrayHeaderTag: /* Array */

		/* Check if we brought the whole array or not */
		length         = (*(int*)oop) >> 2;
		*size_in_bytes = length * getDataSize(com_sun_squawk_Klass_componentType(
		                                          (Address) * (int*)(oop + 4)));
		/* printf("Array [%d] size = %d\n", length, size); */

		return 1;

	case HDR_methodHeaderTag: /* Method */
		/* printf("%p is a method!\n", oop); */
		/* TODO: What's the size of a method? */
		oop += ((*(int*)oop) >> HDR_headerTagBits) * HDR_BYTES_PER_WORD;

		/*
		 * HACK: crash on method header accesses, until we fully
		 * understand their usage
		 */
		fatalVMError("Method header");

		return 2;

	default:
		fatalVMError("Wrong header tag");

		return -1;
	}
}

/**
 * Dumps the contents of the cache
 */
void
sc_dump()
{
	int   i, j;
	UWord key, val;

	printf("-------------------- CACHE DUMP START --------------------\n");

	for (i = 0; i < SC_HASHTABLE_SIZE; ++i) {
		if (cacheDirectory_g[i].val && cacheDirectory_g[i].val <
		    (UWord)cacheAllocTop_g) {
			key = cacheDirectory_g[i].key & SC_ADDRESS_MASK;
			val = cacheDirectory_g[i].val & SC_ADDRESS_MASK;
			printf("%p cached @ %p\n", key, val);

			for (j = 0; j < 12; j += 4, key += 4, val += 4) {
				printf("\t %p = %p |? (%p = %p) \n", val, *(int*)val, key,
				       *(int*)key);
			}
		}
	}

	printf("--------------------  CACHE DUMP END  --------------------\n");
}

/**
 * Dumps the statistics of the cache
 */
void
sc_stats()
{
#ifdef SC_STATS
	printf("-------------------- CACHE DUMP STATS --------------------\n");

	/* Counter for the number of flushes due to full cache. */
	printf(" Flushes: %10u\n", cacheFlushes_g);
	/* Counts how many times the cache was cleared. */
	printf(" Clears:  %10u\n", cacheClears_g);
	/* Counter for the number of cached objects. */
	printf(" Cached:  %10u\n", cacheObjects_g);

	printf("----------------------------------------------------------\n");
#endif /* ifdef SC_STATS */
}                  /* sc_stats */
