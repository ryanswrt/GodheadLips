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

#ifndef __ALGORITHM_U32DIC_H__
#define __ALGORITHM_U32DIC_H__

#include <lipsofsuna/system.h>
#include "algorithm-bst.h"

typedef struct _LIAlgU32dic LIAlgU32dic;
typedef struct _LIAlgU32dicNode LIAlgU32dicNode;
typedef struct _LIAlgU32dicIter LIAlgU32dicIter;

struct _LIAlgU32dic
{
	int size;
	LIAlgBst* tree;
	LIAlgU32dicNode* list;
};

struct _LIAlgU32dicNode
{
	uint32_t key;
	void* value;
	LIAlgBstNode node;
	LIAlgU32dicNode* prev;
	LIAlgU32dicNode* next;
};

struct _LIAlgU32dicIter
{
	LIAlgU32dic* assoc;
	uint32_t key;
	void* value;
	LIAlgU32dicNode* node;
	LIAlgU32dicNode* next;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LIAlgU32dic*, lialg_u32dic_new, ());

LIAPICALL (void, lialg_u32dic_free, (
	LIAlgU32dic* self));

LIAPICALL (void, lialg_u32dic_clear, (
	LIAlgU32dic* self));

LIAPICALL (void*, lialg_u32dic_find, (
	LIAlgU32dic* self,
	uint32_t     key));

LIAPICALL (LIAlgU32dicNode*, lialg_u32dic_find_node, (
	LIAlgU32dic* self,
	uint32_t     key));

LIAPICALL (LIAlgU32dicNode*, lialg_u32dic_insert, (
	LIAlgU32dic* self,
	uint32_t     key,
	void*        value));

LIAPICALL (int, lialg_u32dic_remove, (
	LIAlgU32dic* self,
	uint32_t     key));

LIAPICALL (void, lialg_u32dic_remove_node, (
	LIAlgU32dic*     self,
	LIAlgU32dicNode* node));

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#define LIALG_U32DIC_FOREACH(iter, assoc) \
	for (lialg_u32dic_iter_start (&iter, assoc) ; \
	     iter.node != NULL ; \
	     lialg_u32dic_iter_next (&iter))

static inline void
lialg_u32dic_iter_start (LIAlgU32dicIter* self,
                         LIAlgU32dic*     assoc)
{
	self->assoc = assoc;
	if (assoc->list == NULL)
	{
		self->node = NULL;
		self->next = NULL;
		self->key = 0;
		self->value = NULL;
	}
	else
	{
		self->node = assoc->list;
		self->next = self->node->next;
		self->key = self->node->key;
		self->value = self->node->value;
	}
}

static inline int
lialg_u32dic_iter_next (LIAlgU32dicIter* self)
{
	if (self->next == NULL)
	{
		self->node = NULL;
		self->next = NULL;
		self->key = 0;
		self->value = NULL;
		return 0;
	}
	else
	{
		self->node = self->next;
		self->next = self->node->next;
		self->key = self->node->key;
		self->value = self->node->value;
		return 1;
	}
}

#endif
