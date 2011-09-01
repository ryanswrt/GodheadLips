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

#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <lipsofsuna/callback.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "physics-types.h"

LIAPICALL (LIPhyPhysics*, liphy_physics_new, (
	LICalCallbacks* callbacks));

LIAPICALL (void, liphy_physics_free, (
	LIPhyPhysics* self));

LIAPICALL (int, liphy_physics_cast_ray, (
	const LIPhyPhysics* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	int                 group,
	int                 mask,
	LIPhyObject**       ignore_array,
	int                 ignore_count,
	LIPhyCollision*     result));

LIAPICALL (int, liphy_physics_cast_shape, (
	const LIPhyPhysics*   self,
	const LIMatTransform* start,
	const LIMatTransform* end,
	const LIPhyShape*     shape,
	int                   group,
	int                   mask,
	LIPhyObject**         ignore_array,
	int                   ignore_count,
	LIPhyCollision*       result));

LIAPICALL (int, liphy_physics_cast_sphere, (
	const LIPhyPhysics* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	float               radius,
	int                 group,
	int                 mask,
	LIPhyObject**       ignore_array,
	int                 ignore_count,
	LIPhyCollision*     result));

LIAPICALL (void, liphy_physics_clear_constraints, (
	LIPhyPhysics* self,
	LIPhyObject*  object));

LIAPICALL (LIPhyModel*, liphy_physics_find_model, (
	LIPhyPhysics* self,
	uint32_t      id));

LIAPICALL (LIPhyObject*, liphy_physics_find_object, (
	LIPhyPhysics* self,
	uint32_t      id));

LIAPICALL (void, liphy_physics_remove_model, (
	LIPhyPhysics* self,
	LIPhyModel*   model));

LIAPICALL (void, liphy_physics_update, (
	LIPhyPhysics* self,
	float         secs));

LIAPICALL (void*, liphy_physics_get_userdata, (
	LIPhyPhysics* self));

LIAPICALL (void, liphy_physics_set_userdata, (
	LIPhyPhysics* self,
	void*         data));

#endif
