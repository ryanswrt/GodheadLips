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

#ifndef __RENDER21_OBJECT_H__
#define __RENDER21_OBJECT_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"
#include "render.h"
#include "render-buffer.h"
#include "render-model.h"
#include "render-types.h"

LIAPICALL (LIRenObject21*, liren_object21_new, (
	LIRenScene21* scene,
	int           id));

LIAPICALL (void, liren_object21_free, (
	LIRenObject21* self));

LIAPICALL (void, liren_object21_deform, (
	LIRenObject21* self));

LIAPICALL (void, liren_object21_particle_animation, (
	LIRenObject21* self,
	float          start,
	int            loop));

LIAPICALL (void, liren_object21_update, (
	LIRenObject21* self,
	float          secs));

LIAPICALL (void, liren_object21_get_bounds, (
	const LIRenObject21* self,
	LIMatAabb*           result));

LIAPICALL (LIRenModel21*, liren_object21_get_model, (
	LIRenObject21* self));

LIAPICALL (int, liren_object21_set_model, (
	LIRenObject21* self,
	LIRenModel21*  model));

LIAPICALL (int, liren_object21_set_pose, (
	LIRenObject21* self,
	LIMdlPose*     pose));

LIAPICALL (int, liren_object21_get_realized, (
	const LIRenObject21* self));

LIAPICALL (int, liren_object21_set_realized, (
	LIRenObject21* self,
	int            value));

LIAPICALL (void, liren_object21_get_transform, (
	LIRenObject21*  self,
	LIMatTransform* value));

LIAPICALL (void, liren_object21_set_transform, (
	LIRenObject21*        self,
	const LIMatTransform* value));

#endif
