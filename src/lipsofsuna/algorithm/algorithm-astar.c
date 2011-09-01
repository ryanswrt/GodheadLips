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

/**
 * \addtogroup LIAlg Algorithm
 * @{
 * \addtogroup LIAlgAstar Astar
 * @{
 */

#include <lipsofsuna/system.h>
#include "algorithm-astar.h"

typedef struct _LIAlgAstarNode LIAlgAstarNode;
struct _LIAlgAstarNode
{
	LIAlgAstarNode* prev;
	void* node;
	float cost;
	float heuristic;
	struct
	{
		LIAlgPtrdicNode* assoc;
		LIAlgPriorityQueueNode* priority;
	} nodes;
};

static LIAlgAstarResult*
private_build_path (LIAlgAstar*     self,
                    LIAlgAstarNode* last);

static void
private_clear (LIAlgAstar* self);

static LIAlgAstarNode*
private_next_open (LIAlgAstar*     self,
                   LIAlgAstarNode* current);

static int
private_node_opened (LIAlgAstar*     self,
                     LIAlgAstarNode* prev,
                     void*               node);

static int
private_node_closed (LIAlgAstar*     self,
                     LIAlgAstarNode* prev,
                     void*           node);

static int
private_open_node (LIAlgAstar*     self,
                   LIAlgAstarNode* prev,
                   void*           node);

/*****************************************************************************/

/**
 * \brief Creates a new path solver.
 *
 * \param cost Function for inspecting the cost of movement.
 * \param heuristic Function for estimating the cost to reach the goal.
 * \param passable Function for inspecting the passability of nodes.
 * \param successor Function for retrieving connected nodes.
 * \return New path solver or NULL.
 */
LIAlgAstar*
lialg_astar_new (LIAlgAstarCost      cost,
                 LIAlgAstarHeuristic heuristic,
                 LIAlgAstarPassable  passable,
                 LIAlgAstarSuccessor successor)
{
	LIAlgAstar* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIAlgAstar));
	if (self == NULL)
		return NULL;
	self->calls.cost = cost;
	self->calls.heuristic = heuristic;
	self->calls.passable = passable;
	self->calls.successor = successor;

	/* Create the open node list. */
	self->open = lialg_ptrdic_new ();
	if (self->open == NULL)
		goto error;

	/* Create the closed node list. */
	self->closed = lialg_ptrdic_new ();
	if (self->closed == NULL)
		goto error;

	/* Create the open node priority queue. */
	self->priority = lialg_priority_queue_new ();
	if (self->priority == NULL)
		goto error;

	return self;

error:
	lialg_astar_free (self);
	return NULL;
}

/**
 * \brief Frees the path solver.
 *
 * \param self Path solver.
 */
void
lialg_astar_free (LIAlgAstar* self)
{
	if (self->open != NULL)
		lialg_ptrdic_free (self->open);
	if (self->closed != NULL)
		lialg_ptrdic_free (self->closed);
	if (self->priority != NULL)
		lialg_priority_queue_free (self->priority);
	lisys_free (self);
}

/**
 * \brief Solves a path.
 *
 * You can reuse a single path solver instance by just
 * calling this function over and over.
 *
 * \param self Path solver.
 * \param world User defined value passed to callbacks.
 * \param object User defined value passed to callbacks.
 * \param start Starting position.
 * \param end Target position.
 * \return List of path nodes or NULL.
 */
LIAlgAstarResult*
lialg_astar_solve (LIAlgAstar* self,
                   void*       world,
                   void*       object,
                   void*       start,
                   void*       end)
{
	int i;
	int ret;
	void* node;
	LIAlgAstarResult* result;
	LIAlgAstarNode* current;

	/* Initialize state. */
	self->world = world;
	self->object = object;
	self->target = end;

	/* Add the start node to the open list. */
	current = lisys_malloc (sizeof (LIAlgAstarNode));
	if (current == NULL)
		return NULL;
	current->prev = NULL;
	current->node = start;
	current->cost = 0.0f;
	current->heuristic = self->calls.heuristic (self->world, object, start, end);

	/* Handle nodes in the open list. */
	while (1)
	{
		/* Check if we have found a solution. */
		if (current->node == end)
			break;

		/* Add successors to the open list. */
		for (i = 0 ; ; i++)
		{
			/* Next successor. */
			node = self->calls.successor (self->world, object, current->node, i);
			if (node == NULL)
				break;

			/* Check if valid. */
			if (!self->calls.passable (self->world, object, current->node, node))
				continue;

			/* Already in open list? */
			ret = private_node_opened (self, current, node);
			if (ret == 1) continue;
			if (ret == 2) goto error;

			/* Already in closed list? */
			ret = private_node_closed (self, current, node);
			if (ret == 1) continue;
			if (ret == 2) goto error;

			/* Create new open node. */
			if (!private_open_node (self, current, node))
				goto error;
		}

		/* Next open node. */
		current = private_next_open (self, current);
		if (current == NULL)
		{
			private_clear (self);
			return NULL;
		}
	}

	/* Build the final path. */
	result = private_build_path (self, current);
	private_clear (self);

	return result;

error:
	lisys_free (current);
	private_clear (self);
	return NULL;
}

/**
 * \brief Frees a path returned by the path finder.
 *
 * \param self Path finder path.
 */
void
lialg_astar_result_free (LIAlgAstarResult* self)
{
	lisys_free (self->nodes);
	lisys_free (self);
}

/*****************************************************************************/

static LIAlgAstarResult*
private_build_path (LIAlgAstar*     self,
                    LIAlgAstarNode* last)
{
	int count;
	LIAlgAstarResult* path;
	LIAlgAstarNode* ptr;

	/* Calculate length. */
	count = 0;
	for (ptr = last ; ptr != NULL ; ptr = ptr->prev)
	{
		lisys_assert (ptr->node != NULL);
		count++;
	}

	/* Allocate path. */
	path = lisys_calloc (1, sizeof (LIAlgAstarResult));
	if (path == NULL)
		return NULL;
	path->length = count;
	path->nodes = lisys_calloc (count, sizeof (void*));
	if (path->nodes == NULL)
	{
		lisys_free (path);
		return NULL;
	}

	/* Copy nodes to path. */
	for (ptr = last ; ptr != NULL ; ptr = ptr->prev)
		path->nodes[--count] = ptr->node;

	return path;
}

static void
private_clear (LIAlgAstar* self)
{
	LIAlgPtrdicIter iter;

	LIALG_PTRDIC_FOREACH (iter, self->open)
		lisys_free (iter.value);
	LIALG_PTRDIC_FOREACH (iter, self->closed)
		lisys_free (iter.value);
	lialg_ptrdic_clear (self->open);
	lialg_ptrdic_clear (self->closed);
	lialg_priority_queue_clear (self->priority);
}

static LIAlgAstarNode*
private_next_open (LIAlgAstar*     self,
                   LIAlgAstarNode* current)
{
	/* Close previous node. */
	if (!lialg_ptrdic_insert (self->closed, current->node, current))
	{
		lisys_free (current);
		return NULL;
	}

	/* Highest priority open node. */
	current = lialg_priority_queue_pop_lowest (self->priority);
	if (current == NULL)
		return NULL;
	lialg_ptrdic_remove_node (self->open, current->nodes.assoc);

	return current;
}

/**
 * \brief Checks if the node is already in the open list.
 *
 * If another path to the node has already been discovered, choose
 * the shorter of the two. If the currently evaluated path is
 * shorter, overwrite the cost and path information already present
 * in the open list. Otherwise, keep the old values and reject the
 * currently evaluated path.
 *
 * \param self Path solver.
 * \param prev Previous node of the currently evaluated path.
 * \param node Currently evaluated node.
 * \return Zero if not in list, one if found and handled, two if an error occurred.
 */
static int
private_node_opened (LIAlgAstar*     self,
                     LIAlgAstarNode* prev,
                     void*           node)
{
	float cost;
	LIAlgAstarNode* successor;

	/* Find the node from the open list. */
	successor = lialg_ptrdic_find (self->open, node);
	if (successor == NULL)
		return 0;
	lisys_assert (successor->node == node);

	/* Check if the previously evaluated path is shorter. */
	cost = prev->cost + self->calls.cost (self->world, self->object, prev->node, node);
	if (successor->cost <= cost)
		return 1;

	/* Overwrite the existing path since it was longer. */
	lialg_priority_queue_remove_node (self->priority, successor->nodes.priority);
	successor->prev = prev;
	successor->cost = cost;
	successor->heuristic = self->calls.heuristic (self->world, self->object, node, self->target);
	successor->nodes.priority = lialg_priority_queue_insert (self->priority, successor->cost + successor->heuristic, successor);
	if (successor->nodes.priority == NULL)
	{
		lisys_free (successor);
		return 2;
	}

	return 1;
}

/**
 * \brief Checks if the node is already in the closed list.
 *
 * If another path to the node has already been discovered, choose
 * the shorter of the two. If the currently evaluated path is
 * shorter, overwrite the cost and path information already present
 * in the closed list and reopen the node. Otherwise, keep the old
 * values and reject the currently evaluated path.
 *
 * \param self Path solver.
 * \param prev Previous node of the currently evaluated path.
 * \param node Currently evaluated node.
 * \return Zero if not in list, one if found and handled, two if an error occurred.
 */
static int
private_node_closed (LIAlgAstar*     self,
                     LIAlgAstarNode* prev,
                     void*           node)
{
	float cost;
	LIAlgAstarNode* successor;

	/* Find the node from the closed list. */
	successor = lialg_ptrdic_find (self->closed, node);
	if (successor == NULL)
		return 0;
	lisys_assert (successor->node == node);

	/* Check if the previously evaluated path is shorter. */
	cost = prev->cost + self->calls.cost (self->world, self->object, prev->node, node);
	if (successor->cost <= cost)
		return 1;

	/* Overwrite the existing path since it was longer. */
	lialg_ptrdic_remove (self->closed, node);
	successor->prev = prev;
	successor->cost = cost;
	successor->heuristic = self->calls.heuristic (self->world, self->object, node, self->target);
	successor->nodes.assoc = lialg_ptrdic_insert (self->open, node, successor);
	if (successor->nodes.assoc == NULL)
	{
		lisys_free (successor);
		return 2;
	}
	successor->nodes.priority = lialg_priority_queue_insert (self->priority, successor->cost + successor->heuristic, successor);
	if (successor->nodes.priority == NULL)
	{
		lialg_ptrdic_remove (self->open, node);
		lisys_free (successor);
		return 2;
	}

	return 1;
}

static int
private_open_node (LIAlgAstar*     self,
                   LIAlgAstarNode* prev,
                   void*           node)
{
	LIAlgAstarNode* successor;

	successor = lisys_malloc (sizeof (LIAlgAstarNode));
	if (successor == NULL)
		return 0;
	successor->node = node;
	successor->prev = prev;
	successor->cost = prev->cost + self->calls.cost (self->world, self->object, prev->node, node);
	successor->heuristic = self->calls.heuristic (self->world, self->object, node, self->target);
	successor->nodes.assoc = lialg_ptrdic_insert (self->open, node, successor);
	if (successor->nodes.assoc == NULL)
	{
		lisys_free (successor);
		return 0;
	}
	successor->nodes.priority = lialg_priority_queue_insert (self->priority, successor->cost + successor->heuristic, successor);
	if (successor->nodes.priority == NULL)
	{
		lialg_ptrdic_remove (self->open, node);
		lisys_free (successor);
		return 0;
	}
	return 1;
}

/** @} */
/** @} */
