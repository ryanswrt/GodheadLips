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
#include "lipsofsuna/video.h"
#include "render.h"
#include "render-private.h"
#include "render32/render-private.h"

LIRenRender* liren_render_new (
	LIPthPaths* paths)
{
	LIRenRender* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenRender));
	if (self == NULL)
		return NULL;
	lialg_random_init (&self->random, lisys_time (NULL));

	/* Initialize the image dictionary. */
	self->images = lialg_strdic_new ();
	if (self->images == NULL)
		return 0;

	/* Initialize the shader dictionary. */
	self->shaders = lialg_strdic_new ();
	if (self->shaders == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Initialize model dictionaries. */
	self->models = lialg_u32dic_new ();
	if (self->models == NULL)
	{
		liren_render_free (self);
		return NULL;
	}
	self->models_ptr = lialg_ptrdic_new ();
	if (self->models_ptr == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Initialize the backend. */
	if (GLEW_VERSION_3_2 && getenv ("LOS_OPENGL21") == NULL)
	{
		self->v32 = liren_render32_new (self, paths);
		if (self->v32 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}
	else
	{
		self->v21 = liren_render21_new (self, paths);
		if (self->v21 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	return self;
}

void liren_render_free (
	LIRenRender* self)
{
	LIAlgStrdicIter iter1;
	LIAlgPtrdicIter iter2;

	/* Free models. */
	if (self->models != NULL)
		lialg_u32dic_free (self->models);
	if (self->models_ptr != NULL)
	{
		LIALG_PTRDIC_FOREACH (iter2, self->models_ptr)
			liren_model_free (iter2.value);
		lialg_ptrdic_free (self->models_ptr);
	}

	/* Free images. */
	if (self->images != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->images)
			liren_image_free (iter1.value);
		lialg_strdic_free (self->images);
	}

	/* Free shaders. */
	if (self->shaders != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->shaders)
			liren_shader_free (iter1.value);
		lialg_strdic_free (self->shaders);
	}

	/* Free the backend. */
	if (self->v21 != NULL)
		liren_render21_free (self->v21);
	if (self->v32 != NULL)
		liren_render32_free (self->v32);
	lisys_free (self);
}

void liren_render_draw_clipped_buffer (
	LIRenRender*       self,
	LIRenShader*       shader,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection,
	GLuint             texture,
	const float*       diffuse,
	const int*         scissor,
	LIRenBuffer*       buffer)
{
	if (self->v32 != NULL)
	{
		liren_render32_draw_clipped_buffer (self->v32, shader->v32, modelview,
			projection, texture, diffuse, scissor, buffer->v32);
	}
	else
	{
		liren_render21_draw_clipped_buffer (self->v21, shader->v21, modelview,
			projection, texture, diffuse, scissor, buffer->v21);
	}
}

void liren_render_draw_indexed_triangles_T2V3 (
	LIRenRender*      self,
	LIRenShader*      shader,
	LIMatMatrix*      matrix,
	GLuint            texture,
	const float*      diffuse,
	const float*      vertex_data,
	const LIRenIndex* index_data,
	int               index_count)
{
	if (self->v32 != NULL)
	{
		liren_render32_draw_indexed_triangles_T2V3 (self->v32, shader->v32, matrix,
			texture, diffuse, vertex_data, index_data, index_count);
	}
	else
	{
		liren_render21_draw_indexed_triangles_T2V3 (self->v21, shader->v21, matrix,
			texture, diffuse, vertex_data, index_data, index_count);
	}
}

/**
 * \brief Finds a shader by name.
 * \param self Renderer.
 * \param name Name of the shader.
 * \return Shader or NULL.
 */
LIRenShader* liren_render_find_shader (
	LIRenRender* self,
	const char*  name)
{
	return lialg_strdic_find (self->shaders, name);
}

/**
 * \brief Finds a texture by name.
 *
 * Searches for a texture from the texture cache and returns the match, if any.
 * If no match is found, NULL is returned.
 *
 * \param self Renderer.
 * \param name Name of the texture.
 * \return Texture or NULL.
 */
LIRenImage* liren_render_find_image (
	LIRenRender* self,
	const char*  name)
{
	return lialg_strdic_find (self->images, name);
}

/**
 * \brief Finds a model by ID.
 * \param self Renderer.
 * \param id Model ID.
 * \return Model.
 */
LIRenModel* liren_render_find_model (
	LIRenRender* self,
	int          id)
{
	return lialg_u32dic_find (self->models, id);
}

/**
 * \brief Forces the renderer to load or reload a texture image.
 *
 * Reloads the requested texture and updates any materials that reference it
 * to point to the new texture. Any other references to the texture become
 * invalid and need to be manually replaced.
 *
 * \param self Renderer.
 * \param name Texture name.
 * \return Nonzero on success.
 */
int liren_render_load_image (
	LIRenRender* self,
	const char*  name)
{
	LIRenImage* image;

	if (self->v32 != NULL)
	{
		image = liren_render_find_image (self, name);
		if (image != NULL)
			return liren_render32_reload_image (self->v32, image->v32);
		image = liren_image_new (self, name);
		if (image == NULL)
			return 0;
	}
	else
	{
		image = liren_render_find_image (self, name);
		if (image != NULL)
			return liren_render21_reload_image (self->v21, image->v21);
		image = liren_image_new (self, name);
		if (image == NULL)
			return 0;
	}

	return 1;
}

/**
 * \brief Reloads all images, shaders and other graphics state.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads all data that was lost when the context was erased.
 *
 * \param self Renderer.
 * \param pass Reload pass.
 */
void liren_render_reload (
	LIRenRender* self,
	int          pass)
{
	if (self->v32 != NULL)
		liren_render32_reload (self->v32, pass);
	else
		liren_render21_reload (self->v21, pass);
}

/**
 * \brief Updates the renderer state.
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void liren_render_update (
	LIRenRender* self,
	float        secs)
{
	if (self->v32 != NULL)
		liren_render32_update (self->v32, secs);
	else
		liren_render21_update (self->v21, secs);
}

int liren_render_get_anisotropy (
	const LIRenRender* self)
{
	if (self->v32 != NULL)
		return liren_render32_get_anisotropy (self->v32);
	else
		return liren_render21_get_anisotropy (self->v21);
}

void liren_render_set_anisotropy (
	LIRenRender* self,
	int          value)
{
	if (self->v32 != NULL)
		liren_render32_set_anisotropy (self->v32, value);
	else
		liren_render21_set_anisotropy (self->v21, value);
}

/** @} */
/** @} */
