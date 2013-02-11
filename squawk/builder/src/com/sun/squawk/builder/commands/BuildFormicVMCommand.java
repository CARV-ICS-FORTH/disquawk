/*
 * Copyright 2005-2010 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2012 Oracle. All Rights Reserved.
 *
 * This software is the proprietary information of Oracle.
 * Use is subject to license terms.
 *
 * This is a part of the Squawk JVM.
 */
package com.sun.squawk.builder.commands;

import java.io.*;
import java.util.*;

import com.sun.squawk.builder.*;
import com.sun.squawk.builder.ccompiler.*;
import com.sun.squawk.builder.util.*;

public class BuildFormicVMCommand extends FormicCommand {

  public static final File VM_SRC_DIR = new File("vmcore/src/vm");
  public static final File VM_BLD_DIR = new File("vmcore/build");
  public static final File FP_SRC_DIR = new File("vmcore/src/vm/fp");
  public static final File UTIL_SRC_DIR = new File("vmcore/src/vm/util");
  public static final File VM_SRC_RTS_DIR = new File("vmcore/src/rts");
  public static final File VM_SRC_FILE = new File(VM_SRC_DIR, "squawk.c");
  public static final File VM2C_SRC_FILE = new File(VM_SRC_DIR, "vm2c.c.spp");
  public static final File linkerOutputFile = new File(VM_BLD_DIR, "vmcore-flash.elf");
  public static final File objCopyOutputFile = new File(VM_BLD_DIR, "vm-formic.bin");

  public BuildFormicVMCommand(Build env) {
    super(env, "buildFormicVM");
  }

  /**
   * {@inheritDoc}
   */
  public String getDescription() {
    return "builds the VM executable for the Formic target";
  }


  /**
   *
   */
  protected void build() {

    CCompiler ccompiler = env.getCCompiler();

    // Preprocess any files with the ".spp" suffix
    List<File> generatedFiles = new ArrayList<File>();
    FileSet sppFiles = new FileSet(VM_SRC_DIR, new FileSet.SuffixSelector(".spp"));
    for (File sppFile : sppFiles.list()) {
      SppFilePreprocessCommand.preprocess(sppFile, generatedFiles,
          env.getPreprocessor(), env.getMacroizer(),
          ccompiler.options.macroize);
    }

    env.log(env.info, "Building VM for Formic target");

    File[] includeDirs;

    File VM_SRC_RTS_PLAT_DIR =
      new File(VM_SRC_RTS_DIR, ccompiler.getRtsIncludeName());

    includeDirs = new File[] {
      VM_SRC_DIR,
        FP_SRC_DIR,
        VM_SRC_RTS_PLAT_DIR
    };

    File MMU_SRC_FILE =  new File(VM_SRC_RTS_PLAT_DIR, "mmu_fat.c");

    Build.mkdir(VM_BLD_DIR);

    List<File> objectFiles = new ArrayList<File>();
    if (ccompiler.options.floatsSupported) {
      /*
       * The floating point code must be compiled without optimization for the reasons stated in this
       * excerpt from the KVM porting guide (Squawk's FP implementation is derived from the KVM):
       *
       * 10.1.2 Implementing Java virtual machine floating point semantics
       *
       *   Many processor architectures natively support IEEE 754 arithmetic on float and
       *   double formats. Therefore, there is often a straightforward mapping between Java
       *   virtual machine floating-point operations, C code implementing those operations,
       *   and floating-point instructions on the underlying processor. However, various
       *   complications are possible:
       *
       *   o The floating-point semantics of the Java virtual machine are tightly specified,
       *     much more tightly specified than C language floating-point semantics. Therefore,
       *     a C compiler could perform an  optimization  that was an allowed transformation
       *     in C but broke Java virtual machine semantics. For example, in the Java virtual
       *     machine and Java:
       *
       *        x + 0.0
       *
       *     cannot be replaced with
       *
       *        x
       *
       *     since different answers can be generated.
       *
       *     Therefore, the portions of the KVM that implement Java virtual machine floating-point
       *     semantics should be compiled without aggressive optimization to help avoid such (in
       *     this case) unhelpful code transformations. Many C compilers also have separate flags
       *     affecting floating-point code generation, such as flags to improve floating-point
       *     consistency and make the generated code have semantics more closely resembling a
       *     literal translation of the source. Regardless of the processor architecture, using
       *     such flags might be necessary to implement Java virtual machine semantics in C code.
       */

      env.log(env.brief, "[compiling floating point sources in " + FP_SRC_DIR + " ...]");
      List<File> sources = new FileSet(FP_SRC_DIR, new FileSet.SuffixSelector(".c")).list();
      for (File source : sources) {
        objectFiles.add(ccompiler.compile(includeDirs, source, VM_BLD_DIR, true));
      }
    }



    if (ccompiler.options.nativeVerification) {
      env.log(env.brief, "[compiling native verification sources in " + UTIL_SRC_DIR + " ...]");
      List<File> sources = new FileSet(UTIL_SRC_DIR, new FileSet.SuffixSelector(".c")).list();
      for (File source : sources) {
        objectFiles.add(ccompiler.compile(includeDirs, source, VM_BLD_DIR, false));
      }
    }

    env.log(env.brief, "[compiling '" + VM_SRC_FILE + "' ...]");
    objectFiles.add(ccompiler.compile(includeDirs, VM_SRC_FILE, VM_BLD_DIR, false));

    env.log(env.info, "[assembling '" + VM_SRC_RTS_PLAT_DIR + "/java-irq-hndl.s' ...]");
    File assembled_f = new File(VM_SRC_RTS_PLAT_DIR, "java-irq-hndl.o");
    String assemble;
    assemble = "mb-as -mlittle-endian -o " + assembled_f + "  ";
    assemble += new File(VM_SRC_RTS_PLAT_DIR, "java-irq-hndl.s");
    env.exec(assemble);
    objectFiles.add(assembled_f);

    env.log(env.brief, "[compiling '" + MMU_SRC_FILE + "' ...]");
    objectFiles.add(ccompiler.compile(includeDirs, MMU_SRC_FILE, VM_BLD_DIR, false));

    env.log(env.info, "[linking '" + linkerOutputFile + "'...]");
    File linkerScript = new File(VM_SRC_RTS_PLAT_DIR, "link.dat");
    String link;
    link = "mb-ld -N -M -T " + linkerScript + " ";
    link += Build.join((File[])objectFiles.toArray(new File[objectFiles.size()])) + " ";
    link += "--format elf32-microblazele --oformat elf32-microblazele ";
    link += "-Map " + (new File(linkerOutputFile.getParentFile(), "link.map "));
    link += "-o " + linkerOutputFile + " ";
    env.exec(link, null, VM_BLD_DIR);

    if (!env.verbose) {
      for (File file : generatedFiles) {
        Build.delete(file);
        Build.clear(VM_BLD_DIR, true);
      }
    }

    // Create the bin file
    env.log(env.info, "[dumping '" + objCopyOutputFile + "'...]");
    String dump;
    dump = "mb-objcopy -O binary --gap-fill 0 " + linkerOutputFile;
    dump = " " + objCopyOutputFile;
    env.exec(dump);

    env.log(env.info, "Build complete");
  }

  /**
   * {@inheritDoc}
   */
  public void clean() {

    // remove intermediate build files
    Build.clear(VM_BLD_DIR, true);

    // remove vm-formic.bin, etc.
    Build.delete(objCopyOutputFile);
    Build.delete(linkerOutputFile);

    // this may not be empty. ignore if this fails.
    try {
      Build.delete(VM_BLD_DIR);
    } catch (BuildException be) {}
  }

}
