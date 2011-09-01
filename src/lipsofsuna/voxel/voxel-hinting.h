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

#ifndef __VOXEL_HINTING_H__
#define __VOXEL_HINTING_H__

#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "voxel-manager.h"

LIAPICALL (LIVoxVoxel*, livox_hinting_process_area, (
	LIVoxManager* manager,
	int           px,
	int           py,
	int           pz,
	int           sx,
	int           sy,
	int           sz));

LIAPICALL (void, livox_hinting_process_voxel, (
	LIVoxManager*   manager,
	LIVoxVoxel*     voxels,
	LIVoxMaterial** materials,
	int             vx,
	int             vy,
	int             vz,
	int             sx,
	int             sy,
	int             sz));

#endif
