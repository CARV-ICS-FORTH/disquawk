// ===========================================================================
//
//                      LLNL / CASC and FORTH-ICS / CARV
//
//                        Proprietary and confidential
//                             Copyright (c) 2011
//
//
// ==========================[ Static Information ]===========================
//
// Author        : Spyros LYBERIS
// Abstract      : Header file for processing layer functions and data types
//
// =============================[ CVS Variables ]=============================
// 
// File name     : $RCSfile: processing.h,v $
// CVS revision  : $Revision: 1.39 $
// Last modified : $Date: 2013/01/29 17:31:52 $
// Last author   : $Author: lyberis-spree $
// 
// ===========================================================================

#ifndef _PROCESSING_H
#define _PROCESSING_H

#include <stdarg.h>

#include <types.h>


// ===========================================================================
// Core setup configurations
// ===========================================================================

typedef enum {

  PR_CFG_1_MB,         // Single-core, MicroBlaze
  PR_CFG_1_ARM,        // Single-core, ARM

  PR_CFG_2_MB_FLAT,    //   2-core MB:     1 L0 sch, 1 wrk
  PR_CFG_2_ARM_FLAT,   //   2-core ARM:    1 L0 sch, 1 wrk
  PR_CFG_2_HET_FLAT,   //   2-core MB+ARM: 1 L0 sch, 1 wrk

  PR_CFG_3_MB_FLAT,    //   3-core MB:     1 L0 sch, 2 wrk
  PR_CFG_3_ARM_FLAT,   //   3-core ARM:    1 L0 sch, 2 wrk
  PR_CFG_3_HET_FLAT,   //   3-core MB+ARM: 1 L0 sch, 2 wrk

  PR_CFG_5_MB_FLAT,    //   5-core MB:     1 L0 sch, 4 wrk
  PR_CFG_5_HET_FLAT,   //   5-core MB+ARM: 1 L0 sch, 4 wrk

  PR_CFG_8_ARM_FLAT,   //   8-core ARM:    1 L0 sch, 7 wrk

  PR_CFG_9_MB_FLAT,    //   9-core MB:     1 L0 sch, 8 wrk
  PR_CFG_9_HET_FLAT,   //   9-core MB+ARM: 1 L0 sch, 8 wrk

  PR_CFG_10_MB_FLAT,   //   9-core MB:     1 L0 sch, 9 wrk

  PR_CFG_11_MB_HIER,   //  11-core MB:     2 L0 + 1 L1 sch, 8 wrk
  PR_CFG_11_HET_HIER,  //  11-core MB+ARM: 2 L0 + 1 L1 sch, 8 wrk

  PR_CFG_17_MB_FLAT,   //  17-core MB:     1 L0 sch, 16 wrk
  PR_CFG_17_HET_FLAT,  //  17-core MB+ARM: 1 L0 sch, 16 wrk

  PR_CFG_19_MB_HIER,   //  19-core MB:     2 L0 + 1 L1 sch, 16 wrk

  PR_CFG_25_MB_FLAT,   //  25-core MB:     1 L0 sch, 24 wrk
  PR_CFG_25_HET_FLAT,  //  25-core MB+ARM: 1 L0 sch, 24 wrk

  PR_CFG_26_MB_FLAT,   //  26-core MB:     1 L0 sch, 25 wrk

  PR_CFG_28_HET_HIER,  //  28-core MB+ARM: 3 L0, 1 L1 sch, 24 wrk

  PR_CFG_31_MB_HIER,   //  31-core MB:     8 L0 + 4 L1 + 2 L2 + 1 L3 sch, 16 wrk
  
  PR_CFG_33_MB_FLAT,   //  33-core MB:     1 L0 sch, 32 wrk
  PR_CFG_33_HET_FLAT,  //  33-core MB+ARM: 1 L0 sch, 32 wrk

  PR_CFG_35_HET_HIER,  //  35-core MB+ARM: 2 L0 + 1 L1 sch, 32 wrk
  
  PR_CFG_37_MB_FLAT,   //  37-core MB:     1 L0 sch, 36 wrk

  PR_CFG_52_HET_HIER,  //  52-core MB+ARM: 3 L0 + 1 L1 sch, 48 wrk
  
  PR_CFG_65_HET_FLAT,  //  65-core MB+ARM: 1 L0 sch, 64 wrk

  PR_CFG_69_HET_HIER,  //  69-core MB+ARM: 4 L0 + 1 L1 sch, 64 wrk
  
  PR_CFG_120_HET_HIER, // 120-core MB+ARM: 7 L0 + 1 L1 sch, 112 wrk

  PR_CFG_129_HET_FLAT, // 129-core MB+ARM: 1 L0 sch, 128 wrk

  PR_CFG_136_HET_HIER, // 136-core MB+ARM: 7 L0 + 1 L1 sch, 128 wrk

  PR_CFG_232_HET_HIER, // 232-core MB+ARM: 7 L0 + 1 L1 sch, 224 wrk
  
  PR_CFG_257_HET_FLAT, // 257-core MB+ARM: 1 L0 sch, 256 wrk
  
  PR_CFG_264_HET_HIER, // 264-core MB+ARM: 7 L0 + 1 L1 sch, 256 wrk
  
  PR_CFG_513_HET_FLAT, // 513-core MB+ARM: 1 L0 sch, 512 wrk

  PR_CFG_520_HET_HIER3,// 520-core MB+ARM: 64 L0 + 7 L1 + 1 L2 sch, 448 wrk

  PR_CFG_520_HET_HIER2 // 520-core MB+ARM: 7 L0 + 1 L1 sch, 512 wrk

} PrCfgMode;


// ===========================================================================
// Request/reply messages
// ===========================================================================

#define PR_REQ_MAX_SIZE      8  // This makes a request to add up to 64B 
                                // (in 32-bit machines)

#define PR_RPL_MAX_SIZE      5  // This makes a reply to add up to 64B 
                                // (in 32-bit machines)


// Set for 32-bit signed integer message IDs:
#define PR_TRIES_MSG_ID_MSB  30                 // Message ID-based tries MSB
#define PR_MAX_MSG_ID        0x7FFFFFFF         // Max ID before wrap-around



typedef enum {

  ROLE_SCHEDULER,
  ROLE_WORKER

} PrRole;

// Message types
typedef enum {

  // Communication requests
  REQ_ALLOC,                    // Allocate object in a region (basic)
  REQ_BALLOC,                   // Bulk allocate objects in a region (basic)
  REQ_FREE,                     // Free object (basic)
  REQ_RALLOC,                   // Allocate new region (basic)
  REQ_RALLOC_ORPHAN,            // Allocate parentless new region (basic)
  REQ_RFREE,                    // Free region (basic)
  REQ_RFREE_UPDATE_PARENT,      // Child region freed, update parent (basic)
  REQ_PACK,                     // Get address of a region and/or object (basic)
  EXT_REQ_PACK,                 // Get addresses of regions/objects (extended)
  REQ_QUERY_POINTER,            // Find out size & region of a pointer (basic)
  REQ_GET_PAGES,                // Give more pages (basic)
  REQ_RETURN_PAGES,             // Return pages back (basic)
  REQ_GET_RIDS,                 // Give more region IDs (basic)
  REQ_RETURN_RIDS,              // Return region IDs back (basic)
  REQ_LOAD_REPORT,              // Report load condition (basic)
  REQ_SHUTDOWN,                 // Exit main loop after killing children (basic)
  EXT_REQ_SPAWN,                // Spawn a task (extended)
  EXT_REQ_DELEGATE,             // Spawned task creation delegation (extended)
  EXT_REQ_DEP_START,            // Start dependency analysis on these args (ext)
  EXT_REQ_DEP_ROUTE,            // Discover region tree route (extended)
  EXT_REQ_DEP_ENQUEUE,          // Enqueue dependency (extended)
  REQ_DEP_OK,                   // Dependency now at head of the queue (basic)
  EXT_REQ_DEP_STOP,             // Move to next dependency on these args (ext)
  REQ_DEP_CHILD_FREE,           // A child region has finished all tasks (basic)
  EXT_REQ_UPDATE_LOCATION,      // Update location of these args (ext)
  EXT_REQ_EXEC,                 // Execute a task (extended)
  REQ_EXEC_DONE,                // Task execution finished (basic)
  EXT_REQ_SCHEDULE,             // Schedule a task on this subtree (extended)

  // Requests for local action (not communicated among workers/schedulers; just
  // part of the event system as "notes to self")
  SELF_RALLOC_UPDATE_PARENT,    // Remote region child created; update local 
                                // parent (basic)
  SELF_PACK_MERGE,              // Receive pack reply from other scheduler and
                                // merge results with already gathered elements
  SELF_RFREE_WAIT_CHILDREN,     // Remote child freed; check if all of them
                                // are done, so we can redo local rfree (basic)
  SELF_SCHEDULE_RESULT,         // Scheduling finished; update local task 
                                // descriptor with core ID of the worker
  SELF_WAIT_SPAWN,              // Worker: the last spawn we made has been
                                // completed by the parent task scheduler

  // Replies to communication requests
  REPLY_ALLOC,                  // Allocate object in a region (basic)
  EXT_REPLY_BALLOC,             // Bulk allocate objects in a region (extended)
  REPLY_FREE,                   // Free object (basic)
  REPLY_RALLOC,                 // Allocate new region (basic)
  REPLY_RFREE,                  // Free region (basic)
  EXT_REPLY_PACK,               // Get addresses of regions/objects (extended)
  REPLY_QUERY_POINTER,          // Find out size & region of a pointer (basic)
  REPLY_GET_PAGES,              // Give more pages (basic)
  REPLY_RETURN_PAGES,           // Return pages back (basic)
  REPLY_GET_RIDS,               // Give more region IDs (basic)
  REPLY_RETURN_RIDS,            // Return region IDs back (basic)
  REPLY_SCHEDULE,               // Scheduling done (basic)
  REPLY_SPAWN                   // Spawn concluded, new one can be made (basic)

} PrMsgType;


typedef struct {
  
  // Basic part of request (exists for both basic and extended requests)

  int           type;           // Message type. Its actual type is PrMsgType,
                                // but we declare it as int to avoid GCC
                                // optimizing it differently across ARM/MB
                                // architectures (this actually happened...)
  int           req_id;         // Unique request ID for this core
  int           core_id;        // Source unique core ID
  size_t        size;           // Size (REQ_ALLOC, REQ_BALLOC, REQ_GET_PAGES, 
                                // REQ_RETURN_PAGES, REQ_GET_RIDS,
                                // REQ_RETURN_RIDS) or pack option bitmap 
                                // (REQ_PACK, EXT_REQ_PACK) or parent location
                                // (REQ_RALLOC_ORPHAN) or I/O task type bitmap
                                // (EXT_REQ_SPAWN, EXT_REQ_DELEGATE,
                                // EXT_REQ_DEP_START, EXT_REQ_DEP_STOP) or
                                // scheduled task load (REQ_LOAD_REPORT) or
                                // parent task ID (EXT_REQ_DEP_ROUTE,
                                // EXT_REQ_DEP_ENQUEUE) or new task ID
                                // (REQ_DEP_OK, EXT_REQ_SCHEDULE) or child RO
                                // decrement (REQ_DEP_CHILD_FREE) or number
                                // of task arguments (EXT_REQ_EXEC)
  rid_t         region;         // Region (REQ_ALLOC, REQ_BALLOC, REQ_RALLOC, 
                                // REQ_RALLOC_ORPHAN, REQ_RFREE,
                                // REQ_RFREE_UPDATE_PARENT, REQ_PACK,
                                // EXT_REQ_PACK, SELF_REQ_UPDATE_PARENT,
                                // REQ_DEP_CHILD_FREE) or region/safe type
                                // bitmap (EXT_REQ_SPAWN, EXT_REQ_DELEGATE,
                                // EXT_REQ_DEP_START, EXT_REQ_DEP_STOP, 
                                // EXT_REQ_UPDATE_LOCATION), or
                                // region load (REQ_LOAD_REPORT), or
                                // highest-level region found so far that has
                                // the parent task in its queue
                                // (EXT_REQ_DEP_ROUTE, EXT_REQ_DEP_ENQUEUE) or
                                // unique core ID of worker to execute task
                                // (EXT_REQ_EXEC)
  void          *ptr;           // Object (REQ_FREE, REQ_PACK, EXT_REQ_PACK,
                                // REQ_QUERY_POINTER) or update remote parent
                                // flag, or number of objects (REQ_BALLOC), or
                                // start address of pages/rids
                                // (REQ_RETURN_PAGES, REQ_RETURN_RIDS), or
                                // level hint (REQ_RALLOC), or child region ID
                                // (REQ_RFREE_UPDATE_PARENT), or running task
                                // load (REQ_LOAD_REPORT) or parent task ID +
                                // task idx to spawn (EXT_REQ_SPAWN,
                                // EXT_REQ_DELEGATE) or parent task ID
                                // (EXT_REQ_DEP_START) or new task ID + task
                                // idx to exec (EXT_REQ_EXEC, REQ_EXEC_DONE) or
                                // dependency object/region (EXT_REQ_DEP_ROUTE,
                                // EXT_REQ_DEP_ENQUEUE, REQ_DEP_OK) or child
                                // RW decrement (REQ_DEP_CHILD_FREE) or local
                                // task desc. pointer (SELF_SCHEDULE_RESULT)

  // Extended part of request (exists only for extended requests)
  
  void          *data                // Array of region IDs and object pointers
                  [PR_REQ_MAX_SIZE]; // (EXT_REQ_PACK) or task arguments
                                     // (EXT_REQ_SPAWN, EXT_REQ_DELEGATE,
                                     // EXT_REQ_DEP_START, EXT_REQ_DEP_STOP,
                                     // EXT_REQ_UPDATE_LOCATION) or task 
                                     // arguments in req->size first positions
                                     // followed by packed list of addresses and
                                     // sizes/location/options in the rest of
                                     // the positions (EXT_REQ_EXEC), or stack
                                     // of parent regions (EXT_REQ_DEP_ROUTE,
                                     // EXT_REQ_DEP_ENQUEUE), or location and
                                     // weights (EXT_REQ_SCHEDULE)
  int           num_regions;         // Number of regions IDs in array. They
                                     // are in array slots 0 ... num_regions-1
                                     // (EXT_REQ_PACK, EXT_REQ_DEP_ROUTE, 
                                     // EXT_REQ_DEP_ENQUEUE). For
                                     // EXT_REQ_SPAWN, EXT_REQ_DELEGATE and
                                     // EXT_REQ_EXEC, if 0 it's a new task
                                     // spawn, if 1 it's a continuing request
                                     // from previous message(s) with the same
                                     // req_id. For EXT_REQ_DEP_START, new task
                                     // ID. For EXT_REQ_DEP_STOP, task ID
                                     // being collected. For 
                                     // EXT_REQ_UPDATE_LOCATION, the new
                                     // location. For EXT_REQ_SCHEDULE, number
                                     // of location/weights in array.
  int           num_ptrs;            // Number of pointers in array. They are
                                     // in slots num_regions ... 
                                     //          (num_regions + num_ptrs-1) 
                                     // (EXT_REQ_PACK). For EXT_REQ_SPAWN,
                                     // EXT_REQ_DELEGATE and EXT_REQ_EXEC
                                     // number of arguments in data: if -1,
                                     // data has PR_REQ_MAX_SIZE args and more
                                     // follow on new request(s). For
                                     // EXT_REQ_DEP_START, EXT_REQ_DEP_STOP and
                                     // EXT_REQ_UPDATE_LOCATION, number of
                                     // arguments in data (but no -1 allowed).
                                     // For EXT_REQ_DEP_ROUTE and
                                     // EXT_REQ_DEP_ENQUEUE, new task ID + dep
                                     // flags.
} PrMsgReq;


typedef struct {

  // Basic part of reply (exists for all basic and extended replies)

  int           type;           // Message type. Its actual type is PrMsgType,
                                // but we declare it as int to avoid GCC
                                // optimizing it differently across ARM/MB
                                // architectures (this actually happened...)
  int           req_id;         // Request ID this reply refers to
  int           status;         // Reply status (0: success, other: error code)
  void          *result;        // Resulting pointer, region ID, page 
                                // starting address of request or core ID
  size_t        size;           // Number of pages/rids (for REPLY_GET_PAGES,
                                // REPLY_GET_RIDS) or object size (for
                                // REPLY_QUERY_POINTER)

  // Extended part of reply (exists only for extended replies)
  
  void          *adr_ar              // Array of base addresses
                  [PR_RPL_MAX_SIZE];
  int           size_ar              // Array of sizes for each base address
                  [PR_RPL_MAX_SIZE];
  int           num_elements;        // Number of base addresses and sizes.
                                     // If -1, the number is PR_RPL_MAX_SIZE
                                     // and another reply follows with more.
} PrMsgReply;


// ===========================================================================
// Event handling
// ===========================================================================

// Available actions
#define PR_ACT_FORWARD          (1 << 0) // Forward reply as is to requestor
#define PR_ACT_REDO             (1 << 1) // Reprocess request as-is
#define PR_ACT_REENTRANT        (1 << 2) // Reprocess request, using data hook
                                         // as state for reentering
#define PR_ACT_REDO_ERRORS      (1 << 3) // Same as PR_ACT_REDO, but call
                                         // process handler even if error
#define PR_ACT_REENTRANT_ERRORS (1 << 4) // Same as PR_ACT_REENTRANT, but call
                                         // process handler even if error

// Generic pending event
typedef struct PrEvtPendingStruct PrEvtPending;
struct PrEvtPendingStruct {

  PrMsgReq      *req;           // Original request this event refers to.
  int           action;         // Action to do when event wakes up
  PrEvtPending  *prev;          // Previous event in this list
  PrEvtPending  *next;          // Next event in this list
  void          *data;          // Event-specific data hook
};


// Case-specific event data hooks
typedef struct {

  void          **objects;      // Objects allocated so far
  int           num_objects;    // Number of objects in array

} PrEvtHookBalloc;

typedef struct {

  size_t        *addresses;     // Addresses collected so far
  int           *sizes;         // Sizes collected so far
  int           num_elements;   // Number of addresses & sizes in above arrays
  int           alloc_elements; // Allocated size of above arrays (may be
                                // bigger than num_elements)
  int           wait_replies;   // How many replies do we still wait from
                                // other schedulers, before we can answer
  unsigned int  native_task_id; // 0:  When finished, send reply(ies) back to
                                //     the core that requested the packing
                                // >0: When finished, call the appropriate
                                //     handler for this task ID, because
                                //     packing was initiated by our scheduler
                                //     for a native task
  void          *error_ptr;     // Object or region ID that caused the error
  int           error_status;   // If < 0, pack failed with this error status

} PrEvtHookPack;

typedef struct {

  int           wait_replies;   // How many replies do we still wait from
                                // other schedulers, before we can repeat
                                // the local region freeing
} PrEvtHookRfree;

typedef struct {

  void          **args;         // Task arguments array
  unsigned char *deps;          // Task dependency types
  int           num_args;       // Number of task arguments/dependencies
  unsigned int  parent_task_id; // Task ID of the parent task

} PrEvtHookDoDeps;


// Incomplete multi-part request holding
typedef struct {

  int           req_id;         // Request ID of core_id (same for all parts)
  int           core_id;        // Source core_id

  PrMsgReq      **parts;        // Array of received request parts
  int           num_parts;      // Number of received request parts in array

  int           forward_idx;    // Index of child we're forwarding this multi-
                                // part request
  int           forward_msg_id; // Message ID we're using for the forwarding

} PrMultiPartReq;


// ===========================================================================
// Task-related
// ===========================================================================

#define PR_TASK_ID_SIZE      24 // Bits of task ID field (unique ID of every
                                // monitored task, high PR_TASK_ID_SCH_BITS
                                // is the responsible scheduler)
#define PR_TASK_IDX_SIZE     8  // Bits of task index field (index of the
                                // compiler-supplied task table). Used to
                                // pack the task index with a task ID, so
                                // PR_TASK_ID_SIZE + PR_TASK_IDX_SIZE must be 
                                // <= machine word size. Also used to pack
                                // dep flags with a task ID.

#define PR_TASK_ID_SCH_BITS  8  // Part of task ID field (high bits of the
                                // PR_TASK_ID_SIZE total bits) which specify
                                // who's the responsible scheduler

#define PR_TASK_MAX_ARGS     30 // Maximum number of arguments (used for sanity
                                // checks only)

// Load balancing reporting granularity
#define PR_LOAD_REPORT_CHANGE   0.25     // Percentile (0...1) change of task
                                         // load in order to generate a new
                                         // load balance message (not to be
                                         // confused with MM_LOAD_REPORT_CHANGE)


// Brief guide on what messages are exchanged during a task's life
// ---------------------------------------------------------------
//
// 1. Spawning
//
//    A worker which executes the parent task spawns a new one: EXT_REQ_SPAWN
//    is sent to his leaf scheduler, which is forwarded upstream until it
//    reaches the responsible scheduler for the parent task.
//
//    This scheduler decides if it wants to create the new task there or if
//    it wants to delegate it. In the latter case, the task creation is 
//    delegated downstream (EXT_REQ_DELEGATE) until it reaches a scheduler
//    that accepts the child task and creates it locally. 
//
// 2. Dependency analysis
//
//    The scheduler of the child task begins the dependency analysis. The task
//    arguments are separated per children scheduler subtrees, and for each 
//    child scheduler an EXT_REQ_DEP_START is sent, which may continue
//    downstream until all schedulers for all task arguments are reached. For
//    each argument, each responsible scheduler starts discovering parent 
//    regions, issuing an EXT_REQ_DEP_ROUTE which continues upstream. This
//    routing discovers the path from the task argument up to the level that
//    the parent task is enqueued (and also marks the highest level that this
//    happens). The routing stops at the scheduler responsible for the parent
//    task.
//
//    After this first, distributed, down-up traversal of the region tree, we
//    now know the region-to-region path we have to follow and where the parent
//    task is enqueued. So, we begin a second traversal to actually do the
//    enqueueing. A downstream EXT_REQ_DEP_ENQUEUE begins for each argument,
//    which will enqueue the new task into the queue of the first level that is
//    blocked, starting from the highest level that the routing has discovered
//    and ending down at the argument queue itself. If and when the enqueueing
//    reaches the wanted argument queue and is at the head of the queue, a
//    final upstream REQ_DEP_OK is sent to the new task scheduler to indicate
//    that the waiting for this argument dependency is finished.
//
// 3. Packing & Scheduling
//
//    When all dependencies are satisfied, the new task scheduler requests
//    a packing operation for all the task arguments from the memory subsystem.
//    The packing assembles a merged list of address ranges and sizes of all
//    the task arguments (regions/objects), along with the information who
//    was the last producer of each such range. This is yet again a distributed
//    process which generates upstream/downstream messages among all schedulers
//    that are responsible for the task argument regions and objects (we don't
//    describe further all the process here --- the message types exchanged
//    are REQ_PACK, EXT_REQ_PACK and EXT_REPLY_PACK).
//
//    When the final address range list is received by the new task scheduler,
//    a hiearchical scheduling decision is reached as follows. The initial
//    scheduler decides which of its children subtrees is more suitable to
//    run the new task, based on the periodic load information he has for its
//    children as well as judging by the locality of the packed address range
//    list compared to the core IDs in the subtree. Higher-level schedulers
//    send a EXT_REQ_SCHEDULE message downstream to the selected child; leaf
//    schedulers decide which of their workers is suitable, based on the same
//    alorithm, and reply upstream with a REPLY_SCHEDULE message. This is
//    forwarded back to the new task scheduler, which now knows who is the
//    assigned worker for the new task.
//
// 4. Update location and Dispatch
//
//    When the task is scheduled, the new task scheduler dispatches the task
//    to be executed (EXT_REQ_EXEC, forwarded downstream). When it reaches the
//    designated worker it is placed on the worker queue of tasks to be
//    executed.
//
//    In the meanwhile, the new task scheduler also updates the location of all
//    the task arguments to be the newly assigned worker core. This may
//    generate EXT_REQ_UPDATE_LOCATION messages that flow downstream untill all
//    region/object locations are updated (note that when a task gets a region,
//    it also gets all its children regions, so this is also a hierarchical
//    operation).
//
// 5. DMAs and Execution
//
//    When an EXT_REQ_EXEC reaches its designated worker, it is placed on the
//    worker queue of tasks to be executed. When it reaches the head of the
//    queue it can begin running.
//
//    The worker is responsible to start and monitor all the DMAs that will be
//    needed to transfer task arguments from their last producer worker core
//    locally, BEFORE the task is allowed to run. Ideally, DMAs for the next
//    task in the queue are started before the current task begins executing,
//    so that the new task DMAs can be finished while the current task
//    executes.
//
//    DMAs are started and the worker assigns a local hardware counter to
//    monitor their progress. The worker event loop is modified to handle a new
//    message and then check on the progress of existing DMAs. Note that
//    contrary to common messaging, the DMAs are opportunistic (i.e. there are
//    no guarantees that the remote source cores DMA engines can handle them).
//    In the case a DMA receives a Nack, the worker restarts the DMA until
//    it succeeds.
//
//    When the DMAs for the current task are successfully completed, the
//    current task begins execution.
//
// 6. Collection
//
//    When the task finishes execution on the worker core, it sends a message
//    to its leaf scheduler (REQ_EXEC_DONE), which is forwarded upstream
//    towards the responsible task scheduler. Similar to the start of the 
//    dependency analysis, the task arguments are separated per children
//    scheduler subtrees, and for each child scheduler an EXT_REQ_DEP_STOP is
//    sent, which may continue downstream until all schedulers for all task
//    arguments are reached. 
//
//    Each such scheduler, for each task argument, removes the task from the
//    dependency queue and sees if anyone is next; if so, a REQ_DEP_OK is sent
//    as described in the dependency analysis above. If there are tasks waiting
//    on this queue but their real target is further down, they are unblocked
//    and continue their descent as described above. If no more tasks wait in
//    this queue, then (provided no children regions are busy) the parent
//    region is examined for tasks waiting there. If the parent region belongs
//    to our parent scheduler, a REQ_DEP_CHILD_FREE message is sent. The 
//    ascension continues upstream until a task is found or the top-level
//    region is reached.


// Task descriptor. This is used in two places:
//
// - Held by a single responsible scheduler in its task trie for the whole
//   life of the task
//
// - Held by the designated worker in its ready queue, after the task is
//   scheduled and dispatched to it
typedef struct {

  unsigned int  pid;            // Unique task ID of our parent task
  unsigned int  id;             // Unique task ID of this task
  int           index;          // Function index in the task table
  void          **args;         // Task arguments
  unsigned char *deps;          // Dependency array (one entry per arg).
                                // Comprises of the SYS_TYPE_* fields found
                                // in syscall.h
  int           num_args;       // Number of arguments/dependencies
  int           deps_waiting;   // Number of args to wait before task is
                                // free of dependencies w.r.t. other tasks
  int           dmas_started;   // When 1, DMAs for this task have been started
  int           dmas_waiting;   // Number of DMAs to wait before all task
                                // arguments have been safely transfered to
                                // the worker core that will execute it
  size_t        *dma_addresses; // Packed addresses to DMA
  int           *dma_sizes;     // Sizes + source locations + options to DMA
  int           num_dmas;       // Number of DMAs in two arrays above
  int           run_core_id;    // Core ID of the worker that the task is
                                // scheduled to run on. The value will be set
                                // to -1 until the task is scheduled.
  int           spawn_msg_id;   // When this task spawns another one, the 
                                // message ID of the spawn request we got
  int           spawn_core_id;  // When this task spawns another one, the core
                                // ID of the core who sent us the spawn request
  int           spawn_rem_deps; // While this task spawns another one, the
                                // remaining arguments of the dependency 
                                // analysis before we can send back the reply
} PrTaskDescr;


extern PrTaskDescr *pr_task_create(unsigned int par_task_id, int task_idx, 
                                   void **args, unsigned char *deps, 
                                   int num_args);
extern int         pr_task_dispatch(PrTaskDescr *task);
extern int         pr_task_collect(unsigned int task_id);
extern int         pr_task_must_delegate(PrMultiPartReq *multi,  
                                         PrMsgReq *final_part, int *ret_child);
extern int         pr_task_scheduled(PrTaskDescr *task);
extern int         pr_task_start_dmas(PrTaskDescr *task);


// ===========================================================================
// Scheduling
// ===========================================================================

#define PR_MAX_SCHEDULER_CHILDREN  512  // Maximum expected children per
                                        // scheduler instance. Increase this
                                        // based on desired core configuration
                                        // (74 is enough for 7 ARM L0
                                        // schedulers each having 73-74 MB
                                        // children in the 520-core het setup).

extern int pr_sched_start_scheduling(PrEvtHookPack *state);
extern int pr_sched_schedule(unsigned int task_id, int *locations, 
                             int *weights, int num_locations, int *ret_msg_id, 
                             int *ret_core_id);
extern int pr_sched_update_location(int location, int num_args, void **args,
                                    unsigned char *dep_flags, int req_mode);
extern int pr_sched_report_load();


// ===========================================================================
// Dependency analysis
// ===========================================================================

// Basic dependency structure
typedef struct {

  unsigned int  task_id;           // ID of the task waiting on this dependency
  unsigned int  par_task_id;       // ID of its parent task
  rid_t         par_highest_rid;   // Highest region the parent task is enqueued
                                   // or -1 for recursions
  void          *final_target;     // Final dependency target the task is
                                   // waiting for. This may be different than
                                   // the dep queue owner this PrDep is
                                   // enqueued in, if a task is blocked at a
                                   // higher level before reaching its target.
  unsigned char flags;             // Dependency flags (as defined in syscall.h)
  rid_t         route[             // Parent regions of final_target, where
                 PR_REQ_MAX_SIZE]; // entry 0 is its immediate parent and 
                                   // entry cur_route is the level of the
                                   // region this PrDep is enqueued in
  int           cur_route;         // Index of current level as described above

} PrDep;

// Per-object holding structure
typedef struct {

  int           parent_enqs_ro;   // How many task enqueues we have received by
                                  // our parent region since our creation,
                                  // for read-only access
  int           parent_enqs_rw;   // As above, for read/write access

  List          *dep_queue;       // Object dependency queue (data: PrDep *)

} PrObjDep;


// High-level functions
extern int      pr_dep_start_stop(int start_mode, unsigned int par_task_id, 
                             unsigned int task_id, int num_args, 
                             void **args, unsigned char *dep_flags);
extern int      pr_dep_route(unsigned int parent_task_id, 
                             unsigned int new_task_id, void *dep, 
                             unsigned char dep_flags, int dep_is_local,
                             int num_remote_parents, rid_t *remote_parents,
                             rid_t highest_remote_parent);
extern int      pr_dep_descend_enqueue(unsigned int parent_task_id,
                                       unsigned int new_task_id, void *dep,
                                       unsigned char dep_flags, int num_parents,
                                       rid_t *par_rids, rid_t highest_rid,
                                       rid_t dont_incr_parent_enq, int resume);
extern int      pr_dep_next(unsigned int task_id, void *dep, int dep_is_region,
                            int child_decr_mode, int child_decr_ro, 
                            int child_decr_rw);

// Queue management functions
extern int      pr_dep_enqueue(List *dep_queue, void *final_dep, 
                               unsigned char dep_flags, unsigned int task_id, 
                               unsigned int par_task_id, rid_t par_highest_rid,
                               unsigned int ref_task_id, int insert_mode,
                               rid_t *route, int cur_route, PrDep **ret_pr_dep);
extern ListNode *pr_dep_search_queue(List *dep_queue, unsigned int task_id);
extern int      pr_dep_at_head_of_queue(unsigned int task_id, void *dep);


// ===========================================================================
// Exported macros
// ===========================================================================
#define pr_advance_msg_id(n)    (((n) == PR_MAX_MSG_ID) ? 1 : (n) + 1)

#define pr_advance_task_id(n)   (((n) == (1 << (PR_TASK_ID_SIZE - \
                                                PR_TASK_ID_SCH_BITS)) - 1) ? \
                                         1 : (n) + 1)


// ===========================================================================
// Exported functions
// ===========================================================================
extern int         pr_event_process_message(void *main_in_buf, void *aux_in_buf,
                                            void *saved_state);
extern char        *pr_event_dbg_type_string(PrMsgType type);

extern void        pr_event_main_loop();
extern void        *pr_event_worker_inner_loop(int wait_mode, PrMsgType type, 
                                               int msg_id, PrTaskDescr *task);

extern int         pr_select_init(PrCfgMode select);
extern int         pr_init_app(func_t **task_table, void **args, int num_args);

extern PrRole      pr_get_role();
extern int         pr_get_core_id();
extern int         pr_get_worker_id();
extern int         pr_get_scheduler_id();
extern int         pr_get_parent_scheduler_id();
extern int         pr_get_num_cores();
extern int         pr_get_num_schedulers();
extern int         pr_get_num_workers();
extern int         pr_worker_core_id(int worker_id);
extern int         pr_scheduler_core_id(int sched_id);
extern int         pr_core_worker_id(int core_id);
extern int         pr_core_scheduler_id(int core_id);
extern int         pr_core_child_index(int core_id);
extern void        pr_core_arch_bid_cid(int core_id, int *ret_bid, 
                                        int *ret_cid);


#endif
