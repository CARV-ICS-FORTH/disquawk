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
 * @file   softcache.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * Header file for the software cache
 */

#ifndef __SOFTCACHE_H__
#define __SOFTCACHE_H__

/**
 * Since objects are cache line aligned we use the LSB as the dirty
 * bit and the 1-5 bits to store the acknowledgment counter for the
 * object if there is a pending DMA transfer for it
 */
#define SC_DIRTY_MASK   0x1
//#define SC_CNT_MASK     0x3E
#define SC_ADDRESS_MASK ~(SC_DIRTY_MASK)

/**
 * For the write back DMA acknowledgments we use the hardware counters
 * 90-122
 */
#define SC_DMA_WB_CNT_START 90

/**
 * A node of the directory.
 */
typedef struct sc_object sc_object_st;

// The hash-table size must be the closest prime to the lines that can
// fit in the cache. To calculate this value use the
// cache_memory/(cache_line+sizeof(2*(void*))) formula and pass it to
// http://easycalculation.com/prime-number.php to find the next
// smallest (nearest) prime number
#define SC_HASHTABLE_SIZE 94651
#define SC_DIRECTORY_SIZE SC_HASHTABLE_SIZE * (2*sizeof(int))
#define SC_CACHE_SIZE     SC_HASHTABLE_SIZE * MM_CACHELINE_SIZE

// The size of a Klass object rounded up to cache line size
#define SC_KLASS_SIZE     64

#endif /* __SOFTCACHE_H__ */
