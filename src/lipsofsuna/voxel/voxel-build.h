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

#ifndef __VOXEL_BUILD_H__
#define __VOXEL_BUILD_H__

#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "voxel-manager.h"

typedef struct _LIVoxBuilder LIVoxBuilder;

LIAPICALL (LIVoxBuilder*, livox_builder_new, (
	LIVoxManager* manager,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize));

LIAPICALL (void, livox_builder_free, (
	LIVoxBuilder* self));

LIAPICALL (int, livox_builder_build_model, (
	LIVoxBuilder* self,
	LIMdlModel**  result));

LIAPICALL (void, livox_builder_preprocess, (
	LIVoxBuilder* self));

#endif
