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
 * \addtogroup LIRenContext32 Context32
 * @{
 */

#include "render-context.h"
#include "render-private.h"

void liren_context32_init (
	LIRenContext32* self)
{
	LIRenRender32* render;
	LIRenUniforms32 uniforms;

	render = self->render;
	uniforms = self->uniforms;
	memset (self, 0, sizeof (LIRenContext32));
	self->render = render;
	self->scene = NULL;
	self->array = 0;
	self->incomplete = 1;
	self->blend.enable = 0;
	self->blend.blend_src = GL_SRC_ALPHA;
	self->blend.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
	self->changed.blend = 1;
	self->changed.buffer = 1;
	self->changed.cull = 1;
	self->changed.depth = 1;
	self->changed.shader = 1;
	self->cull.enable = 1;
	self->cull.front_face = GL_CCW;
	self->depth.enable_test = 1;
	self->depth.enable_write = 1;
	self->depth.depth_func = GL_LEQUAL;
	self->matrix.model = limat_matrix_identity ();
	self->matrix.modelview = limat_matrix_identity ();
	self->matrix.modelviewinverse = limat_matrix_identity ();
	self->matrix.projection = limat_matrix_identity ();
	self->matrix.projectioninverse = limat_matrix_identity ();
	self->matrix.view = limat_matrix_identity ();
	self->textures.count = 0;
	self->uniforms = uniforms;
}

void liren_context32_bind (
	LIRenContext32* self)
{
	/* Bind shader. */
	if (self->changed.shader)
	{
		if (self->shader != NULL && self->shader->passes[self->shader_pass].program)
		{
			glUseProgram (self->shader->passes[self->shader_pass].program);
			self->incomplete = 0;
		}
		else
		{
			glUseProgram (0);
			self->incomplete = 1;
		}
	}

	/* Bind vertex array. */
	if (self->changed.buffer)
		glBindVertexArray (self->array);

	/* Update blend, cull, and depth modes. */
	if (self->changed.blend)
	{
		if (self->blend.enable)
		{
			glEnable (GL_BLEND);
			glBlendFunc (self->blend.blend_src, self->blend.blend_dst);
		}
		else
			glDisable (GL_BLEND);
	}
	if (self->changed.cull)
	{
		if (self->cull.enable)
		{
			glEnable (GL_CULL_FACE);
			glFrontFace (self->cull.front_face);
		}
		else
			glDisable (GL_CULL_FACE);
	}
	if (self->changed.depth)
	{
		if (self->depth.enable_test)
		{
			glEnable (GL_DEPTH_TEST);
			glDepthFunc (self->depth.depth_func);
		}
		else
			glDisable (GL_DEPTH_TEST);
		if (self->depth.enable_write)
			glDepthMask (GL_TRUE);
		else
			glDepthMask (GL_FALSE);
	}

	/* Update uniforms. */
	liren_uniforms32_commit (&self->uniforms);

	/* All state changes were applied. */
	self->changed.blend = 0;
	self->changed.buffer = 0;
	self->changed.cull = 0;
	self->changed.depth = 0;
	self->changed.shader = 0;
}

/**
 * \brief Renders an array of primitives.
 * \param self Rendering context.
 * \param type Primitive type.
 * \param start Starting vertex.
 * \param count Number of vertices to draw.
 */
void liren_context32_render_array (
	LIRenContext32* self,
	GLenum        type,
	int           start,
	int           count)
{
	if (self->incomplete)
		return;

	lisys_assert (start >= 0);

	glDrawArrays (type, start, count);
}

/**
 * \brief Renders primitives.
 * \param self Rendering context.
 * \param type Primitive type.
 * \param vertices Array of vertices.
 * \param count Number of vertices.
 */
int liren_context32_render_immediate (
	LIRenContext32*      self,
	GLenum             type,
	const LIRenVertex* vertices,
	int                count)
{
	int* offset;
	LIRenBuffer32* buffer;

	offset = &self->render->immediate.offset;
	buffer = self->render->immediate.buffer;
	lisys_assert (count > 0);
	lisys_assert (count <= buffer->vertices.count);

	/* Make sure the right buffer is bound. */
	if (self->array != buffer->vertex_array)
	{
		liren_context32_set_buffer (self, buffer);
		liren_context32_bind (self);
	}

	/* Upload the data to the vertex buffer. */
	/* The array buffer is bound here because upload_vertices unbound it. */
	if (*offset > buffer->vertices.count - count)
		*offset = 0;
	liren_buffer32_upload_vertices (buffer, *offset, count, vertices);
	glBindBuffer (GL_ARRAY_BUFFER, buffer->vertex_buffer);

	/* Render the primitive array. */
	liren_context32_render_array (self, type, *offset, count);
	*offset += count;

	return 1;
}

/**
 * \brief Renders indexed triangles.
 * \param self Rendering context.
 * \param start Starting index.
 * \param count Number of indices to draw.
 */
void liren_context32_render_indexed (
	LIRenContext32* self,
	int           start,
	int           count)
{
	if (self->incomplete)
		return;

	lisys_assert (start >= 0);

	if (sizeof (LIRenIndex) == 2)
		glDrawElements (GL_TRIANGLES, count, GL_UNSIGNED_SHORT, NULL + start * sizeof (LIRenIndex));
	else if (sizeof (LIRenIndex) == 4)
		glDrawElements (GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL + start * sizeof (LIRenIndex));
	else
		lisys_assert (0);

#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.materials++;
	self->render->profiling.faces += self->buffer->indices.count / 3;
	self->render->profiling.vertices += self->buffer->indices.count;
#endif
}

void liren_context32_set_alpha_to_coverage (
	LIRenContext32* self,
	int           value)
{
	if (self->alpha_to_coverage != value)
	{
		self->alpha_to_coverage = value;
		if (value)
			glEnable (GL_SAMPLE_ALPHA_TO_COVERAGE);
		else
			glDisable (GL_SAMPLE_ALPHA_TO_COVERAGE);
	}
}

void liren_context32_set_blend (
	LIRenContext32* self,
	int           enable,
	GLenum        blend_src,
	GLenum        blend_dst)
{
	if (self->blend.enable != enable ||
	    self->blend.blend_src != blend_src ||
	    self->blend.blend_dst != blend_dst)
	{
		self->blend.enable = enable;
		self->blend.blend_src = blend_src;
		self->blend.blend_dst = blend_dst;
		self->changed.blend = 1;
	}
}

void liren_context32_set_buffer (
	LIRenContext32* self,
	LIRenBuffer32*  buffer)
{
	GLuint array;

	array = (buffer != NULL)? buffer->vertex_array : 0;
	if (self->array != array)
	{
		self->array = array;
		self->changed.buffer = 1;
	}
}

void liren_context32_set_camera_position (
	LIRenContext32*    self,
	const LIMatVector* value)
{
	float tmp[3];

	self->camera_position = *value;
	tmp[0] = value->x;
	tmp[1] = value->y;
	tmp[2] = value->z;
	liren_uniforms32_set_vec3 (&self->uniforms, LIREN_UNIFORM_CAMERA_POSITION, tmp);
}

void liren_context32_set_cull (
	LIRenContext32* self,
	int           enable,
	int           front_face)
{
	if (self->cull.enable != enable ||
	    self->cull.front_face != front_face)
	{
		self->cull.enable = enable;
		self->cull.front_face = front_face;
		self->changed.cull = 1;
	}
}

int liren_context32_get_deferred (
	LIRenContext32* self)
{
	return self->deferred;
}

void liren_context32_set_deferred (
	LIRenContext32* self,
	int           value)
{
	if (self->deferred != value)
	{
		self->deferred = value;
		self->changed.blend = 1;
		self->changed.depth = 1;
	}
}

void liren_context32_set_color_write (
	LIRenContext32* self,
	int           value)
{
	if (self->color_write != value)
	{
		if (value)
			glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		else
			glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}
}

void liren_context32_set_depth (
	LIRenContext32* self,
	int           enable_test,
	int           enable_write,
	GLenum        depth_func)
{
	if (self->depth.enable_test != enable_test ||
	    self->depth.enable_write != enable_write ||
	    self->depth.depth_func != depth_func)
	{
		self->depth.enable_test = enable_test;
		self->depth.enable_write = enable_write;
		self->depth.depth_func = depth_func;
		self->changed.depth = 1;
	}
}

void liren_context32_set_diffuse (
	LIRenContext32* self,
	const float*  value)
{
	liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_DIFFUSE, value);
}

void liren_context32_set_flags (
	LIRenContext32* self,
	int           value)
{
/*	self->lighting = ((value & LIREN_FLAG_LIGHTING) != 0);
	self->texturing = ((value & LIREN_FLAG_TEXTURING) != 0);*/
	self->shadows = ((value & LIREN_FLAG_SHADOW1) != 0);
}

void liren_context32_set_frustum (
	LIRenContext32*       self,
	const LIMatFrustum* frustum)
{
	self->frustum = *frustum;
}

void liren_context32_set_light (
	LIRenContext32* self,
	int           index,
	LIRenLight32*   value)
{
	const float black[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const float infinite[3] = { 999999.0f, 999999.0f, 999999.0f };

	if (value != NULL)
	{
		liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT0_AMBIENT + index, value->ambient);
		liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT0_DIFFUSE + index, value->diffuse);
		liren_uniforms32_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT0_EQUATION + index, value->equation);
		liren_uniforms32_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT0_POSITION + index, value->cache.pos_world);
		liren_uniforms32_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT0_POSITION_PREMULT + index, value->cache.pos_view);
		liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT0_SPECULAR + index, value->specular);
	}
	else
	{
		liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT0_AMBIENT + index, black);
		liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT0_DIFFUSE + index, black);
		liren_uniforms32_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT0_EQUATION + index, infinite);
		liren_uniforms32_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT0_POSITION + index, infinite);
		liren_uniforms32_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT0_POSITION_PREMULT + index, infinite);
		liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT0_SPECULAR + index, black);
	}
}

void liren_context32_set_material (
	LIRenContext32*        self,
	const LIRenMaterial32* value)
{
	lisys_assert (value->shininess > 0.0f);
	lisys_assert (value->shininess <= 127.0f);

	liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_DIFFUSE, value->diffuse);
	liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_PARAM0, value->parameters);
	liren_uniforms32_set_float (&self->uniforms, LIREN_UNIFORM_MATERIAL_SHININESS, value->shininess);
	liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_SPECULAR, value->specular);

	/* Set material flags. */
	if (value->flags & LIREN_MATERIAL_FLAG_CULLFACE)
		liren_context32_set_cull (self, 1, GL_CCW);
	else
		liren_context32_set_cull (self, 0, GL_CCW);
}

void liren_context32_set_mesh (
	LIRenContext32* self,
	LIRenMesh32*    mesh)
{
	if (self->array != mesh->vao)
	{
		self->array = mesh->vao;
		self->changed.buffer = 1;
	}
}

void liren_context32_set_modelmatrix (
	LIRenContext32*      self,
	const LIMatMatrix* value)
{
	if (memcmp (&self->matrix.model, value, sizeof (LIMatMatrix)))
	{
		self->matrix.model = *value;
		self->matrix.modelview = limat_matrix_multiply (self->matrix.view, self->matrix.model);
		self->matrix.modelviewinverse = limat_matrix_invert (self->matrix.modelview);
		liren_uniforms32_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_MODELVIEW, self->matrix.modelview.m);
		liren_uniforms32_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_MODELVIEW_INVERSE, self->matrix.modelviewinverse.m);
		liren_uniforms32_set_mat3 (&self->uniforms, LIREN_UNIFORM_MATRIX_NORMAL, self->matrix.modelview.m);
	}
}

void liren_context32_set_param (
	LIRenContext32* self,
	const float*  value)
{
	liren_uniforms32_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_PARAM0, value);
}

int liren_context32_set_pose (
	LIRenContext32*  self,
	const LIMdlPose* pose)
{
	int i;
	int j;
	int count;
	int count1;
	GLfloat* data;
	LIMatDualquat dq;
	LIMdlPoseGroup* group;

	/* Clear the old pose. */
	if (self->buffer_texture.size)
		liren_buffer_texture32_clear (&self->buffer_texture);

	/* Collect pose data. */
	/* The first transformation in the list is the fallback identity
	   transformation used by vertices that don't have all four weights. */
	if (pose != NULL)
		count1 = pose->groups.count;
	else
		count1 = 0;
	count = 12 * (count1 + 1);
	data = lisys_calloc (count, sizeof (GLfloat));
	if (data == NULL)
		return 0;
	j = 0;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 1.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 1.0f;
	for (i = 0 ; i < count1 ; i++)
	{
		group = pose->groups.array + i;
		dq = limat_dualquat_multiply (
			limat_dualquat_init (group->head_pose, group->rotation),
			limat_dualquat_init_translation (limat_vector_multiply (group->head_rest, -1.0f)));
		data[j++] = dq.r.x;
		data[j++] = dq.r.y;
		data[j++] = dq.r.z;
		data[j++] = dq.r.w;
		data[j++] = dq.d.x;
		data[j++] = dq.d.y;
		data[j++] = dq.d.z;
		data[j++] = dq.d.w;
		data[j++] = group->head_rest.x;
		data[j++] = group->head_rest.y;
		data[j++] = group->head_rest.z;
		data[j++] = group->scale_pose;
	}

	/* Upload to the buffer texture. */
	liren_buffer_texture32_init (&self->buffer_texture, data, count * sizeof (GLfloat));
	lisys_free (data);
	glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_BUFFER_TEXTURE);
	glBindTexture (GL_TEXTURE_BUFFER, self->buffer_texture.texture);

	return 1;
}

void liren_context32_set_viewmatrix (
	LIRenContext32*      self,
	const LIMatMatrix* value)
{
	if (memcmp (&self->matrix.view, value, sizeof (LIMatMatrix)))
	{
		self->matrix.view = *value;
		self->matrix.modelview = limat_matrix_multiply (self->matrix.view, self->matrix.model);
		self->matrix.modelviewinverse = limat_matrix_invert (self->matrix.modelview);
		liren_uniforms32_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_MODELVIEW, self->matrix.modelview.m);
		liren_uniforms32_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_MODELVIEW_INVERSE, self->matrix.modelviewinverse.m);
		liren_uniforms32_set_mat3 (&self->uniforms, LIREN_UNIFORM_MATRIX_NORMAL, self->matrix.modelview.m);
	}
}

void liren_context32_set_projection (
	LIRenContext32*      self,
	const LIMatMatrix* value)
{
	if (memcmp (&self->matrix.projection, value, sizeof (LIMatMatrix)))
	{
		self->matrix.projection = *value;
		self->matrix.projectioninverse = limat_matrix_invert (self->matrix.projection);
		liren_uniforms32_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_PROJECTION, self->matrix.projection.m);
		liren_uniforms32_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_PROJECTION_INVERSE, self->matrix.projectioninverse.m);
	}
}

void liren_context32_set_scene (
	LIRenContext32* self,
	LIRenScene32*   scene)
{
	self->scene = scene;
	/* FIXME */
	self->textures.count = 0;
}

int liren_context32_get_scissor (
	LIRenContext32* self,
	int*          x,
	int*          y,
	int*          w,
	int*          h)
{
	*x = self->scissor.rect[0];
	*y = self->scissor.rect[1];
	*w = self->scissor.rect[2];
	*h = self->scissor.rect[3];
	return self->scissor.enabled;
}

void liren_context32_set_scissor (
	LIRenContext32* self,
	int           enabled,
	int           x,
	int           y,
	int           w,
	int           h)
{
	if (self->scissor.enabled != enabled)
	{
		self->scissor.enabled = enabled;
		if (enabled)
			glEnable (GL_SCISSOR_TEST);
		else
			glDisable (GL_SCISSOR_TEST);
	}
	if (self->scissor.rect[0] != x ||
	    self->scissor.rect[1] != y ||
	    self->scissor.rect[2] != w ||
	    self->scissor.rect[3] != h)
	{
		self->scissor.rect[0] = x;
		self->scissor.rect[1] = y;
		self->scissor.rect[2] = w;
		self->scissor.rect[3] = h;
		glScissor (x, y, w, h);
	}
}

void liren_context32_set_shader (
	LIRenContext32* self,
	int           pass,
	LIRenShader32*  value)
{
	if (pass != self->shader_pass || value != self->shader)
	{
		self->shader_pass = pass;
		self->shader = value;
		self->changed.shader = 1;
	}
	if (value != NULL)
	{
		liren_context32_set_alpha_to_coverage (self, value->passes[pass].alpha_to_coverage);
		liren_context32_set_blend (self, value->passes[pass].blend_enable,
			value->passes[pass].blend_src, value->passes[pass].blend_dst);
		liren_context32_set_color_write (self, value->passes[pass].color_write);
		liren_context32_set_depth (self, value->passes[pass].depth_test,
			value->passes[pass].depth_write, value->passes[pass].depth_func);
	}
}

void liren_context32_set_textures (
	LIRenContext32* self,
	LIRenImage32**  value,
	int             count)
{
	int c;
	int i;
	GLenum gltarget;
	GLuint gltexture;
	LIRenContextTexture32* texture;
	LIRenImage32* image;

	c = LIMAT_MIN (count, 4);
	for (i = 0 ; i < c ; i++)
	{
		/* Get the texture handle. */
		texture = self->textures.array + i;
		image = value[i];
		if (image != NULL)
		{
			gltarget = image->texture->target;
			gltexture = image->texture->texture;
		}
		else
		{
			gltarget = GL_TEXTURE_2D;
			gltexture = self->render->helpers.empty_texture;
		}

		/* Bind cube maps. */
		/* Only one cube map is supported at a time and it's bound to a
		   special sampler. The 2D sampler whose index contained a cube map
		   is set to an empty texture. */
		if (gltarget == GL_TEXTURE_CUBE_MAP)
		{
			if (self->textures.cubemap.texture != gltexture)
			{
				self->textures.cubemap.texture = gltexture;
				glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_CUBE_TEXTURE);
				glBindTexture (gltarget, gltexture);
			}
			gltarget = GL_TEXTURE_2D;
			gltexture = self->render->helpers.empty_texture;
		}

		/* Bind 2D textures. */
		if (texture->texture != gltexture)
		{
			texture->texture = gltexture;
			glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0 + i);
			glBindTexture (gltarget, gltexture);
		}
	}
	for ( ; i < 4 ; i++)
	{
		texture = self->textures.array + i;
		gltarget = GL_TEXTURE_2D;
		gltexture = self->render->helpers.empty_texture;
		if (texture->texture != gltexture)
		{
			texture->texture = gltexture;
			glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0 + i);
			glBindTexture (gltarget, gltexture);
		}
	}
}

void liren_context32_set_textures_raw (
	LIRenContext32* self,
	GLuint*       value,
	int           count)
{
	int c;
	int i;
	LIRenContextTexture32* texture;

	c = LIMAT_MIN (count, 4);
	for (i = 0 ; i < c ; i++)
	{
		texture = self->textures.array + i;
		if (texture->texture != value[i])
		{
			texture->texture = value[i];
			glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0 + i);
			glBindTexture (GL_TEXTURE_2D, texture->texture);
		}
	}
	for ( ; i < 4 ; i++)
	{
		texture = self->textures.array + i;
		if (texture->texture != self->render->helpers.empty_texture)
		{
			texture->texture = self->render->helpers.empty_texture;
			glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0 + i);
			glBindTexture (GL_TEXTURE_2D, texture->texture);
		}
	}
}

void liren_context32_set_time (
	LIRenContext32* self,
	float         time)
{
	liren_uniforms32_set_float (&self->uniforms, LIREN_UNIFORM_TIME, time);
}

/** @} */
/** @} */
