/*
 * Copyright     2015, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
 * Copyright 2003-2008 Sun Microsystems, Inc. All Rights Reserved.
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


/*
 * ceil(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *  Bit twiddling.
 * Exception:
 *  Inexact flag raised if x not equal to ceil(x).
 */

#include "../platform.h"
#include "global.h"

static const double huge_number = 1.0e300;

double JFP_lib_ceil(double x) {
    int i0,i1,j0;
    unsigned i,j;
    i0 =  __HI(x);
    i1 =  __LO(x);
    j0 = ((i0>>20)&0x7ff)-0x3ff;
    if(j0<20) {
        if(j0<0) {  /* raise inexact if x != 0 */
        if(huge_number+x>0.0) {/* return 0*sign(x) if |x|<1 */
            if(i0<0) {i0=0x80000000;i1=0;}
            else if((i0|i1)!=0) { i0=0x3ff00000;i1=0;}
        }
        } else {
        i = (0x000fffff)>>j0;
        if(((i0&i)|i1)==0) return x; /* x is integral */
        if(huge_number+x>0.0) {    /* raise inexact flag */
            if(i0>0) i0 += (0x00100000)>>j0;
            i0 &= (~i); i1=0;
        }
        }
    } else if (j0>51) {
        if(j0==0x400) return x+x;   /* inf or NaN */
        else return x;      /* x is integral */
    } else {
        i = ((unsigned)(0xffffffff))>>(j0-20);
        if((i1&i)==0) return x; /* x is integral */
        if(huge_number+x>0.0) {        /* raise inexact flag */
        if(i0>0) {
            if(j0==20) i0+=1;
            else {
            j = i1 + (1<<(52-j0));
            if(j<i1) i0+=1; /* got a carry */
            i1 = j;
            }
        }
        i1 &= (~i);
        }
    }
    __HI(x) = i0;
    __LO(x) = i1;
    return x;
}
