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
// Abstract      : Formic FPGA MBS block registers
//
// =============================[ CVS Variables ]=============================
// 
// File name     : $RCSfile: mbs_regs.h,v $
// CVS revision  : $Revision: 1.1 $
// Last modified : $Date: 2012/01/27 12:30:15 $
// Last author   : $Author: lyberis-spree $
// 
// ===========================================================================

#ifndef _MBS_REGS_H
#define _MBS_REGS_H

#define MBS_CPU_SYSCALL         ((volatile unsigned int *) 0xFFF00000)
#define MBS_CPU_CONTROL         ((volatile unsigned int *) 0xFFF00004)
#define MBS_CPU_STATUS          ((volatile unsigned int *) 0xFFF00008)
#define MBS_CPU_INTERRUPT       ((volatile unsigned int *) 0xFFF0000C)

#define MBS_ART_ENTRY0          ((volatile unsigned int *) 0xFFF20000)
#define MBS_ART_ENTRY1          ((volatile unsigned int *) 0xFFF20004)
#define MBS_ART_ENTRY2          ((volatile unsigned int *) 0xFFF20008)
#define MBS_ART_ENTRY3          ((volatile unsigned int *) 0xFFF2000C)
#define MBS_ART_ENTRY4          ((volatile unsigned int *) 0xFFF20010)

#define MBS_CHE_CONTROL         ((volatile unsigned int *) 0xFFF40000)
#define MBS_CHE_MAINT           ((volatile unsigned int *) 0xFFF40004)

#define MBS_PRF_IL1             ((volatile unsigned int *) 0xFFF60000)
#define MBS_PRF_DL1             ((volatile unsigned int *) 0xFFF60004)
#define MBS_PRF_IL2             ((volatile unsigned int *) 0xFFF60008)
#define MBS_PRF_DL2             ((volatile unsigned int *) 0xFFF6000C)
#define MBS_PRF_REGS            ((volatile unsigned int *) 0xFFF60010)
#define MBS_PRF_NET_OPS         ((volatile unsigned int *) 0xFFF60014)
#define MBS_PRF_NET_READS       ((volatile unsigned int *) 0xFFF60018)
#define MBS_PRF_NET_WRITES      ((volatile unsigned int *) 0xFFF6001C)
#define MBS_PRF_PACKETS0        ((volatile unsigned int *) 0xFFF60020)
#define MBS_PRF_PACKETS1        ((volatile unsigned int *) 0xFFF60024)
#define MBS_PRF_PACKETS2        ((volatile unsigned int *) 0xFFF60028)
#define MBS_PRF_ENERGY          ((volatile unsigned int *) 0xFFF6002C)
#define MBS_PRF_POWER           ((volatile unsigned int *) 0xFFF60030)

#define MBS_TMR_GLOBAL          ((volatile unsigned int *) 0xFFF80000)
#define MBS_TMR_PRIVATE         ((volatile unsigned int *) 0xFFF80004)

#define MBS_MNI_OPCODE          ((volatile unsigned int *) 0xFFFA0000)
#define MBS_MNI_DMA_SIZE        ((volatile unsigned int *) 0xFFFA0004)
#define MBS_MNI_DST_ADR         ((volatile unsigned int *) 0xFFFA0008)
#define MBS_MNI_BRD_NODE        ((volatile unsigned int *) 0xFFFA000C)
#define MBS_MNI_ACK_ADR         ((volatile unsigned int *) 0xFFFA0010)
#define MBS_MNI_SRC_ADR         ((volatile unsigned int *) 0xFFFA0014)
#define MBS_MNI_MSG0            ((volatile unsigned int *) 0xFFFA0018)
#define MBS_MNI_MSG1            ((volatile unsigned int *) 0xFFFA001C)
#define MBS_MNI_STATUS          ((volatile unsigned int *) 0xFFFA0020)

#define MBS_CNT_INTERRUPT       ((volatile unsigned int *) 0xFFFC0000)
#define MBS_CNT_VAL(x)          ((volatile unsigned int *) \
                                 (0xFFFC1000 + ((x) << 4)))
#define MBS_CNT_NTF_ADR0(x)     ((volatile unsigned int *) \
                                 (0xFFFC1004 + ((x) << 4)))
#define MBS_CNT_NTF_ADR1(x)     ((volatile unsigned int *) \
                                 (0xFFFC1008 + ((x) << 4)))
#define MBS_CNT_NTF_BRD_NODE(x) ((volatile unsigned int *) \
                                 (0xFFFC100C + ((x) << 4)))

#define MBS_MBX_ACCESS          ((volatile unsigned int *) 0xFFFE0000)
#define MBS_MBX_STATUS          ((volatile unsigned int *) 0xFFFE0004)
#define MBS_MSL_ACCESS          ((volatile unsigned int *) 0xFFFF0000)
#define MBS_MSL_STATUS          ((volatile unsigned int *) 0xFFFF0004)

#endif
