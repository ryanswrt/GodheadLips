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

#ifndef __RENDER32_H__
#define __RENDER32_H__

#include "lipsofsuna/paths.h"
#include "lipsofsuna/system.h"
#include "render-buffer.h"
#include "render-image.h"
#include "render-light.h"
#include "render-material.h"
#include "render-object.h"
#include "render-shader.h"
#include "render-types.h"

LIAPICALL (LIRenRender32*, liren_render32_new, (
	void*       render,
	LIPthPaths* paths));

LIAPICALL (void, liren_render32_free, (
	LIRenRender32* self));

LIAPICALL (void, liren_render32_draw_clipped_buffer, (
	LIRenRender32*     self,
	LIRenShader32*     shader,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection,
	GLuint             texture,
	const float*       diffuse,
	const int*         scissor,
	LIRenBuffer32*     buffer));

LIAPICALL (void, liren_render32_draw_indexed_triangles_T2V3, (
	LIRenRender32*    self,
	LIRenShader32*    shader,
	LIMatMatrix*      matrix,
	GLuint            texture,
	const float*      diffuse,
	const float*      vertex_data,
	const LIRenIndex* index_data,
	int               index_count));

LIAPICALL (LIRenShader32*, liren_render32_find_shader, (
	LIRenRender32* self,
	const char*  name));

LIAPICALL (LIRenImage32*, liren_render32_find_image, (
	LIRenRender32* self,
	const char*  name));

LIAPICALL (LIRenModel32*, liren_render32_find_model, (
	LIRenRender32* self,
	int          id));

LIAPICALL (LIRenImage32*, liren_render32_load_image, (
	LIRenRender32* self,
	const char*    name));

LIAPICALL (void, liren_render32_reload, (
	LIRenRender32* self,
	int            pass));

LIAPICALL (int, liren_render32_reload_image, (
	LIRenRender32* self,
	LIRenImage32*  image));

LIAPICALL (void, liren_render32_update, (
	LIRenRender32* self,
	float        secs));

LIAPICALL (int, liren_render32_get_anisotropy, (
	const LIRenRender32* self));

LIAPICALL (void, liren_render32_set_anisotropy, (
	LIRenRender32* self,
	int          value));

LIAPICALL (LIRenContext32*, liren_render32_get_context, (
	LIRenRender32* self));

#endif
