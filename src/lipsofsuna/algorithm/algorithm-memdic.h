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

#ifndef __ALGORITHM_MEMDIC_H__
#define __ALGORITHM_MEMDIC_H__

#include "lipsofsuna/system.h"
#include "algorithm-bst.h"

typedef struct _LIAlgMemdic LIAlgMemdic;
typedef struct _LIAlgMemdicNode LIAlgMemdicNode;
typedef struct _LIAlgMemdicIter LIAlgMemdicIter;

struct _LIAlgMemdic
{
	int size;
	LIAlgBst* tree;
	LIAlgMemdicNode* list;
};

struct _LIAlgMemdicNode
{
	int keysize;
	void* key;
	void* value;
	LIAlgBstNode node;
	LIAlgMemdicNode* prev;
	LIAlgMemdicNode* next;
};

struct _LIAlgMemdicIter
{
	LIAlgMemdic* assoc;
	int keysize;
	void* key;
	void* value;
	LIAlgMemdicNode* node;
	LIAlgMemdicNode* next;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LIAlgMemdic*, lialg_memdic_new, ());

LIAPICALL (void, lialg_memdic_free, (
	LIAlgMemdic* self));

LIAPICALL (void, lialg_memdic_clear, (
	LIAlgMemdic* self));

LIAPICALL (void*, lialg_memdic_find, (
	LIAlgMemdic* self,
	const void*  key,
	int          keysize));

LIAPICALL (LIAlgMemdicNode*, lialg_memdic_find_node, (
	LIAlgMemdic* self,
	const void*  key,
	int          keysize));

LIAPICALL (LIAlgMemdicNode*, lialg_memdic_insert, (
	LIAlgMemdic* self,
	const void*  key,
	int          keysize,
	void*        value));

LIAPICALL (int, lialg_memdic_remove, (
	LIAlgMemdic* self,
	const void*  key,
	int          keysize));

LIAPICALL (void, lialg_memdic_remove_node, (
	LIAlgMemdic*     self,
	LIAlgMemdicNode* node));

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#define LIALG_MEMDIC_FOREACH(iter, assoc) \
	for (lialg_memdic_iter_start (&iter, assoc) ; \
	     iter.node != NULL ; \
	     lialg_memdic_iter_next (&iter))

static inline void
lialg_memdic_iter_start (LIAlgMemdicIter* self,
                         LIAlgMemdic*     assoc)
{
	self->assoc = assoc;
	if (assoc->list == NULL)
	{
		self->node = NULL;
		self->next = NULL;
		self->key = NULL;
		self->keysize = 0;
		self->value = NULL;
	}
	else
	{
		self->node = assoc->list;
		self->next = self->node->next;
		self->key = self->node->key;
		self->keysize = self->node->keysize;
		self->value = self->node->value;
	}
}

static inline int
lialg_memdic_iter_next (LIAlgMemdicIter* self)
{
	if (self->next == NULL)
	{
		self->node = NULL;
		self->next = NULL;
		self->key = NULL;
		self->keysize = 0;
		self->value = NULL;
		return 0;
	}
	else
	{
		self->node = self->next;
		self->next = self->node->next;
		self->key = self->node->key;
		self->keysize = self->node->keysize;
		self->value = self->node->value;
		return 1;
	}
}

#endif
