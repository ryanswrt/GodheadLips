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

#ifndef __RENDER32_OBJECT_H__
#define __RENDER32_OBJECT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-buffer.h"
#include "render-material.h"
#include "render-model.h"
#include "render-types.h"

LIAPICALL (LIRenObject32*, liren_object32_new, (
	LIRenScene32* scene,
	int           id));

LIAPICALL (void, liren_object32_free, (
	LIRenObject32* self));

LIAPICALL (void, liren_object32_deform, (
	LIRenObject32* self));

LIAPICALL (void, liren_object32_particle_animation, (
	LIRenObject32* self,
	float          start,
	int            loop));

LIAPICALL (void, liren_object32_update, (
	LIRenObject32* self,
	float          secs));

LIAPICALL (void, liren_object32_get_bounds, (
	const LIRenObject32* self,
	LIMatAabb*           result));

LIAPICALL (void, liren_object32_get_center, (
	const LIRenObject32* self,
	LIMatVector*         center));

LIAPICALL (int, liren_object32_set_effect, (
	LIRenObject32* self,
	const char*    shader,
	const float*   params));

LIAPICALL (LIRenModel32*, liren_object32_get_model, (
	LIRenObject32* self));

LIAPICALL (int, liren_object32_set_model, (
	LIRenObject32* self,
	LIRenModel32*  model));

LIAPICALL (int, liren_object32_set_pose, (
	LIRenObject32* self,
	LIMdlPose*     pose));

LIAPICALL (int, liren_object32_get_realized, (
	const LIRenObject32* self));

LIAPICALL (int, liren_object32_set_realized, (
	LIRenObject32* self,
	int            value));

LIAPICALL (void, liren_object32_get_transform, (
	LIRenObject32*  self,
	LIMatTransform* value));

LIAPICALL (void, liren_object32_set_transform, (
	LIRenObject32*        self,
	const LIMatTransform* value));

#endif
