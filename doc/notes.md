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


                                   Board info
================================================================================

My Board:
---------
  board BID
  up    0x10
  down  0x00
  RESET: first the lower then the upper board

Core-IDs = 0-7  --> 0x000-0x111
DRAM = 0xC
BRD_CTL = 0xF

Communication:
--------------
  Messages up to 2 words
  DMA

Serial port:
------------
  we use minicom
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


                                     squawk
================================================================================

Read http://java.net/projects/squawk/pages/SquawkBuildOptions
Modify the build.properties file

To build the builder:
---------------------
    cd builder; ./bld.sh

Checkout and build myrmics in vmcore/src/rts/formic/myrmics

To build Squawk:
----------------
    ./d -prod -mac 2>&1
    ./d -prod -mac vm2c 2>&1
    ./d -prod -mac -comp:formic buildFormicVM 2>&1

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

