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
 * \addtogroup LIRenShader21 Shader21
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
LIRenShader21* liren_shader21_new (
	LIRenRender21* render,
	const char*    name)
{
	LIRenShader21* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenShader21));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->name = lisys_string_dup (name);
	if (self->name == NULL)
	{
		liren_shader21_free (self);
		return NULL;
	}

	/* TODO */

	return self;
}

/**
 * \brief Frees the shader program.
 * \param self Shader.
 */
void liren_shader21_free (
	LIRenShader21* self)
{
	/* TODO */

	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Removes a pass from the shader.
 * \param self Shader.
 * \param pass Pass number.
 */
void liren_shader21_clear_pass (
	LIRenShader21* self,
	int            pass)
{
	/* TODO */
}

/**
 * \brief Recompiles the shader out of new code.
 * \param self Shader.
 * \param pass Pass number.
 * \param vertex Vertex shader code.
 * \param geometry Geometry shader code or NULL.
 * \param fragment Fragment shader code.
 * \param feedback Nonzero to enable transform feedback.
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
int liren_shader21_compile (
	LIRenShader21* self,
	int            pass,
	const char*    vertex,
	const char*    geometry,
	const char*    fragment,
	int            feedback,
	int            alpha_to_coverage,
	int            blend_enable,
	GLenum         blend_src,
	GLenum         blend_dst,
	int            color_write,
	int            depth_test,
	int            depth_write,
	GLenum         depth_func)
{
	/* TODO */

	return 1;
}

/**
 * \brief Reloads the shader.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads the shader that was lost when the context was erased.
 *
 * \param self Shader.
 */
void liren_shader21_reload (
	LIRenShader21* self)
{
	/* TODO */
}

int liren_shader21_get_sort (
	LIRenShader21* self)
{
	return self->sort;
}

void liren_shader21_set_sort (
	LIRenShader21* self,
	int            value)
{
	self->sort = value;
}

/** @} */
/** @} */
