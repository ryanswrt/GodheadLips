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

#ifndef __MODEL_PARTICLE_H__
#define __MODEL_PARTICLE_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "model-types.h"

struct _LIMdlParticle
{
	int frame_start;
	int frame_end;
	struct
	{
		int count;
		LIMdlParticleFrame* array;
	} frames;
};

struct _LIMdlParticleFrame
{
	LIMatVector position;
};

struct _LIMdlParticleSystem
{
	int flags;
	int frame_start;
	int frame_end_emit;
	int frame_end;
	float particle_size;
	char* shader;
	char* texture;
	struct
	{
		int count;
		LIMdlParticle* array;
	} particles;
};

LIAPICALL (void, limdl_particle_clear, (
	LIMdlParticle* self));

LIAPICALL (int, limdl_particle_read, (
	LIMdlParticle* self,
	LIArcReader*   reader));

LIAPICALL (int, limdl_particle_write, (
	const LIMdlParticle* self,
	LIArcWriter*         writer));

LIAPICALL (int, limdl_particle_get_state, (
	LIMdlParticle* self,
	float          time,
	int            loop,
	LIMatVector*   position,
	float*         fade));

LIAPICALL (int, limdl_particle_frame_read, (
	LIMdlParticleFrame* self,
	LIArcReader*        reader));

LIAPICALL (int, limdl_particle_frame_write, (
	const LIMdlParticleFrame* self,
	LIArcWriter*              writer));

LIAPICALL (void, limdl_particle_system_clear, (
	LIMdlParticleSystem* self));

LIAPICALL (int, limdl_particle_system_read, (
	LIMdlParticleSystem* self,
	LIArcReader*         reader));

LIAPICALL (int, limdl_particle_system_write, (
	const LIMdlParticleSystem* self,
	LIArcWriter*               writer));

#endif
