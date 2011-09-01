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

#ifndef __RENDER32_LOD_H__
#define __RENDER32_LOD_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"
#include "lipsofsuna/video.h"
#include "render-mesh.h"
#include "render-types.h"

typedef struct _LIRenLodGroup32 LIRenLodGroup32;
struct _LIRenLodGroup32
{
	int start;
	int count;
	LIMatVector center;
};

typedef struct _LIRenLod32 LIRenLod32;
struct _LIRenLod32
{
	LIRenMesh32 mesh;
	struct
	{
		int count;
		LIRenLodGroup32* array;
	} groups;
};

LIAPICALL (int, liren_lod32_init, (
	LIRenLod32* self,
	LIMdlModel* model,
	LIMdlLod*   lod));

LIAPICALL (void, liren_lod32_clear, (
	LIRenLod32* self));

#endif

