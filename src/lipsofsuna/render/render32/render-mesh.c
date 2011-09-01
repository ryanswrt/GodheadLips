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

#include "render.h"
#include "render-attribute.h"
#include "render-mesh.h"

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenMesh32 Mesh32
 * @{
 */

int liren_mesh32_init (
	LIRenMesh32* self,
	LIRenIndex*  index_data,
	int          index_count,
	void*        vertex_data,
	int          vertex_count)
{
	GLint restore;

	const int vertex_size = sizeof (LIMdlVertex);
	const int vertex_tex_offset = offsetof (LIMdlVertex, texcoord);
	const int vertex_nml_offset = offsetof (LIMdlVertex, normal);
	const int vertex_vtx_offset = offsetof (LIMdlVertex, coord);
	const int vertex_tan_offset = offsetof (LIMdlVertex, tangent);
	const int vertex_col_offset = offsetof (LIMdlVertex, color);
	const int vertex_we1_offset = offsetof (LIMdlVertex, weights);
	const int vertex_bo1_offset = offsetof (LIMdlVertex, bones);

	/* Allocate objects. */
	memset (self, 0, sizeof (LIRenMesh32));
	if (!index_count || !vertex_count)
		return 1;
	glGenBuffers (2, self->vbos);
	glGenVertexArrays (1, &self->vao);
	self->counts[0] = index_count;
	self->counts[1] = vertex_count;
	self->sizes[0] = self->counts[0] * sizeof (LIRenIndex);
	self->sizes[1] = self->counts[1] * vertex_size;

	/* Don't break the active vertex array. */
	glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
	glBindVertexArray (0);

	/* Setup the index buffer. */
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->vbos[0]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, self->sizes[0], index_data, GL_STATIC_DRAW);

	/* Setup the vertex buffer. */
	glBindBuffer (GL_ARRAY_BUFFER, self->vbos[1]);
	glBufferData (GL_ARRAY_BUFFER, self->sizes[1], vertex_data, GL_STATIC_DRAW);

	/* Setup vertex array object. */
	glBindVertexArray (self->vao);
	glBindBuffer (GL_ARRAY_BUFFER, self->vbos[1]);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->vbos[0]);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_TEXCOORD);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_NORMAL);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_COORD);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_TANGENT);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_COLOR);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_WEIGHTS1);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_BONES1);
	glVertexAttribPointer (LIREN_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, vertex_size, NULL + vertex_tex_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, vertex_size, NULL + vertex_nml_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_COORD, 3, GL_FLOAT, GL_FALSE, vertex_size, NULL + vertex_vtx_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, vertex_size, NULL + vertex_tan_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertex_size, NULL + vertex_col_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_WEIGHTS1, 4, LIREN_WEIGHT_FORMAT, GL_TRUE, vertex_size, NULL + vertex_we1_offset);
	glVertexAttribIPointer (LIREN_ATTRIBUTE_BONES1, 4, GL_UNSIGNED_BYTE, vertex_size, NULL + vertex_bo1_offset);

	/* Don't break the active vertex array. */
	glBindVertexArray (restore);

	/* Store values needed by the Windows context rebuild. */
	self->reload_index_count = index_count;
	self->reload_index_data = lisys_calloc (self->sizes[0], 1);
	memcpy (self->reload_index_data, index_data, self->sizes[0]);
	self->reload_vertex_count = vertex_count;
	self->reload_vertex_data = lisys_calloc (self->sizes[1], 1);
	memcpy (self->reload_vertex_data, vertex_data, self->sizes[1]);

	return 1;
}

void liren_mesh32_clear (
	LIRenMesh32* self)
{
	glDeleteVertexArrays (1, &self->vao);
	glDeleteBuffers (2, self->vbos);
	lisys_free (self->reload_index_data);
	lisys_free (self->reload_vertex_data);
	memset (self, 0, sizeof (LIRenMesh32));
}

void liren_mesh32_deform (
	LIRenMesh32* self)
{
}

/**
 * \brief Locks the deformed vertex array for reading.
 * \param self Mesh.
 * \param start Start vertex.
 * \param count Count of locked vertices.
 * \return Pointer to the locked array, or NULL.
 */
void* liren_mesh32_lock_vertices (
	const LIRenMesh32* self,
	int                start,
	int                count)
{
	void* ret;
	GLint restore;
	LIRenFormat format;

	if (self->vbos[1])
	{
		liren_mesh32_get_format (self, &format);
		glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
		glBindVertexArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, self->vbos[1]);
		ret = glMapBufferRange (GL_ARRAY_BUFFER, format.size * start, format.size * count, GL_MAP_READ_BIT);
		glBindVertexArray (restore);
		return ret;
	}
	else
		return NULL;
}

/**
 * \brief Reloads the mesh.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads the mesh that was lost when the context was erased.
 *
 * \param self Mesh.
 * \param pass Reload pass.
 */
void liren_mesh32_reload (
	LIRenMesh32* self,
	int          pass)
{
	void* tmp1;
	void* tmp2;

	if (!pass)
	{
		glDeleteVertexArrays (1, &self->vao);
		glDeleteBuffers (2, self->vbos);
	}
	else
	{
		tmp1 = self->reload_index_data;
		tmp2 = self->reload_vertex_data;
		liren_mesh32_init (self, tmp1, self->reload_index_count, tmp2, self->reload_vertex_count);
		lisys_free (tmp1);
		lisys_free (tmp2);
	}
}

void liren_mesh32_unlock_vertices (
	const LIRenMesh32* self)
{
	GLint restore;

	if (self->vbos[1])
	{
		glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
		glBindVertexArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, self->vbos[1]);
		glUnmapBuffer (GL_ARRAY_BUFFER);
		glBindVertexArray (restore);
	}
}

void liren_mesh32_get_format (
	const LIRenMesh32* self,
	LIRenFormat*       value)
{
	const LIRenFormat format =
	{
		sizeof (LIMdlVertex),
		GL_FLOAT, offsetof (LIMdlVertex, texcoord),
		GL_FLOAT, offsetof (LIMdlVertex, normal),
		GL_FLOAT, offsetof (LIMdlVertex, coord)
	};
	*value = format;
}

/** @} */
/** @} */

