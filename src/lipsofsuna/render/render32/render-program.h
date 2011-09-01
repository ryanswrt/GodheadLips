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

#ifndef __RENDER32_PROGRAM_H__
#define __RENDER32_PROGRAM_H__

#include <lipsofsuna/system.h>
#include "render-types.h"
#include "render-attribute.h"
#include "render-uniform.h"

typedef struct _LIRenProgram32 LIRenProgram32;
struct _LIRenProgram32
{
	LIRenRender32* render;
	int alpha_to_coverage;
	int depth_test;
	int depth_write;
	GLenum depth_func;
	int blend_enable;
	GLenum blend_src;
	GLenum blend_dst;
	int color_write;
	GLuint program;
	GLuint vertex;
	GLuint geometry;
	GLuint fragment;
	char* reload_name;
	char* reload_vertex;
	char* reload_geometry;
	char* reload_fragment;
	int animated;
};

LIAPICALL (int, liren_program32_init, (
	LIRenProgram32* self,
	LIRenRender32*  render));

LIAPICALL (void, liren_program32_clear, (
	LIRenProgram32* self));

LIAPICALL (int, liren_program32_compile, (
	LIRenProgram32* self,
	const char*   name,
	const char*   vertex,
	const char*   geometry,
	const char*   fragment,
	int           animated));

LIAPICALL (void, liren_program32_reload, (
	LIRenProgram32* self,
	int             pass));

LIAPICALL (void, liren_program32_set_alpha_to_coverage, (
	LIRenProgram32* self,
	int           value));

LIAPICALL (void, liren_program32_set_blend, (
	LIRenProgram32* self,
	int           blend_enabled,
	GLenum        blend_src,
	GLenum        blend_dst));

LIAPICALL (void, liren_program32_set_color, (
	LIRenProgram32* self,
	int           color_write));

LIAPICALL (void, liren_program32_set_depth, (
	LIRenProgram32* self,
	int           depth_test,
	int           depth_write,
	GLenum        depth_func));

#endif

