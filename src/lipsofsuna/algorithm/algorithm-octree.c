/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIAlgOctree Octree
 * @{
 */

#include <lipsofsuna/system.h>
#include "algorithm-octree.h"

#define LIALG_OCTREE_STACK_SIZE 256

static void private_foreach (
	LIAlgOctree* self,
	LIAlgOceach  func,
	void*        data,
	LIAlgOcnode* node,
	int          depth);

static void private_free_node (
	LIAlgOctree* self,
	LIAlgOcnode* node,
	int          depth);

/*****************************************************************************/

/**
 * \brief Creates a new octree.
 *
 * The number of elements along each axis will be 2^depth.
 *
 * \param depth The depth of the tree.
 * \return New octree or NULL.
 */
LIAlgOctree* lialg_octree_new (
	int depth)
{
	LIAlgOctree* self;

	lisys_assert (depth < LIALG_OCTREE_STACK_SIZE);

	self = lisys_calloc (1, sizeof (LIAlgOctree));
	if (self == NULL)
		return NULL;
	self->depth = depth;

	return self;
}

/**
 * \brief Frees the octree.
 *
 * Frees the octree structure and the branch nodes but not any
 * of the potential leaves stored to it.
 *
 * \param self Octree.
 */
void lialg_octree_free (
	LIAlgOctree* self)
{
	if (self->root != NULL)
		private_free_node (self, self->root, 0);
	lisys_free (self);
}

void lialg_octree_foreach (
	LIAlgOctree* self,
	LIAlgOceach  func,
	void*        data)
{
	if (self->root != NULL)
		private_foreach (self, func, data, self->root, 0);
}

void* lialg_octree_get_data_offset (
	LIAlgOctree* self,
	int          x,
	int          y,
	int          z)
{
	LIMatVector off;
	LIMatVector point;

	point = limat_vector_init (x, y, z);
	off = limat_vector_multiply (point, 1.0f / (1 << self->depth));

	return lialg_octree_get_data_point (self, &off);
}

int lialg_octree_set_data_offset (
	LIAlgOctree* self,
	int          x,
	int          y,
	int          z,
	void*        data)
{
	LIMatVector off;
	LIMatVector point;

	point = limat_vector_init (x, y, z);
	off = limat_vector_multiply (point, 1.0f / (1 << self->depth));

	return lialg_octree_set_data_point (self, &off, data);
}

void* lialg_octree_get_data_point (
	LIAlgOctree*       self,
	const LIMatVector* point)
{
	int i;
	int x;
	int y;
	int z;
	LIMatVector off = *point;
	LIAlgOcnode* node = self->root;

	/* Iterate branches. */
	for (i = 0 ; i < self->depth && node != NULL ; i++)
	{
		x = (off.x >= 0.5f);
		y = (off.y >= 0.5f);
		z = (off.z >= 0.5f);
		node = node->children[x][y][z];
		off.x = 2.0f * (off.x - 0.5f * x);
		off.y = 2.0f * (off.y - 0.5f * y);
		off.z = 2.0f * (off.z - 0.5f * z);
	}

	return node;
}

int lialg_octree_set_data_point (
	LIAlgOctree*       self,
	const LIMatVector* point,
	void*              data)
{
	int i;
	int x;
	int y;
	int z;
	LIMatVector off = *point;
	LIAlgOcnode** stack[LIALG_OCTREE_STACK_SIZE];
	LIAlgOcnode** node = &self->root;

	if (data != NULL)
	{
		/* Create branches. */
		for (i = 0 ; i < self->depth ; i++)
		{
			if (*node == NULL)
			{
				*node = lisys_calloc (1, sizeof (LIAlgOcnode));
				if (*node == NULL)
					return 0;
			}
			x = (off.x >= 0.5f);
			y = (off.y >= 0.5f);
			z = (off.z >= 0.5f);
			node = &((*node)->children[x][y][z]);
			off.x = 2.0f * (off.x - 0.5f * x);
			off.y = 2.0f * (off.y - 0.5f * y);
			off.z = 2.0f * (off.z - 0.5f * z);
		}

		/* Create the leaf. */
		*node = data;
	}
	else
	{
		/* Find the path to the leaf. */
		for (i = 0 ; i < self->depth ; i++)
		{
			if (*node == NULL)
				return 1;
			x = (off.x >= 0.5f);
			y = (off.y >= 0.5f);
			z = (off.z >= 0.5f);
			node = &((*node)->children[x][y][z]);
			stack[i] = node;
			off.x = 2.0f * (off.x - 0.5f * x);
			off.y = 2.0f * (off.y - 0.5f * y);
			off.z = 2.0f * (off.z - 0.5f * z);
		}

		/* Clear the leaf. */
		*node = NULL;

		/* Delete branches. */
		for (i-- ; i >= 0 ; i--)
		{
			node = stack[i];
			if ((*node)->children[0][0][0] != NULL ||
			    (*node)->children[1][0][0] != NULL ||
			    (*node)->children[0][1][0] != NULL ||
			    (*node)->children[1][1][0] != NULL ||
			    (*node)->children[0][0][1] != NULL ||
			    (*node)->children[1][0][1] != NULL ||
			    (*node)->children[0][1][1] != NULL ||
			    (*node)->children[1][1][1] != NULL)
				return 1;
			lisys_free (*node);
			*node = NULL;
		}
	}

	return 1;
}

/**
 * \brief Gets the number of elements along any axis.
 *
 * The total number of elements in the whole tree is size^3.
 *
 * \param self Octree.
 * \return Number of elements.
 */
int lialg_octree_get_size (
	const LIAlgOctree* self)
{
	return 1 << self->depth;
}

/*****************************************************************************/

static void private_foreach (
	LIAlgOctree* self,
	LIAlgOceach  func,
	void*        data,
	LIAlgOcnode* node,
	int          depth)
{
	if (depth == self->depth)
	{
		func (data, (void*) node);
		return;
	}
	depth++;
	if (node->children[0][0][0] != NULL)
		private_foreach (self, func, data, node->children[0][0][0], depth);
	if (node->children[1][0][0] != NULL)
		private_foreach (self, func, data, node->children[1][0][0], depth);
	if (node->children[0][1][0] != NULL)
		private_foreach (self, func, data, node->children[0][1][0], depth);
	if (node->children[1][1][0] != NULL)
		private_foreach (self, func, data, node->children[1][1][0], depth);
	if (node->children[0][0][1] != NULL)
		private_foreach (self, func, data, node->children[0][0][1], depth);
	if (node->children[1][0][1] != NULL)
		private_foreach (self, func, data, node->children[1][0][1], depth);
	if (node->children[0][1][1] != NULL)
		private_foreach (self, func, data, node->children[0][1][1], depth);
	if (node->children[1][1][1] != NULL)
		private_foreach (self, func, data, node->children[1][1][1], depth);
}

static void private_free_node (
	LIAlgOctree* self,
	LIAlgOcnode* node,
	int          depth)
{
	if (depth++ == self->depth)
		return;
	if (node->children[0][0][0] != NULL) private_free_node (self, node->children[0][0][0], depth);
	if (node->children[0][0][1] != NULL) private_free_node (self, node->children[0][0][1], depth);
	if (node->children[0][1][0] != NULL) private_free_node (self, node->children[0][1][0], depth);
	if (node->children[0][1][1] != NULL) private_free_node (self, node->children[0][1][1], depth);
	if (node->children[1][0][0] != NULL) private_free_node (self, node->children[1][0][0], depth);
	if (node->children[1][0][1] != NULL) private_free_node (self, node->children[1][0][1], depth);
	if (node->children[1][1][0] != NULL) private_free_node (self, node->children[1][1][0], depth);
	if (node->children[1][1][1] != NULL) private_free_node (self, node->children[1][1][1], depth);
}

/** @} */
/** @} */
