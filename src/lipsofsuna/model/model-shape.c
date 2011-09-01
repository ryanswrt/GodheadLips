/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup LIMdlShape Shape
 * @{
 */

#include <lipsofsuna/system.h>
#include "model-shape.h"

int limdl_shape_init_copy (
	LIMdlShape*       self,
	const LIMdlShape* shape)
{
	int i;
	int j;
	LIMatVector* vertex;
	LIMdlShapePart* srcpart;
	LIMdlShapePart* dstpart;

	/* Copy the name. */
	self->name = lisys_string_dup (shape->name);
	if (self->name == NULL)
	{
		limdl_shape_clear (self);
		return 0;
	}

	/* Copy the bounding box. */
	self->bounds = shape->bounds;
	self->center = shape->center;

	/* Allocate parts. */
	if (shape->parts.count)
	{
		self->parts.array = lisys_calloc (shape->parts.count, sizeof (LIMdlShapePart));
		if (self->parts.array == NULL)
		{
			limdl_shape_clear (self);
			return 0;
		}
		self->parts.count = shape->parts.count;
	}

	/* Read parts. */
	for (i = 0 ; i < self->parts.count ; i++)
	{
		srcpart = shape->parts.array + i;
		dstpart = self->parts.array + i;

		/* Allocate vertices. */
		if (srcpart->vertices.count)
		{
			dstpart->vertices.count = srcpart->vertices.count;
			dstpart->vertices.array = lisys_calloc (3 * srcpart->vertices.count, sizeof (LIMatVector));
			if (dstpart->vertices.array == NULL)
			{
				limdl_shape_clear (self);
				return 0;
			}
		}

		/* Copy vertices. */
		for (j = 0 ; j < srcpart->vertices.count ; j++)
		{
			vertex = dstpart->vertices.array + j;
			*vertex = srcpart->vertices.array[j];
		}
	}

	return 1;
}

void limdl_shape_clear (
	LIMdlShape*  self)
{
	int i;

	for (i = 0 ; i < self->parts.count ; i++)
	{
		lisys_free (self->parts.array[i].vertices.array);
	}
	lisys_free (self->parts.array);
	lisys_free (self->name);
}

int limdl_shape_read (
	LIMdlShape*  self,
	LIArcReader* reader)
{
	int i;
	int j;
	uint32_t tmp;
	LIMatVector* vertex;
	LIMdlShapePart* part;

	/* Read header. */
	if (!liarc_reader_get_text (reader, "", &self->name) ||
	    !liarc_reader_get_uint32 (reader, &tmp))
		return 0;

	/* Allocate parts. */
	if (tmp)
	{
		self->parts.array = lisys_calloc (tmp, sizeof (LIMdlShapePart));
		if (self->parts.array == NULL)
			return 0;
		self->parts.count = tmp;
	}

	/* Read parts. */
	for (i = 0 ; i < self->parts.count ; i++)
	{
		/* Read part header. */
		part = self->parts.array + i;
		if (!liarc_reader_get_uint32 (reader, &tmp))
			return 0;

		/* Allocate vertices. */
		if (tmp)
		{
			part->vertices.count = tmp;
			part->vertices.array = lisys_calloc (3 * tmp, sizeof (LIMatVector));
			if (part->vertices.array == NULL)
				return 0;
		}

		/* Read vertices. */
		for (j = 0 ; j < tmp ; j++)
		{
			/* Read the vertex position. */
			vertex = part->vertices.array + j;
			if (!liarc_reader_get_float (reader, &vertex->x) ||
				!liarc_reader_get_float (reader, &vertex->y) ||
				!liarc_reader_get_float (reader, &vertex->z))
				return 0;

			/* Update the bounding box. */
			if (!i && !j)
			{
				self->bounds.min = *vertex;
				self->bounds.max = *vertex;
			}
			else
				limat_aabb_add_point (&self->bounds, vertex);
		}
	}

	/* Calculate the center of mass. */
	limat_aabb_get_center (&self->bounds, &self->center);

	return 1;
}

int limdl_shape_write (
	const LIMdlShape* self,
	LIArcWriter*      writer)
{
	int i;
	int j;
	LIMdlShapePart* part;

	/* Write shape header. */
	if (!liarc_writer_append_string (writer, self->name) ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->parts.count))
		return 0;

	/* Write shape parts. */
	for (i = 0 ; i < self->parts.count ; i++)
	{
		part = self->parts.array + i;
		if (!liarc_writer_append_uint32 (writer, part->vertices.count))
			return 0;
		for (j = 0 ; j < part->vertices.count ; j++)
		{
			if (!liarc_writer_append_float (writer, part->vertices.array[j].x) ||
				!liarc_writer_append_float (writer, part->vertices.array[j].y) ||
				!liarc_writer_append_float (writer, part->vertices.array[j].z))
				return 0;
		}
	}

	return 1;
}

/**
 * \brief Gets the center of mass of the shape.
 * \param self Shape.
 * \param result Return location for the center of mass vector.
 */
void limdl_shape_get_center (
	const LIMdlShape* self,
	LIMatVector*      result)
{
	*result = self->center;
}

/** @} */
/** @} */
