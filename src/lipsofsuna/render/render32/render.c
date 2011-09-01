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
 * \addtogroup LIRenRender32 Render32
 * @{
 */

#include "lipsofsuna/system.h"
#include "lipsofsuna/video.h"
#include "render.h"
#include "render-context.h"
#include "render-private.h"
#include "render-uniforms.h"
#include "../render.h"
#include "../render-private.h"

static void private_free_helpers (
	LIRenRender32* self);

static int private_init_helpers (
	LIRenRender32* self);

static int private_init_resources (
	LIRenRender32* self);

/*****************************************************************************/

LIRenRender32* liren_render32_new (
	void*       render,
	LIPthPaths* paths)
{
	LIRenRender32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenRender32));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->paths = paths;
	self->lod_near = 10.0f;
	self->lod_far = 50.0f;

	/* Allocate scene list. */
	self->scenes = lialg_ptrdic_new ();
	if (self->scenes == NULL)
	{
		liren_render32_free (self);
		return NULL;
	}

	/* Allocate framebuffer list. */
	self->framebuffers = lialg_ptrdic_new ();
	if (self->framebuffers == NULL)
	{
		liren_render32_free (self);
		return NULL;
	}

	/* Load data. */
	if (!private_init_resources (self))
	{
		liren_render32_free (self);
		return NULL;
	}

	return self;
}

void liren_render32_free (
	LIRenRender32* self)
{
	private_free_helpers (self);
	if (self->scenes != NULL)
	{
		lisys_assert (self->scenes->size == 0);
		lialg_ptrdic_free (self->scenes);
	}
	if (self->framebuffers != NULL)
	{
		lisys_assert (self->framebuffers->size == 0);
		lialg_ptrdic_free (self->framebuffers);
	}
	if (self->context != NULL)
	{
		liren_uniforms32_clear (&self->context->uniforms);
		lisys_free (self->context);
	}
	lisys_free (self);
}

void liren_render32_draw_clipped_buffer (
	LIRenRender32*     self,
	LIRenShader32*     shader,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection,
	GLuint             texture,
	const float*       diffuse,
	const int*         scissor,
	LIRenBuffer32*     buffer)
{
	int scissor1[5];
	LIMatMatrix identity = limat_matrix_identity ();

	/* Enable clipping. */
	scissor1[4] = liren_context32_get_scissor (self->context,
		scissor1 + 0, scissor1 + 1, scissor1 + 2, scissor1 + 3);
	liren_context32_set_scissor (self->context, 1,
		scissor[0], scissor[1], scissor[2], scissor[3]);

	/* Render the vertex buffer. */
	liren_context32_set_buffer (self->context, buffer);
	liren_context32_set_cull (self->context, 0, GL_CCW);
	liren_context32_set_modelmatrix (self->context, &identity);
	liren_context32_set_viewmatrix (self->context, modelview);
	liren_context32_set_projection (self->context, projection);
	liren_context32_set_shader (self->context, 0, shader);
	liren_context32_set_diffuse (self->context, diffuse);
	liren_context32_set_textures_raw (self->context, &texture, 1);
	liren_context32_bind (self->context);
	liren_context32_render_array (self->context, GL_TRIANGLES, 0, buffer->vertices.count);

	/* Disable clipping. */
	liren_context32_set_scissor (self->context, scissor1[4],
		scissor1[0], scissor1[1], scissor1[2], scissor1[3]);
}

void liren_render32_draw_indexed_triangles_T2V3 (
	LIRenRender32*    self,
	LIRenShader32*    shader,
	LIMatMatrix*      matrix,
	GLuint            texture,
	const float*      diffuse,
	const float*      vertex_data,
	const LIRenIndex* index_data,
	int               index_count)
{
	int j;
	LIMatMatrix identity = limat_matrix_identity ();

	liren_context32_set_shader (self->context, 0, shader);
	liren_context32_set_blend (self->context, 1, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	liren_context32_set_buffer (self->context, NULL);
	liren_context32_set_cull (self->context, 0, GL_CCW);
	liren_context32_set_diffuse (self->context, diffuse);
	liren_context32_set_depth (self->context, 0, 0, GL_LEQUAL);
	liren_context32_set_modelmatrix (self->context, &identity);
	liren_context32_set_viewmatrix (self->context, &identity);
	liren_context32_set_projection (self->context, matrix);
	liren_context32_set_textures_raw (self->context, &texture, 1);
	liren_context32_bind (self->context);
	glBegin (GL_TRIANGLES);
	for (j = 0 ; j < index_count ; j++)
	{
		glVertexAttrib2fv (LIREN_ATTRIBUTE_TEXCOORD, vertex_data + 5 * index_data[j] + 0);
		glVertexAttrib2fv (LIREN_ATTRIBUTE_COORD, vertex_data + 5 * index_data[j] + 2);
	}
	glEnd ();
}

/**
 * \brief Finds a shader by name.
 * \param self Renderer.
 * \param name Name of the shader.
 * \return Shader or NULL.
 */
LIRenShader32* liren_render32_find_shader (
	LIRenRender32* self,
	const char*    name)
{
	LIRenShader* shader;

	shader = liren_render_find_shader (self->render, name);
	if (shader == NULL)
		return NULL;

	return shader->v32;
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
LIRenImage32* liren_render32_find_image (
	LIRenRender32* self,
	const char*  name)
{
	LIRenImage* image;

	image = liren_render_find_image (self->render, name);
	if (image == NULL)
		return NULL;

	return image->v32;
}

/**
 * \brief Finds a model by ID.
 *
 * \param self Renderer.
 * \param id Model ID.
 * \return Model.
 */
LIRenModel32* liren_render32_find_model (
	LIRenRender32* self,
	int            id)
{
	LIRenModel* model;

	model = liren_render_find_model (self->render, id);
	if (model == NULL)
		return NULL;

	return model->v32;
}

LIRenImage32* liren_render32_load_image (
	LIRenRender32* self,
	const char*    name)
{
	LIRenImage32* image;

	image = liren_render32_find_image (self, name);
	if (image != NULL)
		return image;
	liren_render_load_image (self->render, name);
	image = liren_render32_find_image (self, name);

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
void liren_render32_reload (
	LIRenRender32* self,
	int            pass)
{
	LIAlgStrdicIter iter;
	LIAlgPtrdicIter iter1;

	if (!pass)
	{
		private_free_helpers (self);
	}
	else
	{
		private_init_helpers (self);
		liren_uniforms32_reload (&self->context->uniforms, pass);
	}

	LIALG_STRDIC_FOREACH (iter, self->render->shaders)
		liren_shader32_reload (((LIRenShader*) iter.value)->v32, pass);
	LIALG_STRDIC_FOREACH (iter, self->render->images)
		liren_image32_reload (((LIRenImage*) iter.value)->v32, pass);
	LIALG_PTRDIC_FOREACH (iter1, self->render->models_ptr)
		liren_model32_reload (((LIRenModel*) iter1.value)->v32, pass);
	LIALG_PTRDIC_FOREACH (iter1, self->framebuffers)
		liren_framebuffer32_reload (iter1.value, pass);
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
int liren_render32_reload_image (
	LIRenRender32* self,
	LIRenImage32*  image)
{
	LIAlgPtrdicIter iter;
	LIRenModel* model;

	/* Reload the image. */
	if (!liren_image32_load (image))
		return 0;

	/* Replace in all models. */
	LIALG_PTRDIC_FOREACH (iter, self->render->models_ptr)
	{
		model = iter.value;
		liren_model32_replace_image (model->v32, image);
	}

	return 1;
}

/**
 * \brief Updates the renderer state.
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void liren_render32_update (
	LIRenRender32* self,
	float          secs)
{
	/* Update time. */
	self->helpers.time += secs;
}

int liren_render32_get_anisotropy (
	const LIRenRender32* self)
{
	return self->anisotrophy;
}

void liren_render32_set_anisotropy (
	LIRenRender32* self,
	int            value)
{
	int binding;
	LIAlgStrdicIter iter;
	LIRenImage* image;

	/* Check for changes. */
	/* The OpenGL implementation takes care of clamping the filter setting to
	   to the maximum supported by the implementation. AMD driver seems to
	   return zero for GL_MAX_TEXTURE_MAX_ANISTROPY_EXT so it's not even
	   possible to clamp to any meaningful value here. */
	if (!GLEW_EXT_texture_filter_anisotropic)
		return;
	value = LIMAT_MIN (value, 128);
	if (value == self->anisotrophy)
		return;
	self->anisotrophy = value;

	/* Update the anisotrophy setting of each image. */
	glGetIntegerv (GL_TEXTURE_BINDING_2D, &binding);
	LIALG_STRDIC_FOREACH (iter, self->render->images)
	{
		image = iter.value;
		if (image->v32->texture != NULL)
		{
			glBindTexture (GL_TEXTURE_2D, image->v32->texture->texture);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
		}
	}
	glBindTexture (GL_TEXTURE_2D, binding);
}

LIRenContext32* liren_render32_get_context (
	LIRenRender32* self)
{
	return self->context;
}

/*****************************************************************************/

static void private_free_helpers (
	LIRenRender32* self)
{
	glDeleteTextures (1, &self->helpers.noise);
	glDeleteTextures (1, &self->helpers.depth_texture_max);
	glDeleteTextures (1, &self->helpers.empty_texture);
	self->helpers.noise = 0;
	self->helpers.depth_texture_max = 0;
	self->helpers.empty_texture = 0;

	if (self->helpers.unit_quad != NULL)
		liren_buffer32_free (self->helpers.unit_quad);
	if (self->immediate.buffer != NULL)
		liren_buffer32_free (self->immediate.buffer);
}

static int private_init_helpers (
	LIRenRender32* self)
{
	int x;
	int y;
	unsigned char value;
	unsigned char* pixel;
	unsigned char* pixels;
	const float depth_texture[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const uint8_t empty_texture[16] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
	LIRenIndex quad_index_data[] = { 0, 1, 2, 1, 2, 3 };
	LIRenFormat immediate_vertex_format = { 32, GL_FLOAT, 24, GL_FLOAT, 12, GL_FLOAT, 0 };
	LIRenFormat quad_vertex_format = { 20, GL_FLOAT, 12, 0, 0, GL_FLOAT, 0 };
	const float quad_vertex_data[] =
	{
		-1, -1, 0, 0, 0,
		1, -1, 0, 1, 0,
		-1, 1, 0, 0, 1,
		1, 1, 0, 1, 1
	};
	static const int perlin_grad[16][3] =
	{
		{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
		{1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
		{1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0},
		{1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}
	};
	static const int perlin_perm[256] =
	{
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
		142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,
		219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,
		68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,
		133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161,1,216,
		80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,
		100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,
		255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,
		119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,
		19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,
		193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,
		214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,
		236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	/* Initialize the empty texture. */
	glGenTextures (1, &self->helpers.empty_texture);
	glBindTexture (GL_TEXTURE_2D, self->helpers.empty_texture);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, empty_texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap (GL_TEXTURE_2D);

	/* Initialize the empty depth texture. */
	glGenTextures (1, &self->helpers.depth_texture_max);
	glBindTexture (GL_TEXTURE_2D, self->helpers.depth_texture_max);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2, 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depth_texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Initialize the noise texture. */
	pixels = lisys_malloc (4 * 256 * 256);
	pixel = pixels;
	if (pixels == NULL)
		return 0;
	for (y = 0 ; y < 256 ; y++)
	{
		for (x = 0 ; x < 256 ; x++)
		{
			value = perlin_perm[(x + perlin_perm[y]) & 0xFF];
			*(pixel++) = perlin_grad[value & 0x0F][0] * 64 + 64;
			*(pixel++) = perlin_grad[value & 0x0F][1] * 64 + 64;
			*(pixel++) = perlin_grad[value & 0x0F][2] * 64 + 64;
			*(pixel++) = value;
		}
	}
	glGenTextures (1, &self->helpers.noise);
	glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_NOISE_TEXTURE);
	glBindTexture (GL_TEXTURE_2D, self->helpers.noise);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap (GL_TEXTURE_2D);
	glActiveTexture (GL_TEXTURE0);
	lisys_free (pixels);

	/* Initialize unit quad buffer. */
	self->helpers.unit_quad = liren_buffer32_new (quad_index_data, 6,
		&quad_vertex_format, &quad_vertex_data, 4, LIREN_BUFFER_TYPE_STATIC);
	if (self->helpers.unit_quad == NULL)
		return 0;

	/* Initialize immediate mode style vertex buffer. */
	self->immediate.buffer = liren_buffer32_new (NULL, 0,
		&immediate_vertex_format, NULL, 300000, LIREN_BUFFER_TYPE_STREAM);
	if (self->immediate.buffer == NULL)
		return 0;

	return 1;
}

static int private_init_resources (
	LIRenRender32* self)
{
	/* Initialize context manager. */
	self->context = lisys_calloc (1, sizeof (LIRenContext32));
	if (self->context == NULL)
		return 0;
	self->context->render = self;
	liren_uniforms32_init (&self->context->uniforms);
	liren_context32_init (self->context);

	/* Initialize helpers. */
	if (!private_init_helpers (self))
		return 0;

	return 1;
}

/** @} */
/** @} */
