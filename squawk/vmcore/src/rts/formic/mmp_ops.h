#ifndef _MMP_OPS_H
#define _MMP_OPS_H

/* NOTE:
 * This enum can only take values up to 64, because it is getting
 * packed in the messages and there are only 6 bits reserved for it
 */
typedef enum {
	// Threads
	MMP_OPS_TH_SPAWN=1,
	// Hash-Table
	MMP_OPS_HT_INSERT,
	MMP_OPS_HT_LOOKUP,
	MMP_OPS_HT_REMOVE,
	// Stack
	MMP_OPS_ST_PUSH,
	MMP_OPS_ST_POP,
	// Atomic Primitives
	MMP_OPS_AT_CAS,
	MMP_OPS_AT_FAD,
	MMP_OPS_AT_SWAP,
	MMP_OPS_AT_WRITE,
	// Synchronize
	MMP_OPS_SY_JOIN,
	MMP_OPS_SY_TERMINATE,
} msg_op;

#endif /* _MMP_OPS_H */
