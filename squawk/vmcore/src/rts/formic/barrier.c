/*
 * Copyright 2014 FORTH-ICS. All Rights Reserved.
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Foundation for Research and Technology - Hellas (FORTH)
 * Institute of Computer Science (ICS), N. Plastira 100, Vassilika Vouton,
 * GR-700 13 Heraklion, Crete, Greece or visit www.ics.forth.gr if you need
 * additional information or have any questions.
 */

/* #ifdef ATOMIC_BARRIER
 * /\**
 *  * @brief Global barrier.
 *  * Dissemination barrier (from: Two Algorithms for Barrier
 *  * Synchronization) implementation using bitfields and atomic
 *  * increment.
 *  *
 *  * NOTE: NOT WORKING for sparse cores (i.e. 1 core per board).
 *  *
 *  * Performance:
 *  *
 *  * |  Cores  |  Ticks  |
 *  * |--------:|--------:|
 *  * |      1  |     11  |
 *  * |      8  |    368  |
 *  * |     32  |    674  |
 *  * |    512  |   1248  |
 *  *\/
 * // FIXME: Add parity
 * void sysBarrier() {
 * 	register int i, tmp, log2_cores;
 * 	register int partner_cid, partner_bid;
 *
 * 	if(TOTAL_CORES==1)
 * 		return;
 *
 * 	// This could also be calculated at initialization
 * 	log2_cores = int_log2(TOTAL_CORES) + TOTAL_CORES%2;
 *
 * 	for (i=0; i<=log2_cores; ++i) {
 * 		// core + 2^i % P
 * 		tmp = ((my_bid<<3 | my_cid)+(1<<i))%TOTAL_CORES;
 * 		partner_bid = tmp >> 3;
 * 		partner_cid = tmp & 0x7;
 * 		ar_cnt_incr(my_cid, partner_bid, partner_cid, HWCNT_BARRIER_COUNTER, 1<<i);
 * 		while (!(ar_cnt_get(my_cid, HWCNT_BARRIER_COUNTER) & 1<<i));
 * 	}
 *
 * 	// reset the counter
 * 	ar_cnt_set(my_cid, HWCNT_BARRIER_COUNTER, 0);
 * }
 * #else */

/**
 * @brief Global barrier.
 * Implements a simple, centralized barrier using the counters
 *
 * Performance:
 *
 * |  Cores  |  Ticks  |
 * |--------:|--------:|
 * |      8  |    509  |
 * |    512  |  22000  |
 */
void sysBarrierCentralized() {
	int i, x, y, z, slave_bid;

	// Compute how many cores should be activated

	// The master/coordinator
	if((my_bid == BARRIER_MASTER_BID) && !my_cid) {

		/* Detect cases where we get stack in a barrier and print a backtrace */
		i = 0;
		while (ar_cnt_get(my_cid, HWCNT_BARRIER_COUNTER)) {
			if (i++ == ((1 << 30) -1)) {
				i = 0;
				kt_printf("Waiting %d\n", ar_cnt_get(my_cid, HWCNT_BARRIER_COUNTER));
				ar_backtrace();
			}
		}

		// Re-Initialize the barrier counter
		ar_cnt_set(my_cid, HWCNT_BARRIER_COUNTER, 1+8-TOTAL_CORES);  // HACK exclude bid 63

		// Make Formic slaves reach their boot barrier
		for (x = AR_FORMIC_MIN_X; x <= AR_FORMIC_MAX_X; x++)
			for (y = AR_FORMIC_MIN_Y; y <= AR_FORMIC_MAX_Y; y++)
				for (z = AR_FORMIC_MIN_Z; z <= AR_FORMIC_MAX_Z; z++)
					for (i = 0; i < AR_FORMIC_CORES_PER_BOARD; i++) {
						slave_bid = (x << 4) | (y << 2) | z;

						if (!( ( (slave_bid == my_bid) && (i == my_cid) ) ||
						       ( (slave_bid >= 63) ) )) // HACK exclude bid 63
							ar_cnt_incr(my_cid, slave_bid, i, HWCNT_BARRIER_COUNTER, 1);
					}

	} else { // the slaves
		// Initialize the local barrier counter
		ar_cnt_set(my_cid, HWCNT_BARRIER_COUNTER, -1);
		// Notify master we are here
		ar_cnt_incr(my_cid, BARRIER_MASTER_BID, 0, HWCNT_BARRIER_COUNTER, 1);
		// Block until master indicates all cores have reached the barrier
		while (ar_cnt_get(my_cid, HWCNT_BARRIER_COUNTER));
	}
}

#ifdef HIER_BARRIER
/**
 * @brief Global barrier.
 * Find the proper counter according to our children and parent.
 *
 * @param tree_direction    0 = forward tree (barrier enter),
 *                          1 = reverse tree (barrier exit)
 * @param level             0 = leaf,
 *                          9 = log2(512) = root
 * @param id                rank we're searching for, from 0 to 511
 * @param *ret_cnt:         returned counter number (valid for rank and its sibling)
 * @param *ret_id:          returned rank of *ret_cnt
 * @param *ret_sibling_id:  returned rank of sibling in the tree
 */
static void sysBarrierFindCounter(int tree_direction, int level, int id,
                                  int *ret_cnt, int *ret_id,
                                  int *ret_sibling_id) {

	int exp_level;
	int cnt;
	int whose;
	int whose_sibling;
	int i;


	// Forward tree has no counters for leaves
	ar_assert((tree_direction > 0) || (level > 0));

	// Find whose rank's the counter is
	whose = 0;
	for (i = 0; i < 9; i++) {
		if (i >= level) {
			whose |= id & (1 << i);
		}
	}

	// Find its sibling id
	whose_sibling = whose ^ (1 << level);

	// Expand level so that 0 ... 9 = forward tree, 9 ... 18 = reverse tree
	// (9 = root for both)
	if (tree_direction) {
		exp_level = 9 + (9 - level);
	}
	else {
		exp_level = level;
	}

	// Counter depends on the expanded level (no leaf for forward)
	cnt = exp_level - 1;


	// Return stuff
	if (ret_cnt) {
		*ret_cnt = cnt;
	}
	if (ret_id) {
		*ret_id = whose;
	}
	if (ret_sibling_id) {
		*ret_sibling_id = whose_sibling;
	}
}

/**
 * @brief Global barrier initializer.
 * Initialize the variables needed by sysBarrier
 */
void sysBarrierInit() {

	int           i;
	int           cnt;
	int           id;
	int           my_id;
	int           tmp_cnt;
	int           tmp_id;
	int           tmp_bid;
	int           tmp_cid;
	int           tmp_sibling;
	int           id_to_enter;


	// Compute which counters comprise our part of the trees, set their
	// notifications and store their info so we can init them again.
	//
	// NOTE: we know it's a maximum of 18 counters per core, so we maintain 2
	// sets of trees by adding 32 to all computed counter numbers.
	sysBarrierCounters_g = 0;
	my_id = (my_bid << 3) | my_cid;

	// Forward tree: counters wait 2 arrivals and notify next level
	for (i = 1; i < 9; i++) {

		// Find counter for this tree level
		sysBarrierFindCounter(0, i, my_id,
		                      &cnt, &id, NULL);

		// Are we responsible for it?
		if (id == my_id) {

			// Find counter of next tree level
			sysBarrierFindCounter(0, i + 1, my_id,
			                      &tmp_cnt, &tmp_id, NULL);

			// Check if the siblings are enabled in our setup
			tmp_bid = tmp_id >> 3;
			tmp_cid = tmp_id & 0x7;

			// Setup our counter
			ar_cnt_set_notify_cnt(my_cid, cnt, -2,
			                      tmp_bid, tmp_cid, tmp_cnt);
			ar_cnt_set_notify_cnt(my_cid, cnt + 32, -2,
			                      tmp_bid, tmp_cid, tmp_cnt + 32);

			// Remember init value
			sysBarrierCounters2Init_g[sysBarrierCounters_g] = cnt;
			sysBarrierValues2Init_g[sysBarrierCounters_g] = -2;
			sysBarrierCounters_g++;
		}

	}


	// Roots of both trees: counter waits 2 arrivals and notifies two
	// tree siblings
	sysBarrierFindCounter(0, 9, my_id, &cnt, &id, NULL);

	// Are we responsible for it?
	if (id == my_id) {

		// Find counters of next tree level siblings
		sysBarrierFindCounter(1, 8, my_id, &tmp_cnt, &tmp_id, &tmp_sibling);

		// Setup our counter
		ar_cnt_set_dbl_notify_cnt(my_cid, cnt, -2,
		                          tmp_id >> 3,      tmp_id & 0x7,      tmp_cnt,
		                          tmp_sibling >> 3, tmp_sibling & 0x7, tmp_cnt);
		ar_cnt_set_dbl_notify_cnt(my_cid, cnt + 32, -2,
		                          tmp_id >> 3,      tmp_id & 0x7,      tmp_cnt + 32,
		                          tmp_sibling >> 3, tmp_sibling & 0x7, tmp_cnt + 32);

		// Remember init value
		sysBarrierCounters2Init_g[sysBarrierCounters_g] = cnt;
		sysBarrierValues2Init_g[sysBarrierCounters_g] = -2;
		sysBarrierCounters_g++;
	}


	// Reverse tree: counter waits 1 arrival and notifies two tree siblings
	for (i = 8; i >= 1; i--) {

		// Find counter for this tree level
		sysBarrierFindCounter(1, i, my_id, &cnt, &id, NULL);

		// Are we responsible for it?
		if (id == my_id) {

			// Find counters of next tree level siblings
			sysBarrierFindCounter(1, i - 1, my_id,
			                      &tmp_cnt, &tmp_id, &tmp_sibling);

			// Setup our counter
			ar_cnt_set_dbl_notify_cnt(my_cid, cnt, -1,
			                          tmp_id >> 3,      tmp_id & 0x7,      tmp_cnt,
			                          tmp_sibling >> 3, tmp_sibling & 0x7, tmp_cnt);
			ar_cnt_set_dbl_notify_cnt(my_cid, cnt + 32, -1,
			                          tmp_id >> 3,      tmp_id & 0x7,      tmp_cnt + 32,
			                          tmp_sibling >> 3, tmp_sibling & 0x7, tmp_cnt + 32);

			// Remember init value
			sysBarrierCounters2Init_g[sysBarrierCounters_g] = cnt;
			sysBarrierValues2Init_g[sysBarrierCounters_g] = -1;
			sysBarrierCounters_g++;
		}

	}


	// Finally, compute which counter we need to notify in order to enter the
	// barrier...
	sysBarrierFindCounter(0, 1, my_id, &sysBarrierEnterCounter_g, &id_to_enter, NULL);
	sysBarrierEnterBID_g = id_to_enter >> 3;
	sysBarrierEnterCID_g = id_to_enter & 0x7;

	// ... and which counter to spin onto in order to exit the barrier. We need
	// to initialize this one too.
	sysBarrierFindCounter(1, 0, my_id, &sysBarrierExitCounter_g, NULL, NULL);

	ar_cnt_set(my_cid, sysBarrierExitCounter_g, -1);
	ar_cnt_set(my_cid, sysBarrierExitCounter_g + 32, -1);

	sysBarrierCounters2Init_g[sysBarrierCounters_g] = sysBarrierExitCounter_g;
	sysBarrierValues2Init_g[sysBarrierCounters_g] = -1;
	sysBarrierCounters_g++;


	// Sanity checks
	ar_assert(sysBarrierCounters_g < 20);
	for (i = 0; i < sysBarrierCounters_g; i++) {
		ar_assert(sysBarrierCounters2Init_g[i] < 32);
		ar_assert(sysBarrierValues2Init_g[i] < 0);
	}


	// Do a centralized barrier once, so that we know everyone has
	// finished setting up its counters.
	sysBarrierCentralized();
}

/**
 * @brief Global barrier.
 * Implements a hierarchical barrier using the counters
 *
 * Performance:
 *
 * |  Cores  |  Ticks  |
 * |--------:|--------:|
 * |    512  |   ~500  |
 */
void sysBarrier() {
	int           j;

	// Even or odd phase?
	if (sysBarrierPhase_g == 0) {

		// Even-phased barrier
		ar_cnt_incr(my_cid, sysBarrierEnterBID_g, sysBarrierEnterCID_g,
		            sysBarrierEnterCounter_g, 1);
		while (ar_cnt_get(my_cid, sysBarrierExitCounter_g)) {
			;
		}

		// Reset even-phased barrier counters
		for (j = 0; j < sysBarrierCounters_g; j++) {
#ifdef ARCH_ARM
			*ARS_CNT_VAL(my_cid, sysBarrierCounters2Init_g[j]) = sysBarrierValues2Init_g[j];
#endif
#ifdef ARCH_MB
			*MBS_CNT_VAL(sysBarrierCounters2Init_g[j]) = sysBarrierValues2Init_g[j];
#endif
		}

		// Change Phase
		sysBarrierPhase_g = 1;

	} else {

		// Odd-phased barrier
		ar_cnt_incr(my_cid, sysBarrierEnterBID_g, sysBarrierEnterCID_g, sysBarrierEnterCounter_g + 32, 1);
		while (ar_cnt_get(my_cid, sysBarrierExitCounter_g + 32)) {
			;
		}

		// Reset odd-phased barrier counters
		for (j = 0; j < sysBarrierCounters_g; j++) {
#ifdef ARCH_ARM
			*ARS_CNT_VAL(my_cid, sysBarrierCounters2Init_g[j] + 32) = sysBarrierValues2Init_g[j];
#endif
#ifdef ARCH_MB
			*MBS_CNT_VAL(sysBarrierCounters2Init_g[j] + 32) = sysBarrierValues2Init_g[j];
#endif
		}

		// Change Phase
		sysBarrierPhase_g = 0;
	}
}
#else /* HIER_BARRIER */
#define sysBarrier     sysBarrierCentralized
#define sysBarrierInit sysBarrierCentralized
#endif /* HIER_BARRIER */
