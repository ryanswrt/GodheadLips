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

#ifndef __VOXEL_ITERATOR_H__
#define __VOXEL_ITERATOR_H__

#include <lipsofsuna/algorithm.h>
#include "voxel.h"
#include "voxel-block.h"
#include "voxel-manager.h"
#include "voxel-sector.h"
#include "voxel-types.h"

typedef struct _LIVoxBlockIter LIVoxBlockIter;
struct _LIVoxBlockIter
{
	int blocks;
	LIAlgSectorsIter sectors;
	LIVoxBlock* block;
	LIVoxManager* voxels;
	int (*filter)(LIVoxBlock*);
};

#define LIVOX_FOREACH_BLOCK(iter, manager) \
	for (livox_block_iter_first (&iter, manager, NULL) ; \
	     iter.block != NULL ; \
	     livox_block_iter_next (&iter))

static inline int livox_block_iter_next (
	LIVoxBlockIter* self)
{
	int x;
	int y;
	int z;
	LIAlgSector* sector;
	LIVoxBlock* block;
	LIVoxSector* voxsec;

	/* Find next block. */
	for (sector = self->sectors.sector ; sector != NULL ; sector = self->sectors.sector)
	{
		voxsec = (LIVoxSector*) sector->content[LIALG_SECTORS_CONTENT_VOXEL];
		if (voxsec != NULL && self->blocks < self->voxels->blocks_per_sector)
		{
			while (++self->blocks < self->voxels->blocks_per_sector)
			{
				x = self->blocks % self->voxels->blocks_per_line;
				y = self->blocks / self->voxels->blocks_per_line % self->voxels->blocks_per_line;
				z = self->blocks / self->voxels->blocks_per_line / self->voxels->blocks_per_line;
				block = livox_sector_get_block (voxsec, x, y, z);
				if (self->filter == NULL || self->filter (block))
				{
					self->block = block;
					return 1;
				}
			}
		}
		lialg_sectors_iter_next (&self->sectors);
		self->blocks = 0;
	}

	/* No more sectors. */
	self->block = NULL;

	return 0;
}

static inline int livox_block_iter_first (
	LIVoxBlockIter* self,
	LIVoxManager*   manager,
	void*           filter)
{
	self->voxels = manager;
	self->filter = (int(*)(LIVoxBlock*)) filter;
	self->blocks = -1;

	/* Find first sector. */
	lialg_sectors_iter_first_all (&self->sectors, manager->sectors);
	if (self->sectors.sector == NULL)
	{
		self->block = NULL;
		return 0;
	}

	/* Find first block or object. */
	return livox_block_iter_next (self);
}

/*****************************************************************************/

typedef struct _LIVoxVoxelIter LIVoxVoxelIter;
struct _LIVoxVoxelIter
{
	int load;
	int voxel[3];
	LIVoxManager* voxels;
	LIVoxSector* sector;
	LIAlgRange tiles;
	LIAlgRange range0;
	LIAlgRange range1;
	LIAlgRangeIter rangei0;
	LIAlgRangeIter rangei1;
};

#define LIVOX_VOXEL_FOREACH(iter, manager, range, load) \
	for (livox_voxel_iter_first (&(iter), manager, &(range), load) ; \
	     iter.sector != NULL ; \
	     livox_voxel_iter_next (&iter))

static inline int livox_voxel_iter_first (
	LIVoxVoxelIter* self,
	LIVoxManager*   voxels,
	LIAlgRange*     tiles,
	int             load)
{
	int ret;
	int offset[3];

	self->voxels = voxels;
	self->load = load;
	self->tiles = *tiles;
	self->sector = NULL;

	/* Initialize sector range. */
	self->range0 = lialg_range_new (
		tiles->minx / self->voxels->tiles_per_line,
		tiles->miny / self->voxels->tiles_per_line,
		tiles->minz / self->voxels->tiles_per_line,
		tiles->maxx / self->voxels->tiles_per_line,
		tiles->maxy / self->voxels->tiles_per_line,
		tiles->maxz / self->voxels->tiles_per_line);
	self->range0 = lialg_range_clamp (self->range0, 0, voxels->sectors->count - 1);

	/* Find first valid sector. */
	if (!lialg_range_iter_first (&self->rangei0, &self->range0))
		goto empty;
	while (1)
	{
		self->sector = (LIVoxSector*) lialg_sectors_data_index (self->voxels->sectors,
			LIALG_SECTORS_CONTENT_VOXEL, self->rangei0.index, self->load);
		if (self->sector != NULL)
			break;
		if (!lialg_range_iter_next (&self->rangei0))
		{
			self->sector = NULL;
			goto empty;
		}
	}

	/* Initialize voxel range. */
	livox_sector_get_offset (self->sector, offset + 0, offset + 1, offset + 2);
	self->range1 = lialg_range_new (
		self->tiles.minx - offset[0] * self->voxels->tiles_per_line,
		self->tiles.miny - offset[1] * self->voxels->tiles_per_line,
		self->tiles.minz - offset[2] * self->voxels->tiles_per_line,
		self->tiles.maxx - offset[0] * self->voxels->tiles_per_line,
		self->tiles.maxy - offset[1] * self->voxels->tiles_per_line,
		self->tiles.maxz - offset[2] * self->voxels->tiles_per_line);
	self->range1 = lialg_range_clamp (self->range1, 0, self->voxels->tiles_per_line - 1);

	/* Find first voxel. */
	ret = lialg_range_iter_first (&self->rangei1, &self->range1);
	self->voxel[0] = self->rangei1.x;
	self->voxel[1] = self->rangei1.y;
	self->voxel[2] = self->rangei1.z;

	return 1;

empty:
	self->range1 = lialg_range_new (0, 0, 0, 0, 0, 0);
	lialg_range_iter_first (&self->rangei1, &self->range1);
	self->voxel[0] = 0;
	self->voxel[1] = 0;
	self->voxel[2] = 0;

	return 0;
}

static inline int livox_voxel_iter_next (
	LIVoxVoxelIter* self)
{
	int ret;
	int offset[3];

	/* Next voxel. */
	if (lialg_range_iter_next (&self->rangei1))
	{
		self->voxel[0] = self->rangei1.x;
		self->voxel[1] = self->rangei1.y;
		self->voxel[2] = self->rangei1.z;
		return 1;
	}

	/* Next sector. */
	while (1)
	{
		if (!lialg_range_iter_next (&self->rangei0))
		{
			self->sector = NULL;
			return 0;
		}
		self->sector = (LIVoxSector*) lialg_sectors_data_index (self->voxels->sectors,
			LIALG_SECTORS_CONTENT_VOXEL, self->rangei0.index, self->load);
		if (self->sector != NULL)
			break;
	}

	/* Initialize voxel range. */
	livox_sector_get_offset (self->sector, offset + 0, offset + 1, offset + 2);
	self->range1 = lialg_range_new (
		self->tiles.minx - offset[0] * self->voxels->tiles_per_line,
		self->tiles.miny - offset[1] * self->voxels->tiles_per_line,
		self->tiles.minz - offset[2] * self->voxels->tiles_per_line,
		self->tiles.maxx - offset[0] * self->voxels->tiles_per_line,
		self->tiles.maxy - offset[1] * self->voxels->tiles_per_line,
		self->tiles.maxz - offset[2] * self->voxels->tiles_per_line);
	self->range1 = lialg_range_clamp (self->range1, 0, self->voxels->tiles_per_line - 1);

	/* Find first voxel. */
	ret = lialg_range_iter_first (&self->rangei1, &self->range1);
	self->voxel[0] = self->rangei1.x;
	self->voxel[1] = self->rangei1.y;
	self->voxel[2] = self->rangei1.z;

	return 1;
}

#endif
