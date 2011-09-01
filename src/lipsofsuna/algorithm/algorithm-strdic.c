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
 * \addtogroup LIAlgStrdic Strdic
 * @{
 */

#include "lipsofsuna/system.h"
#include "algorithm-strdic.h"

static void private_strdic_node_free (
	LIAlgStrdicNode* self);

static int private_strdic_node_compare (
	const LIAlgStrdicNode* self,
	const LIAlgStrdicNode* node);

/*****************************************************************************/

/**
 * \brief Creates a new associative array.
 * \return New associative array or NULL.
 */
LIAlgStrdic* lialg_strdic_new ()
{
	LIAlgStrdic* self;

	self = (LIAlgStrdic*) lisys_malloc (sizeof (LIAlgStrdic));
	if (self == NULL)
		return NULL;
	self->size = 0;
	self->list = NULL;
	self->tree = lialg_bst_new ((LIAlgBstCompare) private_strdic_node_compare, lisys_malloc, lisys_free);
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
void lialg_strdic_free (
	LIAlgStrdic* self)
{
	lialg_bst_foreach (self->tree, (LIAlgBstForeach) private_strdic_node_free);
	self->tree->root = NULL;
	lialg_bst_free (self->tree);
	lisys_free (self);
}

/**
 * \brief Clears the associative array.
 * \param self Associative array.
 */
void lialg_strdic_clear (
	LIAlgStrdic* self)
{
	lialg_bst_foreach (self->tree, (LIAlgBstForeach) private_strdic_node_free);
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
void* lialg_strdic_find (
	LIAlgStrdic* self,
	const char*  key)
{
	LIAlgStrdicNode tmp;
	LIAlgStrdicNode* anode;
	LIAlgBstNode* tnode;

	tmp.key = (char*) key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	anode = (LIAlgStrdicNode*) tnode->data;
	lisys_assert (&anode->node == tnode);
	return anode->value;
}

/**
 * \brief Finds a node from the associative array.
 * \param self Associative array.
 * \param key Key of the node.
 * \return Associative array node or NULL.
 */
LIAlgStrdicNode* lialg_strdic_find_node (
	LIAlgStrdic* self,
	const char*  key)
{
	LIAlgStrdicNode tmp;
	LIAlgBstNode* tnode;

	tmp.key = (char*) key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	lisys_assert (&((LIAlgStrdicNode*) tnode->data)->node == tnode);
	return (LIAlgStrdicNode*) tnode->data;
}

/**
 * \brief Inserts data to the associative array.
 * \param self An associative array.
 * \param key Key of the inserted node.
 * \param value Value of the inserted node.
 * \return Associative array node or NULL.
 */
LIAlgStrdicNode* lialg_strdic_insert (
	LIAlgStrdic* self,
	const char*  key,
	void*        value)
{
	LIAlgStrdicNode* node;

	/* Create node. */
	node = (LIAlgStrdicNode*) lisys_malloc (sizeof (LIAlgStrdicNode));
	if (node == NULL)
		return NULL;
	node->key = lisys_string_dup (key);
	if (node->key == NULL)
	{
		lisys_free (node);
		return NULL;
	}
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
int lialg_strdic_remove (
	LIAlgStrdic* self,
	const char*    key)
{
	LIAlgBstNode* tnode;
	LIAlgStrdicNode* anode;
	LIAlgStrdicNode tmp;
	
	/* Find node. */
	tmp.key = (char*) key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return 0;
	anode = (LIAlgStrdicNode*) tnode->data;
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
	private_strdic_node_free (anode);
	self->size--;
	return 1;
}

/**
 * \brief Removes a node from the associative array.
 * \param self Associative array.
 * \param node Node to remove.
 */
void lialg_strdic_remove_node (
	LIAlgStrdic*     self,
	LIAlgStrdicNode* node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	else
		self->list = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	lialg_bst_unlink (self->tree, &node->node);
	private_strdic_node_free (node);
	self->size--;
}

/*****************************************************************************/

static void private_strdic_node_free (
	LIAlgStrdicNode* self)
{
	lisys_free (self->key);
	lisys_free (self);
}

static int private_strdic_node_compare (
	const LIAlgStrdicNode* self,
	const LIAlgStrdicNode* node)
{
	return strcmp (self->key, node->key);
}

/** @} */
/** @} */
