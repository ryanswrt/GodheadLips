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

#ifndef __RENDER32_MESH_H__
#define __RENDER32_MESH_H__

#include "lipsofsuna/system.h"
#include "lipsofsuna/video.h"
#include "render-types.h"

typedef struct _LIRenMesh32 LIRenMesh32;
struct _LIRenMesh32
{
	int sizes[2];
	int counts[2];
	GLuint vbos[2];
	GLuint vao;
	int reload_index_count;
	void* reload_index_data;
	int reload_vertex_count;
	void* reload_vertex_data;
};

LIAPICALL (int, liren_mesh32_init, (
	LIRenMesh32* self,
	LIRenIndex*  index_data,
	int          index_count,
	void*        vertex_data,
	int          vertex_count));

LIAPICALL (void, liren_mesh32_clear, (
	LIRenMesh32* self));

LIAPICALL (void, liren_mesh32_deform, (
	LIRenMesh32* self));

LIAPICALL (void*, liren_mesh32_lock_vertices, (
	const LIRenMesh32* self,
	int                start,
	int                count));

LIAPICALL (void, liren_mesh32_reload, (
	LIRenMesh32* self,
	int          pass));

LIAPICALL (void, liren_mesh32_unlock_vertices, (
	const LIRenMesh32* self));

LIAPICALL (void, liren_mesh32_get_format, (
	const LIRenMesh32* self,
	LIRenFormat*       value));

#endif

