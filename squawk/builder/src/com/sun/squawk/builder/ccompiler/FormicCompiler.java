/*
 * Copyright 2004-2008 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2013 FORTH-ICS. All Rights Reserved.
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
 * Please contact Foundation for Research and Technology - Hellas (FORTH)
 * Institute of Computer Science (ICS), N. Plastira 100, Vassilika Vouton,
 * GR-700 13 Heraklion, Crete, Greece or visit www.ics.forth.gr if you need
 * additional information or have any questions.
 */

package com.sun.squawk.builder.ccompiler;

import com.sun.squawk.builder.*;
import com.sun.squawk.builder.platform.*;
import java.io.*;
import java.util.Arrays;

/**
 * The interface for the "gcc" compiler.
 */
public class FormicCompiler extends CCompiler {

    public FormicCompiler(String name, Build env, Platform platform) {
        super(name, env, platform);
    }

    public FormicCompiler(Build env, Platform platform) {
        this("formic", env, platform);
    }

    /**
     * {@inheritDoc}
     */
    public String options(boolean disableOpts) {
        StringBuffer buf = new StringBuffer();
        if (!disableOpts) {
            if (options.o1)             { buf.append("-O1 ");               }
            if (options.o2)             { buf.append("-O2 ");               }
            if (options.o3)             { buf.append("-DMAXINLINE -O3 ");   }
        }
        if (options.tracing)            { buf.append("-DTRACE ");           }
        if (options.profiling)          { buf.append("-DPROFILING ");       }
        if (options.macroize)           { buf.append("-DMACROIZE ");        }
        if (options.assume)             { buf.append("-DASSUME ");          }
        if (options.typemap)            { buf.append("-DTYPEMAP ");         }
        if (options.kernel)             { buf.append("-DKERNEL_SQUAWK=true ");     }

        if (options.nativeVerification) { buf.append("-DNATIVE_VERIFICATION=true ");          }

        // Required for definition of RTLD_DEFAULT handle sent to dlsym
        buf.append("-D_GNU_SOURCE ");
        buf.append("-DFLASH_MEMORY ");

        /* Formic FLAGS */
        buf.append("-DARCH_MB ");
        buf.append("-mcpu=v8.00.b ");
        buf.append("-mno-xl-soft-mul ");
        buf.append("-mno-xl-multiply-high ");
        buf.append("-mxl-soft-div ");
        buf.append("-mxl-barrel-shift ");
        buf.append("-mxl-pattern-compare ");
        buf.append("-mlittle-endian ");
        buf.append("-mhard-float ");
        buf.append("-fsingle-precision-constant ");
        buf.append("-mxl-float-convert ");
        buf.append("-mxl-float-sqrt ");
        buf.append("-Wall ");

        // Only enable debug switch if not optimizing
        if (!options.o1 &&
            !options.o2 &&
            !options.o3)                { buf.append("-g ");               }

        buf.append("-DSQUAWK_64=" + options.is64 + " ");

        buf.append("-DPLATFORM_BIG_ENDIAN=" + platform.isBigEndian()).append(' ');
        buf.append("-DPLATFORM_UNALIGNED_LOADS=" + platform.allowUnalignedLoads()).append(' ');

        return buf.append(options.cflags).append(' ').toString();
    }

    /**
     * {@inheritDoc}
     */
    public File compile(File[] includeDirs, File source, File dir, boolean disableOpts) {
        File object = new File(dir, source.getName().replaceAll("\\.c", "\\.o"));
        env.exec("mb-gcc -c " +
                 options(disableOpts) + " " +
                 include(includeDirs, "-I") +
                 " -o " + object + " " + source);
        return object;
    }

    /**
     * {@inheritDoc}
     */
    public File link(File[] objects, String out, boolean dll) {
        String output;
        String exec;

        // objects[0] = linkerScript

        output = out + platform.getExecutableExtension();
        File linkerOutputFile = new File(output);

        // objects[0] is the linker script
        exec = "mb-ld -N -M -T " + objects[0] + " ";
        // This is bad but it works
        exec += Build.join(Arrays.copyOfRange(objects, 1, objects.length)) + " ";
        exec += "--format elf32-microblazele --oformat elf32-microblazele ";
        exec += "-Map " + (new File(linkerOutputFile.getParentFile(), "link.map "));
        exec += "-o " + output + " ";
        env.exec(exec);

        return linkerOutputFile;
    }

    /**
     * {@inheritDoc}
     */
    public String getArchitecture() {
        return "Formic";
    }

    /**
     * {@inheritDoc}
     */
    public boolean isCrossPlatform() {
        return true;
    }
}
