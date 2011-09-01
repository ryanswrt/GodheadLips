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

#ifndef __PHYSICS_TERRAIN_H__
#define __PHYSICS_TERRAIN_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "lipsofsuna/voxel.h"
#include "physics.h"
#include "physics-types.h"

LIAPICALL (LIPhyTerrain*, liphy_terrain_new, (
	LIPhyPhysics* physics,
	LIVoxManager* voxels,
	int           collision_group,
	int           collision_mask));

LIAPICALL (void, liphy_terrain_free, (
	LIPhyTerrain* self));

LIAPICALL (int, liphy_terrain_cast_ray, (
	const LIPhyTerrain* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	LIPhyCollision*     result));

LIAPICALL (int, liphy_terrain_cast_shape, (
	const LIPhyTerrain* self,
	const LIMatTransform* start,
	const LIMatTransform* end,
	const LIPhyShape*     shape,
	LIPhyCollision*       result));

LIAPICALL (int, liphy_terrain_cast_sphere, (
	const LIPhyTerrain* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	float               radius,
	LIPhyCollision*     result));

LIAPICALL (void, liphy_terrain_set_realized, (
	LIPhyTerrain* self,
	int           value));

#endif
