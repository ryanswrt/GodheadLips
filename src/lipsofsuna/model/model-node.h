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

#ifndef __MODEL_NODE_H__
#define __MODEL_NODE_H__

#include "lipsofsuna/archive.h"
#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"
#include "model-bone.h"
#include "model-light.h"
#include "model-types.h"

struct _LIMdlNode
{
	int type;
	char* name;
	LIMdlModel* model;
	LIMdlNode* parent;
	struct
	{
		int count;
		LIMdlNode** array;
	} nodes;
	struct
	{
		float local_scale;
		float global_scale;
		LIMatTransform rest;
		LIMatTransform local;
		LIMatTransform global;
	} transform;
	union
	{
		LIMdlBone bone;
		LIMdlLight light;
	};
};

LIAPICALL (LIMdlNode*, limdl_node_new, (
	LIMdlModel* model));

LIAPICALL (LIMdlNode*, limdl_node_copy, (
	const LIMdlNode* node));

LIAPICALL (void, limdl_node_free, (
	LIMdlNode* self));

LIAPICALL (LIMdlNode*, limdl_node_find_node, (
	const LIMdlNode* self,
	const char*      name));

LIAPICALL (int, limdl_node_read, (
	LIMdlNode*   self,
	LIArcReader* reader));

LIAPICALL (void, limdl_node_rebuild, (
	LIMdlNode* self,
	int        recursive));

LIAPICALL (int, limdl_node_write, (
	const LIMdlNode* self,
	LIArcWriter*     writer));

LIAPICALL (LIMdlNode*, limdl_node_get_child, (
	const LIMdlNode* self,
	int              index));

LIAPICALL (int, limdl_node_get_child_count, (
	const LIMdlNode* self));

LIAPICALL (int, limdl_node_get_child_total, (
	const LIMdlNode* self));

LIAPICALL (const char*, limdl_node_get_name, (
	const LIMdlNode* self));

LIAPICALL (void, limdl_node_get_pose_axes, (
	const LIMdlNode* self,
	LIMatVector*     x,
	LIMatVector*     y,
	LIMatVector*     z));

LIAPICALL (void, limdl_node_set_local_transform, (
	LIMdlNode*            self,
	float                 scale,
	const LIMatTransform* value));

LIAPICALL (void, limdl_node_get_rest_transform, (
	const LIMdlNode* self,
	LIMatTransform*  value));

LIAPICALL (void, limdl_node_get_world_transform, (
	const LIMdlNode* self,
	float*           scale,
	LIMatTransform*  value));

LIAPICALL (LIMdlNodeType, limdl_node_get_type, (
	const LIMdlNode* self));

/*****************************************************************************/

#define LIMDL_ITER_STACK_MAX 16
#define LIMDL_FOREACH_NODE(iter, array) \
	for (limdl_node_iter_start (&iter, array) ; \
	     iter.depth >= 0 ; \
	     limdl_node_iter_next (&iter))

typedef struct _LIMdlNodeIterArray LIMdlNodeIterArray;
struct _LIMdlNodeIterArray
{
	int count;
	LIMdlNode** array;
};

typedef struct _LIMdlNodeIterStack LIMdlNodeIterStack;
struct _LIMdlNodeIterStack
{
	LIMdlNode* node;
	int index;
};

typedef struct _LIMdlNodeIter LIMdlNodeIter;
struct _LIMdlNodeIter
{
	const LIMdlNodeIterArray* array;
	LIMdlNode* value;
	int depth;
	int index;
	int root;
	LIMdlNodeIterStack* pointer;
	LIMdlNodeIterStack stack[LIMDL_ITER_STACK_MAX];
};

#ifdef __cplusplus
extern "C" {
#endif

static inline void
limdl_node_iter_start (LIMdlNodeIter* self,
                       const void*    array)
{
	self->root = 0;
	self->index = 0;
	self->array = (const LIMdlNodeIterArray*) array;
	self->pointer = self->stack;
	if (self->array->count)
	{
		self->value = self->array->array[0];
		self->pointer->index = 0;
		self->pointer->node = self->value;
		self->pointer++;
		self->depth = 0;
	}
	else
	{
		self->value = NULL;
		self->depth = -1;
	}
}

static inline int
limdl_node_iter_next (LIMdlNodeIter* self)
{
	/* Descend if found children. */
	if (self->pointer[-1].node->nodes.count && self->depth < LIMDL_ITER_STACK_MAX)
	{
		self->value = self->pointer[-1].node->nodes.array[self->pointer[-1].index];
		self->pointer->index = 0;
		self->pointer->node = self->value;
		self->pointer++;
		self->depth++;
		return 1;
	}

	/* Move to the next node. */
	/* Ascend and retry if siblings ended. */
	while (++self->pointer[-1].index >= self->pointer[-1].node->nodes.count)
	{
		if (self->depth == 0)
		{
			/* Next root node if old one ended. */
			if (++self->root < self->array->count)
			{
				self->value = self->array->array[self->root];
				self->pointer->index = 0;
				self->pointer->node = self->value;
				self->pointer++;
				self->depth = 0;
				return 1;
			}
			else
			{
				self->pointer = self->stack;
				self->value = NULL;
				self->depth = -1;
				return 0;
			}
		}
		else
		{
			/* Ascend one node. */
			self->pointer--;
			self->depth--;
		}
	}

	/* Return found node. */
	self->value = self->pointer[-1].node->nodes.array[self->pointer[-1].index];
	return 1;
}

#ifdef __cplusplus
}
#endif

#endif
