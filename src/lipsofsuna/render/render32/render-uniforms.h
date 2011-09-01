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

#ifndef __RENDER32_UNIFORMS_H__
#define __RENDER32_UNIFORMS_H__

#include "lipsofsuna/system.h"
#include "lipsofsuna/video.h"
#include "render-uniform.h"

#define LIREN_UNIFORM_BUFFER_SIZE 1600

typedef struct _LIRenUniforms32 LIRenUniforms32;
struct _LIRenUniforms32
{
	int ready;
	char data[LIREN_UNIFORM_BUFFER_SIZE];
	int flush_range_start;
	int flush_range_end;
	GLuint uniform_buffer;
	GLuint uniform_indices[LIREN_UNIFORM_COUNT];
	GLint uniform_offsets[LIREN_UNIFORM_COUNT];
	GLint uniform_matrix_strides[LIREN_UNIFORM_COUNT];
	float uniform_values[LIREN_UNIFORM_COUNT][16];
	int uniform_changed[LIREN_UNIFORM_COUNT];
};

LIAPICALL (void, liren_uniforms32_init, (
	LIRenUniforms32* self));

LIAPICALL (void, liren_uniforms32_clear, (
	LIRenUniforms32* self));

LIAPICALL (void, liren_uniforms32_commit, (
	LIRenUniforms32* self));

LIAPICALL (void, liren_uniforms32_reload, (
	LIRenUniforms32* self,
	int              pass));

LIAPICALL (void, liren_uniforms32_setup, (
	LIRenUniforms32* self,
	GLuint         program));

LIAPICALL (void, liren_uniforms32_set_float, (
	LIRenUniforms32* self,
	int            uniform,
	float          value));

LIAPICALL (void, liren_uniforms32_set_vec3, (
	LIRenUniforms32* self,
	int            uniform,
	const float*   value));

LIAPICALL (void, liren_uniforms32_set_vec4, (
	LIRenUniforms32* self,
	int            uniform,
	const float*   value));

LIAPICALL (void, liren_uniforms32_set_mat3, (
	LIRenUniforms32* self,
	int            uniform,
	const float*   value));

LIAPICALL (void, liren_uniforms32_set_mat4, (
	LIRenUniforms32* self,
	int            uniform,
	const float*   value));

#endif

