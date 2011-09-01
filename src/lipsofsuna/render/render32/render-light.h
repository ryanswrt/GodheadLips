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

#ifndef __RENDER32_LIGHT_H__
#define __RENDER32_LIGHT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-types.h"

/* FIXME: Should be configurable. */
#define SHADOWMAPSIZE 512

LIAPICALL (LIRenLight32*, liren_light32_new, (
	LIRenScene32*  scene,
	const float* ambient,
	const float* diffuse,
	const float* specular,
	const float* equation,
	float        cutoff,
	float        exponent,
	int          shadow));

LIAPICALL (LIRenLight32*, liren_light32_new_directional, (
	LIRenScene32*  scene,
	const float* ambient,
	const float* diffuse,
	const float* specular));

LIAPICALL (LIRenLight32*, liren_light32_new_from_model, (
	LIRenScene32*      scene,
	const LIMdlNode* light));

LIAPICALL (void, liren_light32_free, (
	LIRenLight32* self));

LIAPICALL (int, liren_light32_compare, (
	const LIRenLight32* self,
	const LIRenLight32* light));

LIAPICALL (void, liren_light32_update, (
	LIRenLight32* self));

LIAPICALL (void, liren_light32_update_cache, (
	LIRenLight32*   self,
	LIRenContext32* context));

LIAPICALL (void, liren_light32_update_projection, (
	LIRenLight32* self));

LIAPICALL (void, liren_light32_get_ambient, (
	LIRenLight32* self,
	float*      value));

LIAPICALL (void, liren_light32_set_ambient, (
	LIRenLight32*  self,
	const float* value));

LIAPICALL (int, liren_light32_get_bounds, (
	const LIRenLight32* self,
	LIMatAabb*        result));

LIAPICALL (void, liren_light32_get_diffuse, (
	LIRenLight32* self,
	float*      value));

LIAPICALL (void, liren_light32_set_diffuse, (
	LIRenLight32*  self,
	const float* value));

LIAPICALL (void, liren_light32_get_direction, (
	const LIRenLight32* self,
	LIMatVector*      value));

LIAPICALL (void, liren_light32_set_direction, (
	LIRenLight32*        self,
	const LIMatVector* value));

LIAPICALL (void, liren_light32_set_directional, (
	LIRenLight32* self,
	int         value));

LIAPICALL (int, liren_light32_get_enabled, (
	const LIRenLight32* self));

LIAPICALL (void, liren_light32_get_equation, (
	LIRenLight32* self,
	float*      value));

LIAPICALL (void, liren_light32_set_equation, (
	LIRenLight32*  self,
	const float* value));

LIAPICALL (void, liren_light32_get_modelview, (
	const LIRenLight32* self,
	LIMatMatrix*      value));

LIAPICALL (void, liren_light32_get_position, (
	const LIRenLight32* self,
	GLfloat*          value));

LIAPICALL (float, liren_light32_get_priority, (
	LIRenLight32* self));

LIAPICALL (void, liren_light32_set_priority, (
	LIRenLight32* self,
	float       value));

LIAPICALL (void, liren_light32_get_projection, (
	const LIRenLight32* self,
	LIMatMatrix*      value));

LIAPICALL (void, liren_light32_set_projection, (
	LIRenLight32*        self,
	const LIMatMatrix* value));

LIAPICALL (LIRenScene32*, liren_light32_get_scene, (
	const LIRenLight32* self));

LIAPICALL (int, liren_light32_get_shadow, (
	const LIRenLight32* self));

LIAPICALL (void, liren_light32_set_shadow, (
	LIRenLight32* self,
	int         value));

LIAPICALL (float, liren_light32_get_shadow_far, (
	const LIRenLight32* self));

LIAPICALL (void, liren_light32_set_shadow_far, (
	LIRenLight32* self,
	float       value));

LIAPICALL (float, liren_light32_get_shadow_near, (
	const LIRenLight32* self));

LIAPICALL (void, liren_light32_set_shadow_near, (
	LIRenLight32* self,
	float       value));

LIAPICALL (void, liren_light32_get_specular, (
	LIRenLight32* self,
	float*      value));

LIAPICALL (void, liren_light32_set_specular, (
	LIRenLight32*  self,
	const float* value));

LIAPICALL (float, liren_light32_get_spot_cutoff, (
	const LIRenLight32* self));

LIAPICALL (void, liren_light32_set_spot_cutoff, (
	LIRenLight32* self,
	float       value));

LIAPICALL (float, liren_light32_get_spot_exponent, (
	const LIRenLight32* self));

LIAPICALL (void, liren_light32_set_spot_exponent, (
	LIRenLight32* self,
	float       value));

LIAPICALL (void, liren_light32_get_transform, (
	LIRenLight32*     self,
	LIMatTransform* value));

LIAPICALL (void, liren_light32_set_transform, (
	LIRenLight32*           self,
	const LIMatTransform* transform));

LIAPICALL (int, liren_light32_get_type, (
	const LIRenLight32* self));

#endif
