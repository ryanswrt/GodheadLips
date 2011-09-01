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

#ifndef __VOXEL_MANAGER_H__
#define __VOXEL_MANAGER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/system.h>
#include "voxel-sector.h"
#include "voxel-types.h"

/*****************************************************************************/

enum
{
	LIVOX_FIND_EMPTY = 0x01,
	LIVOX_FIND_FULL  = 0x02,
	LIVOX_FIND_ALL   = 0xFF
};

typedef struct _LIVoxUpdateEvent LIVoxUpdateEvent;
struct _LIVoxUpdateEvent
{
	int sector[3];
	int block[3];
};

struct _LIVoxManager
{
	int fill;
	int blocks_per_line;
	int blocks_per_sector;
	int tiles_per_line;
	int tiles_per_sector;
	float tile_width;
	LIAlgSectors* sectors;
	LIAlgU32dic* materials;
	LICalCallbacks* callbacks;
};

LIAPICALL (LIVoxManager*, livox_manager_new, (
	LICalCallbacks* callbacks,
	LIAlgSectors*   sectors));

LIAPICALL (void, livox_manager_free, (
	LIVoxManager* self));

LIAPICALL (int, livox_manager_check_occluder, (
	const LIVoxManager* self,
	const LIVoxVoxel*   voxel));

LIAPICALL (void, livox_manager_clear_materials, (
	LIVoxManager* self));

LIAPICALL (int, livox_manager_configure, (
	LIVoxManager* self,
	int           blocks_per_line,
	int           tiles_per_line));

LIAPICALL (void, livox_manager_copy_voxels, (
	LIVoxManager* self,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize,
	LIVoxVoxel*   result));

LIAPICALL (LIVoxMaterial*, livox_manager_find_material, (
	LIVoxManager* self,
	uint32_t      id));

LIAPICALL (LIVoxVoxel*, livox_manager_find_voxel, (
	LIVoxManager*      self,
	int                flags,
	const LIMatVector* point,
	float              radius,
	int*               index));

LIAPICALL (int, livox_manager_insert_material, (
	LIVoxManager*  self,
	LIVoxMaterial* material));

LIAPICALL (int, livox_manager_intersect_ray, (
	LIVoxManager*      self,
	const LIMatVector* ray0,
	const LIMatVector* ray1,
	LIMatVector*       result_point,
	LIMatVector*       result_tile));

LIAPICALL (void, livox_manager_mark_updates, (
	LIVoxManager* self));

LIAPICALL (int, livox_manager_paste_voxels, (
	LIVoxManager* self,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize,
	LIVoxVoxel*   voxels));

LIAPICALL (void, livox_manager_remove_material, (
	LIVoxManager* self,
	int           id));

LIAPICALL (int, livox_manager_solve_occlusion, (
	LIVoxManager* self,
	int           xsize,
	int           ysize,
	int           zsize,
	LIVoxVoxel*   voxels,
	int*          result));

LIAPICALL (void, livox_manager_update, (
	LIVoxManager* self,
	float         secs));

LIAPICALL (void, livox_manager_update_marked, (
	LIVoxManager* self));

LIAPICALL (void, livox_manager_set_fill, (
	LIVoxManager* self,
	int           fill));

LIAPICALL (void, livox_manager_set_load, (
	LIVoxManager* self,
	int           value));

LIAPICALL (int, livox_manager_get_memory, (
	const LIVoxManager* self));

LIAPICALL (void, livox_manager_get_voxel, (
	LIVoxManager* self,
	int           x,
	int           y,
	int           z,
	LIVoxVoxel*   value));

LIAPICALL (int, livox_manager_set_voxel, (
	LIVoxManager*     self,
	int               x,
	int               y,
	int               z,
	const LIVoxVoxel* value));

#endif
