DEPENDENCIES
============

TODO

BUILD
=====

Microblaze
----------
  build-mb.properties contains the configuration of the microblaze
  builds

  To create all needed files execute

    make -f mb.mk

  To deploy on Formic 

    make -f mb.mk run

X86
---
  build-x86.properties the configuration of the x86 builds

  To build (the JVM) and run the current application execute

    make -f x86.mk

OPTIONS
-------
  To change the application you want to build define `APP`. For
  example

    make -f x86.mk "APP=../formic-tests/HelloWorld"
