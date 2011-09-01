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
 * \addtogroup LIRenProgram32 Program32
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.h"
#include "render-context.h"
#include "render-private.h"
#include "render-program.h"
#include "render-uniforms.h"

static int private_compile (
	LIRenProgram32* self,
	const char*     name,
	const char*     vertex,
	const char*     geometry,
	const char*     fragment,
	int             animated);

static int private_check_compile (
	LIRenProgram32* self,
	const char*     name,
	GLint           shader,
	const char*     code);

static int private_check_link (
	LIRenProgram32* self,
	const char*     name,
	GLint           program);

/****************************************************************************/

/**
 * \brief Creates a new shader program.
 * \param self Program.
 * \param render Renderer.
 * \return Nonzero on success.
 */
int liren_program32_init (
	LIRenProgram32* self,
	LIRenRender32*  render)
{
	memset (self, 0, sizeof (LIRenProgram32));
	self->render = render;
	self->depth_write = 1;
	self->depth_test = 1;
	self->depth_func = GL_LEQUAL;
	self->blend_enable = 0;
	self->blend_src = GL_SRC_ALPHA;
	self->blend_dst = GL_ONE_MINUS_SRC_ALPHA;
	self->color_write = 1;

	return 1;
}

/**
 * \brief Clears the shader program.
 * \param self Program.
 */
void liren_program32_clear (
	LIRenProgram32* self)
{
	lisys_free (self->reload_name);
	lisys_free (self->reload_vertex);
	lisys_free (self->reload_geometry);
	lisys_free (self->reload_fragment);
	glDeleteProgram (self->program);
	glDeleteShader (self->vertex);
	glDeleteShader (self->geometry);
	glDeleteShader (self->fragment);
	memset (self, 0, sizeof (LIRenProgram32));
}

/**
 * \brief Recompiles the program out of new code.
 * \param self Program.
 * \param name Name to be used in error messages.
 * \param vertex Vertex shader code.
 * \param geometry Geometry shader code or NULL.
 * \param fragment Fragment shader code.
 * \param animated Nonzero to enable skeletal transformation uploads.
 * \return Nonzero on success.
 */
int liren_program32_compile (
	LIRenProgram32* self,
	const char*     name,
	const char*     vertex,
	const char*     geometry,
	const char*     fragment,
	int             animated)
{
	LIRenProgram32 tmp;

	/* Test with a temporary program. */
	liren_program32_init (&tmp, self->render);
	if (!private_compile (&tmp, name, vertex, geometry, fragment, animated))
	{
		liren_program32_clear (&tmp);
		return 0;
	}

	/* Use the compiled program on success. */
	glDeleteProgram (self->program);
	glDeleteShader (self->vertex);
	glDeleteShader (self->geometry);
	glDeleteShader (self->fragment);
	self->program = tmp.program;
	self->vertex = tmp.vertex;
	self->geometry = tmp.geometry;
	self->fragment = tmp.fragment;
	self->animated = tmp.animated;

	/* Store values needed by the Windows context rebuild. */
	/* When the program is reloaded, this function is called with the
	   arguments stored to self->reload_*. We should not free the old
	   values in that case because we'd end up trying to duplicate what
	   we just freed. */
	if (self->reload_name != name)
	{
		lisys_free (self->reload_name);
		self->reload_name = lisys_string_dup (name);
	}
	if (self->reload_vertex != vertex)
	{
		lisys_free (self->reload_vertex);
		self->reload_vertex = lisys_string_dup (vertex);
	}
	if (self->reload_geometry != geometry)
	{
		lisys_free (self->reload_geometry);
		if (geometry != NULL)
			self->reload_geometry = lisys_string_dup (geometry);
		else
			self->reload_geometry = NULL;
	}
	if (self->reload_fragment != fragment)
	{
		lisys_free (self->reload_fragment);
		self->reload_fragment = lisys_string_dup (fragment);
	}

	return 1;
}

/**
 * \brief Reloads the shader program.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads the shader program that was lost when the context was erased.
 *
 * \param self Shader program.
 * \param pass Reload pass.
 */
void liren_program32_reload (
	LIRenProgram32* self,
	int             pass)
{
	if (!pass)
	{
		glDeleteProgram (self->program);
		glDeleteShader (self->vertex);
		glDeleteShader (self->geometry);
		glDeleteShader (self->fragment);
		self->program = 0;
		self->vertex = 0;
		self->geometry = 0;
		self->fragment = 0;
	}
	else if (self->reload_vertex != NULL)
	{
		lisys_assert (self->reload_name != NULL);
		lisys_assert (self->reload_fragment != NULL);
		liren_program32_compile (self,
			self->reload_name,
			self->reload_vertex,
			self->reload_geometry,
			self->reload_fragment,
			self->animated);
	}
}

void liren_program32_set_alpha_to_coverage (
	LIRenProgram32* self,
	int             value)
{
	self->alpha_to_coverage = value;
}

void liren_program32_set_blend (
	LIRenProgram32* self,
	int             blend_enable,
	GLenum          blend_src,
	GLenum          blend_dst)
{
	self->blend_enable = blend_enable;
	self->blend_src = blend_src;
	self->blend_dst = blend_dst;
}

void liren_program32_set_color (
	LIRenProgram32* self,
	int             color_write)
{
	self->color_write = color_write;
}

void liren_program32_set_depth (
	LIRenProgram32* self,
	int             depth_test,
	int             depth_write,
	GLenum          depth_func)
{
	self->depth_test = depth_test;
	self->depth_write = depth_write;
	self->depth_func = depth_func;
}

/****************************************************************************/

static int private_compile (
	LIRenProgram32* self,
	const char*     name,
	const char*     vertex,
	const char*     geometry,
	const char*     fragment,
	int             animated)
{
	int i;
	GLint restore;
	GLint binding;
	GLint lengths[3];
	GLint uniforms;
	const GLchar* strings[3];
	const GLchar* samplers[8] =
	{
		"LOS_diffuse_texture_0",
		"LOS_diffuse_texture_1",
		"LOS_diffuse_texture_2",
		"LOS_diffuse_texture_3",
		"LOS_buffer_texture",
		"LOS_cube_texture",
		"LOS_noise_texture",
		"LOS_shadow_texture"
	};
	const GLchar* outputs[4] =
	{
		"LOS_output_0",
		"LOS_output_1",
		"LOS_output_2",
		"LOS_output_3"
	};
	const GLchar* headers[4] =
	{
		/* Common */
		"#define LOS_LIGHT_MAX 5\n"
		"layout(shared) uniform LOSDATA\n{\n"
		"	vec3 LOS_camera_position;\n"
		"	vec4 LOS_material_diffuse;\n"
		"	vec4 LOS_material_param_0;\n"
		"	float LOS_material_shininess;\n"
		"	vec4 LOS_material_specular;\n"
		"	mat4 LOS_matrix_model;\n"
		"	mat4 LOS_matrix_modelview;\n"
		"	mat4 LOS_matrix_modelview_inverse;\n"
		"	mat4 LOS_matrix_modelview_projection;\n"
		"	mat3 LOS_matrix_normal;\n"
		"	mat4 LOS_matrix_projection;\n"
		"	mat4 LOS_matrix_projection_inverse;\n"
		"	float LOS_time;\n"
		"	struct\n{\n"
		"		vec4 ambient;\n"
		"		vec4 diffuse;\n"
		"		vec3 equation;\n"
		"		vec3 position;\n"
		"		vec3 position_premult;\n"
		"		vec4 specular;\n"
		"	} LOS_light[LOS_LIGHT_MAX];\n"
		"};\n"
		"uniform sampler2D LOS_diffuse_texture_0;\n"
		"uniform sampler2D LOS_diffuse_texture_1;\n"
		"uniform sampler2D LOS_diffuse_texture_2;\n"
		"uniform sampler2D LOS_diffuse_texture_3;\n"
		"uniform samplerBuffer LOS_buffer_texture;\n"
		"uniform samplerCube LOS_cube_texture;\n"
		"uniform sampler2D LOS_noise_texture;\n"
		"uniform sampler2DShadow LOS_shadow_texture;\n",
		/* Vertex */
		"#version 150\n"
		"in vec4 LOS_color;\n"
		"in vec3 LOS_coord;\n"
		"in vec2 LOS_texcoord;\n"
		"in vec3 LOS_normal;\n"
		"in vec3 LOS_tangent;\n"
		"in vec4 LOS_weights1;\n"
		"in vec4 LOS_weights2;\n"
		"in ivec4 LOS_bones1;\n"
		"in ivec4 LOS_bones2;\n",
		/* Geometry */
		"#version 150\n"
		"#extension GL_EXT_geometry_shader4 : enable\n",
		/* Fragment */
		"#version 150\n"
		"out vec4 LOS_output_0;\n"
		"out vec4 LOS_output_1;\n"
		"out vec4 LOS_output_2;\n"
		"out vec4 LOS_output_3;\n"
	};

	/* Create shader objects. */
	self->vertex = glCreateShader (GL_VERTEX_SHADER);
	if (geometry != NULL)
		self->geometry = glCreateShader (GL_GEOMETRY_SHADER);
	self->fragment = glCreateShader (GL_FRAGMENT_SHADER);

	/* Upload shader source. */
	strings[0] = headers[1];
	strings[1] = headers[0];
	strings[2] = vertex;
	lengths[0] = strlen (strings[0]);
	lengths[1] = strlen (strings[1]);
	lengths[2] = strlen (strings[2]);
	glShaderSource (self->vertex, 3, strings, lengths);
	if (geometry != NULL)
	{
		strings[0] = headers[2];
		strings[1] = headers[0];
		strings[2] = geometry;
		lengths[0] = strlen (strings[0]);
		lengths[1] = strlen (strings[1]);
		lengths[2] = strlen (strings[2]);
		glShaderSource (self->geometry, 3, strings, lengths);
	}
	strings[0] = headers[3];
	strings[1] = headers[0];
	strings[2] = fragment;
	lengths[0] = strlen (strings[0]);
	lengths[1] = strlen (strings[1]);
	lengths[2] = strlen (strings[2]);
	glShaderSource (self->fragment, 3, strings, lengths);

	/* Compile the vertex shader. */
	glCompileShader (self->vertex);
	if (!private_check_compile (self, name, self->vertex, vertex))
		return 0;

	/* Compile the geometry shader. */
	if (geometry != NULL)
	{
		glCompileShader (self->geometry);
		if (!private_check_compile (self, name, self->geometry, geometry))
			return 0;
	}

	/* Compile the fragment shader. */
	glCompileShader (self->fragment);
	if (!private_check_compile (self, name, self->fragment, fragment))
		return 0;

	/* Attach shaders. */
	self->program = glCreateProgram ();
	glAttachShader (self->program, self->vertex);
	if (self->geometry)
		glAttachShader (self->program, self->geometry);
	glAttachShader (self->program, self->fragment);

	/* Bind attributes and outputs. */
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_COLOR, "LOS_color");
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_COORD, "LOS_coord");
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_TEXCOORD, "LOS_texcoord");
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_NORMAL, "LOS_normal");
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_TANGENT, "LOS_tangent");
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_WEIGHTS1, "LOS_weights1");
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_WEIGHTS2, "LOS_weights2");
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_BONES1, "LOS_bones1");
	glBindAttribLocation (self->program, LIREN_ATTRIBUTE_BONES2, "LOS_bones2");
	for (i = 0 ; i < sizeof (outputs) / sizeof (*outputs) ; i++)
		glBindFragDataLocation (self->program, i, outputs[i]);

	/* Link the shader program. */
	glLinkProgram (self->program);
	if (!private_check_link (self, name, self->program))
		return 0;

	/* Bind the uniform buffer. */
	/* All shaders share the same uniform block so that we don't need to
	   reset our uniform data every time the shader is changed. */
	uniforms = glGetUniformBlockIndex (self->program, "LOSDATA");
	if (uniforms != GL_INVALID_INDEX)
	{
		glUniformBlockBinding (self->program, uniforms, 0);
		liren_uniforms32_setup (&self->render->context->uniforms, self->program);
	}

	/* Bind samplers to standard indices. */
	/* All shaders use the same texture unit layout so that we don't need
	   to rebind textures every time the shader is changed. */
	glGetIntegerv (GL_CURRENT_PROGRAM, &restore);
	glUseProgram (self->program);
	for (i = 0 ; i < sizeof (samplers) / sizeof (*samplers) ; i++)
	{
		binding = glGetUniformLocation (self->program, samplers[i]);
		if (binding != GL_INVALID_INDEX)
			glUniform1i (binding, i);
	}
	glUseProgram (restore);

	/* Set animation enable. */
	self->animated = animated;

	return 1;
}

static int private_check_compile (
	LIRenProgram32* self,
	const char*     name,
	GLint           shader,
	const char*     code)
{
	char* text;
	const char* type;
	GLint status;
	GLchar log[512];
	GLsizei length;
	LIArcReader* reader;

	if (shader == self->vertex)
		type = "vertex";
	else if (shader == self->fragment)
		type = "fragment";
	else
		type = "geometry";
	glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderInfoLog (shader, sizeof (log), &length, log);
		if (length)
		{
			reader = liarc_reader_new (log, length);
			if (reader != NULL)
			{
				while (liarc_reader_get_text (reader, "\n", &text))
				{
					printf ("WARNING: %s:%s %s\n", name, type, text);
					lisys_free (text);
				}
				liarc_reader_free (reader);
			}
		}
		lisys_error_set (EINVAL, "cannot compile %s shader `%s':\n\n%s", type, name, code);
		return 0;
	}

	return 1;
}

static int private_check_link (
	LIRenProgram32* self,
	const char*     name,
	GLint           program)
{
	char* text;
	GLint status;
	GLchar log[512];
	GLsizei length;
	LIArcReader* reader;

	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramInfoLog (program, sizeof (log), &length, log);
		reader = liarc_reader_new (log, length);
		if (reader != NULL)
		{
			while (liarc_reader_get_text (reader, "\n", &text))
			{
				printf ("WARNING: %s:%s\n", name, text);
				lisys_free (text);
			}
			liarc_reader_free (reader);
		}
		lisys_error_set (EINVAL, "cannot link program `%s'", name);
		return 0;
	}

	return 1;
}

/** @} */
/** @} */

