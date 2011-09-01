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
 * \addtogroup LIAlgPriorityQueue PriorityQueue
 * @{
 */

#include <lipsofsuna/system.h>
#include "algorithm-priority-queue.h"

static int
private_node_compare (const LIAlgPriorityQueueNode* self,
                      const LIAlgPriorityQueueNode* node);

/*****************************************************************************/

/**
 * \brief Creates a new priority queue.
 *
 * \return New priority queue or NULL.
 */
LIAlgPriorityQueue*
lialg_priority_queue_new ()
{
	LIAlgPriorityQueue* self;

	self = (LIAlgPriorityQueue*) lisys_malloc (sizeof (LIAlgPriorityQueue));
	if (self == NULL)
		return NULL;
	self->tree = lialg_bst_new ((LIAlgBstCompare) private_node_compare, lisys_malloc, lisys_free);
	if (self->tree == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	return self;
}

/**
 * \brief Frees the priority queue.
 *
 * \param self Priority queue.
 */
void
lialg_priority_queue_free (LIAlgPriorityQueue* self)
{
	lialg_bst_foreach (self->tree, (LIAlgBstForeach) lisys_free);
	self->tree->root = NULL;
	lialg_bst_free (self->tree);
	lisys_free (self);
}

/**
 * \brief Clears the priority queue.
 *
 * \param self Priority queue.
 */
void
lialg_priority_queue_clear (LIAlgPriorityQueue* self)
{
	lialg_bst_foreach (self->tree, (LIAlgBstForeach) lisys_free);
	self->tree->root = NULL;
	lialg_bst_clear (self->tree);
}

/**
 * \brief Finds the value with the highest priority from the queue.
 *
 * \param self Priority queue.
 * \return Value or NULL.
 */
void*
lialg_priority_queue_find_highest (LIAlgPriorityQueue* self)
{
	LIAlgBstNode* tnode;
	LIAlgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->right != NULL)
		tnode = tnode->right;
	pnode = (LIAlgPriorityQueueNode*) tnode->data;
	lisys_assert (&pnode->node == tnode);
	return pnode->value;
}

/**
 * \brief Finds the node with the lowest priority from the queue.
 *
 * \param self Priority queue.
 * \return Priority queue node or NULL.
 */
LIAlgPriorityQueueNode*
lialg_priority_queue_find_highest_node (LIAlgPriorityQueue* self)
{
	LIAlgBstNode* tnode;
	LIAlgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->left != NULL)
		tnode = tnode->left;
	pnode = (LIAlgPriorityQueueNode*) tnode->data;
	lisys_assert (&pnode->node == tnode);
	return pnode;
}

/**
 * \brief Finds the value with the lowest priority from the queue.
 *
 * \param self Priority queue.
 * \return Value or NULL.
 */
void*
lialg_priority_queue_find_lowest (LIAlgPriorityQueue* self)
{
	LIAlgBstNode* tnode;
	LIAlgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->left != NULL)
		tnode = tnode->left;
	pnode = (LIAlgPriorityQueueNode*) tnode->data;
	lisys_assert (&pnode->node == tnode);
	return pnode->value;
}

/**
 * \brief Finds the node with the lowest priority from the queue.
 *
 * \param self Priority queue.
 * \return Priority queue node or NULL.
 */
LIAlgPriorityQueueNode*
lialg_priority_queue_find_lowest_node (LIAlgPriorityQueue* self)
{
	LIAlgBstNode* tnode;
	LIAlgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->left != NULL)
		tnode = tnode->left;
	pnode = (LIAlgPriorityQueueNode*) tnode->data;
	lisys_assert (&pnode->node == tnode);
	return pnode;
}

/**
 * \brief Inserts data to the priority queue.
 *
 * \param self Priority queue.
 * \param priority Priority of the inserted node.
 * \param value Value of the inserted node.
 * \return Priority queue node or NULL.
 */
LIAlgPriorityQueueNode*
lialg_priority_queue_insert (LIAlgPriorityQueue* self,
                             float               priority,
                             void*               value)
{
	LIAlgPriorityQueueNode* node;

	/* Create node. */
	node = (LIAlgPriorityQueueNode*) lisys_malloc (sizeof (LIAlgPriorityQueueNode));
	if (node == NULL)
		return NULL;
	node->priority = priority;
	node->value = value;

	/* Link to tree. */
	node->node.data = node;
	lialg_bst_link (self->tree, &node->node);
	return node;
}

/**
 * \brief Pops the value with the highest priority from the queue.
 *
 * \param self Priority queue.
 * \return Value or NULL.
 */
void*
lialg_priority_queue_pop_highest (LIAlgPriorityQueue* self)
{
	void* value;
	LIAlgBstNode* tnode;
	LIAlgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->right != NULL)
		tnode = tnode->right;
	pnode = (LIAlgPriorityQueueNode*) tnode->data;
	value = pnode->value;
	lisys_assert (&pnode->node == tnode);
	lialg_bst_unlink (self->tree, tnode);
	lisys_free (pnode);
	return value;
}

/**
 * \brief Pops the value with the lowest priority from the queue.
 *
 * \param self Priority queue.
 * \return Value or NULL.
 */
void*
lialg_priority_queue_pop_lowest (LIAlgPriorityQueue* self)
{
	void* value;
	LIAlgBstNode* tnode;
	LIAlgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->left != NULL)
		tnode = tnode->left;
	pnode = (LIAlgPriorityQueueNode*) tnode->data;
	value = pnode->value;
	lisys_assert (&pnode->node == tnode);
	lialg_bst_unlink (self->tree, tnode);
	lisys_free (pnode);
	return value;
}

/**
 * \brief Removes a node from the priority queue.
 *
 * \param self Priority queue.
 * \param node Node to remove.
 */
void
lialg_priority_queue_remove_node (LIAlgPriorityQueue*     self,
                                  LIAlgPriorityQueueNode* node)
{
	lialg_bst_unlink (self->tree, &node->node);
	lisys_free (node);
}

/*****************************************************************************/

static int
private_node_compare (const LIAlgPriorityQueueNode* self,
                      const LIAlgPriorityQueueNode* node)
{
	if (self->priority < node->priority) return -1;
	if (self->priority > node->priority) return 1;
	return 0;
}

/** @} */
/** @} */
