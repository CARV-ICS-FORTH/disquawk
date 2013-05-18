//===-- lib/extendsfdf2.c - single -> double conversion -----------*- C -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements a fairly generic conversion from a narrower to a wider
// IEEE-754 floating-point type.  The constants and types defined following the
// includes below parameterize the conversion.
//
// This routine can be trivially adapted to support conversions from
// half-precision or to quad-precision. It does not support types that don't
// use the usual IEEE-754 interchange formats; specifically, some work would be
// needed to adapt it to (for example) the Intel 80-bit format or PowerPC
// double-double format.
//
// Note please, however, that this implementation is only intended to support
// *widening* operations; if you need to convert to a *narrower* floating-point
// type (e.g. double -> float), then this routine will not do what you want it
// to.
//
// It also requires that integer types at least as large as both formats
// are available on the target platform; this may pose a problem when trying
// to add support for quad on some 32-bit systems, for example.  You also may
// run into trouble finding an appropriate CLZ function for wide source types;
// you will likely need to roll your own on some platforms.
//
// Finally, the following assumptions are made:
//
// 1. floating-point types and integer types have the same endianness on the
//    target platform
//
// 2. quiet NaNs, if supported, are indicated by the leading bit of the
//    significand field being set
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

typedef float src_t2;
typedef uint32_t src_rep_t2;
#define SRC_REP_C2 UINT32_C
static const int srcSigBits2 = 23;
#define src_rep_t_clz __builtin_clz

typedef double dst_t2;
typedef uint64_t dst_rep_t2;
#define DST_REP_C2 UINT64_C
static const int dstSigBits2 = 52;

// End of specialization parameters.  Two helper routines for conversion to and
// from the representation of floating-point data as integer values follow.

static inline src_rep_t2 srcToRep2(src_t2 x) {
    const union { src_t2 f; src_rep_t2 i; } rep = {.f = x};
    return rep.i;
}

static inline dst_t2 dstFromRep2(dst_rep_t2 x) {
    const union { dst_t2 f; dst_rep_t2 i; } rep = {.i = x};
    return rep.f;
}

// End helper routines.  Conversion implementation follows.

ARM_EABI_FNALIAS(f2d, extendsfdf2)

dst_t2 __extendsfdf2(src_t2 a) {

    // Various constants whose values follow from the type parameters.
    // Any reasonable optimizer will fold and propagate all of these.
    const int srcBits = sizeof(src_t2)*CHAR_BIT;
    const int srcExpBits = srcBits - srcSigBits2 - 1;
    const int srcInfExp = (1 << srcExpBits) - 1;
    const int srcExpBias = srcInfExp >> 1;

    const src_rep_t2 srcMinNormal = SRC_REP_C2(1) << srcSigBits2;
    const src_rep_t2 srcInfinity = (src_rep_t2)srcInfExp << srcSigBits2;
    const src_rep_t2 srcSignMask = SRC_REP_C2(1) << (srcSigBits2 + srcExpBits);
    const src_rep_t2 srcAbsMask = srcSignMask - 1;
    const src_rep_t2 srcQNaN = SRC_REP_C2(1) << (srcSigBits2 - 1);
    const src_rep_t2 srcNaNCode = srcQNaN - 1;

    const int dstBits = sizeof(dst_t2)*CHAR_BIT;
    const int dstExpBits = dstBits - dstSigBits2 - 1;
    const int dstInfExp = (1 << dstExpBits) - 1;
    const int dstExpBias = dstInfExp >> 1;

    const dst_rep_t2 dstMinNormal = DST_REP_C2(1) << dstSigBits2;

    // Break a into a sign and representation of the absolute value
    const src_rep_t2 aRep = srcToRep2(a);
    const src_rep_t2 aAbs = aRep & srcAbsMask;
    const src_rep_t2 sign = aRep & srcSignMask;
    dst_rep_t2 absResult;

    if (aAbs - srcMinNormal < srcInfinity - srcMinNormal) {
        // a is a normal number.
        // Extend to the destination type by shifting the significand and
        // exponent into the proper position and rebiasing the exponent.
        absResult = (dst_rep_t2)aAbs << (dstSigBits2 - srcSigBits2);
        absResult += (dst_rep_t2)(dstExpBias - srcExpBias) << dstSigBits2;
    }

    else if (aAbs >= srcInfinity) {
        // a is NaN or infinity.
        // Conjure the result by beginning with infinity, then setting the qNaN
        // bit (if needed) and right-aligning the rest of the trailing NaN
        // payload field.
        absResult = (dst_rep_t2)dstInfExp << dstSigBits2;
        absResult |= (dst_rep_t2)(aAbs & srcQNaN) << (dstSigBits2 - srcSigBits2);
        absResult |= aAbs & srcNaNCode;
    }

    else if (aAbs) {
        // a is denormal.
        // renormalize the significand and clear the leading bit, then insert
        // the correct adjusted exponent in the destination type.
        const int scale = src_rep_t_clz(aAbs) - src_rep_t_clz(srcMinNormal);
        absResult = (dst_rep_t2)aAbs << (dstSigBits2 - srcSigBits2 + scale);
        absResult ^= dstMinNormal;
        const int resultExponent = dstExpBias - srcExpBias - scale + 1;
        absResult |= (dst_rep_t2)resultExponent << dstSigBits2;
    }

    else {
        // a is zero.
        absResult = 0;
    }

    // Apply the signbit to (dst_t2)abs(a).
    const dst_rep_t2 result = absResult | (dst_rep_t2)sign << (dstBits - srcBits);
    return dstFromRep2(result);
}
