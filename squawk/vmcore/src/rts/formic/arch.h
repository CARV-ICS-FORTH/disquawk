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
// Abstract      : Architecture-specific primitives
//
// =============================[ CVS Variables ]=============================
//
// File name     : $RCSfile: arch.h,v $
// CVS revision  : $Revision: 1.49 $
// Last modified : $Date: 2013/01/25 16:52:05 $
// Last author   : $Author: lyberis-spree $
//
// ===========================================================================

#ifndef _ARCH_H
#define _ARCH_H

#define ARCH_MB __MICROBLAZE__
#define MYRMICS

#include <mbs_regs.h>
#include <ars_regs.h>
#include <formic_regs.h>
#include <memory_management.h>


// ===========================================================================
// Configuration definitions. Change the below to match how many Formic and
// ARM boards you have connected, what links to check, where's the Formic
// UART etc.
// ===========================================================================

// AR_BOOT_MASTER_BID decides who is responsible to boot the Formic boards
// (transfer the code and wake them up). If ARM boards are active, ARM0 should
// be the master. Download the *.arm.elf to its Flash and boot the master ARM
// from it. If it's a Formic board, download the *.mb.elf using Xilinx XMD
// scripts and reset the master MicroBlaze core from it.
//
// AR_ARM0_BID and AR_ARM1_BID decide whether ARM0 and ARM1 are active (and
// their board ID dip switches). Note that when 1 ARM board is active, this
// should be ARM0.
//
// For the above restrictions to be met, there are only 3 options (select
// one of the configurations below): Formic cube alone, Formic cube + ARM0, or
// Formic cube + ARM0 + ARM1.

#if 1 // Formic cube only
#define AR_BOOT_MASTER_BID              0x00    // 0x6B = ARM0, 0x00 = Formic
#define AR_ARM0_BID                     -1      // 0x6B = ARM0, -1 = inactive
#define AR_ARM1_BID                     -1      // 0x4B = ARM1, -1 = inactive
#endif

#if 0 // Formic cube + ARM0
#define AR_BOOT_MASTER_BID              0x6B    // 0x6B = ARM0, 0x00 = Formic
#define AR_ARM0_BID                     0x6B    // 0x6B = ARM0, -1 = inactive
#define AR_ARM1_BID                     -1      // 0x4B = ARM1, -1 = inactive
#endif

#if 0 // Formic cube + ARM0 + ARM1
#define AR_BOOT_MASTER_BID              0x6B    // 0x6B = ARM0, 0x00 = Formic
#define AR_ARM0_BID                     0x6B    // 0x6B = ARM0, -1 = inactive
#define AR_ARM1_BID                     0x4B    // 0x4B = ARM1, -1 = inactive
#endif


// For Formic printing, which board's UART is really connected
#define AR_FORMIC_UART_BID              0x1C

// Which Formic boards are in the cube, how many cores to wake up, and their
// expected board type and version
#define AR_FORMIC_CORES_PER_BOARD       8

#define AR_FORMIC_MIN_X                 0
#define AR_FORMIC_MIN_Y                 0
#define AR_FORMIC_MIN_Z                 0
#define AR_FORMIC_MAX_X                 3
#define AR_FORMIC_MAX_Y                 3
#define AR_FORMIC_MAX_Z                 3

#define AR_FORMIC_BOARD_TYPE            3       // 3 = formic_m8g8
#define AR_FORMIC_BOARD_VERSION         230

// Which ARM cores are active and where ARM boards are connected to Formic cube
#define AR_ARM0_CORES_PER_BOARD         4

#define AR_FORMIC_ARM0_MIN_X            2
#define AR_FORMIC_ARM0_MAX_X            3
#define AR_FORMIC_ARM0_Y                2
#define AR_FORMIC_ARM0_Z                3

#define AR_ARM1_CORES_PER_BOARD         4

#define AR_FORMIC_ARM1_MIN_X            0
#define AR_FORMIC_ARM1_MAX_X            1
#define AR_FORMIC_ARM1_Y                2
#define AR_FORMIC_ARM1_Z                3

// Is a XUP board active and which Formic board is connected to it
#define AR_XUP_BID                      -1      // 0x57 = XUP, -1 = inactive
#define AR_FORMIC_XUP_X                 1
#define AR_FORMIC_XUP_Y                 1
#define AR_FORMIC_XUP_Z                 3

// Board IDs which have this bit set are ARM boards. This is the W
// dimension bit. The W dimension is also used to communicate to the
// XUP board, although it does not contain an ARM processor.
#define ARCH_ARM_BOARD_ID_BIT   (1 << 6)


// ===========================================================================
// Other arch-related definitions
// ===========================================================================

// Mailbox size in bytes
#define AR_MBOX_SIZE            4096

// Allowed DMA alignment and max DMA size
#define AR_DMA_ALIGN            64
#define AR_DMA_MAX_SIZE         1048576


// ===========================================================================
// CPU & Caches
// ===========================================================================

#ifdef ARCH_ARM
extern int ar_get_board_id();
extern int ar_get_core_id();
#endif

#ifdef __MICROBLAZE__
static inline int ar_get_board_id() {
  return (*MBS_CPU_STATUS >> 24);
}

static inline int ar_get_core_id() {
  return (*MBS_CPU_STATUS & 0x7);
}
#endif


#ifdef ARCH_ARM
extern void ar_invalidate_caches();

extern void ar_disable_icache();
extern void ar_enable_icache();

extern void ar_disable_dcache();
extern void ar_enable_dcache();

extern void ar_disable_branch_pred();
extern void ar_enable_branch_pred();

extern void ar_enable_fpu();
#endif


#ifdef __MICROBLAZE__

#define AR_CACHE_EPOCH_CPU_WAYS    4   // CPU current epoch ways (0-7)
#define AR_CACHE_NUM_EPOCHS        8   // Number of L2 epochs

extern void ar_cache_enable(int epoch_mode);
extern void ar_cache_disable();
extern void ar_cache_flush();

static inline void ar_cache_set_epoch(int epoch) {
  *MBS_CHE_CONTROL = (AR_CACHE_EPOCH_CPU_WAYS << 24) | // CPU ways
                     (epoch << 16) |                   // Epoch number
                     (0 << 4)  |                       // LRU
                     (1 << 0);                         // Enable

}

extern void ar_art_install_region(int region, unsigned int base,
                                  unsigned int size, int cacheable,
                                  int read_only, int executable,
                                  int user_accessible,
                                  int privileged_accessible);
extern void ar_art_uninstall_region(int region);
#endif


// ===========================================================================
// Interrupts
// ===========================================================================

#ifdef __MICROBLAZE__
static inline unsigned int ar_intr_cpu_get() {
  return *MBS_CPU_INTERRUPT;
}

static inline void ar_intr_cpu_set(unsigned int value) {
  *MBS_CPU_INTERRUPT = value;
}

static inline unsigned int ar_intr_status_get() {
  return ((*MBS_CPU_STATUS >> 8) & 0xFFFF);
}

static inline unsigned int ar_intr_cnt_get() {
  return *MBS_CNT_INTERRUPT;
}

static inline void ar_intr_cnt_set(unsigned int value) {
  *MBS_CNT_INTERRUPT = value;
}
#endif


// ===========================================================================
// UART
// ===========================================================================
extern void ar_uart_flush();
extern void ar_uart_send_char(int c);
extern void ar_uart_send_str(char *s);

extern void ar_uart_send_hex(unsigned int hex);
extern void ar_uart_send_char_hex(unsigned char hex);
extern void ar_uart_send_dec(unsigned int dec);


// ===========================================================================
// Panic & assertions
// ===========================================================================
extern void __attribute__ ((noreturn)) ar_panic(char *msg);
extern void __attribute__ ((noreturn)) ar_halt();

#define ar_abort() {\
                     char _ar_panic_buf[1024]; \
                     kt_sprintf(_ar_panic_buf, \
                                "Internal error: %s line %d", \
                                __FILE__, __LINE__); \
                         ar_panic(_ar_panic_buf); \
                     }

#define ar_assert(c) {\
                       if (!(c)) {\
                         char _ar_panic_buf[1024]; \
                         kt_sprintf(_ar_panic_buf, \
                                    "Assertion failed: %s line %d", \
                                    __FILE__, __LINE__); \
                         ar_panic(_ar_panic_buf); \
                       }\
                     }


// ===========================================================================
// Math
// ===========================================================================

// Division
extern void ar_uint_divide(unsigned int num, unsigned int div,
                           unsigned int *quot, unsigned int *rem);
extern void ar_int_divide(int num, int div, int *quot, int *rem);

// GCC-compatible integer division primitives
#ifdef ARCH_ARM
extern int __aeabi_idiv(int a, int b);
extern int __aeabi_idivmod(int a, int b);
extern unsigned int __aeabi_uidiv(unsigned int a, unsigned int b);
extern unsigned int __aeabi_uidivmod(unsigned int a, unsigned int b);
#endif
#ifdef __MICROBLAZE__
extern int __divsi3(int a, int b);
extern int __modsi3(int a, int b);
extern unsigned int __udivsi3(unsigned int a, unsigned int b);
extern unsigned int __umodsi3(unsigned int a, unsigned int b);
#endif

// GCC-compatible dummy float <-> double conversion for MicroBlaze
#ifdef __MICROBLAZE__
extern float __truncdfsf2(volatile double d);
extern double __extendsfdf2(volatile float f);
#endif


// Square root
extern float ar_float_sqrt(float num);

// Power
extern float ar_float_pow(float x, float y);


// ffs: returns one plus the index of the least significant 1-bit of x,
// or if x is zero, returns zero.
static inline int ar_ffs(unsigned int x) {
#ifdef ARCH_ARM
  return __builtin_ffs(x);
#endif
#ifdef __MICROBLAZE__
  int i;
  if (!x) {
    return 0;
  }
  for (i = 1; i < 33; i++) {
    if (x & 1) {
      return i;
    }
    x >>= 1;
  }
  return 0;
#endif
}


// ===========================================================================
// Timers. ARM and Formic architectures have a different set of timers, so
// we provide here a separated lower-level interface first, followed by
// a unified higher-level interface which hides some of the unique
// functionalities that can be exploited by the lower-level interface.
// ===========================================================================

// ===== Separated low-level interface ==================================
#ifdef ARCH_ARM
// Private timer (1 tick = 2 clock cycles = 200 MHz = 5 ns)
extern void         ar_timer0_set(unsigned int value);
extern unsigned int ar_timer0_read();
extern void         ar_timer0_busy_wait(unsigned int msec);

// Watchdog timer (1 tick = 2 clock cycles = 200 MHz = 5 ns)
extern void         ar_timer1_set(unsigned int value);
extern unsigned int ar_timer1_read();
extern void         ar_timer1_busy_wait(unsigned int msec);

// Board global timer (1 tick = 40 clock cycles = 10 MHz = 100 ns)
extern unsigned int ar_timer2_read();
#endif

#ifdef __MICROBLAZE__
// Private timer (1 tick = 1 clock cycle = 10 MHz = 100 ns)
extern void         ar_priv_timer_set(unsigned int value);
extern unsigned int ar_priv_timer_read();
extern void         ar_priv_timer_busy_wait(unsigned int msec);

// Board global timer (1 tick = 1 clock cycle = 10 MHz = 100 ns)
extern unsigned int ar_glob_timer_read();
#endif

// ===== Unified high-level interface ===================================

// First timer: can be started/stopped, 1 tick is either 2 CPU clock cycles
//              (ARM) or 1 CPU clock cycle (MB). The msec() and cycles()
//              routines automatically translate the values correctly in
//              an arch-specific way to refer respectively to milliseconds
//              and CPU-specific clock cycles.
extern void         ar_timer_reset();
extern unsigned int ar_timer_get_ticks();
extern unsigned int ar_timer_get_cycles();
extern unsigned int ar_timer_get_msec();
extern void         ar_timer_busy_wait_ticks(unsigned int ticks);
extern void         ar_timer_busy_wait_cycles(unsigned int cycles);
extern void         ar_timer_busy_wait_msec(unsigned int msec);

// Second timer: it is free running (cannot be started/stopped) and wraps
//               around. 1 tick is always 100 ns, i.e. either 40 CPU
//               clock cycles (ARM) or 1 CPU clock cycle (MB). The cycles()
//               routine automatically translates the value correctly in
//               an arch-specific way to refer to CPU-specific clock cycles.
extern unsigned int ar_free_timer_get_ticks();
extern unsigned int ar_free_timer_get_cycles();


// ===========================================================================
// Address translation functions.
//
// When we don't know at compile-time if a remote register is ARM or Formic,
// we supply here macros that translate MBS or ARS registers to an address, so
// it can be used in other arch-related functions. The runtime translation is
// based on the board ID.
// ===========================================================================

#define ar_addr_of_mbox(bid, cid)     ( ((bid) & ARCH_ARM_BOARD_ID_BIT) ? \
                                          (int) ARS_MBX_ACCESS(cid) : \
                                          (int) MBS_MBX_ACCESS )

#define ar_addr_of_mslot(bid, cid)    ( ((bid) & ARCH_ARM_BOARD_ID_BIT) ? \
                                          (int) ARS_MSL_ACCESS(cid) : \
                                          (int) MBS_MSL_ACCESS )

#define ar_addr_of_cnt(bid, cid, cnt) ( ((bid) & ARCH_ARM_BOARD_ID_BIT) ? \
                                          (int) ARS_CNT_VAL((cid), (cnt)) : \
                                          (int) MBS_CNT_VAL(cnt) )

// 1-1 virtual address translations for ARM arch. Formic uses virtual-based
// DMAs, so this is not necessary.
static inline unsigned int ar_virt_to_phys(int bid, unsigned int virt) {
  if ((bid & ARCH_ARM_BOARD_ID_BIT)) {
    if ((virt >= MM_ARM_VA_KERNEL_BASE) &&
        (virt < MM_ARM_VA_KERNEL_BASE + 4 * MM_ARM_KERNEL_SIZE)) {
      return virt + (MM_ARM_PA_KERNEL_BASE - MM_ARM_VA_KERNEL_BASE);
    }
#ifdef MYRMICS
    // Myrmics also has a User space
    else if ((virt >= MM_ARM_VA_USER_BASE) &&
             (virt <  MM_ARM_VA_USER_BASE + MM_ARM_USER_SIZE)) {
      return virt + (MM_ARM_PA_USER_BASE - MM_ARM_VA_USER_BASE);
    }
#endif
    else {
      // Non-kernel and non-user addresses are left untranslated, e.g.
      // ARS pages, ARM peripherals, etc. Change this here if their
      // virtual addresses are not the same as the physical.
      if ((virt > MM_ARM_VA_PRINT_LOCK) && (virt < MM_ARM_VA_ARS0_PAGE)) {
        char _ar_panic_buf[128];
        kt_sprintf(_ar_panic_buf, "ar_virt_to_phys 0x%08X", virt);
        ar_panic(_ar_panic_buf);
      }
      return virt;
    }
  }
  else {
    return virt;
  }
}



// ===========================================================================
// Counters.
//
// Most functions below are inlined, because they are short and expected to
// be in critical paths.
// ===========================================================================

// Set local counter to value, no notification
static inline void ar_cnt_set(int my_cid, int cnt, int val) {
#ifdef ARCH_ARM
  *ARS_CNT_VAL(my_cid, cnt) = val;
  *ARS_CNT_NTF_BRD_NODE(my_cid, cnt) = 0;
#endif
#ifdef __MICROBLAZE__
  *MBS_CNT_VAL(cnt) = val;
  *MBS_CNT_NTF_BRD_NODE(cnt) = 0;
#endif
}

// Set local counter to value, setup single notification to other counter
static inline void ar_cnt_set_notify_cnt(int my_cid, int cnt, int val,
                                         int ntf_bid, int ntf_cid, int ntf_cnt){
#ifdef ARCH_ARM
  *ARS_CNT_VAL(my_cid, cnt) = val;
  *ARS_CNT_NTF_ADR0(my_cid, cnt) = ar_addr_of_cnt(ntf_bid, ntf_cid, ntf_cnt);
  *ARS_CNT_NTF_BRD_NODE(my_cid, cnt) = (1 << 12) | (ntf_bid << 4) | ntf_cid;
#endif
#ifdef __MICROBLAZE__
  *MBS_CNT_VAL(cnt) = val;
  *MBS_CNT_NTF_ADR0(cnt) = ar_addr_of_cnt(ntf_bid, ntf_cid, ntf_cnt);
  *MBS_CNT_NTF_BRD_NODE(cnt) = (1 << 12) | (ntf_bid << 4) | ntf_cid;
#endif
}

// Set local counter to value, setup double notifications to other counters
static inline void ar_cnt_set_dbl_notify_cnt(int my_cid, int cnt, int val,
                                             int ntf_bid0, int ntf_cid0,
                                             int ntf_cnt0, int ntf_bid1,
                                             int ntf_cid1, int ntf_cnt1) {
#ifdef ARCH_ARM
  *ARS_CNT_VAL(my_cid, cnt) = val;
  *ARS_CNT_NTF_ADR0(my_cid, cnt) = ar_addr_of_cnt(ntf_bid0, ntf_cid0, ntf_cnt0);
  *ARS_CNT_NTF_ADR1(my_cid, cnt) = ar_addr_of_cnt(ntf_bid1, ntf_cid1, ntf_cnt1);
  *ARS_CNT_NTF_BRD_NODE(my_cid, cnt) =
                             (1 << 28) | (ntf_bid1 << 20) | (ntf_cid1 << 16) |
                             (1 << 12) | (ntf_bid0 << 4) | ntf_cid0;
#endif
#ifdef __MICROBLAZE__
  *MBS_CNT_VAL(cnt) = val;
  *MBS_CNT_NTF_ADR0(cnt) = ar_addr_of_cnt(ntf_bid0, ntf_cid0, ntf_cnt0);
  *MBS_CNT_NTF_ADR1(cnt) = ar_addr_of_cnt(ntf_bid1, ntf_cid1, ntf_cnt1);
  *MBS_CNT_NTF_BRD_NODE(cnt) = (1 << 28) | (ntf_bid1 << 20) | (ntf_cid1 << 16) |
                               (1 << 12) | (ntf_bid0 << 4)  | ntf_cid0;
#endif
}

// Set local counter to value, setup single notification to local mailslot
static inline void ar_cnt_set_notify_mslot(int my_bid, int my_cid, int cnt,
                                           int val) {
#ifdef ARCH_ARM
  *ARS_CNT_VAL(my_cid, cnt) = val;
  *ARS_CNT_NTF_ADR0(my_cid, cnt) = (int) ARS_MSL_ACCESS(my_cid);
  *ARS_CNT_NTF_BRD_NODE(my_cid, cnt) = (1 << 12) | (my_bid << 4) | my_cid;
#endif
#ifdef __MICROBLAZE__
  *MBS_CNT_VAL(cnt) = val;
  *MBS_CNT_NTF_ADR0(cnt) = (int) MBS_MSL_ACCESS;
  *MBS_CNT_NTF_BRD_NODE(cnt) = (1 << 12) | (my_bid << 4) | my_cid;
#endif
}

// Get local counter value
static inline int ar_cnt_get(int my_cid, int cnt) {
#ifdef ARCH_ARM
  return (*ARS_CNT_VAL(my_cid, cnt));
#endif
#ifdef __MICROBLAZE__
  return (*MBS_CNT_VAL(cnt));
#endif
}

// See if local counter has triggered. Return values:
// 0 = not triggered, 2 = reached value 0 (no Nacks), 3 = triggered by Nack
static inline int ar_cnt_get_triggered(int my_cid, int cnt) {
#ifdef ARCH_ARM
  return (*ARS_CNT_NTF_BRD_NODE(my_cid, cnt) >> 30);
#endif
#ifdef __MICROBLAZE__
  return (*MBS_CNT_NTF_BRD_NODE(cnt) >> 30);
#endif
}

// Increase remote counter with a message (no ack)
static inline void ar_cnt_incr(int my_cid, int dst_bid, int dst_cid,
                               int dst_cnt, int val) {
#ifdef ARCH_ARM
  *ARS_MNI_MSG0(my_cid)    = val;
  *ARS_MNI_DST_ADR(my_cid) = ar_addr_of_cnt(dst_bid, dst_cid, dst_cnt);
  *ARS_MNI_OPCODE(my_cid)  = (dst_bid << 20) | (dst_cid << 16);
#endif
#ifdef __MICROBLAZE__
  *MBS_MNI_MSG0    = val;
  *MBS_MNI_DST_ADR = ar_addr_of_cnt(dst_bid, dst_cid, dst_cnt);
  *MBS_MNI_OPCODE  = (dst_bid << 20) | (dst_cid << 16);
#endif
}


// ===========================================================================
// Communication functions.
//
// Most functions below are inlined, because they are short and expected to
// be in critical paths.
// ===========================================================================

// Send to mailbox, no ack
static inline void ar_mbox_send(int my_cid, int dst_bid, int dst_cid,
                                unsigned int msg) {
#ifdef ARCH_ARM
  *ARS_MNI_MSG0(my_cid)    = msg;
  *ARS_MNI_DST_ADR(my_cid) = ar_addr_of_mbox(dst_bid, dst_cid);
  *ARS_MNI_OPCODE(my_cid)  = (dst_bid << 20) | (dst_cid << 16);
#endif
#ifdef __MICROBLAZE__
  *MBS_MNI_MSG0    = msg;
  *MBS_MNI_DST_ADR = ar_addr_of_mbox(dst_bid, dst_cid);
  *MBS_MNI_OPCODE  = (dst_bid << 20) | (dst_cid << 16);
#endif
}

// Read mailbox (blocking)
static inline unsigned int ar_mbox_get(int my_cid) {
#ifdef ARCH_ARM
  return (*ARS_MBX_ACCESS(my_cid));
#endif
#ifdef __MICROBLAZE__
  return (*MBS_MBX_ACCESS);
#endif
}

// Read mailbox status (non-blocking)
static inline unsigned int ar_mbox_status_get(int my_cid) {
#ifdef ARCH_ARM
  return (*ARS_MBX_STATUS(my_cid));
#endif
#ifdef __MICROBLAZE__
  return (*MBS_MBX_STATUS);
#endif
}

// Read mailslot (blocking)
static inline unsigned int ar_mslot_get(int my_cid) {
#ifdef ARCH_ARM
  return (*ARS_MSL_ACCESS(my_cid));
#endif
#ifdef __MICROBLAZE__
  return (*MBS_MSL_ACCESS);
#endif
}

// Read mailslot status (non-blocking)
static inline unsigned int ar_mslot_status_get(int my_cid) {
#ifdef ARCH_ARM
  return (*ARS_MSL_STATUS(my_cid));
#endif
#ifdef __MICROBLAZE__
  return (*MBS_MSL_STATUS);
#endif
}

// Do a DMA, sending acks to a counter
static inline void ar_dma_with_ack(int my_cid, int src_bid, int src_cid,
                                   unsigned int src_adr, int dst_bid,
                                   int dst_cid, unsigned int dst_adr,
                                   int ack_bid, int ack_cid, int ack_cnt,
                                   int size, int ignore_dirty_on_src,
                                   int clean_on_dst, int write_through_on_dst) {
#ifdef ARCH_ARM
  *ARS_MNI_SRC_ADR(my_cid)  = ar_virt_to_phys(src_bid, src_adr);
  *ARS_MNI_DST_ADR(my_cid)  = ar_virt_to_phys(dst_bid, dst_adr);
  *ARS_MNI_ACK_ADR(my_cid)  = ar_addr_of_cnt(ack_bid, ack_cid, ack_cnt);
  *ARS_MNI_DMA_SIZE(my_cid) = size;
  *ARS_MNI_BRD_NODE(my_cid) = (src_bid << 20) | (src_cid << 16) |
                              (ack_bid  << 4) | (ack_cid << 0);
  *ARS_MNI_OPCODE(my_cid)   = (dst_bid << 20) | (dst_cid << 16) |
                               (ignore_dirty_on_src << 5) |
                               (write_through_on_dst << 4) |
                               (clean_on_dst << 3) | (1 << 2) | 0x2;
#endif
#ifdef __MICROBLAZE__
  *MBS_MNI_SRC_ADR  = ar_virt_to_phys(src_bid, src_adr);
  *MBS_MNI_DST_ADR  = ar_virt_to_phys(dst_bid, dst_adr);
  *MBS_MNI_ACK_ADR  = ar_addr_of_cnt(ack_bid, ack_cid, ack_cnt);
  *MBS_MNI_DMA_SIZE = size;
  *MBS_MNI_BRD_NODE = (src_bid << 20) | (src_cid << 16) |
                      (ack_bid << 4)  | (ack_cid << 0);
  *MBS_MNI_OPCODE   = (dst_bid << 20) | (dst_cid << 16) |
                      (ignore_dirty_on_src << 5) | (write_through_on_dst << 4) |
                      (clean_on_dst << 3) | (1 << 2) | 0x2;
#endif
}

// Do a DMA without using acknowledgements
static inline void ar_dma_no_ack(int my_cid, int src_bid, int src_cid,
                                   unsigned int src_adr, int dst_bid,
                                   int dst_cid, unsigned int dst_adr,
                                   int size, int ignore_dirty_on_src,
                                   int clean_on_dst, int write_through_on_dst) {
#ifdef ARCH_ARM
  *ARS_MNI_SRC_ADR(my_cid)  = ar_virt_to_phys(src_bid, src_adr);
  *ARS_MNI_DST_ADR(my_cid)  = ar_virt_to_phys(dst_bid, dst_adr);
  *ARS_MNI_DMA_SIZE(my_cid) = size;
  *ARS_MNI_BRD_NODE(my_cid) = (src_bid << 20) | (src_cid << 16);
  *ARS_MNI_OPCODE(my_cid)   = (dst_bid << 20) | (dst_cid << 16) |
                               (ignore_dirty_on_src << 5) |
                               (write_through_on_dst << 4) |
                               (clean_on_dst << 3) | (0 << 2) | 0x2;
#endif
#ifdef __MICROBLAZE__
  *MBS_MNI_SRC_ADR  = ar_virt_to_phys(src_bid, src_adr);
  *MBS_MNI_DST_ADR  = ar_virt_to_phys(dst_bid, dst_adr);
  *MBS_MNI_DMA_SIZE = size;
  *MBS_MNI_BRD_NODE = (src_bid << 20) | (src_cid << 16);
  *MBS_MNI_OPCODE   = (dst_bid << 20) | (dst_cid << 16) |
                      (ignore_dirty_on_src << 5) | (write_through_on_dst << 4) |
                      (clean_on_dst << 3) | (0 << 2) | 0x2;
#endif
}


// Read NI status
static inline unsigned int ar_ni_status_get(int my_cid) {
#ifdef ARCH_ARM
  return (*ARS_MNI_STATUS(my_cid));
#endif
#ifdef __MICROBLAZE__
  return (*MBS_MNI_STATUS);
#endif
}

// Read/write Formic board link status register
extern unsigned int ar_read_link_status(int my_bid, int my_cid, int src_bid);
extern void ar_write_link_status(int my_cid, int dst_bid, unsigned int val);

// Read Formic board status register
extern unsigned int ar_read_status(int my_bid, int my_cid, int src_bid);

// Write Formic board control register
extern void ar_write_brd_control(int my_cid, int dst_bid, unsigned int val);

// Wake-up/suspend Formic core
extern void ar_wake_up_core(int my_bid, int my_cid, int dst_bid, int dst_cid);
extern void ar_suspend_core(int my_bid, int my_cid, int dst_bid, int dst_cid);

// XUP peripheral register read/write
extern unsigned int ar_read_xup_register(int my_bid, int my_cid,
                                         int xup_reg_adr);
extern void ar_write_xup_register(int my_cid, int xup_reg_adr,
                                  unsigned int val);

// XUP video frame read/write
extern void ar_send_xup_frame(int my_bid, int my_cid, unsigned int my_adr,
                              unsigned int xup_adr, int my_cnt);
extern void ar_receive_xup_frame(int my_bid, int my_cid, unsigned int my_adr,
                                 unsigned int xup_adr, int my_cnt);


// ===========================================================================
// Code execution
// ===========================================================================
extern void ar_exec(func_t *func_adr, int num_args, void **args, int *types);


// ===========================================================================
// Locking (ARM cache-coherent cores only)
// ===========================================================================
#ifdef ARCH_ARM
extern int ar_lock_get(void *lock_adr, int core_id);
extern int ar_lock_release(void *lock_adr);
#endif


// ===========================================================================
// Compact Flash
// ===========================================================================
#ifdef ARCH_ARM
extern int ar_flash_init_device();

extern int ar_flash_read_sector(int sector, unsigned char *buf);
extern int ar_flash_read_sectors(int start_sector, int num_sectors,
                                 unsigned char *buf);
extern int ar_flash_write_sector(int sector, unsigned char *buf);
extern int ar_flash_write_sectors(int start_sector, int num_sectors,
                                  unsigned char *buf);
#endif


// ===========================================================================
// Arch init
// ===========================================================================
extern int  ar_check_formic_links(int my_bid, int my_cid,
                                  int reset_link_error);
extern void ar_wake_up_formic_boards(int my_bid, int my_cid);

extern void ar_init(int my_cid, int my_bid);


#endif
