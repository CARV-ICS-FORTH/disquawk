ERRORS
======

#1: Trying to create a 64-bit value from two 32-bit values (A << 32 | B).
    However, we only support 32-bit long values, resulting in A beeing
    dropped.

#2: We don't support files of any type, thus you can't get a file's
    size.

#3: We don't support files of any type, thus you can't read a file.

#4:


WARNINGS
========

#1: int getHi(jlong value) invocation. Since jlong is 32-bit there is
    no meaning in calling this function.
