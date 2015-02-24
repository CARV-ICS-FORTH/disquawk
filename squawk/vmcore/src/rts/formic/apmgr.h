/*
 * Copyright      2015 FORTH-ICS / CARV
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
 * @file   apmgr.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * Header file for the Atomic Primitives Manager (APMGR) implementations
 */

#ifndef APMGR_H_
#define APMGR_H_
#include <util.h>

void apmgrCASHandler(int bid, int cid, Address object, int expected, int new);

#endif /* APMGR_H_ */
