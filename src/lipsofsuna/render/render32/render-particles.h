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

#ifndef __RENDER32_PARTICLES_H__
#define __RENDER32_PARTICLES_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "render-image.h"

typedef struct _LIRenParticle32 LIRenParticle32;
struct _LIRenParticle32
{
	int buffer_start;
	int buffer_end;
	int frame_start;
	int frame_end;
};

typedef struct _LIRenParticleFrame32 LIRenParticleFrame32;
struct _LIRenParticleFrame32
{
	LIMatVector coord;
};

typedef struct _LIRenParticleSystem32 LIRenParticleSystem32;
struct _LIRenParticleSystem32
{
	int frame_start;
	int frame_end;
	int frame_end_emit;
	int buffer_start;
	int buffer_end;
	int particle_start;
	int particle_end;
	float particle_size;
	char* shader;
	LIRenImage32* image;
};

typedef struct _LIRenParticles32 LIRenParticles32;
struct _LIRenParticles32
{
	LIMatAabb bounds;
	struct
	{
		int count;
		LIRenParticleFrame32* array;
	} frames;
	struct
	{
		int count;
		LIRenParticle32* array;
	} particles;
	struct
	{
		int count;
		LIRenParticleSystem32* array;
	} systems;
};

LIAPICALL (int, liren_particles32_init, (
	LIRenParticles32* self,
	LIRenRender32*    render,
	LIMdlModel*     model));

LIAPICALL (void, liren_particles32_clear, (
	LIRenParticles32* self));

LIAPICALL (int, liren_particles32_evaluate_particle, (
	LIRenParticles32* self,
	int             system,
	int             particle,
	float           time,
	int             loop,
	LIMatVector*    position,
	float*          color));

LIAPICALL (void, liren_particles32_sort, (
	LIRenParticles32*       self,
	float                 time,
	int                   loop,
	const LIMatTransform* transform,
	LIRenSort32*            sort));

#endif
