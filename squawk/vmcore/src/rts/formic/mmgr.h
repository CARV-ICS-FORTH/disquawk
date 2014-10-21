/*
 * Copyright (C) 2013-2014 FORTH-ICS / CARV
 *                         (Foundation for Research & Technology -- Hellas,
 *                          Institute of Computer Science,
 *                          Computer Architecture & VLSI Systems Laboratory)
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file   mmgr.h
 * @author Foivos S. Zakkak <zakkak@ics.forth.gr>
 *
 * Header file for the Monitors and the Monitors Manager implementations
 */

#ifndef MMGR_H_
#define MMGR_H_

#define MMGR_HT_SIZE 128

typedef struct wait_node waiter_queue_t;

struct wait_node {
	int            id;          /**< The waiter's ID packed as
	                             * (board_ID << 3) | (core_ID) */
	waiter_queue_t *next;       /**< Pointer to the next waiter in the
	                             * queue */
};

typedef struct monitor monitor_t;

struct monitor {
	void           *object;      /**< The object's reference coupled
	                             * with this monitor */
	int            owner;       /**< The owner's ID packed as
	                             * (board_ID << 3) | (core_ID) */
	waiter_queue_t *waiters;    /**< Pointer to the list of waiters */
	monitor_t      *lchild;     /**< Pointer to the left child in the
	                             * Monitor Manager's monitor binary
	                             * search tree */
	monitor_t      *rchild;     /**< Pointer to the right child in the
	                             * Monitor Manager's monitor binary
	                             * search tree */
};

void mmgrInitialize(monitor_t **mmgrHT);

#endif /* MMGR_H_ */
