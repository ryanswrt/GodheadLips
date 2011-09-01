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
 * \addtogroup LIMdlHair Hair
 * @{
 */

#include <lipsofsuna/system.h>
#include "model-hair.h"

int
limdl_hair_read (LIMdlHair*   self,
                 LIArcReader* reader)
{
	uint32_t i;
	uint32_t count;
	LIMdlHairNode* node;

	/* Read node count. */
	if (!liarc_reader_get_uint32 (reader, &count))
		return 0;

	/* Allocate nodes. */
	self->nodes = lisys_calloc (count, sizeof (LIMdlHairNode));
	if (self->nodes == NULL)
		return 0;
	self->count = count;

	/* Read nodes. */
	for (i = 0 ; i < count ; i++)
	{
		node = self->nodes + i;
		if (!liarc_reader_get_float (reader, &node->position.x) ||
		    !liarc_reader_get_float (reader, &node->position.y) ||
		    !liarc_reader_get_float (reader, &node->position.z) ||
		    !liarc_reader_get_float (reader, &node->size))
			return 0;
	}

	return 1;
}

int
limdl_hair_write (LIMdlHair*   self,
                  LIArcWriter* writer)
{
	int i;
	LIMdlHairNode* node;

	/* Write node count. */
	if (!liarc_writer_append_uint32 (writer, self->count))
		return 0;

	/* Write nodes. */
	for (i = 0 ; i < self->count ; i++)
	{
		node = self->nodes + i;
		if (!liarc_writer_append_float (writer, node->position.x) ||
		    !liarc_writer_append_float (writer, node->position.y) ||
		    !liarc_writer_append_float (writer, node->position.z) ||
		    !liarc_writer_append_float (writer, node->size))
			return 0;
	}

	return 1;
}

/*****************************************************************************/

void
limdl_hairs_free (LIMdlHairs* self)
{
	int i;

	for (i = 0 ; i < self->count ; i++)
		lisys_free (self->hairs[i].nodes);
	lisys_free (self->hairs);
}

int
limdl_hairs_read (LIMdlHairs*  self,
                  LIArcReader* reader)
{
	uint32_t i;
	uint32_t mat;
	uint32_t count;
	LIMdlHair* hair;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &mat) ||
	    !liarc_reader_get_uint32 (reader, &count))
		return 0;
	self->material = mat;

	/* Allocate hairs. */
	self->hairs = lisys_calloc (count, sizeof (LIMdlHair));
	if (self->hairs == NULL)
		return 0;
	self->count = count;

	/* Read hairs. */
	for (i = 0 ; i < count ; i++)
	{
		hair = self->hairs + i;
		if (!limdl_hair_read (hair, reader))
			return 0;
	}

	return 1;
}

int
limdl_hairs_write (LIMdlHairs*  self,
                   LIArcWriter* writer)
{
	int i;
	LIMdlHair* hair;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->material) ||
	    !liarc_writer_append_uint32 (writer, self->count))
		return 0;

	/* Write hairs. */
	for (i = 0 ; i < self->count ; i++)
	{
		hair = self->hairs + i;
		if (!limdl_hair_write (hair, writer))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
