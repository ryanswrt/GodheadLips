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
 * \addtogroup LIRenRender21 Render21
 * @{
 */

#include "lipsofsuna/system.h"
#include "lipsofsuna/video.h"
#include "render.h"
#include "render-private.h"
#include "../render.h"
#include "../render-private.h"

static void private_free_helpers (
	LIRenRender21* self);

static int private_init_resources (
	LIRenRender21* self);

/*****************************************************************************/

LIRenRender21* liren_render21_new (
	void*       render,
	LIPthPaths* paths)
{
	LIRenRender21* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenRender21));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->paths = paths;

	/* Allocate scene list. */
	self->scenes = lialg_ptrdic_new ();
	if (self->scenes == NULL)
	{
		liren_render21_free (self);
		return NULL;
	}

	/* Load data. */
	if (!private_init_resources (self))
	{
		liren_render21_free (self);
		return NULL;
	}

	glEnable (GL_TEXTURE_2D);

	return self;
}

void liren_render21_free (
	LIRenRender21* self)
{
	private_free_helpers (self);
	if (self->scenes != NULL)
	{
		lisys_assert (self->scenes->size == 0);
		lialg_ptrdic_free (self->scenes);
	}
	lisys_free (self);
}

void liren_render21_draw_clipped_buffer (
	LIRenRender21*     self,
	LIRenShader21*     shader,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection,
	GLuint             texture,
	const float*       diffuse,
	const int*         scissor,
	LIRenBuffer21*     buffer)
{
	/* Enable clipping. */
	glPushAttrib (GL_SCISSOR_BIT);
	glEnable (GL_SCISSOR_TEST);
	glScissor (scissor[0], scissor[1], scissor[2], scissor[3]);

	/* Set the rendering mode. */
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable (GL_CULL_FACE);
	glColor4fv (diffuse);
	glDisable (GL_DEPTH_TEST);
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (projection->m);
	glMatrixMode (GL_MODELVIEW);
	glLoadMatrixf (modelview->m);
	glBindTexture (GL_TEXTURE_2D, texture);

	/* Enable the vertex buffer. */
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, buffer->index_buffer);
	glBindBuffer (GL_ARRAY_BUFFER, buffer->vertex_buffer);
	glEnableClientState (GL_VERTEX_ARRAY);
	glVertexPointer (3, buffer->vertex_format.vtx_format,
		buffer->vertex_format.size, NULL + buffer->vertex_format.vtx_offset);
	if (buffer->vertex_format.tex_format)
	{
		glEnableClientState (GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer (2, buffer->vertex_format.tex_format,
			buffer->vertex_format.size, NULL + buffer->vertex_format.tex_offset);
	}

	/* Render the vertex buffer. */
	glDrawArrays (GL_TRIANGLES, 0, buffer->vertices.count);

	/* Disable the vertex buffer. */
	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);

	/* Disable clipping. */
	glPopAttrib ();
}

void liren_render21_draw_indexed_triangles_T2V3 (
	LIRenRender21*    self,
	LIRenShader21*    shader,
	LIMatMatrix*      matrix,
	GLuint            texture,
	const float*      diffuse,
	const float*      vertex_data,
	const LIRenIndex* index_data,
	int               index_count)
{
	int j;

	/* Set the rendering mode. */
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable (GL_CULL_FACE);
	glColor4fv (diffuse);
	glDisable (GL_DEPTH_TEST);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (matrix->m);
	glBindTexture (GL_TEXTURE_2D, texture);

	/* Draw the triangles. */
	glBegin (GL_TRIANGLES);
	for (j = 0 ; j < index_count ; j++)
	{
		glTexCoord2fv (vertex_data + 5 * index_data[j] + 0);
		glVertex2fv (vertex_data + 5 * index_data[j] + 2);
	}
	glEnd ();
}

/**
 * \brief Finds a shader by name.
 * \param self Renderer.
 * \param name Name of the shader.
 * \return Shader or NULL.
 */
LIRenShader21* liren_render21_find_shader (
	LIRenRender21* self,
	const char*    name)
{
	LIRenShader* shader;

	shader = liren_render_find_shader (self->render, name);
	if (shader == NULL)
		return NULL;

	return shader->v21;
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
LIRenImage21* liren_render21_find_image (
	LIRenRender21* self,
	const char*    name)
{
	LIRenImage* image;

	image = liren_render_find_image (self->render, name);
	if (image == NULL)
		return NULL;

	return image->v21;
}

/**
 * \brief Finds a model by ID.
 *
 * \param self Renderer.
 * \param id Model ID.
 * \return Model.
 */
LIRenModel21* liren_render21_find_model (
	LIRenRender21* self,
	int            id)
{
	LIRenModel* model;

	model = liren_render_find_model (self->render, id);
	if (model == NULL)
		return NULL;

	return model->v21;
}

LIRenImage21* liren_render21_load_image (
	LIRenRender21* self,
	const char*    name)
{
	LIRenImage21* image;

	image = liren_render21_find_image (self, name);
	if (image != NULL)
		return image;
	liren_render_load_image (self->render, name);
	image = liren_render21_find_image (self, name);

	return image;
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
void liren_render21_reload (
	LIRenRender21* self,
	int            pass)
{
	LIAlgStrdicIter iter;
	LIAlgPtrdicIter iter1;

	if (pass)
		glEnable (GL_TEXTURE_2D);

	LIALG_STRDIC_FOREACH (iter, self->render->shaders)
		liren_shader21_reload (((LIRenShader*) iter.value)->v21);
	LIALG_STRDIC_FOREACH (iter, self->render->images)
		liren_image21_reload (((LIRenImage*) iter.value)->v21, pass);
	LIALG_PTRDIC_FOREACH (iter1, self->render->models_ptr)
		liren_model21_reload (((LIRenModel*) iter1.value)->v21, pass);
}

/**
 * \brief Forces the renderer to reload a texture image.
 *
 * Reloads the requested image and updates any materials that reference it
 * to point to the new texture. Any other references to the image become
 * invalid and need to be manually replaced.
 *
 * \param self Renderer.
 * \param image Image.
 * \return Nonzero on success.
 */
int liren_render21_reload_image (
	LIRenRender21* self,
	LIRenImage21*  image)
{
	LIAlgPtrdicIter iter;
	LIRenModel* model;

	/* Reload the image. */
	if (!liren_image21_load (image))
		return 0;

	/* Replace in all models. */
	LIALG_PTRDIC_FOREACH (iter, self->render->models_ptr)
	{
		model = iter.value;
		liren_model21_replace_image (model->v21, image);
	}

	return 1;
}

/**
 * \brief Updates the renderer state.
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void liren_render21_update (
	LIRenRender21* self,
	float          secs)
{
}

int liren_render21_get_anisotropy (
	const LIRenRender21* self)
{
	return 0.0f;
}

void liren_render21_set_anisotropy (
	LIRenRender21* self,
	int            value)
{
}

/*****************************************************************************/

static void private_free_helpers (
	LIRenRender21* self)
{
	/* TODO */
}

static int private_init_resources (
	LIRenRender21* self)
{
	/* TODO */

	return 1;
}

/** @} */
/** @} */
