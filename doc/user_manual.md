# Configure, Build, Deploy #

## DEPENDENCIES ##

In order to build Squawk you need
 * Xilinix Tools (only for deploying on the Formic board: see details bellow)
 * JRE 1.5 or later
 * git
 * GNU make
 * awk
 * sed
 * wc (wordcount)
 * od (objectdump)

### BUILD ###

#### on Formic board (Microblaze) ####
  build-mb.properties contains the configuration of the microblaze
  builds

  To create all needed files execute

    make -f mb.mk

  To deploy on the Formic-Mesh (Note: your IP must be allowed to
  access formicarium)

    make -f mb.mk run

  ***Caution:*** When running on Formicarium always tail
  /home/lyberis/measure.log and /home/lyberis/server.log to monitor
  the logs and make sure the mesh shuts down when you finish.

#### on X86 ####
  build-x86.properties contains the configuration of the x86 builds

  To build (the JVM) and run the current application execute

    make -f x86.mk

#### OPTIONS ####
  To change the application you want to build define `APP`. For
  example

    make -f mb.mk "APP=../formic-tests/HelloWorld"
    make -f mb.mk "APP=../formic-tests/HelloWorld" run

### Xilinx Tools ###

#### Download and extract: ####
  [ISE Design Suite - 12.4 Full Product Installation](http://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/design-tools/v12_4.html)
  tar xvf Xilinx_ISE_DS_Lin_12.4_M.81d.2.0.tar

#### Install: ####
  sudo ./xsetup

#### LICENSE: ####
  27000@carvouno.ics.forth.gr

#### Scripts to source: ####
  . /opt/Xilinx/12.4/ISE_DS/EDK/settings64.sh /opt/Xilinx/12.4/ISE_DS/EDK
  . /opt/Xilinx/12.4/ISE_DS/ISE/settings64.sh /opt/Xilinx/12.4/ISE_DS/ISE
  #. /opt/Xilinx/12.4/ISE_DS/PlanAhead/settings64.sh /opt/Xilinx/12.4/ISE_DS/PlanAhead
  . /opt/Xilinx/12.4/ISE_DS/common/settings64.sh /opt/Xilinx/12.4/ISE_DS/common
  
  It is recommended to avoid adding these lines in your .bash_profile
  or .bashrc

#### For the usb: ####
  If you are planning to deploy on a Formic-Board using your usb port
  you will need to follow the following tutorial.
  
  http://www.george-smart.co.uk/wiki/Xilinx_JTAG_Linux
  Alternatives:
    #http://www.petalogix.com/support/kb/xilinx_ubuntu_install
    #http://forums.xilinx.com/t5/Installation-and-Licensing/installing-platform-cable-USB-II-ubuntu/td-p/66729
    #http://rmdir.de/~michael/xilinx/

# Known issues, Limitations #

  * Float autoboxing in throwException (breaks preverify)
  * errno is **NOT** implemented

  * GC=com.sun.squawk.Lisp2GenerationalCollector is not working on Formic
  * GC=com.sun.squawk.Lisp2Collector is not working on Formic

# Design #

## Formic ##

### DRAM mapping ###
  Code starts at 0x00 and is 10MB long
  User space is 84MB

## Squawk ##

  We modified Squawk's toolchain to use
  [retrotranslator](http://retrotranslator.sourceforge.net/) to be
  able to compile 1.5 source code to valid 1.4 classes.
