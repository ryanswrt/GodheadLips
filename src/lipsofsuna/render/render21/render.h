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

#ifndef __RENDER21_H__
#define __RENDER21_H__

#include "lipsofsuna/paths.h"
#include "lipsofsuna/system.h"
#include "render-buffer.h"
#include "render-image.h"
#include "render-light.h"
#include "render-object.h"
#include "render-shader.h"
#include "render-types.h"

LIAPICALL (LIRenRender21*, liren_render21_new, (
	void*       render,
	LIPthPaths* paths));

LIAPICALL (void, liren_render21_free, (
	LIRenRender21* self));

LIAPICALL (void, liren_render21_draw_clipped_buffer, (
	LIRenRender21*     self,
	LIRenShader21*     shader,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection,
	GLuint             texture,
	const float*       diffuse,
	const int*         scissor,
	LIRenBuffer21*     buffer));

LIAPICALL (void, liren_render21_draw_indexed_triangles_T2V3, (
	LIRenRender21*    self,
	LIRenShader21*    shader,
	LIMatMatrix*      matrix,
	GLuint            texture,
	const float*      diffuse,
	const float*      vertex_data,
	const LIRenIndex* index_data,
	int               index_count));

LIAPICALL (LIRenShader21*, liren_render21_find_shader, (
	LIRenRender21* self,
	const char*    name));

LIAPICALL (LIRenImage21*, liren_render21_find_image, (
	LIRenRender21* self,
	const char*    name));

LIAPICALL (LIRenModel21*, liren_render21_find_model, (
	LIRenRender21* self,
	int            id));

LIAPICALL (LIRenImage21*, liren_render21_load_image, (
	LIRenRender21* self,
	const char*    name));

LIAPICALL (void, liren_render21_reload, (
	LIRenRender21* self,
	int            pass));

LIAPICALL (int, liren_render21_reload_image, (
	LIRenRender21* self,
	LIRenImage21*  image));

LIAPICALL (void, liren_render21_update, (
	LIRenRender21* self,
	float          secs));

LIAPICALL (int, liren_render21_get_anisotropy, (
	const LIRenRender21* self));

LIAPICALL (void, liren_render21_set_anisotropy, (
	LIRenRender21* self,
	int            value));

#endif
