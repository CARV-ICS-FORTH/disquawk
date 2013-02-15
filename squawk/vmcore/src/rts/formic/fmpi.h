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
// Author        : Iakovos Mavroidis / Spyros Lyberis
// Abstract      : Minimal MPI library header file
//
// =============================[ CVS Variables ]=============================
// 
// File name     : $RCSfile: fmpi.h,v $
// CVS revision  : $Revision: 1.12 $
// Last modified : $Date: 2013/01/22 13:40:54 $
// Last author   : $Author: lyberis-spree $
// 
// ===========================================================================

#ifndef _FMPI_H
#define _FMPI_H

#include <stdarg.h>

#include <types.h>


// Cache mode
#define FMPI_CACHE_EPOCH_MODE   1  // Set 1 for Epoch mode (i.e. random
                                   // replacements, since epochs are not 
                                   // advanced) or 0 for LRU mode

// Performance optimizations (uncomment the defines below)

#ifdef ARCH_MB
//#define FMPI_INTR_ENABLE      // Interrupts optimization (MicroBlaze only)
#endif

//#define FMPI_EAGER_ENABLE     // Eager data transfer optimization


// Basic definitions

#define FMPI_NUM_DESCRIPTORS    1024
#define FMPI_NUM_DESCR_RECV     1024
#define FMPI_NUM_USER_REQUESTS  1024

#define FMPI_CREDITS_PER_CORE   2       // Mailbox words reserved for each 
                                        // possible peer. 2 * 512 = 1024 words.
                                        // Mailbox is 4 KB, so this is fine
                                        // for a 512-core system.
#define FMPI_REQ_PENDING        0
#define FMPI_REQ_INIT           1
#define FMPI_REQ_PROGRESS       2
#define FMPI_REQ_DONE           3

#define FMPI_DMA_THRESHOLD      5
#define FMPI_DMA_WORD           64

#define FMPI_MBX_SEND           0

// ===========================================================================
// Standard MPI definitions
// ===========================================================================

// Error codes
#define MPI_SUCCESS             0   // No error   
#define MPI_ERR_BUFFER          1   // Invalid buffer pointer
#define MPI_ERR_COUNT           2   // Invalid count argument
#define MPI_ERR_TYPE            3   // Invalid datatype argument
#define MPI_ERR_TAG             4   // Invalid tag argument
#define MPI_ERR_COMM            5   // Invalid communicator
#define MPI_ERR_RANK            6   // Invalid rank 
#define MPI_ERR_REQUEST         7   // Invalid request 
#define MPI_ERR_ROOT            8   // Invalid root 
#define MPI_ERR_GROUP           9   // Invalid group 
#define MPI_ERR_OP              10  // Invalid operation 
#define MPI_ERR_TOPOLOGY        11  // Invalid topology 
#define MPI_ERR_DIMS            12  // Invalid dimensions argument 
#define MPI_ERR_ARG             13  // Invalid argument of some other kind 
#define MPI_ERR_UNKNOWN         14  // Unknown error 
#define MPI_ERR_TRUNCATE        15  // Message truncated on receive
#define MPI_ERR_OTHER           16  // Known error not in this list   
#define MPI_ERR_INTERN          17  // Internal MPI error   
#define MPI_ERR_IN_STATUS       18  // Error code is in status  
#define MPI_ERR_PENDING         19  // Pending request 
#define MPI_ERR_LASTCODE        20  // Last error code

// Misc
#define MPI_ANY_SOURCE          -1
#define MPI_ANY_TAG             -2
#define MPI_TAG_BCAST           -3
#define MPI_TAG_REDUCE          -4
#define MPI_TAG_ALL             -5


typedef int MPI_Datatype;

// Predefined data types
#define MPI_BYTE                1
#define MPI_PACKED              2
#define MPI_CHAR                3
#define MPI_SHORT               4
#define MPI_INT                 5
#define MPI_LONG                6
#define MPI_FLOAT               7
#define MPI_DOUBLE              8
#define MPI_LONG_DOUBLE         9
#define MPI_UNSIGNED_CHAR       10

typedef int MPI_Comm;

// Predefined communicators
#define MPI_COMM_NULL           0
#define MPI_COMM_WORLD          1
#define MPI_COMM_SELF           2

typedef struct {

  int   MPI_SOURCE;
  int   MPI_TAG;
  int   MPI_ERROR;

} MPI_Status;

typedef struct {
  int   status;
  // 
} MPI_Request;

#define MPI_STATUS_IGNORE       NULL


// ===========================================================================
// FMPI internals
// ===========================================================================

typedef enum {

  FMPI_OPCODE_SEND,             // Blocking send
  FMPI_OPCODE_ISEND,            // Non-blocking send
  FMPI_OPCODE_RECV,             // Blocking receive
  FMPI_OPCODE_IRECV,            // Non-blocking receive
  FMPI_OPCODE_BARRIER           // Barrier

} FMPI_Opcode;


typedef enum {
  MPI_MAX,
  MPI_MIN,
  MPI_SUM,
  MPI_PROD,
  MPI_LAND,
  MPI_BAND,
  MPI_LOR,
  MPI_BOR,
  MPI_LXOR,
  MPI_BXOR,
  MPI_MAXLOC,
  MPI_MINLOC,
  FMPI_INIT // used for initialization of buffer
} MPI_Op;


//typedef struct {
//
//  unsigned int  rank:12;        // Sender rank
//  unsigned int  descr_id:12;    // Sender descriptor index
//  unsigned int  cnt_id:8;       // Sender counter index
//
//} FMPI_MboxMessage;             // Single mailbox word (32 bits)



#define FMPI_ALL2ALL_GS 64         // All to all group size 
#define FMPI_ALL2ALL_RBUF_SZ 1024  // size of receive buffer

#define FMPI_EAGER_SIZE 52        // Fix this so that FMPI_Descriptor is
                                  // always 64 bytes

typedef struct {
  // rank is sent to the mailbox
  unsigned int  src_tag;
  void          *src_buf;
  unsigned int  size;
  unsigned int  eager_payload[FMPI_EAGER_SIZE / sizeof(int)];

} FMPI_Descriptor;  // 64-Byte descriptor


// general list
typedef struct FMPI_List {

  int                   id;
  struct FMPI_List      *next;
  struct FMPI_List      *prev;

} FMPI_List; 


typedef struct {

  MPI_Status    *ext_status;    // MPI status
  MPI_Request   *ext_request;   // MPI request
  int           status;         // 0: pending, 1: in progress, 
                                // 2: complete
  FMPI_Opcode   opcode;         // Type of MPI operation
  void          *usr_buf;       // User buffer
  unsigned int  size;
  int           peer_rank;      // Rank of peer to communicate
  int           peer_cnt;       // cnt of peer for notification
  int           tag;            // Tag
  FMPI_List     *descr;         // Descriptor (for in-progress only)
  FMPI_List     *cnt;           // Counter (for in-progress only)

} FMPI_UserReq;


typedef struct {

  FMPI_List     *descr;         // Descriptor from external core
  FMPI_List     *cnt;           // Counter for receiving descriptor
  int           arrived;        // When 1, descriptor has already arrived
  int           peer_rank;      // Rank of peer to communicate
  int           peer_cnt;       // Counter of peer

} FMPI_DescrRecv;


typedef struct FMPI_Context {

  // FMPI_Descriptors should be first in order to make sure that the
  // address of each descriptor is 64-byte aligned. Also, we depend
  // on it to discover peer descriptor addresses.
  FMPI_Descriptor       descr[FMPI_NUM_DESCRIPTORS];      // MPI descriptors
  int                   rank;                             // My rank
  int                   num_ranks;                        // Total active cores
  struct FMPI_Context   **rank_context;                   // Array that maps
                                                          // each rank to its
                                                          // FMPI Context adr
  int                   *rank_bid_cid;                    // Array that maps
                                                          // each rank to a 
                                                          // board and core ID
  FMPI_UserReq          user_req[FMPI_NUM_USER_REQUESTS]; // User requests
  FMPI_DescrRecv        descr_recv[FMPI_NUM_DESCR_RECV];  // User requests

  unsigned int          mbx_recv;       // word from mailbox
  int                   mbx_pending;    // mbx_recv is valid

  int                   cnt_breq;       // counter for barrier recv requests
  int                   cnt_back;       // counter for barrier acks

  // Tree-like structure of ranks
  int                   father;         // father for implementing barrier
  int                   children[24];   // children should be less than 24
  int                   num_children;   // total barrier recv requests

  // interrupts
  char                  intr_enabled;   // interrupt enable
  unsigned int          intr_mask;      // mask bits of CPU Interrupt register
  unsigned int          intr_cnt_pos;   // pointer to next counter out of 
                                        // four total intr counters
  // lists
  FMPI_List             user_req_list[FMPI_NUM_USER_REQUESTS];  
  FMPI_List             *cnt_list;      // One per usable counter
  FMPI_List             descr_list[FMPI_NUM_DESCRIPTORS];  
  FMPI_List             descr_recv_list[FMPI_NUM_DESCR_RECV];

  // free lists
  FMPI_List             *user_req_free;
  FMPI_List             *cnt_free;
  FMPI_List             *descr_free;
  FMPI_List             *descr_recv_free;

  // used lists
  FMPI_List             *user_req_used_first, *user_req_used_last;
  FMPI_List             *cnt_used_first, *cnt_used_last;
  FMPI_List             *descr_used_first, *descr_used_last;
  FMPI_List             *descr_recv_used_first, *descr_recv_used_last;

  int                   incoming_dmas;  // Incoming DMAs budget
  unsigned char         *mbox_credits;  // Mbox credits per rank

  unsigned int          time_init;  // global time on MPI_Init
  unsigned int          time_last;  // global time on MPI_Wtime
  unsigned int          time_ovfl;  // global timer overflowed
} FMPI_Context;


// ===========================================================================
// Possible rank configurations
// ===========================================================================
typedef enum {

  FMPI_CFG_1_MB,         // Single-core, MicroBlaze
  FMPI_CFG_1_ARM,        // Single-core, ARM

  FMPI_CFG_2_MB,         //   2 cores, MicroBlaze
  FMPI_CFG_2_ARM,        //   2 cores, ARM
  FMPI_CFG_2_HET,        //   2 cores, Heterogeneous (1 MB + 1 ARM)

  FMPI_CFG_4_MB,         //   4 cores, MicroBlaze
  FMPI_CFG_4_ARM,        //   4 cores, ARM
  FMPI_CFG_4_HET,        //   4 cores, Heterogeneous (3 MB + 1 ARM)

  FMPI_CFG_8_MB,         //   8 cores, MicroBlaze
  FMPI_CFG_8_HET,        //   8 cores, Heterogeneous (4 MB + 4 ARM)
  
  FMPI_CFG_12_HET,       //  12 cores, Heterogeneous (8 MB + 4 ARM)

  FMPI_CFG_16_MB,        //  16 cores, MicroBlaze

  FMPI_CFG_20_HET,       //  20 cores, Heterogeneous (16 MB + 4 ARM)

  FMPI_CFG_64_MB,        //  64 cores, MicroBlaze

  FMPI_CFG_128_MB,       // 128 cores, MicroBlaze

  FMPI_CFG_256_MB,       // 256 cores, MicroBlaze
  
  FMPI_CFG_512_MB,       // 512 cores, MicroBlaze
  
  FMPI_CFG_516_HET       // 516 cores, Heterogeneous (512 MB + 4 ARM)

} FMPI_CfgMode;



// ===========================================================================
// Internal functions
// ===========================================================================

extern int  fmpi_select_rank_init(FMPI_CfgMode select);
extern void fmpi_get_bid_cid(FMPI_Context *context, int rank, int *bid, 
                             int *cid);
extern void fmpi_process_pending_events(FMPI_Context *context);
extern void fmpi_serve_send(FMPI_Context *context, FMPI_UserReq *request);
extern void fmpi_serve_recv(FMPI_Context *context, FMPI_UserReq *request);
extern void fmpi_find_father(FMPI_Context *context);
extern void fmpi_find_children(FMPI_Context *context);
extern void fmpi_barrier_init(FMPI_Context *context);
extern void fmpi_mbx_barrier(FMPI_Context *context);
extern int  fmpi_commit_cmd(volatile void *buf, int count, 
                            MPI_Datatype datatype, int dest, int tag, 
                            MPI_Comm comm, FMPI_Opcode opcode, 
                            MPI_Status *status, MPI_Request *request);
extern void fmpi_barrier_send(FMPI_Context *context, int dest, int cnt);
extern int  fmpi_req_check(FMPI_Context *context, volatile void *buf, 
                           int count, MPI_Datatype datatype, int dest, 
                           int tag, MPI_Comm comm);
extern int  fmpi_bcast_recv(FMPI_Context  *context, int src);
extern void fmpi_reduce(volatile void *data, volatile void *result, int count, 
                        MPI_Datatype datatype, MPI_Op op);


// ===========================================================================
// Inlined functions
// ===========================================================================
//
static inline void fmpi_intr_enable(FMPI_Context *context) {
#ifdef FMPI_INTR_ENABLE
  // this should go first
  context->intr_enabled = 1;

  // restore interrupts
  ar_intr_cpu_set(context->intr_mask);
#endif
}

static inline void fmpi_intr_disable(FMPI_Context *context) {
#ifdef FMPI_INTR_ENABLE
  // disable mailbox and counter interrupts
  context->intr_enabled = 0;
  ar_intr_cpu_set(0xff);
#endif
}

static inline int fmpi_sizeof(MPI_Datatype datatype, int count) {
  int size;

  switch (datatype) {
    case MPI_BYTE:          size = 1;                     break;
    case MPI_CHAR:          size = sizeof(char);          break;
    case MPI_SHORT:         size = sizeof(short);         break;
    case MPI_INT:           size = sizeof(int);           break;
    case MPI_LONG:          size = sizeof(long);          break;
    case MPI_FLOAT:         size = sizeof(float);         break;
    case MPI_DOUBLE:        size = sizeof(double);        break;
    case MPI_LONG_DOUBLE:   size = sizeof(long);          break;
    case MPI_UNSIGNED_CHAR: size = sizeof(unsigned char); break;
    default:                size = 0;                     break;
  }

  return size*count;
}


// ===========================================================================
// Exported functions
// ===========================================================================

extern int  MPI_Init(int *argc, char ***argv);
extern int  MPI_Finalize();

extern int  MPI_Comm_rank(MPI_Comm comm, int *rank);
extern int  MPI_Comm_size(MPI_Comm comm, int *size);

extern int  MPI_Send(volatile void *buf, int count, MPI_Datatype datatype, 
                     int dest, int tag, MPI_Comm comm);
extern int  MPI_Isend(volatile void *buf, int count, MPI_Datatype datatype, 
                     int dest, int tag, MPI_Comm comm, MPI_Request *request);
extern int  MPI_Recv(volatile void *buf, int count, MPI_Datatype datatype, 
                     int source, int tag, MPI_Comm comm, MPI_Status *status);
extern int  MPI_Irecv(volatile void *buf, int count, MPI_Datatype datatype, 
                     int source, int tag, MPI_Comm comm, MPI_Request *request);

extern int MPI_Waitall(int count, MPI_Request *reqs, MPI_Status *status);

extern int  MPI_Barrier(MPI_Comm comm);

extern int  MPI_Bcast(volatile void *buf, int count, MPI_Datatype datatype, 
                      int root, MPI_Comm comm);

extern int MPI_Wait(MPI_Request *request, MPI_Status *status);

extern int MPI_Allreduce(volatile void *sendbuf, volatile void *recvbuf, int count,
                         MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

extern int MPI_Alltoall(volatile void *sendbuf, int sendcount, MPI_Datatype sendtype,
                        volatile void *recvbuf, int recvcount,
                        MPI_Datatype recvtype, MPI_Comm comm);

extern int MPI_Alltoallv(volatile void *sendbuf, int *sendcounts, int *sdispls,
                        MPI_Datatype sendtype, volatile void *recvbuf, int *recvcounts,
                        int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);

extern int MPI_Reduce(volatile void *sendbuf, volatile void *recvbuf, int count,
                      MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);

extern unsigned int FMPI_Wtime(void);

// FIXME change place

extern FMPI_List *fmpi_list_init(FMPI_List *first, int nums);

extern FMPI_List *fmpi_list_alloc(FMPI_List **free, FMPI_List **used_first, FMPI_List **used_last);

extern void fmpi_list_free(FMPI_List **free, FMPI_List **used_first, FMPI_List **used_last, FMPI_List *object);

extern void fmpi_print_list(FMPI_List *object);

// Interrupt 
#ifdef FMPI_INTR_ENABLE
extern void fmpi_intr_serve();
#endif

#endif
