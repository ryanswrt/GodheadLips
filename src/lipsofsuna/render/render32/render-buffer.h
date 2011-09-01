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

#ifndef __RENDER32_BUFFER_H__
#define __RENDER32_BUFFER_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/video.h>
#include "render-material.h"
#include "render-types.h"

LIAPICALL (LIRenBuffer32*, liren_buffer32_new, (
	const LIRenIndex*  index_data,
	int                index_count,
	const LIRenFormat* vertex_format,
	const void*        vertex_data,
	int                vertex_count,
	int                type));

LIAPICALL (void, liren_buffer32_free, (
	LIRenBuffer32* self));

LIAPICALL (void*, liren_buffer32_lock_indices, (
	LIRenBuffer32* self,
	int            write));

LIAPICALL (void*, liren_buffer32_lock_vertices, (
	LIRenBuffer32* self,
	int            write));

LIAPICALL (void, liren_buffer32_unlock_indices, (
	LIRenBuffer32* self,
	void*          data));

LIAPICALL (void, liren_buffer32_unlock_vertices, (
	LIRenBuffer32* self,
	void*          data));

LIAPICALL (void, liren_buffer32_upload_vertices, (
	LIRenBuffer32* self,
	int            start,
	int            count,
	const void*    data));

LIAPICALL (int, liren_buffer32_get_size, (
	LIRenBuffer32* self));

#endif
