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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlShapeKey ShapeKey
 * @{
 */

#include <lipsofsuna/system.h>
#include "model-shape-key.h"

int limdl_shape_key_init_copy (
	LIMdlShapeKey*       self,
	const LIMdlShapeKey* key)
{
	int i;
	LIMdlShapeKeyVertex* vertex;

	/* Copy the name. */
	self->name = lisys_string_dup (key->name);
	if (self->name == NULL)
		return 0;

	/* Allocate vertices. */
	if (key->vertices.count)
	{
		self->vertices.array = lisys_calloc (key->vertices.count, sizeof (LIMdlShapeKeyVertex));
		if (self->vertices.array == NULL)
		{
			lisys_free (self->name);
			self->name = NULL;
			return 0;
		}
		self->vertices.count = key->vertices.count;
	}

	/* Copy vertices. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		vertex = self->vertices.array + i;
		*vertex = key->vertices.array[i];
	}

	return 1;
}

void limdl_shape_key_clear (
	LIMdlShapeKey*  self)
{
	lisys_free (self->vertices.array);
	lisys_free (self->name);
}

int limdl_shape_key_read (
	LIMdlShapeKey* self,
	LIArcReader*   reader)
{
	int i;
	uint32_t tmp;
	LIMdlShapeKeyVertex* vertex;

	/* Read header. */
	if (!liarc_reader_get_text (reader, "", &self->name) ||
	    !liarc_reader_get_uint32 (reader, &tmp))
		return 0;

	/* Allocate vertices. */
	if (tmp)
	{
		self->vertices.array = lisys_calloc (tmp, sizeof (LIMdlShapeKeyVertex));
		if (self->vertices.array == NULL)
			return 0;
		self->vertices.count = tmp;
	}

	/* Read vertices. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		vertex = self->vertices.array + i;
		if (!liarc_reader_get_float (reader, &vertex->coord.x) ||
			!liarc_reader_get_float (reader, &vertex->coord.y) ||
			!liarc_reader_get_float (reader, &vertex->coord.z) ||
			!liarc_reader_get_float (reader, &vertex->normal.x) ||
			!liarc_reader_get_float (reader, &vertex->normal.y) ||
			!liarc_reader_get_float (reader, &vertex->normal.z))
			return 0;
	}

	return 1;
}

int limdl_shape_key_write (
	const LIMdlShapeKey* self,
	LIArcWriter*         writer)
{
	int i;
	LIMdlShapeKeyVertex* v;

	/* Write shape key header. */
	if (!liarc_writer_append_string (writer, self->name) ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->vertices.count))
		return 0;

	/* Write vertices. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		v = self->vertices.array + i;
		if (!liarc_writer_append_float (writer, v->coord.x) ||
			!liarc_writer_append_float (writer, v->coord.y) ||
			!liarc_writer_append_float (writer, v->coord.z) ||
			!liarc_writer_append_float (writer, v->normal.x) ||
			!liarc_writer_append_float (writer, v->normal.y) ||
			!liarc_writer_append_float (writer, v->normal.z))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
