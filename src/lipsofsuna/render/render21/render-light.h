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

#ifndef __RENDER21_LIGHT_H__
#define __RENDER21_LIGHT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-types.h"

LIAPICALL (LIRenLight21*, liren_light21_new, (
	LIRenScene21* scene,
	const float*  ambient,
	const float*  diffuse,
	const float*  specular,
	const float*  equation,
	float         cutoff,
	float         exponent,
	int           shadow));

LIAPICALL (LIRenLight21*, liren_light21_new_directional, (
	LIRenScene21*  scene,
	const float* ambient,
	const float* diffuse,
	const float* specular));

LIAPICALL (LIRenLight21*, liren_light21_new_from_model, (
	LIRenScene21*    scene,
	const LIMdlNode* light));

LIAPICALL (void, liren_light21_free, (
	LIRenLight21* self));

LIAPICALL (int, liren_light21_compare, (
	const LIRenLight21* self,
	const LIRenLight21* light));

LIAPICALL (void, liren_light21_get_ambient, (
	LIRenLight21* self,
	float*      value));

LIAPICALL (void, liren_light21_set_ambient, (
	LIRenLight21* self,
	const float*  value));

LIAPICALL (int, liren_light21_get_bounds, (
	const LIRenLight21* self,
	LIMatAabb*          result));

LIAPICALL (void, liren_light21_get_diffuse, (
	LIRenLight21* self,
	float*        value));

LIAPICALL (void, liren_light21_set_diffuse, (
	LIRenLight21* self,
	const float*  value));

LIAPICALL (void, liren_light21_get_direction, (
	const LIRenLight21* self,
	LIMatVector*        value));

LIAPICALL (void, liren_light21_set_direction, (
	LIRenLight21*      self,
	const LIMatVector* value));

LIAPICALL (void, liren_light21_set_directional, (
	LIRenLight21* self,
	int           value));

LIAPICALL (int, liren_light21_get_enabled, (
	const LIRenLight21* self));

LIAPICALL (void, liren_light21_get_equation, (
	LIRenLight21* self,
	float*        value));

LIAPICALL (void, liren_light21_set_equation, (
	LIRenLight21* self,
	const float*  value));

LIAPICALL (void, liren_light21_get_modelview, (
	const LIRenLight21* self,
	LIMatMatrix*        value));

LIAPICALL (void, liren_light21_get_position, (
	const LIRenLight21* self,
	GLfloat*            value));

LIAPICALL (float, liren_light21_get_priority, (
	LIRenLight21* self));

LIAPICALL (void, liren_light21_set_priority, (
	LIRenLight21* self,
	float         value));

LIAPICALL (void, liren_light21_get_projection, (
	const LIRenLight21* self,
	LIMatMatrix*        value));

LIAPICALL (void, liren_light21_set_projection, (
	LIRenLight21*      self,
	const LIMatMatrix* value));

LIAPICALL (LIRenScene21*, liren_light21_get_scene, (
	const LIRenLight21* self));

LIAPICALL (int, liren_light21_get_shadow, (
	const LIRenLight21* self));

LIAPICALL (void, liren_light21_set_shadow, (
	LIRenLight21* self,
	int           value));

LIAPICALL (float, liren_light21_get_shadow_far, (
	const LIRenLight21* self));

LIAPICALL (void, liren_light21_set_shadow_far, (
	LIRenLight21* self,
	float         value));

LIAPICALL (float, liren_light21_get_shadow_near, (
	const LIRenLight21* self));

LIAPICALL (void, liren_light21_set_shadow_near, (
	LIRenLight21* self,
	float         value));

LIAPICALL (void, liren_light21_get_specular, (
	LIRenLight21* self,
	float*        value));

LIAPICALL (void, liren_light21_set_specular, (
	LIRenLight21* self,
	const float*  value));

LIAPICALL (float, liren_light21_get_spot_cutoff, (
	const LIRenLight21* self));

LIAPICALL (void, liren_light21_set_spot_cutoff, (
	LIRenLight21* self,
	float         value));

LIAPICALL (float, liren_light21_get_spot_exponent, (
	const LIRenLight21* self));

LIAPICALL (void, liren_light21_set_spot_exponent, (
	LIRenLight21* self,
	float         value));

LIAPICALL (void, liren_light21_get_transform, (
	LIRenLight21*   self,
	LIMatTransform* value));

LIAPICALL (void, liren_light21_set_transform, (
	LIRenLight21*         self,
	const LIMatTransform* transform));

LIAPICALL (int, liren_light21_get_type, (
	const LIRenLight21* self));

#endif
