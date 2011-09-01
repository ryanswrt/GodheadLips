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
 * \addtogroup LIArc Archive
 * @{
 * \addtogroup LIArcPacket Packet
 * @{
 */

#include "lipsofsuna/system.h"
#include "archive-packet.h"

LIArcPacket* liarc_packet_new_readable (
	const char* buffer,
	int         length)
{
	LIArcPacket* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIArcPacket));
	if (self == NULL)
		return NULL;

	/* Copy the buffer. */
	self->buffer = lisys_calloc (length, 1);
	if (self->buffer == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	memcpy (self->buffer, buffer, length);

	/* Allocate the reader. */
	self->reader = liarc_reader_new (self->buffer, length);
	if (self->reader == NULL)
	{
		lisys_free (self->buffer);
		lisys_free (self);
		return NULL;
	}

	return self;
}

LIArcPacket* liarc_packet_new_writable (
	int type)
{
	LIArcPacket* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIArcPacket));
	if (self == NULL)
		return NULL;

	/* Allocate the writer. */
	self->writer = liarc_writer_new_packet (type);
	if (self->writer == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void liarc_packet_free (
	LIArcPacket* self)
{
	if (self->writer != NULL)
		liarc_writer_free (self->writer);
	if (self->reader != NULL)
		liarc_reader_free (self->reader);
	lisys_free (self->buffer);
	lisys_free (self);
}

/** @} */
/** @} */
