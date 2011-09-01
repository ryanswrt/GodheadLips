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
 * \addtogroup LIMdlParticle Particle
 * @{
 */

#include "lipsofsuna/system.h"
#include "model-particle.h"

#define TIMESCALE 0.02f

void limdl_particle_clear (
	LIMdlParticle* self)
{
	lisys_free (self->frames.array);
	self->frames.array = NULL;
	self->frames.count = 0;
}

int limdl_particle_read (
	LIMdlParticle* self,
	LIArcReader*   reader)
{
	int i;
	uint32_t tmp[2];

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, tmp + 0) ||
	    !liarc_reader_get_uint32 (reader, tmp + 1))
		return 0;

	/* Allocate frames. */
	self->frames.array = lisys_calloc (tmp[1], sizeof (LIMdlParticleFrame));
	if (self->frames.array == NULL)
		return 0;
	self->frames.count = tmp[1];
	self->frame_start = tmp[0];
	self->frame_end = tmp[0] + tmp[1];

	/* Read frames. */
	for (i = 0 ; i < self->frames.count ; i++)
	{
		if (!limdl_particle_frame_read (self->frames.array + i, reader))
			return 0;
	}

	return 1;
}

int limdl_particle_write (
	const LIMdlParticle* self,
	LIArcWriter*         writer)
{
	int i;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->frame_start) ||
	    !liarc_writer_append_uint32 (writer, self->frames.count))
		return 0;

	/* Write frames. */
	for (i = 0 ; i < self->frames.count ; i++)
	{
		if (!limdl_particle_frame_write (self->frames.array + i, writer))
			return 0;
	}

	return 1;
}

int limdl_particle_get_state (
	LIMdlParticle* self,
	float          time,
	int            loop,
	LIMatVector*   position,
	float*         fade)
{
	int last;
	float frame;
	LIMdlParticleFrame* frame0;
	LIMdlParticleFrame* frame1;

	frame = time / TIMESCALE;
	frame -= self->frame_start;
	if (frame < 0.0)
		return 0;
	last = self->frames.count - 1;
	if (frame >= last)
	{
		if (!loop)
			return 0;
		frame -= (int)(frame / last) * last;
		lisys_assert (frame >= 0.0f);
		lisys_assert (frame < last);
	}

	frame0 = self->frames.array + (int) frame;
	frame1 = self->frames.array + (int) frame + 1;
	*position = limat_vector_lerp (frame1->position, frame0->position, frame - (int) frame);
	*fade = 1.0f - frame / last;
	return 1;
}

int limdl_particle_frame_read (
	LIMdlParticleFrame* self,
	LIArcReader*        reader)
{
	return liarc_reader_get_float (reader, &self->position.x) &&
	       liarc_reader_get_float (reader, &self->position.y) &&
	       liarc_reader_get_float (reader, &self->position.z);
}

int limdl_particle_frame_write (
	const LIMdlParticleFrame* self,
	LIArcWriter*              writer)
{
	return liarc_writer_append_float (writer, self->position.x) &&
	       liarc_writer_append_float (writer, self->position.y) &&
	       liarc_writer_append_float (writer, self->position.z);
}

/** @} */
/** @} */
