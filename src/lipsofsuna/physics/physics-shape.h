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

#ifndef __PHYSICS_SHAPE_H__
#define __PHYSICS_SHAPE_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "physics.h"
#include "physics-types.h"

LIAPICALL (LIPhyShape*, liphy_shape_new, (
	LIPhyPhysics* physics));

LIAPICALL (void, liphy_shape_free, (
	LIPhyShape* self));

LIAPICALL (void, liphy_object_clear, (
	LIPhyShape* self));

LIAPICALL (int, liphy_shape_add_aabb, (
	LIPhyShape*           self,
	const LIMatAabb*      aabb,
	const LIMatTransform* transform));

LIAPICALL (int, liphy_shape_add_convex, (
	LIPhyShape*           self,
	const LIMatVector*    vertices,
	int                   count,
	const LIMatTransform* transform));

LIAPICALL (int, liphy_shape_add_model_shape, (
	LIPhyShape*           self,
	const LIMdlShape*     shape,
	const LIMatTransform* transform,
	float                 scale));

LIAPICALL (int, liphy_shape_add_shape, (
	LIPhyShape*           self,
	LIPhyShape*           shape,
	const LIMatTransform* transform));

LIAPICALL (void, liphy_shape_clear, (
	LIPhyShape* self));

LIAPICALL (void, liphy_shape_get_inertia, (
	const LIPhyShape* self,
	float             mass,
	LIMatVector*      result));

LIAPICALL (void, liphy_shape_set_center_of_mass, (
	LIPhyShape*        self,
	const LIMatVector* center));

#endif
