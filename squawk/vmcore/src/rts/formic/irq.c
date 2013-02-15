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
// Abstract      : Microblaze interrupt handling functions
//
// =============================[ CVS Variables ]=============================
//
// File name     : $RCSfile: irq.c,v $
// CVS revision  : $Revision: 1.5 $
// Last modified : $Date: 2012/05/15 07:42:58 $
// Last author   : $Author: jacob $
//
// ===========================================================================

#include <arch.h>
/*#include <fmpi.h>*/

#define IRQ_SYSCALL             (1 << 8)
#define IRQ_TLB_MISS_FAULT      (1 << 9)
#define IRQ_ART_MISS_FAULT      (1 << 10)
#define IRQ_PERM_FAULT          (1 << 11)
#define IRQ_NI_FIFO_EXCEPTION   (1 << 12)
#define IRQ_SOFTWARE            (1 << 16)
#define IRQ_MBOX_MSLOT          (1 << 17)
#define IRQ_COUNTER0            (1 << 18)
#define IRQ_COUNTER1            (1 << 19)
#define IRQ_COUNTER2            (1 << 20)
#define IRQ_COUNTER3            (1 << 21)
#define IRQ_TIMER               (1 << 22)
#define IRQ_UART_RX             (1 << 23)


// ==========================================================================
// ==========================================================================
__attribute__ ((interrupt_handler)) void ar_irq_handler() {

  unsigned int r14;
  unsigned int status;

  // Get interrupt cause
  status = *MBS_CPU_STATUS;

  // Is it for the FMPI library?
/*#if (defined FMPI) && (defined FMPI_INTR_ENABLE)*/
  /*if ((status & (IRQ_MBOX_MSLOT | IRQ_COUNTER0 | IRQ_COUNTER1 |*/
                                  /*IRQ_COUNTER2 | IRQ_COUNTER3))) {*/

    /*// Pass it to the FMPI interrupt handler*/
    /*fmpi_intr_serve();*/

    /*// We're done*/
    /*return;*/
  /*}*/
/*#endif*/

  // Get instruction before interrupt
  asm("add %0, r14, r0" : "=r"(r14) );

  // Print
  kt_printf("0x%02X/%d: IRQ 0x%08X, r14 = 0x%p\r\n",
            ar_get_board_id(), ar_get_core_id(), status, r14);

  // Block
  ar_panic("IRQ exception");
  while (1)
    ;
}


// ==========================================================================
// ==========================================================================
void ar_abort_handler() {
  ar_panic("Abort exception");
}

