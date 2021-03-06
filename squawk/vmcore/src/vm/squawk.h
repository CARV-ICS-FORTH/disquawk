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
 * @file   squawk.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 */

#ifndef VM_SQUAWK_H_
#define VM_SQUAWK_H_

#include "address.h"
#include "platform.h"

/**
 * Forward definition of printf/fprintf helper function.
 */
const char *format(const char* fmt);

NORETURN void stopVM0(int, boolean);
NORETURN void stopVM(int);

Address lookupKlass(int cid);
Address lookupStaticMethod(int cid, int methodOffset);

#endif /* VM_SQUAWK_H_ */
