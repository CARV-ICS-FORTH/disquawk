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
// Abstract      : Header file for network-on-chip functionality
//
// =============================[ CVS Variables ]=============================
// 
// File name     : $RCSfile: noc.h,v $
// CVS revision  : $Revision: 1.9 $
// Last modified : $Date: 2012/12/18 16:40:29 $
// Last author   : $Author: lyberis-spree $
// 
// ===========================================================================

#ifndef _NOC_H
#define _NOC_H


// Uncomment the following to enable the credit warning (once per core)
#define NOC_WARN_OUT_OF_CREDITS

// Reserved hardware counters
#define NOC_COUNTER_UART        127     // Formic UART transfers
#define NOC_COUNTER_WAKEUP0     126     // Formic board suspend/wakeup and misc
#define NOC_COUNTER_WAKEUP1     125     // init barriers (NoC, FMPI, boot 
#define NOC_COUNTER_WAKEUP2     124     // sequence, etc.)
#define NOC_COUNTER_WAKEUP3     123

#define NOC_MAX_COUNTERS        123     // Available counters: from 0 up to the
                                        // max reserved above

// Software message size in bytes
#define NOC_MESSAGE_SIZE        64      // Must be a multiple of cache line
                                        // (i.e. 64 bytes)

// Credit-based mailbox word fields
#define NOC_MBOX_COREID_OFFSET  0
#define NOC_MBOX_COREID_MASK    0x0000FFFF
#define NOC_MBOX_BUFID_OFFSET   16
#define NOC_MBOX_BUFID_MASK     0xFFFF0000


// ===========================================================================
// Types
// ===========================================================================
typedef enum {

  NOC_MODE_MAILBOX_ONLY,        // Mailbox-only mode, no counters per peer,
                                // slowest, may use retries
  NOC_MODE_CREDIT_ONLY,         // Credit-based mode, 2 counters per peer,
                                // adjustable credits per peer, fast send,
                                // polling-based receive
  NOC_MODE_CREDIT_MAILBOX       // Credit-based mode, 2 counters per peer,
                                // credits based on mailbox capacity, 
                                // fast send/recv
} NocMode;


typedef struct {

  int           cnt_send_data;    // Remote hw counter to ack when sending msg
  int           cnt_send_credits; // Local hw credit cntr to check before send
  unsigned int  send_credits;     // Used credits so far
  void          **loc_send_bufs;  // Local send buffers base for this peer
  void          **rem_send_bufs;  // Remote send buffers base for this peer
  int           num_send_bufs;    // Number of remote send buffers
  int           cur_send_buf_id;  // Which send buf is the next one to be used

  int           cnt_recv_data;    // Local hw cntr to check when receiving msg
  unsigned int  recv_data;        // Received data bytes so far
  int           cnt_recv_credits; // Remote hw credit counter to increment
  void          **recv_bufs;      // Local receive buffers base for this peer
  int           num_recv_bufs;    // Number of local receive buffers
  int           cur_recv_buf_id;  // Which recv buf is the next one to be read
                                  // (credit-only mode)

  int           rem_core_id;      // Remote core ID

} NocCreditMode;


typedef struct {

  int           src_bid;          // Source arch-level board ID
  int           src_cid;          // Source arch-level core ID
  unsigned int  src_adr;          // Source address
  int           dst_bid;          // Destination arch-level board ID
  int           dst_cid;          // Destination arch-level core ID
  unsigned int  dst_adr;          // Destination address
  int           size;             // Size of transfer in bytes
  unsigned char flags[3];         // flags[0]: I flag for DMA engine
                                  // flags[1]: W flag for DMA engine
                                  // flags[2]: C flag for DMA engine
  int           local_cnt;        // Local hw counter which monitors this DMA.
                                  // If -1, counter was not available when the
                                  // DMA was requested. If >= 0, counter was
                                  // available and DMA has been started.
  int           *loc_notif_var;   // Local sw counter to reduce by 1 when this
                                  // DMA is finished
  void          *loc_notif_data;  // Generic local data hook to be given back
                                  // to the requestor of a DMA group when all
                                  // DMAs of a group is finished
} NocDma;


// ===========================================================================
// Functions
// ===========================================================================
extern void noc_init(NocMode mode, int credit_only_credits);

extern void *noc_msg_send_get_buf(int dst_core_id);
extern int  noc_msg_send();
extern int  noc_msg_recv(int block, void **ret_buf);

extern int noc_dma_add_new(int src_core_id, void *src_adr, int dst_core_id,
                           void *dst_adr, int size, int i_flag, int w_flag,
                           int c_flag, int *group_notif_var,
                           void *group_notif_data);
extern int noc_dma_check_progress(void ***ret_group_data);


#endif
