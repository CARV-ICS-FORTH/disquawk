/*
 * Copyright 2005-2010 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2012 Oracle. All Rights Reserved.
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

  private ArrayList<String> vm2cRootClasses;

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
   * Converts the arguments given to this command to romizer arguments.
   *
   * @param   args   the arguments passed to this command
   * @param   arch   the target architecture hint provided by the C compiler
   * @return  <code>args</code> reformatted for the romizer
   */
  private String[] convertToRomizerOptions(String arch) {
    String[] args;
    String bootstrapSuiteName = "squawk";
    List<String> romizerArgs = new ArrayList<String>();
    String endian = env.getPlatform().isBigEndian() ? "big" : "little";
    SuiteMetadata parentSuiteMetadata = new SuiteMetadata();
    SuiteMetadata suiteMetadata = new SuiteMetadata();

    if (env.getspecfifiedBuildDotOverrideFileName() != null) {
      romizerArgs.add("-override:" + env.getspecfifiedBuildDotOverrideFileName());
    }

    // pass in properties builder was passed on command line:
    for (Object key: env.dynamicProperties.keySet()) {
      Object value = env.dynamicProperties.get(key);
      romizerArgs.add("-D" + key.toString() + "=" + value.toString());
    }

    // The remaining args are the modules making up one or more suites
    boolean isBootstrapSuite = true;
    List<Target.FilePair> allClassesLocations = new ArrayList<Target.FilePair>();
    List<Target.FilePair> allJava5ClassesLocations = new ArrayList<Target.FilePair>();
    List<Target.FilePair> classesLocations = new ArrayList<Target.FilePair>();
    List<Target.FilePair> java5ClassesLocations = new ArrayList<Target.FilePair>();
    boolean createJars = true;

    String module = "cldc";
    Command moduleCommand = env.getCommand(module);
    Target target = null;
    if (moduleCommand instanceof Target) {
      target = (Target) moduleCommand;
    } else {
      // see if we can conjure up a target for this module.
      File moduleDir = new File(module);
      if (moduleDir.exists() && new File(moduleDir, "src").exists()) {
        target = env.addTarget(true, module, "");
      }
    }
    if (target != null) {
      suiteMetadata.addTargetIncluded(module);
      if (!parentSuiteMetadata.includesTarget(module)) {
        suiteMetadata.addTargetIncluded(module);
        suiteMetadata.addTargetsIncluded(target.getDependencyNames());
        List<Target.FilePair> dirs = new ArrayList<Target.FilePair>();
        target.addDependencyDirectories(target.getPreverifiedDirectoryName(), dirs, parentSuiteMetadata.getTargetsIncluded());
        target.addDependencyDirectories(target.getResourcesDirectoryName(), dirs, parentSuiteMetadata.getTargetsIncluded());
        for (Target.FilePair file : dirs) {
          if (!allClassesLocations.contains(file) && file.getCanonicalFile().exists()) {
            classesLocations.add(file);
            allClassesLocations.add(file);
          }
        }
        if (target.j2me) {
          dirs = new ArrayList<Target.FilePair>();
          target.addDependencyDirectories(target.getCompiledDirectoryName(), dirs, parentSuiteMetadata.getTargetsIncluded());
          for (Target.FilePair file : dirs) {
            if (!allJava5ClassesLocations.contains(file) && file.getCanonicalFile().exists()) {
              java5ClassesLocations.add(file);
              allJava5ClassesLocations.add(file);
            }
          }
        }
      }
    } else {
      throw new BuildException("'" + module + "' module is not a jar/zip file and does not have a target defined for it");
    }

    romizerArgs.add("-o:" + bootstrapSuiteName);
    romizerArgs.add("-arch:" + arch);
    romizerArgs.add("-endian:" + endian);
    StringBuilder cp = new StringBuilder();
    cp.append("-cp:");
    for (Target.FilePair fp: classesLocations) {
      File file = fp.getFile();
      cp.append(file.getPath());
      cp.append(File.pathSeparatorChar);
    }
    romizerArgs.add(cp.toString());
    if (!java5ClassesLocations.isEmpty()) {
      StringBuilder java5Cp = new StringBuilder();
      java5Cp.append("-java5cp:");
      for (Target.FilePair fp: java5ClassesLocations) {
        File file = fp.getFile();
        java5Cp.append(file.getPath());
        java5Cp.append(File.pathSeparatorChar);
      }
      romizerArgs.add(java5Cp.toString());
    }
    if (createJars) {
      romizerArgs.add("-jars");
    }

    for (Target.FilePair fp: classesLocations) {
      File file = fp.getFile();
      String path = file.getPath();
      if (!romizerArgs.contains(path)) {
        romizerArgs.add(path);
      }
    }

    args = new String[romizerArgs.size()];
    romizerArgs.toArray(args);
    return args;
  }

  public void runRomizer() {
    String[] args;
    CCompiler ccompiler = env.getCCompiler();
    String arch = ccompiler == null ? "X86" : ccompiler.getArchitecture();
    args = convertToRomizerOptions(arch);
    env.runCommand("romize", args);
  }

  /**
   *
   */
  protected void build() {

    CCompiler ccompiler = env.getCCompiler();

    runRomizer();

    updateVM2CGeneratedFile();

    // Preprocess any files with the ".spp" suffix
    List<File> generatedFiles = new ArrayList<File>();
    FileSet sppFiles = new FileSet(VM_SRC_DIR, new FileSet.SuffixSelector(".spp"));
    for (File sppFile : sppFiles.list()) {
      SppFilePreprocessCommand.preprocess(sppFile, generatedFiles,
          env.getPreprocessor(), env.getMacroizer(),
          ccompiler.options.macroize);
    }

    env.log(env.info, "Building VM for Formic target");

    String options = ccompiler.options(false).trim();
    createBuildFlagsHeaderFile(options);

    File MYRMICS_DIR =
      new File(VM_SRC_RTS_DIR, ccompiler.getRtsIncludeName()+"/myrmics/src");

    File[] includeDirs = new File[] {
      VM_SRC_DIR,
        FP_SRC_DIR,
        new File(VM_SRC_RTS_DIR, ccompiler.getRtsIncludeName()),
        new File(MYRMICS_DIR, "include")
    };

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

    File OBJ_JAVA      = new File(MYRMICS_DIR, "obj/java");
    // TODO: Compile myrmics
    // Add the myrmics' compiled files for linking
    File OBJ_JAVA_ARCH = new File(MYRMICS_DIR, "arch");
    File OBJ_JAVA_KT   = new File(MYRMICS_DIR, "kt");
    File OBJ_JAVA_DBG  = new File(MYRMICS_DIR, "dbg");
    File OBJ_JAVA_NOC  = new File(MYRMICS_DIR, "noc");
    File OBJ_JAVA_MM   = new File(MYRMICS_DIR, "mm");
    File OBJ_JAVA_PR   = new File(MYRMICS_DIR, "pr");
    File OBJ_JAVA_SYS  = new File(MYRMICS_DIR, "sys");
    FileSet.SuffixSelector sselector = new FileSet.SuffixSelector(".mb.o");
    //TODO: Add main
    //objectFiles.add(new File(OBJ_JAVA, "java_main.mb.o"));
    objectFiles.addAll(new FileSet(OBJ_JAVA      , sselector).list());
    objectFiles.addAll(new FileSet(OBJ_JAVA_ARCH , sselector).list());
    objectFiles.addAll(new FileSet(OBJ_JAVA_KT   , sselector).list());
    objectFiles.addAll(new FileSet(OBJ_JAVA_DBG  , sselector).list());
    objectFiles.addAll(new FileSet(OBJ_JAVA_NOC  , sselector).list());
    objectFiles.addAll(new FileSet(OBJ_JAVA_MM   , sselector).list());
    objectFiles.addAll(new FileSet(OBJ_JAVA_PR   , sselector).list());
    objectFiles.addAll(new FileSet(OBJ_JAVA_SYS  , sselector).list());

    env.log(env.brief, "[linking '" + linkerOutputFile.toString() + "'...]");
    objectFiles.add(0, new File(MYRMICS_DIR, "linker.java.mb.ld"));
    File[] objects = (File[])objectFiles.toArray(new File[objectFiles.size()]);
    ccompiler.link(objects, linkerOutputFile.toString(), false);

    if (!env.verbose) {
      for (File file : generatedFiles) {
        Build.delete(file);
      }
      for (File file : objectFiles) {
        Build.delete(file);
      }
    }

    // Create the bin file
    env.log(env.brief, "[objcopy '" + objCopyOutputFile.toString() + "'...]");
    String dump;
    dump = "mb-objcopy -O binary --gap-fill 0 " + linkerOutputFile.toString();
    dump += " " + objCopyOutputFile.toString();
    env.exec(dump);

    env.log(env.info, "Build complete");
  }

  /**
   * Creates the "buildflags.h" file containing the string constant indicating what
   * C compiler flags the VM was built with.
   *
   * @param buildFlags   the C compiler flags
   */
  private void createBuildFlagsHeaderFile(String buildFlags) {
    File buildFlagsFile = new File(VM_SRC_DIR, "buildflags.h");
    try {
      FileOutputStream fos = new FileOutputStream(buildFlagsFile);
      PrintStream out = new PrintStream(fos);
      out.println("#define BUILD_FLAGS \"" + buildFlags + '"');
      fos.close();
    } catch (IOException e) {
      throw new BuildException("could not create " + buildFlagsFile, e);
    }
  }

  private void updateVM2CGeneratedFile() {
    vm2cRootClasses = new ArrayList<String>();

    // always translate these classes:
    vm2cRootClasses.add("com.sun.squawk.VM");
    vm2cRootClasses.add("com.sun.squawk.MethodHeader");

    // may translate GC:
    if (env.getBooleanProperty("GC2C")) {
      vm2cRootClasses.add(env.getProperty("GC"));
    }

    FileSet.Selector isOutOfDate = new FileSet.AndSelector(Build.JAVA_SOURCE_SELECTOR, new FileSet.DependSelector(new FileSet.Mapper() {
      public File map(File from) {
        return VM2C_SRC_FILE;
      }
    }));

    File srcDir = new File("cldc", "src");
    if (!srcDir.exists()) {
      env.log(env.info, "No " + srcDir + " so assuming no need to generate vm2c files, ie remote build and files are already generated");
      return;
    }
    File preDir;
    if (env.isJava5SyntaxSupported()) {
      preDir = new File("cldc", "preprocessed");
    } else {
      Target cldcTarget = (Target) env.getCommandForced("cldc");
      preDir = env.preprocess(new File("cldc"), cldcTarget.srcDirs, true, true);
    }

    // Rebuilds the generated file if any of the *.java files in cldc/src or cldc/preprocessed have
    // a later modification date than the generated file.
    if (!VM2C_SRC_FILE.exists() || !new FileSet(srcDir, isOutOfDate).list().isEmpty() || (preDir.exists() && !new FileSet(preDir, isOutOfDate).list().isEmpty())) {
      // Verify that the vm2c commands were installed in my env, if not then throw an exception
      // This is to handle case where vm2c did not install itself, but we still wanted to run the rom command to do just compilation
      Command command = env.getCommand("runvm2c");
      if (command == null) {
        throw new BuildException("The module vm2c and runvm2c we're not installed, this is very likely due to these modules requiring the use of JDK 1.5 or higher");
      }
      // Ensure that the *existing* preprocessed files in cldc are in sync with the original sources
      FileSet.Selector selector = new FileSet.AndSelector(Build.JAVA_SOURCE_SELECTOR, new FileSet.DependSelector(new FileSet.SourceDestDirMapper(srcDir, preDir)) {
        public boolean isSelected(File file) {
          File dependentFile = mapper.map(file);
          return dependentFile.exists() && dependentFile.lastModified() < file.lastModified();

        }
      });
      List<File> ood = new FileSet(srcDir, selector).list();
      if (!ood.isEmpty()) {
        throw new BuildException("need to re-build 'cldc' as the preprocessed version of the following files are out of date: " + ood);
      }

      List<String> args = new ArrayList<String>();
      args.add("-o:" + VM2C_SRC_FILE);
      args.add("-cp:");
      args.add("-sp:." + File.pathSeparator + preDir.getPath());
      for (String rootClass: vm2cRootClasses) {
        env.log(env.verbose, "vm2c root class: " + rootClass);
        args.add("-root:" + rootClass);
      }
      args.addAll(new FileSet(preDir, Build.JAVA_SOURCE_SELECTOR).listStrings());

      File argsFile = new File("vm2c", "vm2c.input");
      JavaCompiler.createArgsFile(args, argsFile);
      VM2C_SRC_FILE.delete();
      VM2C_SRC_FILE.getParentFile().mkdirs();
      env.log(env.info, "[running 'vm2c @" + argsFile + "'...]");
      env.runCommand("runvm2c", new String[] { "@" + argsFile.getPath() });
      VM2C_SRC_FILE.setReadOnly();
    }
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
