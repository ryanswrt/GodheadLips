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

#ifndef __PHYSICS_OBJECT_H__
#define __PHYSICS_OBJECT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "physics.h"
#include "physics-shape.h"
#include "physics-types.h"

LIAPICALL (LIPhyObject*, liphy_object_new, (
	LIPhyPhysics*    physics,
	uint32_t         id,
	LIPhyModel*      model,
	LIPhyControlMode control_mode));

LIAPICALL (void, liphy_object_free, (
	LIPhyObject* self));

LIAPICALL (int, liphy_object_approach, (
	LIPhyObject*       self,
	const LIMatVector* target,
	float              speed,
	float              dist));

LIAPICALL (int, liphy_object_cast_ray, (
	LIPhyObject*       self,
	const LIMatVector* relsrc,
	const LIMatVector* reldst,
	LIPhyCollision*    result));

LIAPICALL (int, liphy_object_cast_sphere, (
	LIPhyObject*       self,
	const LIMatVector* relsrc,
	const LIMatVector* reldst,
	float              radius,
	LIPhyCollision*    result));

LIAPICALL (void, liphy_object_impulse, (
	LIPhyObject*       self,
	const LIMatVector* point,
	const LIMatVector* impulse));

LIAPICALL (void, liphy_object_jump, (
	LIPhyObject*       self,
	const LIMatVector* impulse));

LIAPICALL (LIPhyObject*, liphy_object_scan_sphere, (
	LIPhyObject*       self,
	const LIMatVector* relctr,
	float              radius));

LIAPICALL (int, liphy_object_get_activated, (
	LIPhyObject* self));

LIAPICALL (void, liphy_object_set_activated, (
	LIPhyObject* self,
	int          value));

LIAPICALL (void, liphy_object_get_angular, (
	const LIPhyObject* self,
	LIMatVector*       value));

LIAPICALL (void, liphy_object_set_angular, (
	LIPhyObject*       self,
	const LIMatVector* value));

LIAPICALL (void, liphy_object_get_bounds, (
	const LIPhyObject* self,
	LIMatAabb*         result));

LIAPICALL (void, liphy_object_get_bounds_internal, (
	const LIPhyObject* self,
	LIMatAabb*         result));

LIAPICALL (int, liphy_object_get_collision_group, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_collision_group, (
	LIPhyObject* self,
	int          mask));

LIAPICALL (int, liphy_object_get_collision_mask, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_collision_mask, (
	LIPhyObject* self,
	int          mask));

LIAPICALL (int, liphy_object_get_contact_events, (
	LIPhyObject* self));

LIAPICALL (void, liphy_object_set_contact_events, (
	LIPhyObject* self,
	int          value));

LIAPICALL (LIPhyControlMode, liphy_object_get_control_mode, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_control_mode, (
	LIPhyObject*     self,
	LIPhyControlMode value));

LIAPICALL (LIPhyPhysics*, liphy_object_get_engine, (
	LIPhyObject* self));

LIAPICALL (float, liphy_object_get_friction_liquid, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_friction_liquid, (
	LIPhyObject* self,
	float        value));

LIAPICALL (void, liphy_object_get_gravity, (
	const LIPhyObject* self,
	LIMatVector*       value));

LIAPICALL (void, liphy_object_set_gravity, (
	LIPhyObject*       self,
	const LIMatVector* value));

LIAPICALL (void, liphy_object_get_gravity_liquid, (
	const LIPhyObject* self,
	LIMatVector*       value));

LIAPICALL (void, liphy_object_set_gravity_liquid, (
	LIPhyObject*       self,
	const LIMatVector* value));

LIAPICALL (int, liphy_object_get_ground, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_get_inertia, (
	LIPhyObject* self,
	LIMatVector* result));

LIAPICALL (float, liphy_object_get_mass, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_mass, (
	LIPhyObject* self,
	float        value));

LIAPICALL (LIPhyModel*, liphy_object_get_model, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_model, (
	LIPhyObject* self,
	LIPhyModel*  model));

LIAPICALL (float, liphy_object_get_movement, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_movement, (
	LIPhyObject* self,
	float        value));

LIAPICALL (int, liphy_object_get_realized, (
	const LIPhyObject* self));

LIAPICALL (int, liphy_object_set_realized, (
	LIPhyObject* self,
	int          value));

LIAPICALL (void, liphy_object_set_rotating, (
	LIPhyObject* self,
	float        value));

LIAPICALL (const char*, liphy_object_get_shape, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_shape, (
	LIPhyObject* self,
	const char*  value));

LIAPICALL (float, liphy_object_get_strafing, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_strafing, (
	LIPhyObject* self,
	float        value));

LIAPICALL (float, liphy_object_get_speed, (
	const LIPhyObject* self));

LIAPICALL (void, liphy_object_set_speed, (
	LIPhyObject* self,
	float        value));

LIAPICALL (void, liphy_object_get_transform, (
	const LIPhyObject* self,
	LIMatTransform*    value));

LIAPICALL (void, liphy_object_set_transform, (
	LIPhyObject*          self,
	const LIMatTransform* value));

LIAPICALL (void*, liphy_object_get_userdata, (
	LIPhyObject* self));

LIAPICALL (void, liphy_object_set_userdata, (
	LIPhyObject* self,
	void*        value));

LIAPICALL (void, liphy_object_get_velocity, (
	LIPhyObject* self,
	LIMatVector* value));

LIAPICALL (void, liphy_object_set_velocity, (
	LIPhyObject*       self,
	const LIMatVector* value));

#endif
