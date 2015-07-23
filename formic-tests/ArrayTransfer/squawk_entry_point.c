/*
 * Copyright 2013-2015 FORTH-ICS / CARV
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
 * @file   squawk_entry_point.c
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * @brief The interface/glue for the build system to connect the
 * application with the VM
 */

#include <kernel_toolset.h>

extern void Squawk_main_wrapper(int fakeArgc, char** fakeArgv);

void squawk_entry_point(void)
{
  char          *fakeArgv[5];
  int           fakeArgc;

  fakeArgv[0] = "dummy";
  fakeArgv[1] = "-spotsuite:FormicApp";
  fakeArgv[2] = "-stats";
  fakeArgv[3] = "-verbose";
  fakeArgv[4] = "arraytransfer.Main";
  fakeArgc    = 5;

  Squawk_main_wrapper(fakeArgc, fakeArgv);

  return;
}
