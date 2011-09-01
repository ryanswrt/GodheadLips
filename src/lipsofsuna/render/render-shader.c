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
 * \addtogroup LIRenShader Shader
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-shader.h"
#include "render-private.h"
#include "render21/render-private.h"
#include "render32/render-private.h"

/**
 * \brief Creates a new shader program.
 * \param render Renderer.
 * \param name Unique name.
 * \return New shader or NULL.
 */
LIRenShader* liren_shader_new (
	LIRenRender* render,
	const char*  name)
{
	LIRenShader* self;

	self = lisys_calloc (1, sizeof (LIRenShader));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_shader32_new (render->v32, name);
		if (self->v32 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}
	else
	{
		self->v21 = liren_shader21_new (render->v21, name);
		if (self->v21 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	/* Insert to dictionary. */
	if (!lialg_strdic_insert (render->shaders, name, self))
	{
		liren_shader_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the shader program.
 * \param self Shader.
 */
void liren_shader_free (
	LIRenShader* self)
{
	if (self->v32 != NULL)
	{
		lialg_strdic_remove (self->render->shaders, self->v32->name);
		liren_shader32_free (self->v32);
	}
	if (self->v21 != NULL)
	{
		lialg_strdic_remove (self->render->shaders, self->v21->name);
		liren_shader21_free (self->v21);
	}
	lisys_free (self);
}

/**
 * \brief Removes a pass from the shader.
 * \param self Shader.
 * \param pass Pass number.
 */
void liren_shader_clear_pass (
	LIRenShader* self,
	int          pass)
{
	if (self->v32 != NULL)
		liren_shader32_clear_pass (self->v32, pass);
	else
		liren_shader21_clear_pass (self->v21, pass);
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
int liren_shader_compile (
	LIRenShader* self,
	int          pass,
	const char*  vertex,
	const char*  geometry,
	const char*  fragment,
	int          animated,
	int          alpha_to_coverage,
	int          blend_enable,
	GLenum       blend_src,
	GLenum       blend_dst,
	int          color_write,
	int          depth_test,
	int          depth_write,
	GLenum       depth_func)
{
	if (self->v32 != NULL)
	{
		return liren_shader32_compile (self->v32, pass, vertex, geometry, fragment,
			animated, alpha_to_coverage, blend_enable, blend_src, blend_dst,
			color_write, depth_test, depth_write, depth_func);
	}
	else
	{
		return liren_shader21_compile (self->v21, pass, vertex, geometry, fragment,
			animated, alpha_to_coverage, blend_enable, blend_src, blend_dst,
			color_write, depth_test, depth_write, depth_func);
	}
}

void liren_shader_set_sort (
	LIRenShader* self,
	int          value)
{
	if (self->v32 != NULL)
		liren_shader32_set_sort (self->v32, value);
	else
		liren_shader21_set_sort (self->v21, value);
}

/** @} */
/** @} */
