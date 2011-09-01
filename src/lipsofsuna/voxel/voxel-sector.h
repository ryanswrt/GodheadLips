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

#ifndef __VOXEL_SECTOR_H__
#define __VOXEL_SECTOR_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "voxel.h"
#include "voxel-block.h"
#include "voxel-types.h"

LIAPICALL (LIVoxSector*, livox_sector_new, (
	LIAlgSector* sector));

LIAPICALL (void, livox_sector_free, (
	LIVoxSector* self));

LIAPICALL (int, livox_sector_build_block, (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z));

LIAPICALL (void, livox_sector_fill, (
	LIVoxSector* self,
	LIVoxVoxel*  terrain));

LIAPICALL (int, livox_sector_read_block, (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z,
	LIArcReader* reader));

LIAPICALL (void, livox_sector_update, (
	LIVoxSector* self,
	float        secs));

LIAPICALL (int, livox_sector_write_block, (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z,
	LIArcWriter* writer));

LIAPICALL (LIVoxBlock*, livox_sector_get_block, (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z));

LIAPICALL (void, livox_sector_get_bounds, (
	const LIVoxSector* self,
	LIMatAabb*         result));

LIAPICALL (int, livox_sector_get_dirty, (
	const LIVoxSector* self));

LIAPICALL (void, livox_sector_set_dirty, (
	LIVoxSector* self,
	int          value));

LIAPICALL (int, livox_sector_get_empty, (
	const LIVoxSector* self));

LIAPICALL (int, livox_sector_get_memory, (
	const LIVoxSector* self));

LIAPICALL (void, livox_sector_get_offset, (
	const LIVoxSector* self,
	int*               x,
	int*               y,
	int*               z));

LIAPICALL (void, livox_sector_get_origin, (
	const LIVoxSector* self,
	LIMatVector*       result));

LIAPICALL (LIVoxVoxel*, livox_sector_get_voxel, (
	LIVoxSector* sector,
	int          x,
	int          y,
	int          z));

LIAPICALL (int, livox_sector_set_voxel, (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z,
	LIVoxVoxel   terrain));

#endif
