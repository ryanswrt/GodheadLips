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

#ifndef __MODEL_LOD_H__
#define __MODEL_LOD_H__

#include "lipsofsuna/archive.h"
#include "lipsofsuna/system.h"
#include "model-faces.h"
#include "model-types.h"

typedef struct _LIMdlLod LIMdlLod;
struct _LIMdlLod
{
	struct
	{
		int count;
		LIMdlFaces* array;
	} face_groups;
	struct
	{
		int count;
		LIMdlIndex* array;
	} indices;
};

LIAPICALL (int, limdl_lod_init_copy, (
	LIMdlLod* self,
	LIMdlLod* lod));

LIAPICALL (void, limdl_lod_free, (
	LIMdlLod* self));

LIAPICALL (int, limdl_lod_read, (
	LIMdlLod*    self,
	LIArcReader* reader));

LIAPICALL (int, limdl_lod_read_old, (
	LIMdlLod*    self,
	LIArcReader* reader));

LIAPICALL (int, limdl_lod_write, (
	const LIMdlLod* self,
	LIArcWriter*    writer));

LIAPICALL (int, limdl_lod_get_memory, (
	const LIMdlLod* self));

#endif
