/*
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


/* __kernel_sin( x, y, iy)
 * kernel sin function on [-pi/4, pi/4], pi/4 ~ 0.7854
 * Input x is assumed to be bounded by ~pi/4 in magnitude.
 * Input y is the tail of x.
 * Input iy indicates whether y is 0. (if iy=0, y assume to be 0).
 *
 * Algorithm
 *  1. Since sin(-x) = -sin(x), we need only to consider positive x.
 *  2. if x < 2^-27 (hx<0x3e400000 0), return x with inexact if x!=0.
 *  3. sin(x) is approximated by a polynomial of degree 13 on
 *     [0,pi/4]
 *                   3            13
 *      sin(x) ~ x + S1*x + ... + S6*x
 *     where
 *
 *  |sin(x)         2     4     6     8     10     12  |     -58
 *  |----- - (1+S1*x +S2*x +S3*x +S4*x +S5*x  +S6*x   )| <= 2
 *  |  x                               |
 *
 *  4. sin(x+y) = sin(x) + sin'(x')*y
 *          ~ sin(x) + (1-x*x/2)*y
 *     For better accuracy, let
 *           3      2      2      2      2
 *      r = x *(S2+x *(S3+x *(S4+x *(S5+x *S6))))
 *     then                   3    2
 *      sin(x) = x + (S1*x + (x *(r-y/2)+y))
 */

#ifdef higher_degree
/* The code is modified to calculate the sine using taylors series of
 * degree 25 */
#endif

#include "../platform.h"
#include <global.h>

static const double
S1  = -1.66666666666666666666e-01, /* -1/3!  0xBFC55555, 0x55555549 */
S2  =  8.33333333333333333333e-03, /*  1/5!  0x3F811111, 0x1110F8A6 */
S3  = -1.98412698412698412698e-04, /* -1/7!  0xBF2A01A0, 0x19C161D5 */
S4  =  2.75573192239858906525e-06, /*  1/9!  0x3EC71DE3, 0x57B1FE7D */
S5  = -2.50521083854417187750e-08, /* -1/11! 0xBE5AE5E6, 0x8A2B9CEB */
S6  =  1.60590438368216145993e-10, /*  1/13! 0x3DE5D93A, 0x5ACFD57C */
#ifdef higher_degree
S7  = -7.64716373181981647590e-13, /* -1/15! */
S8  =  2.81145725434552076319e-15, /*  1/17! */
S9  = -8.22063524662432971695e-18, /* -1/19! */
S10 =  1.95729410633912612308e-20, /*  1/21! */
S11 = -3.86817017063068403771e-23, /* -1/23! */
S12 =  6.44695028438447339619e-26, /*  1/25! */
#endif
half =  5.00000000000000000000e-01; /* 0x3FE00000, 0x00000000 */

double __kernel_sin(double x, double y, int iy) {
    double z,r,v;
    int ix;
    ix = __HI(x)&0x7fffffff;    /* high word of x */
    if(ix<0x3e400000)           /* |x| < 2**-27 */
       {if((int)x==0) return x;}        /* generate inexact */
    z   =  x*x;
    v   =  z*x;
#ifdef higher_degree
    r   =  S2+z*(S3+z*(S4+z*(S5+z*(S6+z*(S7+z*(S8+z*(S9+z*(S10+z*(S11+z*S12)))))))));
#else
    r   =  S2+z*(S3+z*(S4+z*(S5+z*S6)));
#endif
    if(iy==0) return x+v*(S1+z*r);
    else      return x-((z*(half*y-v*r)-y)-v*S1);
}
