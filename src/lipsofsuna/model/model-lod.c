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
 * \addtogroup LIMdlLod Lod
 * @{
 */

#include "model-lod.h"

int limdl_lod_init_copy (
	LIMdlLod* self,
	LIMdlLod* lod)
{
	int i;

	if (lod->face_groups.count)
	{
		self->face_groups.array = lisys_calloc (lod->face_groups.count, sizeof (LIMdlFaces));
		self->face_groups.count = lod->face_groups.count;
		for (i = 0 ; i < lod->face_groups.count ; i++)
			limdl_faces_init_copy (self->face_groups.array + i, lod->face_groups.array + i);
	}
	if (lod->indices.count)
	{
		self->indices.array = lisys_calloc (lod->indices.count, sizeof (LIMdlIndex));
		self->indices.count = lod->indices.count;
		memcpy (self->indices.array, lod->indices.array, lod->indices.count * sizeof (LIMdlIndex));
	}

	return 1;
}

void limdl_lod_free (
	LIMdlLod* self)
{
	lisys_free (self->face_groups.array);
	lisys_free (self->indices.array);
}

int limdl_lod_read (
	LIMdlLod*    self,
	LIArcReader* reader)
{
	int pos;
	uint32_t i;
	uint32_t count1;
	uint32_t count2;
	uint32_t index;
	LIMdlFaces* group;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &count1) ||
	    !liarc_reader_get_uint32 (reader, &count2))
		return 0;
	self->face_groups.count = count1;
	self->indices.count = count2;

	/* Allocate face groups. */
	if (self->face_groups.count)
	{
		self->face_groups.array = lisys_calloc (self->face_groups.count, sizeof (LIMdlFaces));
		if (self->face_groups.array == NULL)
			return 0;
	}

	/* Read face groups. */
	for (i = pos = 0 ; i < self->face_groups.count ; i++)
	{
		/* Read the face group header. */
		group = self->face_groups.array + i;
		if (!liarc_reader_get_uint32 (reader, &count1))
			return 0;
		group->start = pos;
		group->count = count1;
		pos += count1;
	}

	/* Allocate indices. */
	if (self->indices.count)
	{
		self->indices.array = lisys_calloc (self->indices.count, sizeof (LIMdlIndex));;
		if (self->indices.array == NULL)
			return 0;
	}

	/* Read indices. */
	for (i = 0 ; i < self->indices.count ; i++)
	{
		if (!liarc_reader_get_uint32 (reader, &index))
			return 0;
		self->indices.array[i] = index;
	}

	return 1;
}

int limdl_lod_read_old (
	LIMdlLod*    self,
	LIArcReader* reader)
{
	uint32_t i;
	uint32_t j;
	uint32_t mat;
	uint32_t count;
	uint32_t index;
	LIMdlFaces* group;
	LIMdlIndex* new_indices;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &count))
		return 0;
	self->face_groups.count = count;

	/* Allocate face groups. */
	if (self->face_groups.count)
	{
		self->face_groups.array = lisys_calloc (self->face_groups.count, sizeof (LIMdlFaces));
		if (self->face_groups.array == NULL)
			return 0;
	}

	/* Read face groups. */
	for (i = 0 ; i < self->face_groups.count ; i++)
	{
		/* Read the face group header. */
		group = self->face_groups.array + i;
		if (!liarc_reader_get_uint32 (reader, &mat) ||
			!liarc_reader_get_uint32 (reader, &count))
			return 0;
		group->start = self->indices.count;
		group->count = count;

		/* Read indices. */
		if (count)
		{
			new_indices = lisys_realloc (self->indices.array, (self->indices.count + count) * sizeof (LIMdlIndex));
			if (new_indices == NULL)
				return 0;
			self->indices.array = new_indices;
			for (j = 0 ; j < count ; j++)
			{
				if (!liarc_reader_get_uint32 (reader, &index))
					return 0;
				self->indices.array[self->indices.count + j] = index;
			}
			self->indices.count += count;
		}
	}

	return 1;
}

int limdl_lod_write (
	const LIMdlLod* self,
	LIArcWriter*    writer)
{
	int i;
	LIMdlFaces* group;

	/* Check if writing is needed. */
	if (!self->face_groups.count)
		return 1;

	if (!liarc_writer_append_uint32 (writer, self->face_groups.count) ||
	    !liarc_writer_append_uint32 (writer, self->indices.count))
		return 0;

	/* Write face groups. */
	for (i = 0 ; i < self->face_groups.count ; i++)
	{
		group = self->face_groups.array + i;
		if (!liarc_writer_append_uint32 (writer, group->count))
			return 0;
	}

	/* Write indices. */
	for (i = 0 ; i < self->indices.count ; i++)
	{
		if (!liarc_writer_append_uint32 (writer, self->indices.array[i]))
			return 0;
	}

	return 1;
}

int limdl_lod_get_memory (
	const LIMdlLod* self)
{
	return sizeof (LIMdlLod) +
		self->face_groups.count * sizeof (LIMdlFaces) +
		self->indices.count * sizeof (LIMdlIndex);
}

/** @} */
/** @} */
