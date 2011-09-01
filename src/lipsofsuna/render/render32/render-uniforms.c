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
 * \addtogroup LIRenUniforms32 Uniforms32
 * @{
 */

#include "render-uniforms.h"

static void private_queue_mat (
	LIRenUniforms32* self,
	int              uniform,
	const float*     value,
	int              size);

static void private_queue_vec (
	LIRenUniforms32* self,
	int              uniform,
	const float*     value,
	int              size);

/****************************************************************************/

void liren_uniforms32_init (
	LIRenUniforms32* self)
{
	/* Initialize self. */
	memset (self, 0, sizeof (LIRenUniforms32));
	self->flush_range_start = 0;
	self->flush_range_end = 0;

	/* Generate the uniform buffer. */
	glGenBuffers (1, &self->uniform_buffer);
	glBindBuffer (GL_UNIFORM_BUFFER, self->uniform_buffer);
	glBufferData (GL_UNIFORM_BUFFER, LIREN_UNIFORM_BUFFER_SIZE, self->data, GL_DYNAMIC_DRAW);
}

void liren_uniforms32_clear (
	LIRenUniforms32* self)
{
	glDeleteBuffers (1, &self->uniform_buffer);
	memset (self, 0, sizeof (LIRenUniforms32));
}

void liren_uniforms32_reload (
	LIRenUniforms32* self,
	int              pass)
{
	if (!pass)
	{
		glDeleteBuffers (1, &self->uniform_buffer);
	}
	else
	{
		glGenBuffers (1, &self->uniform_buffer);
		glBindBuffer (GL_UNIFORM_BUFFER, self->uniform_buffer);
		glBufferData (GL_UNIFORM_BUFFER, LIREN_UNIFORM_BUFFER_SIZE, self->data, GL_DYNAMIC_DRAW);
		self->flush_range_start = 0;
		self->flush_range_end = LIREN_UNIFORM_BUFFER_SIZE;
		self->ready = 0;
	}
}

void liren_uniforms32_setup (
	LIRenUniforms32* self,
	GLuint           program)
{
	int i;
	const char* uniform_names[LIREN_UNIFORM_COUNT] = {
		"LOS_camera_position",
		"LOS_material_diffuse",
		"LOS_material_param_0",
		"LOS_material_shininess",
		"LOS_material_specular",
		"LOS_matrix_model",
		"LOS_matrix_modelview",
		"LOS_matrix_modelview_inverse",
		"LOS_matrix_modelview_projection",
		"LOS_matrix_normal",
		"LOS_matrix_projection",
		"LOS_matrix_projection_inverse",
		"LOS_time",
		"LOS_light[0].ambient",
		"LOS_light[1].ambient",
		"LOS_light[2].ambient",
		"LOS_light[3].ambient",
		"LOS_light[4].ambient",
		"LOS_light[0].diffuse",
		"LOS_light[1].diffuse",
		"LOS_light[2].diffuse",
		"LOS_light[3].diffuse",
		"LOS_light[4].diffuse",
		"LOS_light[0].equation",
		"LOS_light[1].equation",
		"LOS_light[2].equation",
		"LOS_light[3].equation",
		"LOS_light[4].equation",
		"LOS_light[0].position",
		"LOS_light[1].position",
		"LOS_light[2].position",
		"LOS_light[3].position",
		"LOS_light[4].position",
		"LOS_light[0].position_premult",
		"LOS_light[1].position_premult",
		"LOS_light[2].position_premult",
		"LOS_light[3].position_premult",
		"LOS_light[4].position_premult",
		"LOS_light[0].specular",
		"LOS_light[1].specular",
		"LOS_light[2].specular",
		"LOS_light[3].specular",
		"LOS_light[4].specular",
	};

	/* Setup when called by the first shader. */
	if (self->ready)
		return;
	self->ready = 1;

	/* Setup the uniform buffer. */
	/* The uniform buffer bound here will remain bound until the graphics
	   system is reset when the game exits or a new mod is launched. */
	glBindBufferBase (GL_UNIFORM_BUFFER, 0, self->uniform_buffer);

	/* Get uniform offsets. */
	glGetUniformIndices (program, LIREN_UNIFORM_COUNT, uniform_names, self->uniform_indices);
	glGetActiveUniformsiv (program, LIREN_UNIFORM_COUNT, self->uniform_indices,
		GL_UNIFORM_OFFSET, self->uniform_offsets);
	glGetActiveUniformsiv (program, LIREN_UNIFORM_COUNT, self->uniform_indices,
		GL_UNIFORM_MATRIX_STRIDE, self->uniform_matrix_strides);

	/* Check for validity. */
	for (i = 0 ; i < LIREN_UNIFORM_COUNT ; i++)
	{
		if (self->uniform_indices[i] == GL_INVALID_INDEX)
		{
			lisys_error_set (EINVAL, "could not find uniform `%s'", uniform_names[i]);
			lisys_error_report ();
		}
	}
}

void liren_uniforms32_commit (
	LIRenUniforms32* self)
{
	int start;
	int end;

	start = self->flush_range_start;
	end = self->flush_range_end;
	if (!self->ready || start > end)
		return;

	glBufferSubData (GL_UNIFORM_BUFFER, start, end - start, NULL);
	glBufferSubData (GL_UNIFORM_BUFFER, start, end - start, self->data + start);
	self->flush_range_start = LIREN_UNIFORM_BUFFER_SIZE;
	self->flush_range_end = 0;
}

void liren_uniforms32_set_float (
	LIRenUniforms32* self,
	int              uniform,
	float            value)
{
	private_queue_vec (self, uniform, &value, 1);
}

void liren_uniforms32_set_vec3 (
	LIRenUniforms32* self,
	int              uniform,
	const float*     value)
{
	private_queue_vec (self, uniform, value, 3);
}

void liren_uniforms32_set_vec4 (
	LIRenUniforms32* self,
	int              uniform,
	const float*     value)
{
	private_queue_vec (self, uniform, value, 4);
}

void liren_uniforms32_set_mat3 (
	LIRenUniforms32* self,
	int              uniform,
	const float*     value)
{
	private_queue_mat (self, uniform, value, 3);
}

void liren_uniforms32_set_mat4 (
	LIRenUniforms32* self,
	int              uniform,
	const float*     value)
{
	private_queue_mat (self, uniform, value, 4);
}

/****************************************************************************/

static void private_queue_mat (
	LIRenUniforms32* self,
	int              uniform,
	const float*     value,
	int              size)
{
	int i;
	int offset;
	int stride;

	if (memcmp (self->uniform_values[uniform], value, 16 * sizeof (float)))
	{
		/* Copy to back buffer. */
		offset = self->uniform_offsets[uniform];
		stride = self->uniform_matrix_strides[uniform];
		memcpy (self->uniform_values[uniform], value, 16 * sizeof (float));
		for (i = 0 ; i < size ; i++)
			memcpy (self->data + offset + i * stride, value + i * 4, size * sizeof (float));

		/* Recalculate dirty range. */
		self->flush_range_start = LIMAT_MIN (self->flush_range_start, offset);
		self->flush_range_end = LIMAT_MAX (self->flush_range_end, offset + size * stride);
	}
}

static void private_queue_vec (
	LIRenUniforms32* self,
	int              uniform,
	const float*     value,
	int              size)
{
	int offset;

	if (memcmp (self->uniform_values[uniform], value, size * sizeof (float)))
	{
		/* Copy to back buffer. */
		offset = self->uniform_offsets[uniform];
		memcpy (self->uniform_values[uniform], value, size * sizeof (float));
		memcpy (self->data + offset, value, size * sizeof (float));

		/* Recalculate dirty range. */
		self->flush_range_start = LIMAT_MIN (self->flush_range_start, offset);
		self->flush_range_end = LIMAT_MAX (self->flush_range_end, offset + size * sizeof (float));
	}
}

/** @} */
/** @} */

