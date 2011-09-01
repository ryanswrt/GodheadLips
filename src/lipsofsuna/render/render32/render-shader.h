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

#ifndef __RENDER32_SHADER_H__
#define __RENDER32_SHADER_H__

#include "lipsofsuna/system.h"
#include "render-types.h"

LIAPICALL (LIRenShader32*, liren_shader32_new, (
	LIRenRender32* render,
	const char*    name));

LIAPICALL (void, liren_shader32_free, (
	LIRenShader32* self));

LIAPICALL (void, liren_shader32_clear_pass, (
	LIRenShader32* self,
	int            pass));

LIAPICALL (int, liren_shader32_compile, (
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
	GLenum         depth_func));

LIAPICALL (void, liren_shader32_reload, (
	LIRenShader32* self,
	int            pass));

LIAPICALL (int, liren_shader32_get_sort, (
	LIRenShader32* self));

LIAPICALL (void, liren_shader32_set_sort, (
	LIRenShader32* self,
	int          value));

#endif
