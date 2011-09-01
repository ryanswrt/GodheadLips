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
 * \addtogroup LIRenShader32 Shader32
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-shader.h"
#include "render-private.h"

/**
 * \brief Creates a new shader program.
 * \param render Renderer.
 * \param name Unique name.
 * \return New shader or NULL.
 */
LIRenShader32* liren_shader32_new (
	LIRenRender32* render,
	const char*    name)
{
	int i;
	LIRenShader32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenShader32));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->name = lisys_string_dup (name);
	if (self->name == NULL)
	{
		liren_shader32_free (self);
		return NULL;
	}

	/* Initialize passes. */
	for (i = 0 ; i < LIREN_SHADER_PASS_COUNT ; i++)
		liren_program32_init (self->passes + i, render);

	return self;
}

/**
 * \brief Frees the shader program.
 * \param self Shader.
 */
void liren_shader32_free (
	LIRenShader32* self)
{
	int i;

	for (i = 0 ; i < LIREN_SHADER_PASS_COUNT ; i++)
		liren_program32_clear (self->passes + i);
	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Removes a pass from the shader.
 * \param self Shader.
 * \param pass Pass number.
 */
void liren_shader32_clear_pass (
	LIRenShader32* self,
	int            pass)
{
	liren_program32_clear (self->passes + pass);
	liren_program32_init (self->passes + pass, self->render);
}

/**
 * \brief Recompiles the shader out of new code.
 * \param self Shader.
 * \param pass Pass number.
 * \param vertex Vertex shader code.
 * \param geometry Geometry shader code or NULL.
 * \param fragment Fragment shader code.
 * \param animated Nonzero to enable skeletal transformation uploads.
 * \param alpha_to_coverage Nonzero to enable alpha to coverage.
 * \param blend_enable Nonzero to enable blending.
 * \param blend_src Source blend function.
 * \param blend_dst Destination blend function.
 * \param color_write Nonzero to enable color writes.
 * \param depth_test Nonzero to enable depth test.
 * \param depth_write Nonzero to enable depth writes.
 * \param depth_func Depth test function.
 * \return Nonzero on success.
 */
int liren_shader32_compile (
	LIRenShader32* self,
	int            pass,
	const char*    vertex,
	const char*    geometry,
	const char*    fragment,
	int            animated,
	int            alpha_to_coverage,
	int            blend_enable,
	GLenum         blend_src,
	GLenum         blend_dst,
	int            color_write,
	int            depth_test,
	int            depth_write,
	GLenum         depth_func)
{
	int ret;
	char* name;

	name = lisys_string_format ("%s[%d]", self->name, pass);
	if (name == NULL)
		return 0;
	liren_program32_set_alpha_to_coverage (self->passes + pass, alpha_to_coverage);
	liren_program32_set_blend (self->passes + pass, blend_enable, blend_src, blend_dst);
	liren_program32_set_color (self->passes + pass, color_write);
	liren_program32_set_depth (self->passes + pass, depth_test, depth_write, depth_func);
	ret = liren_program32_compile (self->passes + pass, name,
		vertex, geometry, fragment, animated);
	lisys_free (name);

	return ret;
}

/**
 * \brief Reloads the shader.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads the shader that was lost when the context was erased.
 *
 * \param self Shader.
 * \param pass Reload stage, not shader pass.
 */
void liren_shader32_reload (
	LIRenShader32* self,
	int            pass)
{
	int i;

	for (i = 0 ; i < LIREN_SHADER_PASS_COUNT ; i++)
		liren_program32_reload (self->passes + i, pass);
}

int liren_shader32_get_sort (
	LIRenShader32* self)
{
	return self->sort;
}

void liren_shader32_set_sort (
	LIRenShader32* self,
	int            value)
{
	self->sort = value;
}

/** @} */
/** @} */
