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

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenLight21 Light21
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-light.h"
#include "render-private.h"
#include "../render-scene.h"
#include "../render-private.h"

/**
 * \brief Creates a new light source.
 * \param scene Scene.
 * \param ambient Ambient color, array of 4 floats.
 * \param diffuse Diffuse color, array of 4 floats.
 * \param specular Specular color, array of 4 floats.
 * \param equation Attenuation equation, array of 3 floats.
 * \param cutoff Spot cutoff in radians.
 * \param exponent Spot expoent.
 * \param shadows Nonzero if the lamp casts shadows.
 * \return New light source or NULL.
 */
LIRenLight21* liren_light21_new (
	LIRenScene21* scene,
	const float*  ambient,
	const float*  diffuse,
	const float*  specular,
	const float*  equation,
	float         cutoff,
	float         exponent,
	int           shadows)
{
	LIRenLight21* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenLight21));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	self->ambient[0] = ambient[0];
	self->ambient[1] = ambient[1];
	self->ambient[2] = ambient[2];
	self->ambient[3] = 1.0f;
	self->diffuse[0] = diffuse[0];
	self->diffuse[1] = diffuse[1];
	self->diffuse[2] = diffuse[2];
	self->diffuse[3] = 1.0f;
	self->specular[0] = specular[0];
	self->specular[1] = specular[1];
	self->specular[2] = specular[2];
	self->specular[3] = 1.0f;
	self->equation[0] = equation[0];
	self->equation[1] = equation[1];
	self->equation[2] = equation[2];
	self->cutoff = cutoff;
	self->exponent = exponent;
	self->transform = limat_transform_identity ();

	return self;
}

/**
 * \brief Frees a light source.
 * \param self Light source.
 */
void liren_light21_free (
	LIRenLight21* self)
{
	lisys_free (self);
}

/**
 * \brief Compares the priorities of two lights.
 * \param self Light source.
 * \param light Light source.
 * \return Integer indicating which light contributes more.
 */
int liren_light21_compare (
	const LIRenLight21* self,
	const LIRenLight21* light)
{
	/* Sorting is done primarily by priority. */
	/* This allows the user to decide what lights are the most important
	   ones. If there's an ambient light in the scene, for example, the
	   user may want to give that higher priority than other lights. */
	if (self->priority < light->priority)
		return -1;
	if (self->priority > light->priority)
		return 1;

	/* Secondary sorting by pointer. */
	/* This ensures that the choice is consistent across frames
	   when there are multiple lights with the same priority. */
	if (self < light)
		return -1;
	return 1;
}

void liren_light21_get_ambient (
	LIRenLight21* self,
	float*        value)
{
	memcpy (value, self->ambient, 4 * sizeof (float));
}

void liren_light21_set_ambient (
	LIRenLight21* self,
	const float*  value)
{
	memcpy (self->ambient, value, 4 * sizeof (float));
}

/**
 * \brief Gets the area of effect of the light source.
 * \param self Light source.
 * \param result Return location for a bounding box.
 * \return Nonzero if has bounds.
 */
int liren_light21_get_bounds (
	const LIRenLight21* self,
	LIMatAabb*          result)
{
	*result = self->bounds;

	return 1;
}

void liren_light21_get_diffuse (
	LIRenLight21* self,
	float*        value)
{
	memcpy (value, self->diffuse, 4 * sizeof (float));
}

void liren_light21_set_diffuse (
	LIRenLight21* self,
	const float*  value)
{
	memcpy (self->diffuse, value, 4 * sizeof (float));
}

/**
 * \brief Checks if the light is registered.
 * \param self Light source.
 * \return Nonzero if registered.
 */
int liren_light21_get_enabled (
	const LIRenLight21* self)
{
	return self->enabled;
}

void liren_light21_get_equation (
	LIRenLight21* self,
	float*        value)
{
	memcpy (value, self->equation, 3 * sizeof (float));
}

void liren_light21_set_equation (
	LIRenLight21* self,
	const float*  value)
{
	memcpy (self->equation, value, 3 * sizeof (float));
}

/**
 * \brief Gets the OpenGL position vector for the light.
 * \param self Light source.
 * \param value Return location for 4 floats.
 */
void liren_light21_get_position (
	const LIRenLight21* self,
	GLfloat*            value)
{
	value[0] = self->transform.position.x;
	value[1] = self->transform.position.y;
	value[2] = self->transform.position.z;
	value[3] = 1.0f;
}

/**
 * \brief Gets the priority of the light.
 * \param self Light source.
 * \return Priority value, higher means more important.
 */
float liren_light21_get_priority (
	LIRenLight21* self)
{
	return self->priority;
}

/**
 * \brief Sets the priority of the light.
 * \param self Light source.
 * \param value Priority value, higher means more important.
 */
void liren_light21_set_priority (
	LIRenLight21* self,
	float         value)
{
	self->priority = value;
}

LIRenScene21* liren_light21_get_scene (
	const LIRenLight21* self)
{
	return self->scene;
}

/**
 * \brief Gets the shadow casting mode of the light.
 * \param self Light source.
 * \return Nonzero if shadow casting is allowed, zero if disabled.
 */
int liren_light21_get_shadow (
	const LIRenLight21* self)
{
	return 0;
}

float liren_light21_get_shadow_far (
	const LIRenLight21* self)
{
	return 1.0;
}

void liren_light21_set_shadow_far (
	LIRenLight21* self,
	float         value)
{
}

float liren_light21_get_shadow_near (
	const LIRenLight21* self)
{
	return 1.0;
}

void liren_light21_set_shadow_near (
	LIRenLight21* self,
	float         value)
{
}

/**
 * \brief Sets the shadow casting mode of the light.
 * \param self Light source.
 * \param value Nonzero to allow shadow casting, zero to disable.
 */
void liren_light21_set_shadow (
	LIRenLight21* self,
	int           value)
{
}

void liren_light21_get_specular (
	LIRenLight21* self,
	float*        value)
{
	memcpy (value, self->specular, 4 * sizeof (float));
}

void liren_light21_set_specular (
	LIRenLight21* self,
	const float*  value)
{
	memcpy (self->specular, value, 4 * sizeof (float));
}

float liren_light21_get_spot_cutoff (
	const LIRenLight21* self)
{
	return self->cutoff;
}

void liren_light21_set_spot_cutoff (
	LIRenLight21* self,
	float         value)
{
	self->cutoff = value;
}

float liren_light21_get_spot_exponent (
	const LIRenLight21* self)
{
	return self->exponent;
}

void liren_light21_set_spot_exponent (
	LIRenLight21* self,
	float         value)
{
	self->exponent = value;
}

/**
 * \brief Gets the transformation of the light.
 * \param self Light source.
 * \param value Return value for the transformation.
 */
void liren_light21_get_transform (
	LIRenLight21*   self,
	LIMatTransform* value)
{
	*value = self->transform;
}

/**
 * \brief Sets the transformation of the light.
 * \param self Light source.
 * \param transform Transformation.
 */
void liren_light21_set_transform (
	LIRenLight21*         self,
	const LIMatTransform* transform)
{
	self->transform = *transform;
}

/** @} */
/** @} */
