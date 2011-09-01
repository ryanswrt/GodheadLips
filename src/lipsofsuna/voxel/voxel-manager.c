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
 * \addtogroup LIVox Voxel
 * @{
 * \addtogroup LIVoxManager Manager
 * @{
 */

#include <lipsofsuna/algorithm.h>
#include "voxel-iterator.h"
#include "voxel-manager.h"
#include "voxel-material.h"
#include "voxel-private.h"

#define VOXEL_BORDER_TOLERANCE 0.05f

static void private_clear_materials (
	LIVoxManager* self);

static void private_mark_block (
	LIVoxManager* self,
	LIVoxSector*  sector,
	int           x,
	int           y,
	int           z);

static void private_configure (
	LIVoxManager* self,
	int           blocks_per_line,
	int           tiles_per_line);

/*****************************************************************************/

/**
 * \brief Creates a new voxel manager.
 * \param callbacks Callback manager.
 * \param sectors Sector manager.
 * \return Voxel manager.
 */
LIVoxManager* livox_manager_new (
	LICalCallbacks* callbacks,
	LIAlgSectors*   sectors)
{
	LIVoxManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxManager));
	if (self == NULL)
		return NULL;
	self->callbacks = callbacks;
	self->sectors = sectors;
	private_configure (self, 4, 16);

	/* Allocate materials. */
	self->materials = lialg_u32dic_new ();
	if (self->materials == NULL)
	{
		livox_manager_free (self);
		return NULL;
	}

	/* Allocate sector data. */
	if (!lialg_sectors_insert_content (self->sectors, LIALG_SECTORS_CONTENT_VOXEL, self,
	     (LIAlgSectorFreeFunc) livox_sector_free,
	     (LIAlgSectorLoadFunc) livox_sector_new))
	{
		livox_manager_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the voxel manager.
 * \param self Voxel manager.
 */
void livox_manager_free (
	LIVoxManager* self)
{
	/* Free materials. */
	if (self->materials != NULL)
	{
		private_clear_materials (self);
		lialg_u32dic_free (self->materials);
	}

	/* Free sector data. */
	if (self->sectors != NULL)
		lialg_sectors_remove_content (self->sectors, LIALG_SECTORS_CONTENT_VOXEL);

	lisys_free (self);
}

/**
 * \brief Checks if the voxel is an occluder.
 * \param self Voxel manager.
 * \param voxel Voxel.
 * \return Nonzero if occluder.
 */
int livox_manager_check_occluder (
	const LIVoxManager* self,
	const LIVoxVoxel*   voxel)
{
	LIVoxMaterial* material;

	if (!voxel->type)
		return 0;
	material = livox_manager_find_material ((LIVoxManager*) self, voxel->type);
	if (material == NULL)
		return 0;

	if (!(material->flags & LIVOX_MATERIAL_FLAG_OCCLUDER))
		return 0;

	return 1;
}

/**
 * \brief Removes all the materials.
 * \param self Voxel manager.
 */
void livox_manager_clear_materials (
	LIVoxManager* self)
{
	private_clear_materials (self);
}

/**
 * \brief Reconfigures the block and tile counts of the map.
 * \param self Voxel manager.
 * \param blocks_per_line Number of blocks per sector edge.
 * \param tiles_per_line Number of tiles per sector edge.
 * \return Nonzero on success.
 */
int livox_manager_configure (
	LIVoxManager* self,
	int           blocks_per_line,
	int           tiles_per_line)
{
	if (self->sectors != NULL && self->sectors->sectors->size)
	{
		lisys_error_set (EINVAL, "cannot change grid settings when the map is populated");
		return 0;
	}
	private_configure (self, blocks_per_line, tiles_per_line);

	return 1;
}

/**
 * \brief Copies a box of voxels from the currently loaded scene.
 * \param self Voxel manager.
 * \param xstart Start voxel in voxels in world space.
 * \param ystart Start voxel in voxels in world space.
 * \param zstart Start voxel in voxels in world space.
 * \param xsize Number of voxels to copy.
 * \param ysize Number of voxels to copy.
 * \param zsize Number of voxels to copy.
 * \param result Buffer with room for xsize*ysize*zsize voxels.
 */
void livox_manager_copy_voxels (
	LIVoxManager* self,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize,
	LIVoxVoxel*   result)
{
	int i;
	int min[3];
	int max[3];
	int off[3];
	int sec[3];
	int src[3];
	int dst[3];
	LIVoxSector* sector;

	/* Initialize the buffer. */
	memset (result, 0, xsize * ysize * zsize * sizeof (LIVoxVoxel));

	/* Determine affected sectors. */
	min[0] = xstart / self->tiles_per_line;
	min[1] = ystart / self->tiles_per_line;
	min[2] = zstart / self->tiles_per_line;
	max[0] = (xstart + xsize - 1) / self->tiles_per_line;
	max[1] = (ystart + ysize - 1) / self->tiles_per_line;
	max[2] = (zstart + zsize - 1) / self->tiles_per_line;

	/* Loop through affected sectors. */
	for (sec[2] = min[2] ; sec[2] <= max[2] ; sec[2]++)
	for (sec[1] = min[1] ; sec[1] <= max[1] ; sec[1]++)
	for (sec[0] = min[0] ; sec[0] <= max[0] ; sec[0]++)
	{
		/* Find the sector. */
		sector = lialg_sectors_data_offset (self->sectors, LIALG_SECTORS_CONTENT_VOXEL, sec[0], sec[1], sec[2], 0);
		if (sector == NULL)
			continue;

		/* Calculate copy offset. */
		off[0] = xstart - sec[0] * self->tiles_per_line;
		off[1] = ystart - sec[1] * self->tiles_per_line;
		off[2] = zstart - sec[2] * self->tiles_per_line;

		/* Copy sector voxels to the selection. */
		for (dst[2] = 0, src[2] = off[2], i = 0 ; dst[2] < zsize ; dst[2]++, src[2]++)
		for (dst[1] = 0, src[1] = off[1] ; dst[1] < ysize ; dst[1]++, src[1]++)
		for (dst[0] = 0, src[0] = off[0] ; dst[0] < xsize ; dst[0]++, src[0]++, i++)
		{
			if (0 <= src[0] && src[0] < self->tiles_per_line &&
			    0 <= src[1] && src[1] < self->tiles_per_line &&
			    0 <= src[2] && src[2] < self->tiles_per_line)
				result[i] = *livox_sector_get_voxel (sector, src[0], src[1], src[2]);
		}
	}
}

/**
 * \brief Finds a material by ID.
 * \param self Voxel manager.
 * \param id Material ID.
 * \return Material or NULL.
 */
LIVoxMaterial* livox_manager_find_material (
	LIVoxManager* self,
	uint32_t      id)
{
	return lialg_u32dic_find (self->materials, id);
}

/**
 * \brief Finds the nearest voxel to the given point.
 * \param self Voxel manager.
 * \param flags Search flags.
 * \param point Point in world space.
 * \param radius Search radius in world units.
 * \param index Return location for voxel position or NULL.
 * \return Voxel or NULL.
 */
LIVoxVoxel* livox_manager_find_voxel (
	LIVoxManager*      self,
	int                flags,
	const LIMatVector* point,
	float              radius,
	int*               index)
{
	float d;
	LIAlgRange range;
	LIMatVector tmp;
	LIMatVector diff;
	LIMatVector origin;
	LIVoxVoxel voxel;
	LIVoxVoxelIter iter;
	struct
	{
		int x;
		int y;
		int z;
		float dist;
		LIVoxSector* sector;
	}
	best = { 0, 0, 0, 10.0E10f, NULL };

	/* Enforce minimum search radius. */
	radius = LIMAT_MAX (self->tile_width, radius);

	/* Loop through affected sectors. */
	range = lialg_range_new_from_sphere (point, radius, self->tile_width);
	LIVOX_VOXEL_FOREACH (iter, self, range, 1)
	{
		/* Check that the tile matches the criteria. */
		livox_sector_get_origin (iter.sector, &origin);
		voxel = *livox_sector_get_voxel (iter.sector, iter.voxel[0], iter.voxel[1], iter.voxel[2]);
		if (!(!voxel.type && (flags & LIVOX_FIND_EMPTY)) &&
		    !( voxel.type && (flags & LIVOX_FIND_FULL)))
			continue;

		/* Check that the tile is closer than the best match so far. */
		tmp = limat_vector_init (
			origin.x + self->tile_width * (iter.voxel[0] + 0.5f),
			origin.y + self->tile_width * (iter.voxel[1] + 0.5f),
			origin.z + self->tile_width * (iter.voxel[2] + 0.5f));
		diff = limat_vector_subtract (*point, tmp);
		d = limat_vector_dot (diff, diff);
		if (best.sector != NULL && d >= best.dist)
			continue;

		/* Set the new best match. */
		best.x = iter.voxel[0];
		best.y = iter.voxel[1];
		best.z = iter.voxel[2];
		best.dist = d;
		best.sector = iter.sector;
	}

	/* Return the tile coordinates. */
	if (best.sector == NULL)
		return NULL;
	if (index != NULL)
	{
		index[0] = best.x + best.sector->sector->x * self->tiles_per_line;
		index[1] = best.y + best.sector->sector->y * self->tiles_per_line;
		index[2] = best.z + best.sector->sector->z * self->tiles_per_line;
	}

	/* Return the tile contents. */
	return livox_sector_get_voxel (best.sector, best.x, best.y, best.z);
}

/**
 * \brief Inserts a material to the material database.
 *
 * Any existing material with a conflicting ID is removed and freed
 * before attemtpting to insert the new material.
 *
 * The ownership of the material is transfered to the material manager
 * upon success.
 *
 * \param self Voxel manager.
 * \param material Material.
 * \return Nonzeron on success.
 */
int livox_manager_insert_material (
	LIVoxManager*  self,
	LIVoxMaterial* material)
{
	LIVoxMaterial* tmp;

	tmp = lialg_u32dic_find (self->materials, material->id);
	if (tmp != NULL)
	{
		lialg_u32dic_remove (self->materials, material->id);
		livox_material_free (tmp);
	}
	if (!lialg_u32dic_insert (self->materials, material->id, material))
		return 0;

	return 1;
}

/**
 * \brief Finds the intersection with a ray and a tile.
 * \param self Voxel manager.
 * \param ray0 Ray start, in tiles.
 * \param ray1 Ray end, in tiles.
 * \param result_point Return location for the intersection point, in tiles.
 * \param result_tile Return location for the intersecting tile.
 * \return Nonzero if intersected.
 */
int livox_manager_intersect_ray (
	LIVoxManager*      self,
	const LIMatVector* ray0,
	const LIMatVector* ray1,
	LIMatVector*       result_point,
	LIMatVector*       result_tile)
{
#define STEP_SIZE 0.05
	int p[3];
	int max;
	float i;
	float len;
	LIMatVector dir;
	LIMatVector pos;
	LIVoxVoxel voxel;
	LIVoxMaterial* material;

	max = self->tiles_per_line * self->sectors->count;

	/* Calculate step size. */
	dir = limat_vector_subtract (*ray1, *ray0);
	len = limat_vector_get_length (dir);
	dir = limat_vector_normalize (dir);

	/* Walk through tiles in the path. */
	/* This can miss tiles in some corner cases but it's good enough for
	   normal use cases. */
	for (i = 0.0f ; i <= len + 0.5f * STEP_SIZE ; i += STEP_SIZE)
	{
		pos = limat_vector_add (*ray0, limat_vector_multiply (dir, i));
		p[0] = (int) pos.x;
		p[1] = (int) pos.y;
		p[2] = (int) pos.z;
		if (p[0] < 0 || p[1] < 0 || p[2] < 0 || p[0] >= max || p[1] >= max || p[2] >= max)
			continue;
		livox_manager_get_voxel (self, p[0], p[1], p[2], &voxel);
		if (!voxel.type)
			continue;
		material = livox_manager_find_material (self, voxel.type);
		if (material == NULL || material->type == LIVOX_MATERIAL_TYPE_LIQUID)
			continue;
		*result_point = pos;
		result_tile->x = (int) pos.x;
		result_tile->y = (int) pos.y;
		result_tile->z = (int) pos.z;
		return 1;
	}

	return 0;
#undef STEP_SIZE
}

/**
 * \brief Marks neighbors of dirty sectors for update.
 * \param self Voxel manager.
 */
void livox_manager_mark_updates (
	LIVoxManager* self)
{
	int i;
	int j;
	int x;
	int y;
	int z;
	LIAlgSectorsIter iter;
	LIVoxSector* sector;
	struct
	{
		int x;
		int y;
		int z;
		int mask;
	}
	neighbors[26] =
	{
		{ -1, -1, -1, 0x01|0x04|0x10 },
		{  0, -1, -1, 0x00|0x04|0x10 },
		{  1, -1, -1, 0x02|0x04|0x10 },
		{ -1,  0, -1, 0x01|0x00|0x10 },
		{  0,  0, -1, 0x00|0x00|0x10 },
		{  1,  0, -1, 0x02|0x00|0x10 },
		{ -1,  1, -1, 0x01|0x08|0x10 },
		{  0,  1, -1, 0x00|0x08|0x10 },
		{  1,  1, -1, 0x02|0x08|0x10 },
		{ -1, -1,  0, 0x01|0x04|0x00 },
		{  0, -1,  0, 0x00|0x04|0x00 },
		{  1, -1,  0, 0x02|0x04|0x00 },
		{ -1,  0,  0, 0x01|0x00|0x00 },
		{  1,  0,  0, 0x02|0x00|0x00 },
		{ -1,  1,  0, 0x01|0x08|0x00 },
		{  0,  1,  0, 0x00|0x08|0x00 },
		{  1,  1,  0, 0x02|0x08|0x00 },
		{ -1, -1,  1, 0x01|0x04|0x20 },
		{  0, -1,  1, 0x00|0x04|0x20 },
		{  1, -1,  1, 0x02|0x04|0x20 },
		{ -1,  0,  1, 0x01|0x00|0x20 },
		{  0,  0,  1, 0x00|0x00|0x20 },
		{  1,  0,  1, 0x02|0x00|0x20 },
		{ -1,  1,  1, 0x01|0x08|0x20 },
		{  0,  1,  1, 0x00|0x08|0x20 },
		{  1,  1,  1, 0x02|0x08|0x20 },
	};

	/* Update block boundaries. */
	LIALG_SECTORS_FOREACH (iter, self->sectors)
	{
		sector = iter.sector->content[LIALG_SECTORS_CONTENT_VOXEL];
		if (sector == NULL || !sector->dirty)
			continue;
		for (i = z = 0 ; z < self->blocks_per_line ; z++)
		for (y = 0 ; y < self->blocks_per_line ; y++)
		for (x = 0 ; x < self->blocks_per_line ; x++, i++)
		{
			if (!(sector->blocks[i].dirty & LIVOX_DIRTY_EXPLICIT))
				continue;
			for (j = 0 ; j < 26 ; j++)
			{
				if ((sector->blocks[i].dirty & neighbors[j].mask) != neighbors[j].mask)
					continue;
				private_mark_block (self, sector,
					neighbors[j].x + x,
					neighbors[j].y + y,
					neighbors[j].z + z);
			}
		}
	}
}

/**
 * \brief Pastes a box of voxels from the scene.
 * \param self Voxel manager.
 * \param xstart Start voxel in voxels in world space.
 * \param ystart Start voxel in voxels in world space.
 * \param zstart Start voxel in voxels in world space.
 * \param xsize Number of voxels to paste.
 * \param ysize Number of voxels to paste.
 * \param zsize Number of voxels to paste.
 * \param voxels Buffer containing xsize*ysize*zsize voxels.
 * \return Nonzero on success.
 */
int livox_manager_paste_voxels (
	LIVoxManager* self,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize,
	LIVoxVoxel*   voxels)
{
	int i;
	int min[3];
	int max[3];
	int off[3];
	int sec[3];
	int src[3];
	int dst[3];
	LIVoxSector* sector;

	/* Determine affected sectors. */
	min[0] = xstart / self->tiles_per_line;
	min[1] = ystart / self->tiles_per_line;
	min[2] = zstart / self->tiles_per_line;
	max[0] = (xstart + xsize - 1) / self->tiles_per_line;
	max[1] = (ystart + ysize - 1) / self->tiles_per_line;
	max[2] = (zstart + zsize - 1) / self->tiles_per_line;

	/* Loop through affected sectors. */
	for (sec[2] = min[2] ; sec[2] <= max[2] ; sec[2]++)
	for (sec[1] = min[1] ; sec[1] <= max[1] ; sec[1]++)
	for (sec[0] = min[0] ; sec[0] <= max[0] ; sec[0]++)
	{
		/* Find or create sector. */
		sector = lialg_sectors_data_offset (self->sectors, LIALG_SECTORS_CONTENT_VOXEL, sec[0], sec[1], sec[2], 1);
		if (sector == NULL)
			return 0;

		/* Calculate paint offset. */
		off[0] = xstart - sec[0] * self->tiles_per_line;
		off[1] = ystart - sec[1] * self->tiles_per_line;
		off[2] = zstart - sec[2] * self->tiles_per_line;

		/* Copy brush voxels to sector. */
		for (src[2] = 0, dst[2] = off[2], i = 0 ; src[2] < zsize ; src[2]++, dst[2]++)
		for (src[1] = 0, dst[1] = off[1] ; src[1] < ysize ; src[1]++, dst[1]++)
		for (src[0] = 0, dst[0] = off[0] ; src[0] < xsize ; src[0]++, dst[0]++, i++)
		{
			if (0 <= dst[0] && dst[0] < self->tiles_per_line &&
				0 <= dst[1] && dst[1] < self->tiles_per_line &&
				0 <= dst[2] && dst[2] < self->tiles_per_line)
				livox_sector_set_voxel (sector, dst[0], dst[1], dst[2], voxels[i]);
		}
	}

	return 1;
}

/**
 * \brief Removes a material.
 * \param self Voxel manager.
 * \param id Material ID to remove.
 */
void livox_manager_remove_material (
	LIVoxManager* self,
	int           id)
{
	LIVoxMaterial* material;

	material = lialg_u32dic_find (self->materials, id);
	if (material != NULL)
	{
		lialg_u32dic_remove (self->materials, id);
		livox_material_free (material);
	}
}

/**
 * \brief Updates dirty sectors and their neighbors.
 * \param self Voxel manager.
 * \param secs Seconds since the last update.
 */
void livox_manager_update (
	LIVoxManager* self,
	float         secs)
{
	livox_manager_mark_updates (self);
	livox_manager_update_marked (self);
}

/**
 * \brief Updates marked dirty sectors.
 * \param self Voxel manager.
 */
void livox_manager_update_marked (
	LIVoxManager* self)
{
	int i;
	int x;
	int y;
	int z;
	LIAlgSectorsIter iter;
	LIVoxSector* sector;

	/* Rebuild modified terrain. */
	LIALG_SECTORS_FOREACH (iter, self->sectors)
	{
		sector = iter.sector->content[LIALG_SECTORS_CONTENT_VOXEL];
		if (sector == NULL || !sector->dirty)
			continue;
		for (i = z = 0 ; z < self->blocks_per_line ; z++)
		for (y = 0 ; y < self->blocks_per_line ; y++)
		for (x = 0 ; x < self->blocks_per_line ; x++, i++)
		{
			if (!sector->blocks[i].dirty)
				continue;
			livox_sector_build_block (sector, x, y, z);
			sector->blocks[i].dirty = 0;
		}
		sector->dirty = 0;
	}
}

/**
 * \brief Sets the default fill tile type for empty sectors.
 * \param self Voxel manager.
 * \param type Terrain type, zero for empty.
 */
void livox_manager_set_fill (
	LIVoxManager* self,
	int           type)
{
	self->fill = type;
}

/**
 * \brief Gets the approximate memory used by the voxel manager.
 * \param self Voxel manager.
 * \return Memory used in bytes.
 */
int livox_manager_get_memory (
	const LIVoxManager* self)
{
	int total;
	LIAlgSectorsIter iter;
	LIVoxSector* sector;

	total = sizeof (LIVoxManager);
	LIALG_SECTORS_FOREACH (iter, self->sectors)
	{
		sector = iter.sector->content[LIALG_SECTORS_CONTENT_VOXEL];
		if (sector != NULL)
			total += livox_sector_get_memory (sector);
	}

	return total;
}

/**
 * \brief Gets a voxel by position.
 * \param self Voxel manager.
 * \param x Tile position.
 * \param y Tile position.
 * \param z Tile position.
 * \param value Return location for the voxel.
 */
void livox_manager_get_voxel (
	LIVoxManager* self,
	int           x,
	int           y,
	int           z,
	LIVoxVoxel*   value)
{
	int sx;
	int sy;
	int sz;
	LIVoxSector* sector;

	sx = x / self->tiles_per_line;
	sy = y / self->tiles_per_line;
	sz = z / self->tiles_per_line;
	sector = lialg_sectors_data_offset (self->sectors, LIALG_SECTORS_CONTENT_VOXEL, sx, sy, sz, 1);
	if (sector == NULL)
	{
		livox_voxel_init (value, 0);
		return;
	}
	sx = x % self->tiles_per_line;
	sy = y % self->tiles_per_line;
	sz = z % self->tiles_per_line;
	*value = *livox_sector_get_voxel (sector, sx, sy, sz);
}

/**
 * \brief Sets a voxel by position.
 * \param self Voxel manager.
 * \param x Tile position.
 * \param y Tile position.
 * \param z Tile position.
 * \param value Voxel.
 */
int livox_manager_set_voxel (
	LIVoxManager*     self,
	int               x,
	int               y,
	int               z,
	const LIVoxVoxel* value)
{
	int sx;
	int sy;
	int sz;
	LIVoxSector* sector;

	sx = x / self->tiles_per_line;
	sy = y / self->tiles_per_line;
	sz = z / self->tiles_per_line;
	sector = lialg_sectors_data_offset (self->sectors, LIALG_SECTORS_CONTENT_VOXEL, sx, sy, sz, 1);
	if (sector == NULL)
		return 0;
	sx = x % self->tiles_per_line;
	sy = y % self->tiles_per_line;
	sz = z % self->tiles_per_line;

	return livox_sector_set_voxel (sector, sx, sy, sz, *value);
}

/*****************************************************************************/

static void private_clear_materials (
	LIVoxManager* self)
{
	LIAlgU32dicIter iter;
	LIVoxMaterial* material;

	LIALG_U32DIC_FOREACH (iter, self->materials)
	{
		material = iter.value;
		livox_material_free (material);
	}
	lialg_u32dic_clear (self->materials);
}

static void private_configure (
	LIVoxManager* self,
	int           blocks_per_line,
	int           tiles_per_line)
{
	self->blocks_per_line = blocks_per_line;
	self->blocks_per_sector = self->blocks_per_line * self->blocks_per_line * self->blocks_per_line;
	self->tiles_per_line = tiles_per_line;
	self->tiles_per_sector = self->tiles_per_line * self->tiles_per_line * self->tiles_per_line;
	self->tile_width = self->sectors->width / self->tiles_per_line;
}

static void private_mark_block (
	LIVoxManager* self,
	LIVoxSector*  sector,
	int           x,
	int           y,
	int           z)
{
	int sx;
	int sy;
	int sz;
	LIVoxBlock* block;
	LIVoxSector* sector1;

	/* Find affected sector. */
	sx = sector->sector->x;
	sy = sector->sector->y;
	sz = sector->sector->z;
	if (x < 0)
	{
		x = self->blocks_per_line - 1;
		sx--;
	}
	else if (x >= self->blocks_per_line)
	{
		x = 0;
		sx++;
	}
	if (y < 0)
	{
		y = self->blocks_per_line - 1;
		sy--;
	}
	else if (y >= self->blocks_per_line)
	{
		y = 0;
		sy++;
	}
	if (z < 0)
	{
		z = self->blocks_per_line - 1;
		sz--;
	}
	else if (z >= self->blocks_per_line)
	{
		z = 0;
		sz++;
	}

	/* Mark block as dirty. */
	sector1 = lialg_sectors_data_offset (self->sectors, LIALG_SECTORS_CONTENT_VOXEL, sx, sy, sz, 0);
	if (sector1 == NULL)
		return;
	block = livox_sector_get_block (sector1, x, y, z);
	block->dirty |= LIVOX_DIRTY_PROPAGATED;
	sector1->dirty = 1;
}

/** @} */
/** @} */
