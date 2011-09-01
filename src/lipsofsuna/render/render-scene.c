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
 * \addtogroup LIRenRender Render
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.h"
#include "render-private.h"
#include "render-scene.h"
#include "render21/render-private.h"
#include "render21/render-scene.h"
#include "render32/render-private.h"
#include "render32/render-scene.h"

LIRenScene* liren_scene_new (
	LIRenRender* render)
{
	LIRenScene* self;

	self = lisys_calloc (1, sizeof (LIRenScene));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Allocate the object list. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		liren_scene_free (self);
		return NULL;
	}

	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_scene32_new (self, render->v32);
		if (self->v32 == NULL)
		{
			liren_scene_free (self);
			return NULL;
		}
	}
	else
	{
		self->v21 = liren_scene21_new (self, render->v21);
		if (self->v21 == NULL)
		{
			liren_scene_free (self);
			return NULL;
		}
	}

	return self;
}

void liren_scene_free (
	LIRenScene* self)
{
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);
	if (self->v32 != NULL)
		liren_scene32_free (self->v32);
	if (self->v21 != NULL)
		liren_scene21_free (self->v21);
	lisys_free (self);
}

LIRenObject* liren_scene_find_object (
	LIRenScene* self,
	int         id)
{
	return lialg_u32dic_find (self->objects, id);
}

void liren_scene_insert_light (
	LIRenScene* self,
	LIRenLight* light)
{
	if (self->v32 != NULL)
		liren_scene32_insert_light (self->v32, light->v32);
	else
		liren_scene21_insert_light (self->v21, light->v21);
}

void liren_scene_remove_light (
	LIRenScene* self,
	LIRenLight* light)
{
	if (self->v32 != NULL)
		liren_scene32_remove_light (self->v32, light->v32);
	else
		liren_scene21_remove_light (self->v21, light->v21);
}

void liren_scene_remove_model (
	LIRenScene* self,
	LIRenModel* model)
{
	if (self->v32 != NULL)
		return liren_scene32_remove_model (self->v32, model->v32);
	else
		return liren_scene21_remove_model (self->v21, model->v21);
}

/**
 * \brief Renders the scene.
 * \param self Scene.
 * \param framebuffer Render target framebuffer.
 * \param viewport Viewport array.
 * \param modelview Modelview matrix of the camera.
 * \param projection Projeciton matrix of the camera.
 * \param frustum Frustum of the camera.
 * \param render_passes Array of render passes.
 * \param render_passes_num Number of render passes.
 * \param postproc_passes Array of post-processing passes.
 * \param postproc_passes_num Number of post-processing passes.
 */
void liren_scene_render (
	LIRenScene*        self,
	LIRenFramebuffer*  framebuffer,
	const GLint*       viewport,
	LIMatMatrix*       modelview,
	LIMatMatrix*       projection,
	LIMatFrustum*      frustum,
	LIRenPassRender*   render_passes,
	int                render_passes_num,
	LIRenPassPostproc* postproc_passes,
	int                postproc_passes_num)
{
	if (self->v32 != NULL)
	{
		return liren_scene32_render (self->v32, framebuffer->v32, viewport,
			modelview, projection, frustum, render_passes, render_passes_num,
			postproc_passes, postproc_passes_num);
	}
	else
	{
		return liren_scene21_render (self->v21, framebuffer->v21, viewport,
			modelview, projection, frustum, render_passes, render_passes_num,
			postproc_passes, postproc_passes_num);
	}
}

/**
 * \brief Updates the scene.
 * \param self Scene.
 * \param secs Number of seconds since the last update.
 */
void liren_scene_update (
	LIRenScene* self,
	float       secs)
{
	if (self->v32 != NULL)
		return liren_scene32_update (self->v32, secs);
	else
		return liren_scene21_update (self->v21, secs);
}

/** @} */
/** @} */
