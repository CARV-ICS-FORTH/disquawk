/*
 * Copyright     2015, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
 * Copyright 2008 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Sun Microsystems, Inc., 16 Network Circle, Menlo
 * Park, CA 94025 or visit www.sun.com if you need additional
 * information or have any questions.
 */
package com.sun.squawk.builder.commands;

import java.io.File;
import java.io.PrintStream;

import com.sun.squawk.builder.Build;
import com.sun.squawk.builder.Command;

/**
 * @author Dave
 * @author Foivos S. Zakkak
 *
 */
public abstract class FormicCommand extends Command {

  protected String platformName;

  /**
   * @param env
   * @param name
   */
  public FormicCommand(Build env, String name) {
    super(env, name);
  }

  public void usage(String errMsg) {
    PrintStream out = System.out;

    out.println();
    out.println(errMsg);
    out.println();
    out.println(getDescription());
    out.println("usage: " + getName() + " [ -h | -clean | -app:<path to application>]");
  }

  /**
   * {@inheritDoc}
   */
  public void run(String[] args) {
    if (args.length == 0) {
      build();
    } else {
      if (args[0].equals("-clean")) {
        clean();
      } else if (args[0].equals("-h")) {
        usage("");
      } else if (args[0].startsWith("-app:")) {
        build(args[0].substring(5));
      }
    }
  }

  protected abstract void build();
  protected abstract void build(String app_dir);
}
