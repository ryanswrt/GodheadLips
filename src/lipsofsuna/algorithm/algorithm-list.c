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
 * \addtogroup LIAlgList List
 * @{
 */

#include "algorithm-list.h"

/**
 * \brief Frees the list.
 *
 * \param self Linked list.
 */
void
lialg_list_free (LIAlgList* self)
{
	LIAlgList* tmp;
	LIAlgList* next;

	for (tmp = self ; tmp != NULL ; tmp = next)
	{
		next = tmp->next;
		lisys_free (tmp);
	}
}

/**
 * \brief Iterates throught the list to get its length.
 *
 * \param self Linked list.
 * \return Length of the list.
 */
int
lialg_list_get_length (LIAlgList* self)
{
	int len = 0;
	LIAlgList* tmp;

	for (tmp = self ; tmp != NULL ; tmp = tmp->next)
		len++;
	return len;
}

/**
 * \brief Calls the provided item for each data in the list.
 *
 * \param self Linked list.
 * \param call Function to call with the data.
 */
void
lialg_list_foreach (LIAlgList**      self,
                    LIAlgListForeach call)
{
	LIAlgList* tmp;

	for (tmp = *self ; tmp != NULL ; tmp = tmp->next)
		call (tmp->data);
}

/**
 * \brief Prepends a node to the list.
 *
 * \param self Linked list.
 * \param data Data to store in the node.
 * \return Nonzero on success.
 */
int
lialg_list_append (LIAlgList** self,
                   void*       data)
{
	LIAlgList* tmp;
	LIAlgList* last;

	tmp = (LIAlgList*) lisys_malloc (sizeof (LIAlgList));
	if (tmp == NULL)
		return 0;
	if (*self != NULL)
	{
		for (last = *self ; last->next != NULL ; last = last->next);
		tmp->prev = last;
		tmp->next = NULL;
		tmp->data = data;
		last->next = tmp;
	}
	else
	{
		tmp->prev = NULL;
		tmp->next = NULL;
		tmp->data = data;
		*self = tmp;
	}
	return 1;
}


/**
 * \brief Appends a readily allocated node to the list.
 *
 * The node must have been allocated with malloc and friends if you
 * intend to free the list with #lialg_list_free.
 *
 * \param self Linked list.
 * \param node Linked list node.
 */
void
lialg_list_append_node (LIAlgList** self,
                        LIAlgList*  node)
{
	LIAlgList* last;

	if (*self != NULL)
	{
		for (last = *self ; last->next != NULL ; last = last->next);
		node->prev = last;
		node->next = NULL;
		last->next = node;
	}
	else
	{
		node->prev = NULL;
		node->next = NULL;
		*self = node;
	}
}

/**
 * \brief Prepends a node to the list.
 *
 * \param self Linked list.
 * \param data Data to store in the node.
 * \return Nonzero on success.
 */
int
lialg_list_prepend (LIAlgList** self,
                    void*       data)
{
	LIAlgList* tmp;

	tmp = (LIAlgList*) lisys_malloc (sizeof (LIAlgList));
	if (tmp == NULL)
		return 0;
	tmp->prev = NULL;
	tmp->next = *self;
	tmp->data = data;
	if (*self != NULL)
		(*self)->prev = tmp;
	*self = tmp;
	return 1;
}

/**
 * \brief Prepends an already allocated node to the list.
 *
 * The node must have been allocated with malloc and friends if you
 * intend to free the list with #lialg_list_free. Otherwise use
 * #lialg_list_detach_node to remove the node from list without freeing it.
 *
 * \param self Linked list.
 * \param node Linked list node.
 */
void
lialg_list_prepend_node (LIAlgList** self,
                         LIAlgList*  node)
{
	node->prev = NULL;
	node->next = *self;
	if (*self != NULL)
		(*self)->prev = node;
	*self = node;
}

/**
 * \brief Prepends a node to the list with sorting.
 *
 * \param self Linked list.
 * \param data Data to store in the node.
 * \param cmp Sorting function.
 * \return Nonzero on success.
 */
int
lialg_list_prepend_sorted (LIAlgList**      self,
                           void*            data,
                           LIAlgListCompare cmp)
{
	LIAlgList* tmp;
	LIAlgList* ptr;

	/* Allocate memory. */
	tmp = (LIAlgList*) lisys_malloc (sizeof (LIAlgList));
	if (tmp == NULL)
		return 0;
	tmp->data = data;

	/* Handle empty lists. */
	if (*self == NULL)
	{
		tmp->next = NULL;
		tmp->prev = NULL;
		*self = tmp;
		return 1;
	}

	/* Handle prepending. */
	if (cmp (data, (*self)->data) <= 0)
	{
		tmp->prev = NULL;
		tmp->next = *self;
		(*self)->prev = tmp;
		*self = tmp;
		return 1;
	}

	/* Search for the spot. */
	for (ptr = *self ; ptr->next != NULL ; ptr = ptr->next)
	{
		if (cmp (data, ptr->next->data) <= 0)
			break;
	}

	/* Insert to the list. */
	tmp->prev = ptr;
	tmp->next = ptr->next;
	if (ptr->next != NULL)
		ptr->next->prev = tmp;
	ptr->next = tmp;
	return 1;
}

/**
 * \brief Removes a node from the list without freeing it.
 *
 * \param self Linked list.
 * \param node Node to detach.
 */
void
lialg_list_detach_node (LIAlgList** self,
                        LIAlgList*  node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	if (*self == node)
		*self = node->next;
}

/**
 * \brief Removes a node from the list.
 *
 * \param self Linked list.
 * \param node Node to remove.
 */
void
lialg_list_remove (LIAlgList** self,
                   LIAlgList*  node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	if (*self == node)
		*self = node->next;
	lisys_free (node);
}

/** @} */
/** @} */
