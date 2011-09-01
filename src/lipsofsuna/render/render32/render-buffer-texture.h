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

#ifndef __RENDER32_BUFFER_TEXTURE_H__
#define __RENDER32_BUFFER_TEXTURE_H__

#include "lipsofsuna/video.h"

typedef struct _LIRenBufferTexture32 LIRenBufferTexture32;
struct _LIRenBufferTexture32
{
	int size;
	GLuint buffer;
	GLuint texture;
};

LIAPICALL (void, liren_buffer_texture32_init, (
	LIRenBufferTexture32* self,
	void*                 data,
	int                   size));

LIAPICALL (void, liren_buffer_texture32_clear, (
	LIRenBufferTexture32* self));

LIAPICALL (void, liren_buffer_texture32_upload, (
	LIRenBufferTexture32* self,
	int                   size,
	void*                 data));

#endif
