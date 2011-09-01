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

#ifndef __ALGORITHM_ASTAR_H__
#define __ALGORITHM_ASTAR_H__

#include "algorithm-ptrdic.h"
#include "algorithm-priority-queue.h"

typedef float (*LIAlgAstarCost)(void* world, void* object, void* start, void* end);
typedef float (*LIAlgAstarHeuristic)(void* world, void* object, void* start, void* end);
typedef int (*LIAlgAstarPassable)(void* world, void* object, void* start, void* end);
typedef void* (*LIAlgAstarSuccessor)(void* world, void* object, void* node, int index);

typedef struct _LIAlgAstar LIAlgAstar;
struct _LIAlgAstar
{
	void* world;
	void* object;
	void* target;
	LIAlgPtrdic* open;
	LIAlgPtrdic* closed;
	LIAlgPriorityQueue* priority;
	struct
	{
		LIAlgAstarCost cost;
		LIAlgAstarHeuristic heuristic;
		LIAlgAstarPassable passable;
		LIAlgAstarSuccessor successor;
	} calls;
};

typedef struct _LIAlgAstarResult LIAlgAstarResult;
struct _LIAlgAstarResult
{
	int length;
	void** nodes;
};

LIAlgAstar*
lialg_astar_new (LIAlgAstarCost      cost,
                 LIAlgAstarHeuristic heuristic,
                 LIAlgAstarPassable  passable,
                 LIAlgAstarSuccessor successor);

void
lialg_astar_free (LIAlgAstar* self);

LIAlgAstarResult*
lialg_astar_solve (LIAlgAstar* self,
                   void*       world,
                   void*       object,
                   void*       start,
                   void*       end);

void
lialg_astar_result_free (LIAlgAstarResult* self);

#endif
