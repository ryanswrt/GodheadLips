/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ALGORITHM_PRIORITY_QUEUE_H__
#define __ALGORITHM_PRIORITY_QUEUE_H__

#include <lipsofsuna/system.h>
#include "algorithm-bst.h"

typedef struct _LIAlgPriorityQueue LIAlgPriorityQueue;
struct _LIAlgPriorityQueue
{
	LIAlgBst* tree;
};

typedef struct _LIAlgPriorityQueueNode LIAlgPriorityQueueNode;
struct _LIAlgPriorityQueueNode
{
	float priority;
	void* value;
	LIAlgBstNode node;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAlgPriorityQueue*
lialg_priority_queue_new ();

void
lialg_priority_queue_free (LIAlgPriorityQueue* self);

void
lialg_priority_queue_clear (LIAlgPriorityQueue* self);

void*
lialg_priority_queue_find_highest (LIAlgPriorityQueue* self);

LIAlgPriorityQueueNode*
lialg_priority_queue_find_highest_node (LIAlgPriorityQueue* self);

void*
lialg_priority_queue_find_lowest (LIAlgPriorityQueue* self);

LIAlgPriorityQueueNode*
lialg_priority_queue_find_lowest_node (LIAlgPriorityQueue* self);

LIAlgPriorityQueueNode*
lialg_priority_queue_insert (LIAlgPriorityQueue* self,
                             float               priority,
                             void*               value);

void*
lialg_priority_queue_pop_highest (LIAlgPriorityQueue* self);

void*
lialg_priority_queue_pop_lowest (LIAlgPriorityQueue* self);

void
lialg_priority_queue_remove_node (LIAlgPriorityQueue*     self,
                                  LIAlgPriorityQueueNode* node);

#ifdef __cplusplus
}
#endif

#endif
