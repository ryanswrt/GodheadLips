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

#ifndef __ALGORITHM_BST_H__
#define __ALGORITHM_BST_H__

#include <lipsofsuna/system.h>

typedef int (*LIAlgBstCompare)(const void*, const void*);
typedef void (*LIAlgBstForeach)(void* a);
typedef void (*LIAlgBstFree)(void*);
typedef void* (*LIAlgBstMalloc)(size_t);
typedef int (*LIAlgBstMatch)(const void*, const void*);

typedef struct _LIAlgBstNode LIAlgBstNode;
struct _LIAlgBstNode
{
	void* data;
	LIAlgBstNode* left;
	LIAlgBstNode* right;
	LIAlgBstNode* parent;
};

typedef struct _LIAlgBst LIAlgBst;
struct _LIAlgBst
{
	int size;
	int max_size;
	LIAlgBstNode* root;
	LIAlgBstCompare cmp;
	LIAlgBstFree free;
	LIAlgBstMalloc malloc;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAlgBst*
lialg_bst_new (LIAlgBstCompare cmp,
               LIAlgBstMalloc  malloc,
               LIAlgBstFree    free);

void
lialg_bst_free (LIAlgBst* self);

void
lialg_bst_clear (LIAlgBst* self);

LIAlgBstNode*
lialg_bst_find (LIAlgBst*   self,
                const void* data);

LIAlgBstNode*
lialg_bst_find_by_index (LIAlgBst* self,
                         int       index);

void
lialg_bst_foreach (LIAlgBst*       self,
                   LIAlgBstForeach func);

LIAlgBstNode*
lialg_bst_insert (LIAlgBst* self,
                  void*     data);

void
lialg_bst_link (LIAlgBst*     self,
                LIAlgBstNode* node);

LIAlgBstNode*
lialg_bst_match (LIAlgBst*     self,
                 LIAlgBstMatch func,
                 const void*   data);

void
lialg_bst_remove (LIAlgBst*     self,
                  LIAlgBstNode* node);

void
lialg_bst_unlink (LIAlgBst*     self,
                  LIAlgBstNode* node);

#ifdef __cplusplus
}
#endif

#endif
