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
// Abstract      : Header file for memory management layer functionality
//
// =============================[ CVS Variables ]=============================
//
// File name     : $RCSfile: memory_management.h,v $
// CVS revision  : $Revision: 1.34 $
// Last modified : $Date: 2012/12/18 16:40:29 $
// Last author   : $Author: lyberis-spree $
//
// ===========================================================================

#ifndef _MEMORY_MANAGEMENT_H
#define _MEMORY_MANAGEMENT_H

#include <kernel_toolset.h>
#include <processing.h>
#include <noc.h>
#include <fmpi.h>
#include <types.h>


// Basic defines. Keep MM_ALLOC_ALIGN to cache-line boundaries -- our DMA
// engines depend on it. Also, keep MM_PAGE_SIZE to 1 MB for Formic boards
// if you plan to use TLB. ARM supports 1 MB (sections) and also some
// different pages. The slab size can be anything in between, but keep it
// a power of 2 and a multiple of MM_ALLOC_ALIGN.

#define MM_ALLOC_ALIGN          64        // 64-B memory objects alignment
#define MM_SLAB_SIZE            4096      // 4-KB slab size
#define MM_PAGE_SIZE            0x100000  // 1-MB page size


// ===========================================================================
// Memory Map for Myrmics targets
// ===========================================================================
#ifdef MYRMICS

// --- ARM Memory Map --------------------------------------------------------

// Virtual addresses
#define MM_ARM_VA_CODE_PAGE     0x00000000 // Match with arch/arm/init.c
#define MM_ARM_VA_STACK_PAGE    0x00100000 // Match with arch/arm/boot.s
#define MM_ARM_VA_PAGE_TABLE    0x00200000
#define MM_ARM_VA_PRINT_LOCK    0x002FFFFC

#define MM_ARM_VA_USER_BASE     0x00300000
#define MM_ARM_USER_SIZE        (93 * 1024 * 1024) // 93 MB total space
                             // 0x05F00000 // Last user space page

#define MM_ARM_VA_DEV0_PAGE     0x10000000 // Match with peripheral reg
#define MM_ARM_VA_DEV1_PAGE     0x1E000000 // addresses in arch/arm/*

#define MM_ARM_VA_KERNEL_BASE   0x84000000
#define MM_ARM_KERNEL_SIZE      (112 * 1024 * 1024) // 112 MB per core
                             // 0x9FF00000 // Last page of core #3 kernel

#define MM_ARM_VA_ARS0_PAGE     0xFFC00000 // Match with include/ars_regs.h
#define MM_ARM_VA_ARS1_PAGE     0xFFD00000 // Match with include/ars_regs.h
#define MM_ARM_VA_ARS2_PAGE     0xFFE00000 // Match with include/ars_regs.h
#define MM_ARM_VA_ARS3_PAGE     0xFFF00000 // Match with include/ars_regs.h

// Hardwired physical addresses. Note that code/stacks are in the middle
// of the physical address space because that's the point that is remapped to
// 0x0 during boot. User space is mapped below the code/stacks, kernel space
// is mapped above the code/stacks.
//
// WARNING: Make sure the ar_virt_to_phys() macro in arch.h is up-to-date
#define MM_ARM_PA_DEV0_PAGE     0x10000000
#define MM_ARM_PA_DEV1_PAGE     0x1E000000

#define MM_ARM_PA_USER_BASE     0x60300000
                             // 0x65F00000 // Last user space page

#define MM_ARM_PA_CODE_PAGE     0x80000000
#define MM_ARM_PA_STACK_PAGE    0x80100000
#define MM_ARM_PA_PAGE_TABLE    0x80200000
                             // 0x83F00000 // 64-MB remap section ends here

#define MM_ARM_PA_KERNEL_BASE   0x84000000
                             // 0x9FF00000 // Last page of core #3 kernel

#define MM_ARM_PA_ARS0_PAGE     0xFFC00000 // WARNING: If these ever change,
#define MM_ARM_PA_ARS1_PAGE     0xFFD00000 //          update ar_virt_to_phys().
#define MM_ARM_PA_ARS2_PAGE     0xFFE00000 //          Also make all arch funcs
#define MM_ARM_PA_ARS3_PAGE     0xFFF00000 //          that use ARM peripheral
                                           //          addresses to call
                                           //          ar_virt_to_phys() --
                                           //          now we don't do that.

// ---------------------------------------------------------------------------


// --- MicroBlaze Memory Map -------------------------------------------------

// Virtual addresses
#define MM_MB_VA_CODE_PAGE      0x00000000 // Match with arch/mb/init.c
#define MM_MB_VA_STACK_PAGE     0x00100000 // Match with arch/mb/boot.s and
#define MM_MB_VA_PRINT_BUF      0x00200000 //       arch/mb/init.c
#define MM_MB_PRINT_BUF_SIZE    (4 * 1024) // 4 KB per core
                             // 0x00208000 // next available after print buf

#define MM_MB_VA_USER_BASE      0x00300000
#define MM_MB_USER_SIZE         (93 * 1024 * 1024) // 93 MB total space
                             // 0x05F00000 // last user space page

#define MM_MB_VA_KERNEL_BASE    0x06000000
#define MM_MB_KERNEL_SIZE       (4 * 1024 * 1024) // 4 MB per core
                             // 0x07F00000 // last page of core #7 kernel

                             // 0xFFF00000 // MBS page (private per core)

// For the moment, TLB is in bypass mode and maps these 1-1 to the same
// physical addresses, so we do not define MM_PA_* macros here.

// ---------------------------------------------------------------------------


// Common names
#ifdef ARCH_ARM
#define MM_VA_USER_BASE         MM_ARM_VA_USER_BASE
#define MM_PA_USER_BASE         MM_ARM_PA_USER_BASE
#define MM_USER_SIZE            MM_ARM_USER_SIZE
#define MM_VA_KERNEL_BASE       MM_ARM_VA_KERNEL_BASE
#define MM_PA_KERNEL_BASE       MM_ARM_PA_KERNEL_BASE
#define MM_KERNEL_SIZE          MM_ARM_KERNEL_SIZE
#endif
#ifdef __MICROBLAZE__
#define MM_VA_USER_BASE         MM_MB_VA_USER_BASE
#define MM_PA_USER_BASE         MM_MB_PA_USER_BASE
#define MM_USER_SIZE            MM_MB_USER_SIZE
#define MM_VA_KERNEL_BASE       MM_MB_VA_KERNEL_BASE
#define MM_PA_KERNEL_BASE       MM_MB_PA_KERNEL_BASE
#define MM_KERNEL_SIZE          MM_MB_KERNEL_SIZE
#endif

#endif // MYRMICS ifdef


// ===========================================================================
// Memory Map for FMPI targets
// ===========================================================================
#ifdef FMPI

// --- ARM Memory Map --------------------------------------------------------

// Virtual addresses
#define MM_ARM_VA_CODE_PAGE     0x00000000 // Match with arch/arm/init.c
#define MM_ARM_VA_STACK_PAGE    0x00100000 // Match with arch/arm/boot.s
#define MM_ARM_VA_PAGE_TABLE    0x00200000
#define MM_ARM_VA_PRINT_LOCK    0x002FFFFC

#define MM_ARM_VA_DEV0_PAGE     0x10000000 // Match with peripheral reg
#define MM_ARM_VA_DEV1_PAGE     0x1E000000 // addresses in arch/arm/*

#define MM_ARM_VA_KERNEL_BASE   0x20000000
#define MM_ARM_KERNEL_SIZE      (128 * 1024 * 1024) // 128 MB per core
                             // 0x3FF00000 // Last page of core #3 kernel

#define MM_ARM_VA_ARS0_PAGE     0xFFC00000 // Match with include/ars_regs.h
#define MM_ARM_VA_ARS1_PAGE     0xFFD00000 // Match with include/ars_regs.h
#define MM_ARM_VA_ARS2_PAGE     0xFFE00000 // Match with include/ars_regs.h
#define MM_ARM_VA_ARS3_PAGE     0xFFF00000 // Match with include/ars_regs.h

// Hardwired physical addresses. Note that code/stacks are in the middle
// of the physical address space because that's the point that is remapped to
// 0x0 during boot. User space is mapped below the code/stacks, kernel space
// is mapped above the code/stacks.
//
// WARNING: Make sure the ar_virt_to_phys() macro in arch.h is up-to-date
#define MM_ARM_PA_DEV0_PAGE     0x10000000
#define MM_ARM_PA_DEV1_PAGE     0x1E000000

#define MM_ARM_PA_KERNEL_BASE   0x60000000
                             // 0x7FF00000 // Last page of core #3 kernel

#define MM_ARM_PA_CODE_PAGE     0x80000000
#define MM_ARM_PA_STACK_PAGE    0x80100000
#define MM_ARM_PA_PAGE_TABLE    0x80200000
                             // 0x83F00000 // 64-MB remap section ends here

#define MM_ARM_PA_ARS0_PAGE     0xFFC00000 // WARNING: If these ever change,
#define MM_ARM_PA_ARS1_PAGE     0xFFD00000 //          update ar_virt_to_phys().
#define MM_ARM_PA_ARS2_PAGE     0xFFE00000 //          Also make all arch funcs
#define MM_ARM_PA_ARS3_PAGE     0xFFF00000 //          that use ARM peripheral
                                           //          addresses to call
                                           //          ar_virt_to_phys() --
                                           //          now we don't do that.

// ---------------------------------------------------------------------------


// --- MicroBlaze Memory Map -------------------------------------------------

// Virtual addresses
#define MM_MB_VA_CODE_PAGE      0x00000000 // Match with arch/mb/init.c
#define MM_MB_VA_STACK_PAGE     0x00100000 // Match with arch/mb/boot.s and
#define MM_MB_VA_PRINT_BUF      0x00200000 //       arch/mb/init.c
#define MM_MB_PRINT_BUF_SIZE    (4 * 1024) // 4 KB per core
                             // 0x00208000 // next available after print buf

#define MM_MB_VA_KERNEL_BASE    0x00800000
#define MM_MB_KERNEL_SIZE       (15 * 1024 * 1024) // 15 MB per core
                             // 0x07F00000 // last page of core #7 kernel

                             // 0xFFF00000 // MBS page (private per core)

// For the moment, TLB is in bypass mode and maps these 1-1 to the same
// physical addresses, so we do not define MM_PA_* macros here.

// ---------------------------------------------------------------------------


// Common names
#ifdef ARCH_ARM
#define MM_VA_KERNEL_BASE       MM_ARM_VA_KERNEL_BASE
#define MM_PA_KERNEL_BASE       MM_ARM_PA_KERNEL_BASE
#define MM_KERNEL_SIZE          MM_ARM_KERNEL_SIZE
#endif
#ifdef __MICROBLAZE__
#define MM_VA_KERNEL_BASE       MM_MB_VA_KERNEL_BASE
#define MM_PA_KERNEL_BASE       MM_MB_PA_KERNEL_BASE
#define MM_KERNEL_SIZE          MM_MB_KERNEL_SIZE
#endif

#endif // FMPI ifdef


// Macros to get the per-core address limits
static inline unsigned int mm_va_kernel_base(int cid) {
  return MM_VA_KERNEL_BASE + cid * MM_KERNEL_SIZE;
}

#ifdef ARCH_ARM
static inline unsigned int mm_pa_kernel_base(int cid) {
  return MM_PA_KERNEL_BASE + cid * MM_KERNEL_SIZE;
}
#endif


// ===========================================================================
// Slab allocator
// ===========================================================================

// Bit fields of pointers returned by alloc functions
#define MM_TRIES_POINTER_MSB    31      // Set for 32-bit mem allocations
#define MM_TRIES_POINTER_LSB    12      // Set for 4-KB slabs

// Bit fields of slot sizes supported by alloc functions
#define MM_TRIES_SLOT_MSB       31      // Set for 32-bit requests
#define MM_TRIES_SLOT_LSB       6       // Set for 64-B aligned slots

// Bootstap and preallocation sizes
#define MM_BOOTSTRAP_MAX_SLOT   896     // Max allocation length of objects at
                                        // bootstrap (MM_ALLOC_ALIGN multiple)

#define MM_BOOTSTRAP_SLABS_STEP 16      // Slabs per slot size at bootstrap.
                                        // Also, this is the preferred number
                                        // of slabs taken from the free pool
                                        // and given to the partial pool (for
                                        // small sizes, if enough are available)

#define MM_KERNEL_MIN_EMPTY     4       // For the kernel slab pool, minimum
                                        // number of preallocated empty slabs
                                        // for small slots ( < MM_SLAB_SIZE).

#define MM_MAX_DEFERRED_FREES   64      // For the kernel slab pool, maximum
                                        // number of kt_frees that must be
                                        // done later, because we're busy
                                        // servicing another free


// Examples of slab state usage and bitmaps. Examples assume slab size = 4096.
//
// Example 1:  slot_size = 128, means 32 full slots per slab (128 x 32 = 4096).
//             bitmap_size = 32, offset = 0. Slots: 0, 128, 256, ... , 4032.
//             Slab has nothing to do with adjacent slabs, so num_slabs = 1.
//
// Example 2a: slot_size = 1536, offset = 0. It means that: the first object
//             starts at 0 and fits fully. The second at 1536 and fits. The
//             third starts at 3072 and does not fit fully. Still,
//             bitmap_size = 3 (because 3 objects can _start_ here). If the
//             last slot is not used, bitmap[2] = 0 and num_slabs = 1.
//
// Example 2b: (continues from ex. 2a above). If the 3rd object of ex. 2a is
//             filled, it implies an adjacent slab has been found and used.
//             This adjacent slab will have: slot_size = 1536, offset = 512,
//             trailer_bit = 1. Its first _start_ of an object will be at 512
//             and will be represented by bitmap[0]. The leftover object status
//             is represented by trailer_bit. bitmap_size = 3 again, because
//             three objects can start here (obj. 512, 2048, 3584). Note that
//             the previous slab will have bitmap[2] = 1 and num_slabs = 2 in
//             this case.
//
// Example 3:  slot_size = 10048, offset = 0. Now bitmap_size = 1, because
//             only a single slot can start here (at offset 0). If this slot
//             is filled, _two_ adjacent slabs must be used. On this slab,
//             bitmap[0] = 1 and num_slabs = 3 (meaning this slab, the 2nd
//             one and part of the 3rd one). Note that for the 2nd slab
//             there is no accounting info anywhere anymore, except the
//             num_slabs in the 1st slab (and that's why we're doing this:
//             so if someone allocates 128MB, we only create a single slab
//             entry with num_slabs = 32768). Also note that the 3rd slab
//             will have trailer_bit = 1 and offset = 1856 (because
//             4096 + 4096 + 1586 = 10048) and its bitmap_size = 1, because
//             a single new 10048 slot can start there.

// Data structures
typedef enum {

  SLAB_EMPTY,   // All slots empty
  SLAB_PARTIAL, // Some slots available
  SLAB_FULL     // All slots full

} MmSlabState;


typedef struct MmSlabStruct MmSlab;
struct MmSlabStruct {

  unsigned int bitmap[MM_SLAB_SIZE /   // Bitmap of allocated/free slots. Bits
                      MM_ALLOC_ALIGN / // set to 1 indicate free objects. Slot
                      8 / 4];          // 0 is represented by bitmap[0] LSB.

  char         bitmap_size;            // Exact bitmap bits used, based on slot
                                       // size (array above is sized for the
                                       // worst case)

  char         trailer_bit;            // Status bit of leftover object from
                                       // previous adjacent slab(s). As with
                                       // bitmap, 1 means free. This field
                                       // makes sense only when offset > 0.

  MmSlabState state;                   // Slab state

  int         free_slots;              // Number of free slots, not including
                                       // trailer (if any). Max is bitmap_size.

  size_t      address;                 // Starting address (MM_SLAB_SIZE
                                       // aligned) of the slab

  int         slot_size;               // Size of slots in this slab
                                       // (MM_ALLOC_ALIGN aligned)

  int         offset;                  // Offset of the first _start_ of a
                                       // slot found in this slab. Examples:
                                       // - offset == 0 => bitmap[0] shows the
                                       //   first slot status, starting at 0.
                                       // - offset == 64 => trailer_bit shows
                                       //   the status of an object starting
                                       //   at adjacently previous slab(s),
                                       //   bitmap[0] shows the first start of
                                       //   a slot beginning at byte 64.

  int         num_slabs;               // Number of MM_SLAB_SIZE units
                                       // used by the last slot. If 1, means
                                       // that the slot does not span to
                                       // adjacent slabs (its end is aligned).
                                       // If e.g. 3, it means last slot starts
                                       // here, covers the whole next adj. slab
                                       // and ends at the next one.
                                       // If state == EMPTY, num_slabs shows
                                       // how many adjacent slabs are free
                                       // including this one.

  MmSlab      *prev;                   // Linked list pointers for partial
  MmSlab      *next;                   // slabs (siblings with same slot_size)
};


typedef struct {

  MmSlab      *head;                   // Partial list head
  int         num_members;             // Number of all members in the list
  int         num_empty_members;       // Number of empty members in the list
  size_t      used_slots;              // Total used slots by all members
  size_t      free_slots;              // Total free slots in all members.
                                       // Note that this is inaccurate in case
                                       // of preallocated empty slabs in
                                       // conjuction with non-aligned slots.
} MmSlabPartialHead;


typedef struct MmSlabPoolStruct MmSlabPool;
struct MmSlabPoolStruct {

  int         is_kernel;               // If 1, this is the kernel slab pool
  MmSlabPool  *metadata_pool;          // Slab pool used for metadata
  Trie        *used_slabs;             // Partial or full slabs
                                       // (key: address, data: MmSlab *)
  Trie        *partial_slabs;          // Partial slabs (key: slot_size,
                                       // data: MmSlabPartialHead *)
  Trie        *free_slabs;             // Free slabs (key: address,
                                       // data: MmSlab *)
  size_t      used_space;              // Total pool used space
  size_t      free_space;              // Total pool free space

};


// Exported functions
extern MmSlabPool *mm_slab_create_pool(MmSlabPool *metadata_pool, size_t adr,
                                      int num_slabs, int is_kernel);
extern int mm_slab_destroy_pool(MmSlabPool *pool, int complete_destruction);
extern int mm_slab_reduce_pool(MmSlabPool *pool, int num_slabs,
                               size_t *ret_adr);
extern int mm_slab_expand_pool(MmSlabPool *pool, size_t adr, int num_slabs);

extern int mm_slab_alloc_slot(MmSlabPool *pool, int size, size_t *ret_adr);
extern int mm_slab_free_slot(MmSlabPool *pool, size_t adr);

extern int mm_slab_query_pointer(MmSlabPool *pool, size_t adr);
extern int mm_slab_query_pool(MmSlabPool *pool, int pack_options,
                              int default_location, Trie *exception_locations,
                              int *alloc_num_elements, int *num_elements,
                              int **sizes, size_t **addresses);

// Debugging functions
#if 0
void mm_slab_dbg_pool_print(MmSlabPool *pool);
void mm_slab_dbg_pool_dotty(MmSlabPool *pool, char *filename);
#endif



// ===========================================================================
// Local region-based memory allocation
// ===========================================================================

// Both of the below must be powers of 2.

#define MM_ADR_RANGE_CHUNK_MAX   256      // Max slabs to give to regions when
                                          // no memory pressure (initially):
                                          // 1 MB
#define MM_ADR_RANGE_CHUNK_MIN   32       // Min slabs to give to regions under
                                          // memory pressure, before asking for
                                          // more memory: 128 KB

#define MM_LOCAL_FREE_PAGES_MAX  8        // Max and min pages to retain in the
                                          // local scheduler free pool before
                                          // returning pages back / asking
#define MM_LOCAL_FREE_PAGES_MIN  2        // for more pages to / from parent.
                                          // Higher-level schedulers will trade
                                          // in multiples of these, according
                                          // to level.

#define MM_LOCAL_FREE_RIDS_MAX   1024     // Number or region IDs to retain
                                          // in local scheduler free pool
                                          // before returning IDs back / asking
#define MM_LOCAL_FREE_RIDS_MIN   64       // for more IDs to / from parent.
                                          // Higher-level schedulers will trade
                                          // in multiples of these, according
                                          // to level.


// Region IDs size and type. Note that we use signed integers for things like
// "number of region IDs", so don't max the following to full unsigned size.
#define MM_TRIES_RID_MSB       30         // Region ID tries max bit position
#define MM_MAX_RID             0x7FFFFFFF // Maximum region ID allowable,
                                          // according to the above MSB

// For region packing, number of high bits that will be used in the size
// array entries to indicate the location of the data and any option bits. Note
// that MM_PACK_LOCATION_BITS must be big enough to hold the log2 of the number
// of cores, and MM_PACK_SIZE_BITS restricts the maximum DMA size.
//
// The same structure is used for the DMA sizes array in the task descriptor.
#define MM_PACK_OPTION_BITS     2       // All set for 32-bit archs
#define MM_PACK_LOCATION_BITS   10
#define MM_PACK_SIZE_BITS       20

#define MM_PACK_OPTION_RW       1       // Both must fit in MM_PACK_OPTION_BITS
#define MM_PACK_OPTION_RO       2

typedef struct MmRgnTreeNodeStruct MmRgnTreeNode;

typedef struct {

  rid_t         rid;            // Starting region ID
  int           num_rids;       // Number of IDs
  int           sched_id;       // Owner scheduler (for used_rids only; free
                                // rids have -1 here). It always refers to
                                // ourself or one of our next-level children
                                // schedulers.
} MmRgnRidRange;

typedef struct {

  size_t        address;        // Starting address
  int           num_slabs;      // Number of slabs
  MmRgnTreeNode *node;          // Region node (for local and used ranges only;
                                // free ranges as well as delegated ranges have
                                // NULL here)
  int           sched_id;       // Owner scheduler (for used_ranges only;
                                // free ranges have -1 here). It always refers
                                // to ourself or one of our next-level children
                                // schedulers.
} MmRgnAdrRange;

struct MmRgnTreeNodeStruct {

  rid_t         id;               // Region ID
  MmSlabPool    *pool;            // Slab pool, or NULL, if region is remote
                                  // (managed by another scheduler)
  MmRgnAdrRange **ranges;         // Array of used address ranges (or NULL if
                                  // region is remote)
  int           num_ranges;       // Number of ranges in the array
  rid_t         parent_id;        // Parent region ID

  rid_t         *children_ids;    // Children region IDs
  int           num_children;     // Number of children regions

  List          *dep_queue;       // Dependency queue for region (data: PrDep *)
  Trie          *obj_dep_queues;  // Dependency queue holders for objects that
                                  // belong to the region and are specifically
                                  // named as task dependencies (key: obj ptr,
                                  // data: PrObjDep *)

  int           children_enqs_ro; // How many task enqueues we have sent to our
                                  // direct region children for read-only access
  int           children_enqs_rw; // As above, for read/write access
  int           obj_enqs_ro;      // How many task enqueues we have sent to
                                  // our object children for read-only access
  int           obj_enqs_rw;      // As above, for read/write access
  int           parent_enqs_ro;   // How many task enqueues we have received by
                                  // our parent since the last time we reported
                                  // we were empty for read-only access
  int           parent_enqs_rw;   // As above, for read/write access

  int           location;         // Core ID of the last writer of the whole
                                  // region (minus possible exceptions of
                                  // objects found in obj_locations)
  Trie          *obj_locations;   // Core ID of the last writer for objects
                                  // that were separated from the rest of the
                                  // region, because some task(s) named them as
                                  // explicit dependencies (key: obj ptr,
                                  // data: core ID)
};


// Exported functions
extern int mm_region_init(int top_level);
extern int mm_region_create_region(rid_t parent, int remote_mode,
                                   int remote_location, int level_hint,
                                   rid_t *ret_rid);
extern int mm_region_update_parent(rid_t parent, rid_t child, int add_mode);
extern int mm_region_destroy_region(rid_t region, rid_t **ret_err_regions,
                                    int *ret_num_err_regions,
                                    int *ret_num_frees);

extern int mm_region_alloc_object(rid_t region, size_t size, void **ret_ptr);
extern int mm_region_free_object(void *ptr);

extern int mm_region_query_region(rid_t region, rid_t *ret_par_rid);
extern int mm_region_query_pointer(void *ptr, size_t *ret_size,
                                   rid_t *ret_rid);
extern int mm_region_pack_region(rid_t region, int pack_options, int recurse,
                                 int *alloc_num_elements, int *num_elements,
                                 size_t **addresses, int **sizes,
                                 rid_t **ret_err_children,
                                 int *ret_num_err_children);

extern int mm_region_add_rid_range(Trie *t, int sched_id, rid_t rid,
                                   int num_rids, MmRgnRidRange **ret_range);
extern int mm_region_reduce_rid_range(Trie *t, MmRgnRidRange *range, rid_t rid,
                                      int num_rids, MmRgnRidRange **ret_new);
extern int mm_region_get_rid_range(int num_rids, size_t *ret_rid);

extern int mm_region_add_adr_range(Trie *t, MmRgnTreeNode *node, int sched_id,
                                   size_t adr, int num_slabs,
                                   MmRgnAdrRange **ret_range);
extern int mm_region_reduce_adr_range(Trie *t, MmRgnAdrRange *range, size_t adr,
                                      int num_slabs, MmRgnAdrRange **ret_new);
extern int mm_region_get_adr_range(int min_slabs, size_t *ret_adr,
                                   int *ret_num_slabs);



// Debugging functions
#if 0
void mm_region_dbg_rtree_dotty(MmRgnTreeNode *root, char *filename);
void mm_region_dbg_print_all();
#endif


// ===========================================================================
// Distributed region-based memory allocation
// ===========================================================================

extern int mm_distr_object_sched_id(void *ptr);
extern int mm_distr_region_sched_id(rid_t region);
extern int mm_distr_get_pages(int num_pages);
extern int mm_distr_get_rids(int num_rids);

extern int mm_distr_alloc(size_t size, rid_t region);
extern int mm_distr_balloc(size_t size, rid_t region, int num_elements);
extern int mm_distr_free(void *ptr);

extern int mm_distr_ralloc(rid_t parent, int level_hint);
extern int mm_distr_ralloc_orphan(rid_t parent, int src_core_id);
extern int mm_distr_rfree(rid_t region, int update_remote_parent);
extern int mm_distr_rfree_update_parent(rid_t parent, rid_t child);

extern int mm_distr_pack(rid_t *regions, int *region_options, int num_regions,
                         void **objects, int *object_options, int num_objects,
                         int **ret_msg_ids, int *ret_num_messages,
                         void **ret_error_ptr);
extern int mm_distr_query_pointer(void *ptr);



// ===========================================================================
// Top-level memory allocation
// ===========================================================================

// Load balancing reporting granularity
#define MM_LOAD_REPORT_CHANGE   0.10     // Percentile (0...1) change of
                                         // regions load in order to generate
                                         // a new load balance message




// Exported functions
extern void mm_user_init(PrRole role);

extern int mm_alloc(PrMsgReq *req, void **ret_ptr);
extern int mm_balloc(PrMsgReq *req, PrEvtHookBalloc **state);
extern int mm_free(PrMsgReq *req);

extern int mm_ralloc(PrMsgReq *req, rid_t *ret_region);
extern int mm_ralloc_orphan(PrMsgReq *req, rid_t *ret_region);
extern int mm_ralloc_update_parent(PrMsgReq *stored_req,
                            PrMsgReply *child_reply);
extern int mm_rfree(PrMsgReq *req);
extern int mm_rfree_update_parent(PrMsgReq *req);

extern int mm_pack(PrMsgReq *req, PrTaskDescr *task, PrEvtHookPack **state);
extern int mm_query_pointer(PrMsgReq *req, size_t *ret_size,
                            rid_t *ret_rid);

extern int mm_get_pages(PrMsgReq *req, size_t *ret_adr);
extern int mm_insert_free_pages(size_t adr, int num_pages);
extern int mm_get_rids(PrMsgReq *req, size_t *ret_base);
extern int mm_insert_free_rids(size_t base, int num_rids);



// ===========================================================================
// Global context variable (unique per kernel and core ID)
// ===========================================================================
typedef struct {

  // Debug tracing
  unsigned int  *dbg_trc_data;      // Trace data array
  int           dbg_trc_idx;        // Current trace index
  unsigned int  dbg_trc_time_start; // Local timer upon trace initialization
  int           dbg_trc_offset;     // Timer offset to the top-level scheduler
                                    // during initialization
  // Debug statistics
  unsigned int  *dbg_stats_data;    // Array of statistics data
  unsigned int  dbg_stats_last_tmr; // Timer value at the time of the last
                                    // call to dbg_stats()

  // Filesystem
#ifdef ARCH_ARM
  unsigned int                     fs_flash_num_sectors;
  unsigned int                     fs_num_blocks;
  unsigned int                     fs_max_inodes;
  struct fs_current_state_struct   *fs_state;
  struct fs_fdesc_struct           *fs_fdesc; // Size FS_MAX_DESCRIPTORS
#endif

  // Network-on-chip
  NocMode       noc_mode;           // Network-on-chip communication mode
  List          *noc_cnt_free;      // Free list of available hardware counters
                                    // (data: int (type casted directly to the
                                    //  hardware counter nr.))
  void          *noc_send_buf;      // Single NOC_MESSAGE_SIZE bytes buffer
                                    // for NOC_MODE_MAILBOX_ONLY send mode
  void          *noc_recv_buf;      // Single NOC_MESSAGE_SIZE bytes buffer
                                    // for NOC_MODE_MAILBOX_ONLY receive mode
  int           noc_msg_core_id;    // When -1, no message is being prepared.
                                    // When >= 0, contains the destination core
                                    // ID to be used with noc_msg_send().
  NocCreditMode **noc_credits;      // Per-core array of fields for credit-based
                                    // noc modes. If NULL, we don't exchange
                                    // messages with this core ID.
  NocCreditMode **noc_credits_poll; // A smaller array consisting of only the
                                    // non-NULL entries of the noc_credits one,
                                    // i.e. one entry per communicating peer.
                                    // The two arrays point to the same
                                    // entries, but we need both for fast
                                    // indexing and polling in the two
                                    // credit-based modes.
  int           noc_num_peers;      // Number of communicating peers (and size
                                    // of the noc_credits_poll array)
  int           noc_poll_rr;        // First round-robin based position to poll
                                    // in the credit_only noc mode
  List          *noc_active_dmas;   // List of in-progress DMAs we're doing
                                    // (data: NocDma *)
#ifdef NOC_WARN_OUT_OF_CREDITS
  int           noc_cred_warned;    // When 1, we have already warned once per
                                    // core we encountered an out-of-credits
                                    // situation (which may lead to deadlock)
#endif

  // Slab allocator & kernel heap management
  int           mm_alloc_bootstrap; // When 1, mem allocations are still in
                                    // dummy mode (mem initialization under way)
  int           mm_frees_bootstrap; // When 1, mem frees are still in
                                    // dummy mode (mem initialization under way)
  MmSlabPool    *mm_kernel_pool;    // Kernel objects slab pool
  int           mm_recursion_depth; // Kernel pool malloc recursion counter
  int           mm_prealloc_flags   // Array of flags, marking which slot size
                  [MM_SLAB_SIZE /   // is low on (kernel pool) preallocated
                   MM_ALLOC_ALIGN]; // slabs
  int           mm_prealloc_needed; // When 1, preallocation is needed (one of
                                    // the above flags is set)
  int           mm_busy_freeing;    // For the kernel pool, flag saying we're
                                    // currently freeing something
  size_t        *mm_defer_frees;    // For the kernel pool, pointers that need
                                    // to be freed later
  int           mm_num_defer_frees; // Number in the above array

  // Region management (schedulers only)
  MmRgnTreeNode *mm_region_tree;    // Root of our region (sub)tree
  Trie          *mm_used_rids;      // Region ID ranges that are used/delegated
                                    // (key: rid, data: MmRgnRidRange *)
  Trie          *mm_free_rids;      // Region ID ranges that are still free
                                    // (key: rid, data: MmRgnRidRange *)
  Trie          *mm_used_ranges;    // Address ranges that are used/delegated
                                    // (key: address, data: MmRgnAdrRange *)
  Trie          *mm_free_ranges;    // Address ranges that are still free
                                    // (key: address, data: MmRgnAdrRange *)
  int           mm_free_num_slabs;  // Total free slabs present in
                                    // mm_free_ranges trie
  int           mm_free_num_rids;   // Total free region IDs present in
                                    // mm_free_rids trie
  Trie          *mm_local_rids;     // Locally handled region IDs
                                    // (key: rid, data: MmRgnTreeNode *)
  int           mm_range_chunk;     // Current number of slabs allocated to
                                    // new regions (or regions that ask for
                                    // more memory); lies between
                                    // MM_ADR_RANGE_CHUNK_{MIN, MAX}
  rid_t         mm_last_harvest;    // Last searched region for harvesting
                                    // free slabs of mm_range_chunk. If 0,
                                    // harvest has never happened.
  int           mm_load_rrobin;     // Round-robin variable for same-load
                                    // load balancing decisions
  int           mm_current_load;    // Our current memory region load value,
                                    // including all our children values
  int           mm_reported_load;   // Last regions load value we have reported
                                    // to our parent
  int           *mm_children_load;  // Number of regions handled by our
                                    // children schedulers. Periodically
                                    // updated.

  // Processing layer
  int           pr_core_id;         // System-wise unique core ID
  int           pr_num_cores;       // Total number of active system cores
  PrRole        pr_role;            // Core role (Scheduler or Worker)
  int           *pr_core_bid_cid;   // Mappings of core IDs to arch bid/cid
  int           *pr_core_work_ids;  // Mappings of core IDs to worker IDs
  int           *pr_core_sched_ids; // Mappings of core IDs to scheduler IDs
  int           *pr_core_child_idx; // Mappings of core IDs to children indices
  int           *pr_work_core_ids;  // Mappings of worker IDs to core IDs
  int           *pr_sched_core_ids; // Mappings of scheduler IDs to core IDs
  int           *pr_core_route;     // Routing table: y = pr_core_route[x]
                                    // specifies that messages ultimately
                                    // going to core ID x should be forwarded
                                    // to (parent or child) core ID y as the
                                    // next hop
  int           pr_num_schedulers;  // Total number of schedulers
  int           pr_num_workers;     // Total number of workers
  int           pr_worker_id;       // Worker: which worker are we out
                                    //         of num_workers
  int           pr_scheduler_id;    // Scheduler: which scheduler are we
                                    //            out of num_schedulers
  int           pr_parent_sched_id; // Scheduler ID of our parent scheduler
                                    // (if we're a scheduler) or the scheduler
                                    // responsible for us (if we're a worker),
                                    // or -1 if we're the top level scheduler
  int           pr_scheduler_level; // Scheduler: level in hierarchy.
                                    // 0 is lowest-level scheduler who has
                                    // workers as children.
  int           *pr_children;       // Scheduler: children core IDs
  int           pr_num_children;    // Scheduler: number of children
                                    // (workers, for level 0 schedulers, or
                                    // lev n-1 schedulers for lev n schedulers)
  int           pr_cur_epoch;       // Worker: current L2 Epoch
  func_t        **pr_task_table;    // Task table of application to be run
  List          *pr_ready_queue;    // Worker: tasks assigned to us to be run
                                    // (data: PrTaskDescr *)
  int           pr_spawn_pending;   // Worker: if 1, a spawn request is pending
                                    // to our task scheduler
  Trie          *pr_tasks;          // Scheduler: tasks we're responsible for
                                    // (key: task ID, data: PrTaskDescr *)
  int           pr_avail_task_id;   // Scheduler: next available task ID
                                    // (does not include the scheduler ID)
  int           pr_sched_rr;        // Scheduler: index of last child for
                                    // round-robin scheduling
  int           pr_cur_sched_load;  // Scheduler: Our current scheduled task
                                    // load value, i.e. how many tasks are
                                    // handled by schedulers in this subtree.
  int           pr_cur_run_load;    // Scheduler: Our current running task load
                                    // value, i.e. how many tasks are currently
                                    // running by workers in this subtree.
  int           pr_rep_sched_load;  // Scheduler: Last scheduled task load
                                    // value we have reported to our parent
  int           pr_rep_run_load;    // Scheduler: Last running task load
                                    // value we have reported to our parent
  int           *pr_chld_sched_load;// Scheduler: Scheduled task load value of
                                    // our children schedulers (periodically
                                    // updated)
  int           *pr_chld_run_load;  // Scheduler: Running task load value of
                                    // our children (periodically updated if
                                    // we're a non-leaf scheduler, faithfully
                                    // updated if we are a leaf scheduler)
  int           pr_main_finished;   // Top-level scheduler only: set to 1 when
                                    // main task has been collected

  // Messaging/events
  int           pr_message_id;      // Per-core unique message ID
  Trie          *pr_pending_events; // Events we couldn't handle immediately
                                    // (key: OUR req_id, data: PrEvtPending *)
  List          *pr_incomplete_req; // Incoming multi-part requests
                                    // (data: PrMultiPartReq)
  int           pr_pages_msg_id;    // If > 0, there is an ongoing request
                                    // to get more pages and this is the message
                                    // ID for it
  int           pr_rids_msg_id;     // If > 0, there is an ongoing request
                                    // to get more region IDs and this is the
                                    // message ID for it

  // FMPI context
#ifdef FMPI
  FMPI_Context  *fmpi;
#endif

} Context;

// Exported macro to get the single context structure. This is at a known
// location: it is allocated at bootstrap and never moved. Find nearest
// MM_ALLOC_ALIGN size and return its bootstrap address.
static inline Context *mm_get_context(int cid) {
  if (sizeof(Context) & (MM_ALLOC_ALIGN - 1)) {
    return (Context *) (mm_va_kernel_base(cid) +
                        MM_BOOTSTRAP_SLABS_STEP * MM_SLAB_SIZE *
                          (sizeof(Context) / MM_ALLOC_ALIGN));
  }
  else {
    return (Context *) (mm_va_kernel_base(cid) +
                        MM_BOOTSTRAP_SLABS_STEP * MM_SLAB_SIZE *
                          (sizeof(Context) / MM_ALLOC_ALIGN - 1));
  }
}


#endif
