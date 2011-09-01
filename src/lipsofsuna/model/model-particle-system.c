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
 * \addtogroup LIMdlParticleSystem ParticleSystem
 * @{
 */

#include "lipsofsuna/system.h"
#include "model-particle.h"

static int private_read_v1 (
	LIMdlParticleSystem* self,
	LIArcReader*         reader,
	uint32_t             flags);

static int private_read_v2 (
	LIMdlParticleSystem* self,
	LIArcReader*         reader,
	uint32_t             flags);

/*****************************************************************************/

void limdl_particle_system_clear (
	LIMdlParticleSystem* self)
{
	int i;

	for (i = 0 ; i < self->particles.count ; i++)
		lisys_free (self->particles.array[i].frames.array);
	lisys_free (self->particles.array);
	self->particles.array = NULL;
	self->particles.count = 0;
	lisys_free (self->shader);
	lisys_free (self->texture);
	self->shader = NULL;
	self->texture = NULL;
}

int limdl_particle_system_read (
	LIMdlParticleSystem* self,
	LIArcReader*         reader)
{
	uint32_t flags;

	if (!liarc_reader_get_uint32 (reader, &flags))
		return 0;
	if (flags & 0x80000000)
		return private_read_v2 (self, reader, flags);
	else
		return private_read_v1 (self, reader, flags);

	return 1;
}

int limdl_particle_system_write (
	const LIMdlParticleSystem* self,
	LIArcWriter*               writer)
{
	int i;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->frame_start) ||
	    !liarc_writer_append_uint32 (writer, self->frame_end_emit) ||
	    !liarc_writer_append_uint32 (writer, self->frame_end) ||
	    !liarc_writer_append_float (writer, self->particle_size) ||
	    !liarc_writer_append_string (writer, self->texture) ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->particles.count))
		return 0;

	/* Write particles. */
	for (i = 0 ; i < self->particles.count ; i++)
	{
		if (!limdl_particle_write (self->particles.array + i, writer))
			return 0;
	}

	return 1;
}

/*****************************************************************************/

/* TODO: Remove this when all particle animations of models use the new format. */
static int private_read_v1 (
	LIMdlParticleSystem* self,
	LIArcReader*         reader,
	uint32_t             flags)
{
	int i;
	uint32_t tmp[4];

	tmp[0] = flags;
	if (!liarc_reader_get_uint32 (reader, tmp + 1) ||
	    !liarc_reader_get_uint32 (reader, tmp + 2) ||
	    !liarc_reader_get_float (reader, &self->particle_size) ||
	    !liarc_reader_get_text (reader, "", &self->texture) ||
	    !liarc_reader_get_uint32 (reader, tmp + 3))
		return 0;
	self->frame_start = tmp[0];
	self->frame_end_emit = tmp[1];
	self->frame_end = tmp[2];
	self->shader = lisys_string_dup ("particle");
	if (self->shader == NULL)
		return 0;

	/* Allocate particles. */
	self->particles.array = lisys_calloc (tmp[3], sizeof (LIMdlParticle));
	if (self->particles.array == NULL)
		return 0;
	self->particles.count = tmp[3];

	/* Read particles. */
	for (i = 0 ; i < self->particles.count ; i++)
	{
		if (!limdl_particle_read (self->particles.array + i, reader))
			return 0;
	}

	return 1;
}

static int private_read_v2 (
	LIMdlParticleSystem* self,
	LIArcReader*         reader,
	uint32_t             flags)
{
	int i;
	uint32_t tmp[4];

	if (!liarc_reader_get_uint32 (reader, tmp + 0) ||
	    !liarc_reader_get_uint32 (reader, tmp + 1) ||
	    !liarc_reader_get_uint32 (reader, tmp + 2) ||
	    !liarc_reader_get_float (reader, &self->particle_size) ||
	    !liarc_reader_get_text (reader, "", &self->shader) ||
	    !liarc_reader_get_text (reader, "", &self->texture) ||
	    !liarc_reader_get_uint32 (reader, tmp + 3))
		return 0;
	self->frame_start = tmp[0];
	self->frame_end_emit = tmp[1];
	self->frame_end = tmp[2];

	/* Allocate particles. */
	self->particles.array = lisys_calloc (tmp[3], sizeof (LIMdlParticle));
	if (self->particles.array == NULL)
		return 0;
	self->particles.count = tmp[3];

	/* Read particles. */
	for (i = 0 ; i < self->particles.count ; i++)
	{
		if (!limdl_particle_read (self->particles.array + i, reader))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
