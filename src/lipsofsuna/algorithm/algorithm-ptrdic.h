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

#ifndef __ALGORITHM_PTRDIC_H__
#define __ALGORITHM_PTRDIC_H__

#include <lipsofsuna/system.h>
#include "algorithm-bst.h"

typedef struct _LIAlgPtrdic LIAlgPtrdic;
typedef struct _LIAlgPtrdicNode LIAlgPtrdicNode;
typedef struct _LIAlgPtrdicIter LIAlgPtrdicIter;

struct _LIAlgPtrdic
{
	int size;
	LIAlgBst* tree;
	LIAlgPtrdicNode* list;
};

struct _LIAlgPtrdicNode
{
	void* key;
	void* value;
	LIAlgBstNode node;
	LIAlgPtrdicNode* prev;
	LIAlgPtrdicNode* next;
};

struct _LIAlgPtrdicIter
{
	LIAlgPtrdic* assoc;
	void* key;
	void* value;
	LIAlgPtrdicNode* node;
	LIAlgPtrdicNode* next;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LIAlgPtrdic*, lialg_ptrdic_new, ());

LIAPICALL (void, lialg_ptrdic_free, (
	LIAlgPtrdic* self));

LIAPICALL (void, lialg_ptrdic_clear, (
	LIAlgPtrdic* self));

LIAPICALL (void*, lialg_ptrdic_find, (
	LIAlgPtrdic* self,
	void*        key));

LIAPICALL (LIAlgPtrdicNode*, lialg_ptrdic_find_node, (
	LIAlgPtrdic* self,
	void*        key));

LIAPICALL (LIAlgPtrdicNode*, lialg_ptrdic_insert, (
	LIAlgPtrdic* self,
	void*        key,
	void*        value));

LIAPICALL (int, lialg_ptrdic_remove, (
	LIAlgPtrdic* self,
	void*        key));

LIAPICALL (void, lialg_ptrdic_remove_node, (
	LIAlgPtrdic*     self,
	LIAlgPtrdicNode* node));

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#define LIALG_PTRDIC_FOREACH(iter, assoc) \
	for (lialg_ptrdic_iter_start (&iter, assoc) ; \
	     iter.node != NULL ; \
	     lialg_ptrdic_iter_next (&iter))

static inline void
lialg_ptrdic_iter_start (LIAlgPtrdicIter* self,
                         LIAlgPtrdic*     assoc)
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
lialg_ptrdic_iter_next (LIAlgPtrdicIter* self)
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
