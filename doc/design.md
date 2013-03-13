
Current GVM limitations
=======================

  * No 64bit support
    jlong is defined as int32t
    doubles are actually floats

    As a result there is no proper support for java.lang.Long and
    java.lang.Double

  * exit(int status) halts the system (actually a while(1)) and just
    drops the status value.  After all, since GVM is the OS doesn't
    really make sense to report a status to someone.

  * errno is NOT implemented

Design decisions
================

Choosing a JVM
--------------

* Squawk
    -Still active
    -~150k LOC of which 8k are C and 590 are Assembly
    -bare metal ready
* JamVM
    -Last release Jan '10
    -written for embedded
    -mostly interpreter
    -small footprint
    -well structured
    -19391 LOC
* hotspot
    -the official
    -complex
    -posix
    -JIT and Interpreter
    -~450000 LOC
* kaffe
    -Well known
    -Inactive for many years
    -64376 LOC
* CACAO
    -Last release Sept '12
    -JIT
    -Used to be the default for OpenJDK
    -Well documented
    -121175 LOC
* sableVM
    -Claims to be "A robust, clean, easy to maintain and extend,
     extremely portable, efficient, and specification-compliant Java
     virtual machine."
    -3 flavors of threaded interpretation (switched, threaded and
     inlined),
    -bidirectional object layout,
    -spinlock-free thin locks,
    -sparse interface vtables,
    -low-cost maps for precise garbage collection.
    -limited by the current state of the class libraries, and
     occasionally lacks VM support for some class library features.
    -216893 LOC
* llvm-vmkit
    -depends on llvm for JIT
    -complex and big
* jikesrvm
    -java source
    -ant build system
    -requires another JVM to run it
* gcj
    -complex source code
    -no VM