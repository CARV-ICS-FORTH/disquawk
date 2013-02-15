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
// Abstract      : Utility functions for in-kernel use
//
// =============================[ CVS Variables ]=============================
// 
// File name     : $RCSfile: kernel_toolset.h,v $
// CVS revision  : $Revision: 1.17 $
// Last modified : $Date: 2012/11/01 13:36:37 $
// Last author   : $Author: lyberis-spree $
// 
// ===========================================================================

#ifndef _KERNEL_TOOLSET_H
#define _KERNEL_TOOLSET_H

#include <stdarg.h>

#include <types.h>


// ===========================================================================
// String functions
// ===========================================================================
extern unsigned int kt_strlen(const char *s);
extern char *kt_strstr(const char *s1, const char *s2);
extern int kt_strcmp(const char *s1, const char *s2);
extern char *kt_strcpy(char *dst, const char *src);
extern char *kt_strncpy(char *dst, const char *src, int num_bytes);
extern char *kt_strdup(const char *s);
extern char *kt_strchr(const char *s, char c);
extern void kt_bzero(void *buf, int num_bytes);
extern void *kt_memset(void *buf, char val, int num_bytes);
extern void *kt_memcpy(void *dst, const void *src, int num_bytes);
extern int kt_memcmp(const void *s1, const void *s2, int num_bytes);


// ===========================================================================
// Print functions
// ===========================================================================
extern int kt_vsprintf(char *buf, const char *fmt, va_list args);
extern int kt_vprintf(const char *format, va_list ap);
extern int kt_printf(const char *format, ...);
extern int kt_sprintf(char *buf, const char *format, ...);


// ===========================================================================
// ASCII85 encoding
// ===========================================================================
void kt_encode85(unsigned int *array, int length);


// ===========================================================================
// Kernel allocation functions
// ===========================================================================
extern void kt_mem_init();

extern void *kt_malloc(size_t size);
extern void *kt_zalloc(size_t size);
extern void kt_free(void *ptr);
extern void *kt_realloc(void *old_ptr, size_t new_size);


// ===========================================================================
// Trie data structure
// ===========================================================================

// Types
typedef struct TrieNodeStruct TrieNode;
struct TrieNodeStruct {

  TrieNode      *children[8];   // 8 children, 0 being the lowest in raw key
                                // and 7 being the highest.
  TrieNode      *parent;        // Parent node
  int           bitmap;         // Bitmap of children that are present. A
                                // value of 0, means the node is a leaf.
  void          *data;          // User data pointer (valid when no children)
  size_t        raw_key;        // Raw key (valid when no children)

};

typedef struct {

  int           msb;            // Most significant bit of key values (0...63)
  int           lsb;            // Least significant bit of key values (0...63)
  TrieNode      *roots;         // Array of roots (one root per possible 
                                // number of leading triads of zeroes)
  int           num_roots;      // Number of roots
  TrieNode      *walk;          // Current trie walking position (leaf node)
  TrieNode      *walk_int;      // Same, but for internals function use. We
                                // use this so that user can safely walk
                                // and insert/delete stuff while walking.
  unsigned int  num_keys;       // Number of keys in the Trie

} Trie;


// Exported functions
Trie            *kt_alloc_trie(int msb, int lsb);
void            kt_free_trie(Trie *t, void (*free_data)(void *));
void            kt_reset_trie(Trie *t, void (*free_data)(void *));

int             kt_trie_insert(Trie *t, size_t raw_key, void *data);
int             kt_trie_delete(Trie *t, size_t raw_key, 
                               void (*free_data)(void *));

size_t          kt_trie_find(Trie *t, size_t raw_key, void **ret_data);
size_t          kt_trie_find_approx(Trie *t, int greater, size_t raw_key,
                                    void **ret_data);
size_t          kt_trie_find_minmax(Trie *t, int max, void **ret_data);
size_t          kt_trie_find_next(Trie *t, int greater, void **ret_data);
int             kt_trie_update_data(Trie *t, size_t raw_key, void *new_data,
                                    void (*free_old_data)(void *));

unsigned int    kt_trie_size(Trie *t);

// Debugging functions
//void kt_trie_dbg_dotty(Trie *t, char *filename);


// ===========================================================================
// List data structure
// ===========================================================================

// Types
typedef struct ListNodeStruct ListNode;
struct ListNodeStruct {

  ListNode      *prev;          // Previous in list
  ListNode      *next;          // Next in list
  void          *data;          // User data pointer

};

typedef struct {

  ListNode      *head;          // Head of the list 
  ListNode      *tail;          // Tail of the list 
  unsigned int  num_nodes;      // Number of list nodes

} List;


// Exported functions
extern ListNode *kt_list_insert(List *l, void *data, ListNode *insert_ref,
                                int insert_after);
extern void kt_list_delete(List *l, ListNode *n, void (*free_data)(void *));
extern List *kt_alloc_list();
extern void kt_free_list(List *l, void (*free_data)(void *));

// Inlined functions
static inline ListNode *kt_list_head(List *l) {
  return (l->head);
}

static inline ListNode *kt_list_tail(List *l) {
  return (l->tail);
}

static inline ListNode *kt_list_next(ListNode *n) {
  return (n->next);
}

static inline ListNode *kt_list_prev(ListNode *n) {
  return (n->prev);
}

static inline unsigned int kt_list_size(List *l) {
  return (l->num_nodes);
}


// ===========================================================================
// Math helper macros
// ===========================================================================

// Log base 2
static inline int kt_int_log2(int n) {
  int tmp1 = n;
  int tmp2 = 0;

  while (tmp1 >>= 1) {
    tmp2++;
  }
  
  return tmp2;
}

// Log base 10
static inline int kt_int_log10(int n) {
  int tmp1 = n;
  int tmp2 = 0;

  while (tmp1 /= 10) {
    tmp2++;
  }

  return tmp2;
}

// Random number generator: call with returned result as next seed to get
// up to 2^32 different numbers.
static inline unsigned int kt_rand(unsigned int seed) {
  return (1103515245 * seed + 12345);
}

#endif
