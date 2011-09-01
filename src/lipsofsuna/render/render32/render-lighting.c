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
 * \addtogroup LIRenLighting32 Lighting32
 * @{
 */

#include <lipsofsuna/system.h>
#include "render-private.h"
#include "render-context.h"
#include "render-lighting.h"

/**
 * \brief Creates a new light manager.
 * \param render Renderer.
 * \return New light manager or NULL.
 */
LIRenLighting32* liren_lighting32_new (
	LIRenRender32* render)
{
	LIRenLighting32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenLighting32));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Allocate light arrays. */
	self->lights = lialg_ptrdic_new ();
	if (self->lights == NULL)
		goto error;

	return self;

error:
	liren_lighting32_free (self);
	return NULL;
}

/**
 * \brief Frees the light manager.
 * \param self Light manager.
 */
void liren_lighting32_free (
	LIRenLighting32* self)
{
	if (self->lights != NULL)
	{
		lisys_assert (self->lights->size == 0);
		lialg_ptrdic_free (self->lights);
	}
	lisys_free (self);
}

/**
 * \brief Registers a new light source.
 * \param self Light manager.
 * \param light Light source.
 * \return Nonzero on success.
 */
int liren_lighting32_insert_light (
	LIRenLighting32* self,
	LIRenLight32*    light)
{
	if (!lialg_ptrdic_insert (self->lights, light, light))
		return 0;
	light->enabled = 1;
	return 1;
}

/**
 * \brief Removes a registered light source.
 * \param self Light manager.
 * \param light Light source.
 */
void liren_lighting32_remove_light (
	LIRenLighting32* self,
	LIRenLight32*    light)
{
	lialg_ptrdic_remove (self->lights, light);
	light->enabled = 0;
}

/**
 * \brief Updates the status of all registered light sources.
 * \param self Light manager.
 */
void liren_lighting32_update (
	LIRenLighting32* self)
{
	LIAlgPtrdicIter iter;
	LIRenLight32* light;

	LIALG_PTRDIC_FOREACH (iter, self->lights)
	{
		light = iter.value;
		liren_light32_update (light);
	}
}

/**
 * \brief Uploads the lights to the context.
 * \param self Lighting.
 * \param context Context.
 * \param center Camera center.
 */
void liren_lighting32_upload (
	LIRenLighting32* self,
	LIRenContext32*  context,
	LIMatVector*     center)
{
	int i;
	int cmp;
	int count;
	float dist;
	float dists[LIREN_CONTEXT_MAX_LIGHTS + 1];
	LIAlgPtrdicIter iter;
	LIMatVector diff;
	LIRenLight32* light;
	LIRenLight32* lights[LIREN_CONTEXT_MAX_LIGHTS + 1];

	/* Clear the light list. */
	for (i = 0 ; i < LIREN_CONTEXT_MAX_LIGHTS ; i++)
	{
		lights[i] = NULL;
		dists[i] = 1000000000.0f;
	}

	/* Sort the light list. */
	/* This is an insertion sort algorithm with a small destination array.
	   Lights are sorted to the array based on their priority. Only the few
	   lights with the highest priorites are used for lighting. */
	count = 0;
	LIALG_PTRDIC_FOREACH (iter, self->lights)
	{
		light = iter.value;
		diff = limat_vector_subtract (light->transform.position, *center);
		dist = limat_vector_dot (diff, diff);
		for (i = count ; i > 0 ; i--)
		{
			/* Compare priorities. */
			cmp = liren_light32_compare (light, lights[i - 1]);
			if (cmp < 0)
				break;

			/* Compare distances. */
			if (cmp == 0 && dist > dists[i - 1])
				break;

			/* Shift the weaker light. */
			dists[i] = dists[i - 1];
			lights[i] = lights[i - 1];
		}
		dists[i] = dist;
		lights[i] = light;
		count = LIMAT_MIN (LIREN_CONTEXT_MAX_LIGHTS, count + 1);
	}

	/* Upload the highest priority lights. */
	for (i = 0 ; i < LIREN_CONTEXT_MAX_LIGHTS ; i++)
	{
		light = lights[i];
		if (light != NULL)
			liren_light32_update_cache (light, context);
		liren_context32_set_light (context, i, light);
	}
}

/** @} */
/** @} */
