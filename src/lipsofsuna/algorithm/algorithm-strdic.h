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

#ifndef __ALGORITHM_STRDIC_H__
#define __ALGORITHM_STRDIC_H__

#include "lipsofsuna/system.h"
#include "algorithm-bst.h"

typedef struct _LIAlgStrdic LIAlgStrdic;
typedef struct _LIAlgStrdicNode LIAlgStrdicNode;
typedef struct _LIAlgStrdicIter LIAlgStrdicIter;

struct _LIAlgStrdic
{
	int size;
	LIAlgBst* tree;
	LIAlgStrdicNode* list;
};

struct _LIAlgStrdicNode
{
	char* key;
	void* value;
	LIAlgBstNode node;
	LIAlgStrdicNode* prev;
	LIAlgStrdicNode* next;
};

struct _LIAlgStrdicIter
{
	LIAlgStrdic* assoc;
	char* key;
	void* value;
	LIAlgStrdicNode* node;
	LIAlgStrdicNode* next;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LIAlgStrdic*, lialg_strdic_new, ());

LIAPICALL (void, lialg_strdic_free, (
	LIAlgStrdic* self));

LIAPICALL (void, lialg_strdic_clear, (
	LIAlgStrdic* self));

LIAPICALL (void*, lialg_strdic_find, (
	LIAlgStrdic* self,
	const char*  key));

LIAPICALL (LIAlgStrdicNode*, lialg_strdic_find_node, (
	LIAlgStrdic* self,
	const char*  key));

LIAPICALL (LIAlgStrdicNode*, lialg_strdic_insert, (
	LIAlgStrdic* self,
	const char*  key,
	void*        value));

LIAPICALL (int, lialg_strdic_remove, (
	LIAlgStrdic* self,
	const char*  key));

LIAPICALL (void, lialg_strdic_remove_node, (
	LIAlgStrdic*     self,
	LIAlgStrdicNode* node));

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#define LIALG_STRDIC_FOREACH(iter, assoc) \
	for (lialg_strdic_iter_start (&iter, assoc) ; \
	     iter.node != NULL ; \
	     lialg_strdic_iter_next (&iter))

static inline void
lialg_strdic_iter_start (LIAlgStrdicIter* self,
                         LIAlgStrdic*     assoc)
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
lialg_strdic_iter_next (LIAlgStrdicIter* self)
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
