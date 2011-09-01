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
 * \addtogroup LIRenParticles32 Particles32
 * @{
 */

#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "render.h"
#include "render-private.h"
#include "render-particles.h"
#include "render-sort.h"
#include "../render.h"
#include "../render-private.h"

#define TIMESCALE 0.02f

static int private_evaluate_frame (
	LIRenParticles32* self,
	int               system,
	int               particle,
	int               frame,
	int               loop);

/*****************************************************************************/

int liren_particles32_init (
	LIRenParticles32* self,
	LIRenRender32*    render,
	LIMdlModel*       model)
{
	int i;
	int j;
	int k;
	int l;
	LIMdlParticle* srcparticle;
	LIMdlParticleFrame* srcframe;
	LIMdlParticleSystem* srcsystem;
	LIRenParticle32* dstparticle;
	LIRenParticleFrame32* dstframe;
	LIRenParticleSystem32* dstsystem;

	memset (self, 0, sizeof (LIRenParticles32));

	/* Create particle systems and count particles. */
	if (model->particle_systems.count)
	{
		/* Allocate particle system information. */
		self->systems.count = model->particle_systems.count;
		self->systems.array = lisys_calloc (self->systems.count, sizeof (LIRenParticleSystem32));
		if (self->systems.array == NULL)
		{
			liren_particles32_clear (self);
			return 0;
		}

		/* Copy particle system information. */
		for (i = 0 ; i < model->particle_systems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particle_systems.array + i;
			dstsystem->frame_start = srcsystem->frame_start;
			dstsystem->frame_end = srcsystem->frame_end;
			dstsystem->frame_end_emit = srcsystem->frame_end_emit;
			dstsystem->particle_start = self->particles.count;
			dstsystem->particle_size = srcsystem->particle_size;
			dstsystem->shader = lisys_string_dup (srcsystem->shader);
			if (dstsystem->shader == NULL)
			{
				liren_particles32_clear (self);
				return 0;
			}
			dstsystem->image = liren_render32_find_image (render, srcsystem->texture);
			if (dstsystem->image == NULL)
				dstsystem->image = liren_render32_load_image (render, srcsystem->texture);
			self->particles.count += srcsystem->particles.count;
			dstsystem->particle_end = self->particles.count;
		}
	}

	/* Create particles and count frames. */
	if (self->particles.count)
	{
		/* Allocate particle information. */
		self->particles.array = lisys_calloc (self->particles.count, sizeof (LIRenParticle32));
		if (self->particles.array == NULL)
		{
			liren_particles32_clear (self);
			return 0;
		}

		/* Copy particle information. */
		for (l = i = 0 ; i < model->particle_systems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particle_systems.array + i;
			dstsystem->buffer_start = self->frames.count;
			for (j = 0 ; j < srcsystem->particles.count ; j++)
			{
				dstparticle = self->particles.array + l++;
				srcparticle = srcsystem->particles.array + j;
				dstparticle->frame_start = srcparticle->frame_start;
				dstparticle->frame_end = srcparticle->frame_end;
				dstparticle->buffer_start = self->frames.count;
				self->frames.count += srcparticle->frames.count;
				dstparticle->buffer_end = self->frames.count;
			}
			dstsystem->buffer_end = self->frames.count;
		}
	}

	/* Create frames. */
	if (self->frames.count)
	{
		/* Allocate particle information. */
		self->frames.array = lisys_calloc (self->frames.count, sizeof (LIRenParticleFrame32));
		if (self->frames.array == NULL)
		{
			liren_particles32_clear (self);
			return 0;
		}

		/* Copy particle frame information. */
		for (l = i = 0 ; i < model->particle_systems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particle_systems.array + i;
			for (j = 0 ; j < srcsystem->particles.count ; j++)
			{
				srcparticle = srcsystem->particles.array + j;
				for (k = 0 ; k < srcparticle->frames.count ; k++)
				{
					srcframe = srcparticle->frames.array + k;
					dstframe = self->frames.array + l++;
					dstframe->coord = srcframe->position;
				}
			}
		}
	}

	/* Calculate bounding box. */
	if (self->frames.count)
	{
		self->bounds.min = self->frames.array[0].coord;
		self->bounds.max = self->frames.array[0].coord;
		for (i = 0 ; i < self->frames.count ; i++)
		{
			dstframe = self->frames.array + i;
			if (self->bounds.min.x < dstframe->coord.x)
				self->bounds.min.x = dstframe->coord.x;
			if (self->bounds.min.y < dstframe->coord.y)
				self->bounds.min.y = dstframe->coord.y;
			if (self->bounds.min.z < dstframe->coord.z)
				self->bounds.min.z = dstframe->coord.z;
			if (self->bounds.max.x < dstframe->coord.x)
				self->bounds.max.x = dstframe->coord.x;
			if (self->bounds.max.y < dstframe->coord.y)
				self->bounds.max.y = dstframe->coord.y;
			if (self->bounds.max.z < dstframe->coord.z)
				self->bounds.max.z = dstframe->coord.z;
		}
	}

	return 1;
}

void liren_particles32_clear (
	LIRenParticles32* self)
{
	int i;

	for (i = 0 ; i < self->systems.count ; i++)
		lisys_free (self->systems.array[i].shader);
	lisys_free (self->particles.array);
	lisys_free (self->frames.array);
	lisys_free (self->systems.array);
	memset (self, 0, sizeof (LIRenParticles32));
}

int liren_particles32_evaluate_particle (
	LIRenParticles32* self,
	int               system,
	int               particle,
	float             time,
	int               loop,
	LIMatVector*      position,
	float*            color)
{
	int index0;
	int index1;
	float frame;
	LIRenParticle32* p;
	LIRenParticleFrame32* frame0;
	LIRenParticleFrame32* frame1;
	LIRenParticleSystem32* s;

	/* Calculate the offset into the whole particle animation. */
	s = self->systems.array + system;
	p = self->particles.array + particle;
	lisys_assert (p->frame_start >= 0.0f);
	lisys_assert (p->frame_start < self->frames.count);
	lisys_assert (p->frame_end <= self->frames.count);
	frame = time / TIMESCALE;
	if (loop && frame > s->frame_end_emit)
		frame = fmodf (frame, s->frame_end_emit);

	/* Get the nearest frames. */
	index0 = private_evaluate_frame (self, system, particle, (int) frame, loop);
	if (index0 == -1)
		return 0;
	index1 = private_evaluate_frame (self, system, particle, (int) frame + 1, loop);
	if (index1 == -1)
		return 0;
	frame0 = self->frames.array + p->buffer_start + (index0 - p->frame_start);
	frame1 = self->frames.array + p->buffer_start + (index1 - p->frame_start);

	/* Interpolate between the frames. */
	*position = limat_vector_lerp (frame1->coord, frame0->coord, frame - (int) frame);

	/* Calculate the diffuse color. */
	if (frame < p->frame_start)
	{
		frame += s->frame_end_emit;
		lisys_assert (frame >= 0.0f);
	}
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f - (frame - p->frame_start) / (p->frame_end - p->frame_start);

	return 1;
}

void liren_particles32_sort (
	LIRenParticles32*     self,
	float                 time,
	int                   loop,
	const LIMatTransform* transform,
	LIRenSort32*          sort)
{
	int i;
	int j;
	float color[4];
	LIMatVector coord;
	LIRenParticleSystem32* system;
	LIRenShader32* shader;

	for (i = 0 ; i < self->systems.count ; i++)
	{
		system = self->systems.array + i;
		if (system->image == NULL)
			continue;
		shader = liren_render32_find_shader (sort->render, system->shader);
		if (shader == NULL)
			continue;
		for (j = system->particle_start ; j < system->particle_end ; j++)
		{
			if (liren_particles32_evaluate_particle (self, i, j, time, loop, &coord, color))
			{
				coord = limat_transform_transform (*transform, coord);
				liren_sort32_add_particle (sort, &coord, system->particle_size, color, system->image, shader);
			}
		}
	}
}

/*****************************************************************************/

static int private_evaluate_frame (
	LIRenParticles32* self,
	int               system,
	int               particle,
	int               frame,
	int               loop)
{
	LIRenParticle32* p;
	LIRenParticleSystem32* s;

	/* Get the particle. */
	s = self->systems.array + system;
	p = self->particles.array + particle;

	/* Handle looping. */
	/* If looping is enabled and the frame range of the particle extends
	   beyond the wrap position of the animation, the frames outside of
	   the animation wrap and continue from the beginning. */
	if (loop && frame < p->frame_start)
	{
		if (p->frame_end <= s->frame_end_emit)
			return -1;
		frame += s->frame_end_emit;
	}

	/* Check that the frame exists. */
	if (frame < p->frame_start || frame >= p->frame_end)
		return -1;

	return frame;
}

/** @} */
/** @} */
