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

#ifndef __ALGORITHM_OCTREE_H__
#define __ALGORITHM_OCTREE_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"

typedef struct _LIAlgOcnode LIAlgOcnode;
typedef struct _LIAlgOctree LIAlgOctree;
typedef void (*LIAlgOceach)(void*, void*);

struct _LIAlgOcnode
{
	LIAlgOcnode* children[2][2][2];
};

struct _LIAlgOctree
{
	int depth;
	LIAlgOcnode* root;
};

LIAPICALL (LIAlgOctree*, lialg_octree_new, (
	int depth));

LIAPICALL (void, lialg_octree_free, (
	LIAlgOctree* self));

LIAPICALL (void, lialg_octree_foreach, (
	LIAlgOctree* self,
	LIAlgOceach  func,
	void*        data));

LIAPICALL (void*, lialg_octree_get_data_offset, (
	LIAlgOctree* self,
	int          x,
	int          y,
	int          z));

LIAPICALL (int, lialg_octree_set_data_offset, (
	LIAlgOctree* self,
	int          x,
	int          y,
	int          z,
	void*        data));

LIAPICALL (void*, lialg_octree_get_data_point, (
	LIAlgOctree*       self,
	const LIMatVector* point));

LIAPICALL (int, lialg_octree_set_data_point, (
	LIAlgOctree*       self,
	const LIMatVector* point,
	void*              data));

LIAPICALL (int, lialg_octree_get_size, (
	const LIAlgOctree* self));

#endif
