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
 * \addtogroup LIRenBuffer32 Buffer32
 * @{
 */

#include "render-buffer.h"
#include "render-private.h"

/**
 * \brief Allocates a vertex buffer.
 * \param index_data Pointer to an array indices, or NULL.
 * \param index_count Number of indices to allocate.
 * \param vertex_format Vertex format.
 * \param vertex_data Pointer to vertex format specific array of vertices, or NULL.
 * \param vertex_count Number of vertices to allocate.
 * \param type Usage type.
 * \return Vertex buffer or NULL.
 */
LIRenBuffer32* liren_buffer32_new (
	const LIRenIndex*  index_data,
	int                index_count,
	const LIRenFormat* vertex_format,
	const void*        vertex_data,
	int                vertex_count,
	int                type)
{
	int size;
	LIRenBuffer32* self;

	self = calloc (1, sizeof (LIRenBuffer32));
	if (self == NULL)
		return NULL;
	self->type = type;
	self->vertex_format = *vertex_format;
	glBindVertexArray (0);

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

	/* Create vertex array object. */
	if (self->vertex_buffer)
	{
		glGenVertexArrays (1, &self->vertex_array);
		glBindVertexArray (self->vertex_array);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->index_buffer);
		glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
		glEnableVertexAttribArray (LIREN_ATTRIBUTE_COORD);
		glVertexAttribPointer (LIREN_ATTRIBUTE_COORD, 3, vertex_format->vtx_format,
			GL_FALSE, vertex_format->size, NULL + vertex_format->vtx_offset);
		if (vertex_format->nml_format)
		{
			glEnableVertexAttribArray (LIREN_ATTRIBUTE_NORMAL);
			glVertexAttribPointer (LIREN_ATTRIBUTE_NORMAL, 3, vertex_format->nml_format,
				GL_FALSE, vertex_format->size, NULL + vertex_format->nml_offset);
		}
		if (vertex_format->tex_format)
		{
			glEnableVertexAttribArray (LIREN_ATTRIBUTE_TEXCOORD);
			glVertexAttribPointer (LIREN_ATTRIBUTE_TEXCOORD, 2, vertex_format->tex_format,
				GL_FALSE, vertex_format->size, NULL + vertex_format->tex_offset);
		}
	}

	return self;
}

/**
 * \brief Frees the vertex buffer.
 * \param self Buffer.
 */
void liren_buffer32_free (
	LIRenBuffer32* self)
{
	if (self->vertex_array)
		glDeleteVertexArrays (1, &self->vertex_array);
	if (self->index_buffer)
		glDeleteBuffers (1, &self->index_buffer);
	if (self->vertex_buffer)
		glDeleteBuffers (1, &self->vertex_buffer);
	free (self);
}

/**
 * \brief Locks the index array for reading or writing.
 * \param self Buffer.
 * \param write Nonzero to lock as writable, zero to lock as read-only.
 * \return Pointer to the locked array, or NULL.
 */
void* liren_buffer32_lock_indices (
	LIRenBuffer32* self,
	int            write)
{
	void* ret;

	if (self->index_buffer)
	{
		glBindVertexArray (0);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->index_buffer);
		if (write)
			ret = glMapBuffer (GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
		else
			ret = glMapBuffer (GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
		return ret;
	}
	else
		return NULL;
}

/**
 * \brief Locks the vertex array for reading or writing.
 * \param self Buffer.
 * \param write Nonzero to lock as writable, zero to lock as read-only.
 * \return Pointer to the locked array, or NULL.
 */
void* liren_buffer32_lock_vertices (
	LIRenBuffer32* self,
	int            write)
{
	void* ret;

	if (self->vertex_buffer)
	{
		glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
		if (write)
			ret = glMapBuffer (GL_ARRAY_BUFFER, GL_READ_WRITE);
		else
			ret = glMapBuffer (GL_ARRAY_BUFFER, GL_READ_ONLY);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		return ret;
	}
	else
		return NULL;
}

void liren_buffer32_unlock_indices (
	LIRenBuffer32* self,
	void*          data)
{
	if (self->index_buffer)
	{
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->index_buffer);
		glUnmapBuffer (GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void liren_buffer32_unlock_vertices (
	LIRenBuffer32* self,
	void*          data)
{
	if (self->vertex_buffer)
	{
		glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
		glUnmapBuffer (GL_ARRAY_BUFFER);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	}
}

/**
 * \brief Replaces vertex data.
 * \param self Buffer.
 * \param start Index of the first replaced vertex.
 * \param count Number of vertices to replace.
 * \param data Raw vertex data to upload.
 */
void liren_buffer32_upload_vertices (
	LIRenBuffer32* self,
	int            start,
	int            count,
	const void*    data)
{
	int offs;
	int size;
	void* map;

	lisys_assert (start >= 0);
	lisys_assert (start + count <= self->vertices.count);

	if (self->vertex_buffer)
	{
		offs = start * self->vertex_format.size;
		size = count * self->vertex_format.size;
		glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
		map = glMapBufferRange (GL_ARRAY_BUFFER, offs, size,
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		if (map == NULL)
			return;
		memcpy (map, data, size);
		glUnmapBuffer (GL_ARRAY_BUFFER);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	}
}

int liren_buffer32_get_size (
	LIRenBuffer32* self)
{
	return self->vertices.count * self->vertex_format.size;
}

/** @} */
/** @} */
