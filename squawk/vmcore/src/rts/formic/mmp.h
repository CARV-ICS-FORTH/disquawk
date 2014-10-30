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
 * @file   mmp.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The interface of the Mailbox Message Protocol (MMP)
 *
 */

#ifndef RTS_FORMIC_MMP_H
#define RTS_FORMIC_MMP_H
#include "mmp_ops.h"
#include "address.h"
#include "platform.h"

void mmpSpawnThread(Address thread);
INLINE Address mmpCheckMailbox(Address type);

#endif /* RTS_FORMIC_MMP_H */
