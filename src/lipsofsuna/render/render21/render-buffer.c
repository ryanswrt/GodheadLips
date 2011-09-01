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
 * \addtogroup LIRenBuffer21 Buffer21
 * @{
 */

#include "render-buffer.h"
#include "render-private.h"

/**
 * \brief Allocates a vertex buffer.
 * \param index_data Pointer to an array of 21-bit unsigned indices, or NULL.
 * \param index_count Number of indices to allocate.
 * \param vertex_format Vertex format.
 * \param vertex_data Pointer to vertex format specific array of vertices, or NULL.
 * \param vertex_count Number of vertices to allocate.
 * \param type Usage type.
 * \return Vertex buffer or NULL.
 */
LIRenBuffer21* liren_buffer21_new (
	const LIRenIndex*  index_data,
	int                index_count,
	const LIRenFormat* vertex_format,
	const void*        vertex_data,
	int                vertex_count,
	int                type)
{
	int size;
	LIRenBuffer21* self;

	self = calloc (1, sizeof (LIRenBuffer21));
	if (self == NULL)
		return NULL;
	self->type = type;
	self->vertex_format = *vertex_format;

	/* Create index buffer object. */
	if (index_count)
	{
		size = index_count * sizeof (LIRenIndex);
		glGenBuffers (1, &self->index_buffer);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->index_buffer);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, size, index_data, GL_STATIC_DRAW);
		self->indices.count = index_count;
	}

	/* Create vertex buffer object. */
	if (vertex_count)
	{
		size = vertex_count * vertex_format->size;
		switch (self->type)
		{
			case LIREN_BUFFER_TYPE_DYNAMIC:
				glGenBuffers (1, &self->vertex_buffer);
				glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
				glBufferData (GL_ARRAY_BUFFER, size, vertex_data, GL_DYNAMIC_DRAW);
				break;
			case LIREN_BUFFER_TYPE_STATIC:
				glGenBuffers (1, &self->vertex_buffer);
				glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
				glBufferData (GL_ARRAY_BUFFER, size, vertex_data, GL_STATIC_DRAW);
				break;
			case LIREN_BUFFER_TYPE_STREAM:
				glGenBuffers (1, &self->vertex_buffer);
				glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
				glBufferData (GL_ARRAY_BUFFER, size, vertex_data, GL_STREAM_DRAW);
				break;
			default:
				lisys_assert (0 && "invalid vbo type");
				break;
		}
		self->vertices.count = vertex_count;
	}

	return self;
}

/**
 * \brief Frees the vertex buffer.
 * \param self Buffer.
 */
void liren_buffer21_free (
	LIRenBuffer21* self)
{
	if (self->index_buffer)
		glDeleteBuffers (1, &self->index_buffer);
	if (self->vertex_buffer)
		glDeleteBuffers (1, &self->vertex_buffer);
	free (self);
}

/** @} */
/** @} */
