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
 * \addtogroup LIAlgU32dic U32dic
 * @{
 */

#include <lipsofsuna/system.h>
#include "algorithm-u32dic.h"

static int private_u32dic_node_compare (
	const LIAlgU32dicNode* self,
	const LIAlgU32dicNode* node);

/*****************************************************************************/

/**
 * \brief Creates a new associative array.
 * \return New associative array or NULL.
 */
LIAlgU32dic* lialg_u32dic_new ()
{
	LIAlgU32dic* self;

	self = (LIAlgU32dic*) lisys_malloc (sizeof (LIAlgU32dic));
	if (self == NULL)
		return NULL;
	self->size = 0;
	self->list = NULL;
	self->tree = lialg_bst_new ((LIAlgBstCompare) private_u32dic_node_compare, lisys_malloc, lisys_free);
	if (self->tree == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	return self;
}

/**
 * \brief Frees the associative array.
 * \param self Associative array.
 */
void lialg_u32dic_free (
	LIAlgU32dic* self)
{
	lialg_bst_foreach (self->tree, (LIAlgBstForeach) lisys_free);
	self->tree->root = NULL;
	lialg_bst_free (self->tree);
	lisys_free (self);
}

/**
 * \brief Clears the associative array.
 * \param self Associative array.
 */
void lialg_u32dic_clear (
	LIAlgU32dic* self)
{
	lialg_bst_foreach (self->tree, (LIAlgBstForeach) lisys_free);
	self->size = 0;
	self->list = NULL;
	self->tree->root = NULL;
	lialg_bst_clear (self->tree);
}

/**
 * \brief Finds a value from the associative array.
 * \param self Associative array.
 * \param key Key of the node.
 * \return Value or NULL.
 */
void* lialg_u32dic_find (
	LIAlgU32dic* self,
	uint32_t     key)
{
	LIAlgU32dicNode tmp;
	LIAlgU32dicNode* anode;
	LIAlgBstNode* tnode;

	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	anode = (LIAlgU32dicNode*) tnode->data;
	lisys_assert (&anode->node == tnode);
	return anode->value;
}

/**
 * \brief Finds a node from the associative array.
 * \param self Associative array.
 * \param key Key of the node.
 * \return Associative array node or NULL.
 */
LIAlgU32dicNode* lialg_u32dic_find_node (
	LIAlgU32dic* self,
	uint32_t     key)
{
	LIAlgU32dicNode tmp;
	LIAlgBstNode* tnode;

	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	lisys_assert (&((LIAlgU32dicNode*) tnode->data)->node == tnode);
	return (LIAlgU32dicNode*) tnode->data;
}

/**
 * \brief Inserts data to the associative array.
 * \param self Associative array.
 * \param key Key of the inserted node.
 * \param value Value of the inserted node.
 * \return Associative array node or NULL.
 */
LIAlgU32dicNode* lialg_u32dic_insert (
	LIAlgU32dic* self,
	uint32_t     key,
	void*        value)
{
	LIAlgU32dicNode* node;

	/* Create node. */
	node = (LIAlgU32dicNode*) lisys_malloc (sizeof (LIAlgU32dicNode));
	if (node == NULL)
		return NULL;
	node->key = key;
	node->value = value;
	node->prev = NULL;
	node->next = self->list;

	/* Link to list. */
	node->node.data = node;
	if (self->list != NULL)
		self->list->prev = node;
	self->list = node;

	/* Link to tree. */
	lialg_bst_link (self->tree, &node->node);
	self->size++;
	return node;
}

/**
 * \brief Removes data from the associative array.
 * \param self Associative array.
 * \param key Key of the removed node.
 * \return Nonzero if a node was removed.
 */
int lialg_u32dic_remove (
	LIAlgU32dic* self,
	uint32_t     key)
{
	LIAlgBstNode* tnode;
	LIAlgU32dicNode* anode;
	LIAlgU32dicNode tmp;
	
	/* Find node. */
	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return 1;
	anode = (LIAlgU32dicNode*) tnode->data;
	lisys_assert (&anode->node == tnode);

	/* Unlink from tree. */
	lialg_bst_unlink (self->tree, tnode);

	/* Remove from list. */
	if (anode->prev != NULL)
		anode->prev->next = anode->next;
	else
		self->list = anode->next;
	if (anode->next != NULL)
		anode->next->prev = anode->prev;
	lisys_free (anode);
	self->size--;
	return 0;
}

/**
 * \brief Removes a node from the associative array.
 * \param self Associative array.
 * \param node Node to remove.
 */
void lialg_u32dic_remove_node (
	LIAlgU32dic*     self,
	LIAlgU32dicNode* node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	else
		self->list = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	lialg_bst_unlink (self->tree, &node->node);
	lisys_free (node);
	self->size--;
}

/*****************************************************************************/

static int private_u32dic_node_compare (
	const LIAlgU32dicNode* self,
	const LIAlgU32dicNode* node)
{
	if (self->key < node->key) return -1;
	if (self->key > node->key) return 1;
	return 0;
}

/** @} */
/** @} */
