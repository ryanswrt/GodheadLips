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
 * \addtogroup LIRenBufferTexture32 BufferTexture32
 * @{
 */

#include "render-buffer-texture.h"

void liren_buffer_texture32_init (
	LIRenBufferTexture32* self,
	void*                 data,
	int                   size)
{
	GLint restore;

	memset (self, 0, sizeof (LIRenBufferTexture32));
	self->size = size;
	glGenBuffers (1, &self->buffer);
	glGenTextures (1, &self->texture);

	glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
	glBindVertexArray (0);
	glBindBuffer (GL_TEXTURE_BUFFER, self->buffer);
	glBufferData (GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);
	glBindVertexArray (restore);

	glGetIntegerv (GL_TEXTURE_BINDING_BUFFER, &restore);
	glBindTexture (GL_TEXTURE_BUFFER, self->texture);
	glTexBuffer (GL_TEXTURE_BUFFER, GL_RGBA32F, self->buffer);
	glBindTexture (GL_TEXTURE_BUFFER, restore);
}

void liren_buffer_texture32_clear (
	LIRenBufferTexture32* self)
{
	glDeleteBuffers (1, &self->buffer);
	glDeleteTextures (1, &self->texture);
	memset (self, 0 , sizeof (LIRenBufferTexture32));
}

void liren_buffer_texture32_upload (
	LIRenBufferTexture32* self,
	int                   size,
	void*                 data)
{
	GLint restore;

	self->size = size;
	glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
	glBindVertexArray (0);
	glBindBuffer (GL_TEXTURE_BUFFER, self->buffer);
	glBufferData (GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);
	glBindVertexArray (restore);
}

/** @} */
/** @} */
