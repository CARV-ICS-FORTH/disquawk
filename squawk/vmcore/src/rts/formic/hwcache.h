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
 * @file   hwcache.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The interface for the formic hardware caches
 */

#ifndef HWCACHE_H_
#define HWCACHE_H_

#include <arch.h>

#define hwcache_flush       ar_cache_flush
#define hwcache_clear       ar_cache_clear
#define hwcache_flush_clear ar_cache_flush_clear

#endif /* HWCACHE_H_ */
