                                  Xilinx Tools
================================================================================

Software to download:
---------------------
  ISE Design Suite - 12.4 Full Product Installation
  (Xilinx_ISE_DS_Lin_12.4_M.81d.2.0.tar)

Install:
--------
  sudo ./xsetup

For the usb:
------------
  http://www.george-smart.co.uk/wiki/Xilinx_JTAG_Linux
  Alternatives:
    #http://www.petalogix.com/support/kb/xilinx_ubuntu_install
    #http://forums.xilinx.com/t5/Installation-and-Licensing/installing-platform-cable-USB-II-ubuntu/td-p/66729
    #http://rmdir.de/~michael/xilinx/

LICENSE:
--------
  27000@carvouno.ics.forth.gr

Scripts to source:
------------------
  . /opt/Xilinx/12.4/ISE_DS/EDK/settings64.sh /opt/Xilinx/12.4/ISE_DS/EDK
  . /opt/Xilinx/12.4/ISE_DS/ISE/settings64.sh /opt/Xilinx/12.4/ISE_DS/ISE
  #. /opt/Xilinx/12.4/ISE_DS/PlanAhead/settings64.sh /opt/Xilinx/12.4/ISE_DS/PlanAhead
  . /opt/Xilinx/12.4/ISE_DS/common/settings64.sh /opt/Xilinx/12.4/ISE_DS/common


                                     Formic
================================================================================

Caches:
-------
  Instruction Cache Level 1:
    * IL1 is 4-KB, two-way set associative, no write support
  Data Cache Level 1:
    * DL1 is 8-KB, two-way set associative, write through
    * Forwards without storing any access that is marked non-cacheable
      (C=0) or I/O (I=1). Also, everything is forwarded when the block
      is disabled.
    * Provides a serial clear operation, where all entries are
      invalidated in both ways
    * Provides an invalidation interface to L2 cache, where a single
      entry is invalidated if it exists in the DL1 cache
  Level 2 Cache:
    * 8 ways. Each way has 8192 words of 32 bits = 512 cache lines of
      64 bytes. Total size 8 x 512 x 64 = 256 KB.
    * An LRU policy is also supported, using 3 bits per tag.
    * The L2 data are stored in the off-FPGA SRAM through one of the
      ports of the SRAM CTL block.

Acronyms:
---------
  MBS      = MicroBlaze Slice
    (aka core, L1, L2 counters, mailbox and network engines)
  UART     = Universal Asynchronous Receiver/Transmitter
  XBAR     = Crossbar
  LUT      = LookUp Table
  ART      = Address Region Table block
  MNI      = MBS Netwrork Interface
  CTL      = Control block
  CMX      = Counters & Mailbox block
  XBI      = Crossbar Interface block
  VC       = Virtual Channel
  SRAM_CTL = SRAM Controller block
  TLB      = Translation Lookaside Buffer
  GTP      = Gigabit Transceiver Port
  BCTL     = Formic Board Controller block


                                  Formicarium
================================================================================

  Always tail /home/lyberis/measure.log and /home/lyberis/server.log

  UART board is 0x1C

                                   Board info
================================================================================

My Board:
---------
  board BID
  up    0x010
  down  0x000
  RESET: first the lower then the upper board

  Core-IDs = 0-7  --> 0x000-0x111
  DRAM = 0xC
  BRD_CTL = 0xF

Specs:
------
  8 cores per board
  128MB DRAM (for the whole board)
  22-port crossbar
  5-port TLB

Communication:
--------------
  Messages up to 2 words
  DMA

Serial port:
------------
  We use minicom
  ttyS0
  no flow control
  38400 rate


                                Get it on board
================================================================================

How to download and run a microblaze elf:
-----------------------------------------
  xmd -tcl xmd.tcl

xmd.tcl contents:
-----------------
  connect mb mdm -cable type xilinx_platformusb frequency 1500000 -debugdevice deviceNr 1 cpunr 1
  stop
  dow foo.elf
  run

GDB:
----
  Open xmd (connect, stop, dow, don't run but stop)
  mb-gdb foo.elf (target remote localhost:1234)


                                     squawk
================================================================================

Read http://java.net/projects/squawk/pages/SquawkBuildOptions
Modify the build.properties file

Notes:
------
  * Java strings are not NULL terminated (they rely on .length)
  * The VM is started from squawk.c[.spp]
  * Bootstrap suite is loaded by loadBootstrapSuiteFromFlash in
    vmcore/src/vm/suite.c
  * To add a native method one must:
    1. Add 'throws NativePragma' to his function 
    2. Probably edit vmcore/src/vm/bytecodes.c[.spp] to implement it
       (or proxy it)
    3. run 'make -f mb.mk natives_gen'
  * GC is temporarily disabled, to enable it change the Files
    1. cldc/src/com/sun/squawk/ServiceOperation.java
    2. cldc/src/com/sun/squawk/VM.java
    3. cldc/src/com/sun/squawk/VMThread.java
  * Memory allocation is implemented in the following files;
    1. vmcore/src/vm/memory.c[.spp]
  * To get the class size (in bytes) one can see the *.map at the end
    (Class statistics)
  * To see the instance size of each class (in words) one must:
    1. uncomment the printlns in decodeInstance() in
    mapper/src/com/sun/squawk/ObjectMemoryMapper.java
    2. make -f mb.mk map | tee temp.out
    3. egrep '^class [a-zA-Z]' temp.out | sort | uniq
  * Shift operations are using barrel-shift and take 1 or 2 cycles
    (depending on the microblaze configuration)

To build the builder:
---------------------
    cd builder; ./bld.sh

Checkout and build myrmics in vmcore/src/rts/formic/myrmics

To build Squawk:
----------------
    ./d -prod -mac 2>&1
    ./d -prod -mac -comp:formic buildFormicVM 2>&1

Open Questions:
---------------
    What about Isolate???
    What about Real Time???

                                    Myrmics
================================================================================

To build:
---------
    make java

Notes:
------
  arch/mb/init.c sets permissions
  arch/mb/boot.s clears bss


                                 GNU classpath
================================================================================

Notes:
------
  * classpath 0.99 fails in configuration
  * thus using 0.98
  * I also had to unzip glibj.zip, because we don't want zip support in JamVM
  * Note that GNU classpath does not support out of source compilation (not in
    my case at least)

Configuration:
--------------
  ./configure\
    --disable-gtk-peer\
    --disable-gconf-peer\
    --disable-examples\
    --disable-tools\
    --disable-gjdoc\
    --disable-alsa\
    --disable-dssi\
    --disable-plugin\
    --disable-Werror\
    --without-x\
    --prefix=/home/user/classpath_build


                                     JamVM
================================================================================

Current version: 1.5.4
----------------------

Configuration:
--------------
  ./configure\
    --disable-int-threading\
    --disable-int-direct\
    --disable-int-caching\
    --disable-int-prefetch\
    --disable-int-inlining\
    --disable-zip\
    --disable-ffi\
    --with-classpath-install-dir=/home/user/classpath_build\
    --prefix=/home/user/jamvm_build


                                   LLVM/VMKIT
================================================================================

Building llvm:
--------------
  ../../spare/llvm/configure\
    --enable-optimized\
    --disable-jit\
    --enable-doxygen\
    --disable-threads

Configure vmkit:
----------------
  ../../spare/llvm/projects/vmkit/configure\
    --with-gnu-classpath-libs=/home/zakkak/spare/llvm/projects/classpath-0.97.2/lib\
    --with-gnu-classpath-glibj=/home/zakkak/spare/llvm/projects/classpath-0.97.2/lib/glibj.zip\
    --with-llvmsrc=/home/zakkak/spare/llvm/\
    --with-llvmobj=/home/zakkak/mblaze/llvm\
    --with-mmtk-plan=org.mmtk.plan.marksweep.MS


Makefile mods:
--------------
  -vmkit/Makefile.common.in
    Verb= (to make it print the commands)
    LIBS := $(filter-out -lpthread,$(LIBS)) (to remove pthreads)
    CXX.Flags += -ggdb
