// ===========================================================================
//
//                              FORTH-ICS / CARV
//
//                        Proprietary and confidential
//                           Copyright (c) 2010-2012
//
//
// ==========================[ Static Information ]===========================
//
// Author        : Spyros Lyberis
// Abstract      : Global types
//
// =============================[ CVS Variables ]=============================
// 
// File name     : $RCSfile: types.h,v $
// CVS revision  : $Revision: 1.12 $
// Last modified : $Date: 2013/02/04 06:48:15 $
// Last author   : $Author: lyberis-home $
// 
// ===========================================================================

#ifndef TYPES_H
#define TYPES_H

#ifndef NULL
#define NULL    0
#endif

#define INT_MAX  0x7FFFFFFF
#define UINT_MAX 0xFFFFFFFF
#define FLT_MAX  3.402823466e+38F

typedef unsigned int size_t;

typedef unsigned int rid_t;

typedef void (*func_t)();       // Generic function type


#define ERR_GENERIC             ( -(1 <<  0) )
#define ERR_MISALIGNED          ( -(1 <<  1) )
#define ERR_OUT_OF_RANGE        ( -(1 <<  2) )
#define ERR_NOT_ALLOCED         ( -(1 <<  3) )
#define ERR_OUT_OF_MEMORY       ( -(1 <<  4) )
#define ERR_NO_SUCH_REGION      ( -(1 <<  5) )
#define ERR_OUT_OF_RIDS         ( -(1 <<  6) )
#define ERR_COMMUNICATION       ( -(1 <<  7) )
#define ERR_REPLY_POSTPONED     ( -(1 <<  8) )
#define ERR_BOUNDARY_REGION     ( -(1 <<  9) )
#define ERR_REMOTE_CHILDREN     ( -(1 << 10) )
#define ERR_REMOTE_PARENT       ( -(1 << 11) )
#define ERR_NOTHING_TO_PACK     ( -(1 << 12) )
#define ERR_OUT_OF_COUNTERS     ( -(1 << 13) )

#endif
