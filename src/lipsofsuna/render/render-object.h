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

#ifndef __RENDER_OBJECT_H__
#define __RENDER_OBJECT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-buffer.h"
#include "render-model.h"
#include "render-types.h"

LIAPICALL (LIRenObject*, liren_object_new, (
	LIRenScene* scene,
	int         id));

LIAPICALL (void, liren_object_free, (
	LIRenObject* self));

LIAPICALL (void, liren_object_deform, (
	LIRenObject* self));

LIAPICALL (void, liren_object_particle_animation, (
	LIRenObject* self,
	float        start,
	int          loop));

LIAPICALL (int, liren_object_set_effect, (
	LIRenObject* self,
	const char*  shader,
	const float* params));

LIAPICALL (LIRenModel*, liren_object_get_model, (
	LIRenObject* self));

LIAPICALL (int, liren_object_set_model, (
	LIRenObject* self,
	LIRenModel*  model));

LIAPICALL (int, liren_object_set_pose, (
	LIRenObject* self,
	LIMdlPose*   pose));

LIAPICALL (int, liren_object_set_realized, (
	LIRenObject* self,
	int          value));

LIAPICALL (void, liren_object_set_transform, (
	LIRenObject*          self,
	const LIMatTransform* value));

#endif
