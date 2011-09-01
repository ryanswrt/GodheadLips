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

#ifndef __RENDER_LIGHT_H__
#define __RENDER_LIGHT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-types.h"

LIAPICALL (LIRenLight*, liren_light_new, (
	LIRenScene*  scene,
	const float* ambient,
	const float* diffuse,
	const float* specular,
	const float* equation,
	float        cutoff,
	float        exponent,
	int          shadow));

LIAPICALL (void, liren_light_free, (
	LIRenLight* self));

LIAPICALL (void, liren_light_get_ambient, (
	LIRenLight* self,
	float*      value));

LIAPICALL (void, liren_light_set_ambient, (
	LIRenLight*  self,
	const float* value));

LIAPICALL (void, liren_light_get_diffuse, (
	LIRenLight* self,
	float*      value));

LIAPICALL (void, liren_light_set_diffuse, (
	LIRenLight*  self,
	const float* value));

LIAPICALL (int, liren_light_get_enabled, (
	const LIRenLight* self));

LIAPICALL (void, liren_light_get_equation, (
	LIRenLight* self,
	float*      value));

LIAPICALL (void, liren_light_set_equation, (
	LIRenLight*  self,
	const float* value));

LIAPICALL (void, liren_light_get_position, (
	const LIRenLight* self,
	GLfloat*          value));

LIAPICALL (float, liren_light_get_priority, (
	LIRenLight* self));

LIAPICALL (void, liren_light_set_priority, (
	LIRenLight* self,
	float       value));

LIAPICALL (LIRenScene*, liren_light_get_scene, (
	const LIRenLight* self));

LIAPICALL (int, liren_light_get_shadow, (
	const LIRenLight* self));

LIAPICALL (void, liren_light_set_shadow, (
	LIRenLight* self,
	int         value));

LIAPICALL (float, liren_light_get_shadow_far, (
	const LIRenLight* self));

LIAPICALL (void, liren_light_set_shadow_far, (
	LIRenLight* self,
	float       value));

LIAPICALL (float, liren_light_get_shadow_near, (
	const LIRenLight* self));

LIAPICALL (void, liren_light_set_shadow_near, (
	LIRenLight* self,
	float       value));

LIAPICALL (void, liren_light_get_specular, (
	LIRenLight* self,
	float*      value));

LIAPICALL (void, liren_light_set_specular, (
	LIRenLight*  self,
	const float* value));

LIAPICALL (float, liren_light_get_spot_cutoff, (
	const LIRenLight* self));

LIAPICALL (void, liren_light_set_spot_cutoff, (
	LIRenLight* self,
	float       value));

LIAPICALL (float, liren_light_get_spot_exponent, (
	const LIRenLight* self));

LIAPICALL (void, liren_light_set_spot_exponent, (
	LIRenLight* self,
	float       value));

LIAPICALL (void, liren_light_get_transform, (
	LIRenLight*     self,
	LIMatTransform* value));

LIAPICALL (void, liren_light_set_transform, (
	LIRenLight*           self,
	const LIMatTransform* value));

#endif
