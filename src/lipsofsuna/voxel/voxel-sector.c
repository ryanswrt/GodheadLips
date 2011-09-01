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
 * \addtogroup LIVoxSector Sector
 * @{
 */

#include "lipsofsuna/system.h"
#include "voxel-hinting.h"
#include "voxel-manager.h"
#include "voxel-sector.h"
#include "voxel-private.h"

#define LIVOX_ERASE_SHIFT (0.25f * LIVOX_TILE_WIDTH)
#define LIVOX_TILES_PER_SECLINE (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE)

static int private_build_block (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z);

static int private_set_voxel (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z,
	LIVoxVoxel*  voxel);

/*****************************************************************************/

/**
 * \brief Creates a new sector.
 *
 * \param sector Sector manager sector.
 * \return New sector or NULL.
 */
LIVoxSector*
livox_sector_new (LIAlgSector* sector)
{
	LIVoxSector* self;
	LIVoxVoxel tmp;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxSector));
	if (self == NULL)
		return NULL;
	self->manager = lialg_sectors_get_userdata (sector->manager, LIALG_SECTORS_CONTENT_VOXEL);
	self->sector = sector;

	/* Allocate tiles. */
	self->blocks = lisys_calloc (self->manager->blocks_per_sector, sizeof (LIVoxBlock));
	if (self->blocks == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->tiles = lisys_calloc (self->manager->tiles_per_sector, sizeof (LIVoxVoxel));
	if (self->tiles == NULL)
	{
		lisys_free (self->blocks);
		lisys_free (self);
		return NULL;
	}

	/* Fill sector. */
	if (self->manager->fill)
	{
		livox_voxel_init (&tmp, self->manager->fill);
		livox_sector_fill (self, &tmp);
	}

	return self;
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void
livox_sector_free (LIVoxSector* self)
{
	int x;
	int y;
	int z;
	LIVoxUpdateEvent event;

	/* Invoke callbacks. */
	if (self->blocks != NULL)
	{
		event.sector[0] = self->sector->x;
		event.sector[1] = self->sector->y;
		event.sector[2] = self->sector->z;
		for (z = 0 ; z < self->manager->blocks_per_line ; z++)
		for (y = 0 ; y < self->manager->blocks_per_line ; y++)
		for (x = 0 ; x < self->manager->blocks_per_line ; x++)
		{
			event.block[0] = x;
			event.block[1] = y;
			event.block[2] = z;
			lical_callbacks_call (self->manager->callbacks, "block-free", lical_marshal_DATA_PTR, &event);
		}
	}

	lisys_free (self->blocks);
	lisys_free (self->tiles);
	lisys_free (self);
}

int
livox_sector_build_block (LIVoxSector* self,
                          int          x,
                          int          y,
                          int          z)
{
	return private_build_block (self, x, y, z);
}

/**
 * \brief Fills the sector with the given terrain type.
 *
 * \param self Sector.
 * \param terrain Terrain type.
 */
void
livox_sector_fill (LIVoxSector* self,
                   LIVoxVoxel*  terrain)
{
	int i = 0;

	for (i = 0 ; i < self->manager->tiles_per_sector ; i++)
		self->tiles[i] = *terrain;
	for (i = 0 ; i < self->manager->blocks_per_sector ; i++)
	{
		self->blocks[i].dirty = 0xFF;
		self->blocks[i].stamp++;
	}
	self->dirty = 1;
}

/**
 * \brief Reads block data from a stream.
 * \param self Sector.
 * \param x Block offset.
 * \param y Block offset.
 * \param z Block offset.
 * \param reader Reader.
 * \return Nonzero on success.
 */
int livox_sector_read_block (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z,
	LIArcReader* reader)
{
	int c;
	int tx;
	int ty;
	int tz;
	LIVoxVoxel tmp;

	c = self->manager->tiles_per_line / self->manager->blocks_per_line;
	for (tz = 0 ; tz < c ; tz++)
	for (ty = 0 ; ty < c ; ty++)
	for (tx = 0 ; tx < c ; tx++)
	{
		if (!livox_voxel_read (&tmp, reader))
			return 0;
		if (private_set_voxel (self, tx + x * c, ty + y * c, tz + z * c, &tmp))
			self->dirty = 1;
	}

	return 1;
}

/**
 * \brief Called once per tick to update the status of the sector.
 *
 * \param self Sector.
 * \param secs Number of seconds since the last update.
 */
void
livox_sector_update (LIVoxSector* self,
                     float        secs)
{
}

/**
 * \brief Writes block data to a stream.
 *
 * \param self Sector.
 * \param x Block offset.
 * \param y Block offset.
 * \param z Block offset.
 * \param writer Writer.
 * \return Nonzero on success.
 */
int livox_sector_write_block (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z,
	LIArcWriter* writer)
{
	int c;
	int tx;
	int ty;
	int tz;
	LIVoxVoxel* tile;

	c = self->manager->tiles_per_line / self->manager->blocks_per_line;
	for (tz = 0 ; tz < c ; tz++)
	for (ty = 0 ; ty < c ; ty++)
	for (tx = 0 ; tx < c ; tx++)
	{
		tile = livox_sector_get_voxel (self, tx + x * c, ty + y * c, tz + z * c);
		if (!livox_voxel_write (tile, writer))
			return 0;
	}

	return 1;
}

/**
 * \brief Gets a voxel block.
 *
 * \param self Sector.
 * \param x Block offset.
 * \param y Block offset.
 * \param z Block offset.
 * \return Block.
 */
LIVoxBlock* livox_sector_get_block (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z)
{
	lisys_assert (x >= 0 && y >= 0 && z >= 0);
	lisys_assert (x < self->manager->blocks_per_line);
	lisys_assert (y < self->manager->blocks_per_line);
	lisys_assert (z < self->manager->blocks_per_line);

	return self->blocks + x + y * self->manager->blocks_per_line +
		z * self->manager->blocks_per_line * self->manager->blocks_per_line;
}

/**
 * \brief Gets the bounding box of the sector.
 *
 * \param self Sector.
 * \param result Return location for the bounding box.
 */
void
livox_sector_get_bounds (const LIVoxSector* self,
                         LIMatAabb*         result)
{
	float size;
	LIMatVector min;
	LIMatVector max;

	size = self->sector->manager->width;
	min = self->sector->position;
	max = limat_vector_init (size, size, size);
	max = limat_vector_add (min, max);
	limat_aabb_init_from_points (result, &min, &max);
}

/**
 * \brief Returns nonzero if the sector is dirty.
 *
 * \param self Sector.
 * \return Nonzero if dirty.
 */
int
livox_sector_get_dirty (const LIVoxSector* self)
{
	return self->dirty;
}

/**
 * \brief Sets or clears the dirty flag of the sector.
 *
 * \param self Sector.
 * \param value Zero to clear, nonzero to set.
 */
void
livox_sector_set_dirty (LIVoxSector* self,
                        int          value)
{
	self->dirty = value;
}

/**
 * \brief Checks if the sector contains no terrain.
 *
 * \param self Sector.
 * \return Nonzero if the sector is empty.
 */
int
livox_sector_get_empty (const LIVoxSector* self)
{
	int i;

	for (i = 0 ; i < self->manager->tiles_per_sector ; i++)
	{
		if (self->tiles[i].type != 0)
			return 0;
	}

	return 1;
}

/**
 * \brief Gets the memory used by the sector.
 * \param self Sector.
 * \return Memory used in bytes.
 */
int livox_sector_get_memory (const LIVoxSector* self)
{
	return sizeof (LIVoxSector) +
		self->manager->blocks_per_sector * sizeof (LIVoxBlock) +
		self->manager->tiles_per_sector * sizeof (LIVoxVoxel);
}

/**
 * \brief Gets the offset of the sector in the world in voxels.
 *
 * \param self Sector.
 * \param x Return location for the offset.
 * \param y Return location for the offset.
 * \param z Return location for the offset.
 */
void
livox_sector_get_offset (const LIVoxSector* self,
                         int*               x,
                         int*               y,
                         int*               z)
{
	*x = self->sector->x;
	*y = self->sector->y;
	*z = self->sector->z;
}

/**
 * \brief Gets the origin of the sector.
 *
 * \param self Sector.
 * \param result Return location for the position vector.
 */
void
livox_sector_get_origin (const LIVoxSector* self,
                         LIMatVector*       result)
{
	*result = self->sector->position;
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the sector.
 * \param y Offset of the voxel within the sector.
 * \param z Offset of the voxel within the sector.
 * \return Terrain type or zero.
 */
LIVoxVoxel*
livox_sector_get_voxel (LIVoxSector* self,
                        int          x,
                        int          y,
                        int          z)
{
	lisys_assert (x >= 0 && y >= 0 && z >= 0);
	lisys_assert (x < self->manager->tiles_per_line);
	lisys_assert (y < self->manager->tiles_per_line);
	lisys_assert (z < self->manager->tiles_per_line);

	return self->tiles + x + y * self->manager->tiles_per_line +
		z * self->manager->tiles_per_line * self->manager->tiles_per_line;
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the sector.
 * \param y Offset of the voxel within the sector.
 * \param z Offset of the voxel within the sector.
 * \param terrain Terrain type.
 * \return Nonzero if a voxel was modified.
 */
int
livox_sector_set_voxel (LIVoxSector* self,
                        int          x,
                        int          y,
                        int          z,
                        LIVoxVoxel   terrain)
{
	lisys_assert (x >= 0 && y >= 0 && z >= 0);
	lisys_assert (x < self->manager->tiles_per_line);
	lisys_assert (y < self->manager->tiles_per_line);
	lisys_assert (z < self->manager->tiles_per_line);

	if (private_set_voxel (self, x, y, z, &terrain))
		self->dirty = 1;

	return 1;
}

/*****************************************************************************/

static int private_build_block (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z)
{
	int x1;
	int y1;
	int z1;
	int blockw;
	int sectorw;
	LIVoxUpdateEvent event;
	LIVoxVoxel* src;
	LIVoxVoxel* dst;
	LIVoxVoxel* voxels;

	/* Build triangulation and physics hints. */
	sectorw = self->manager->tiles_per_line;
	blockw = sectorw / self->manager->blocks_per_line;
	voxels = livox_hinting_process_area (self->manager,
		sectorw * self->sector->x + blockw * x,
		sectorw * self->sector->y + blockw * y,
		sectorw * self->sector->z + blockw * z,
		blockw, blockw, blockw);
	if (voxels != NULL)
	{
		for (z1 = 0 ; z1 < blockw ; z1++)
		for (y1 = 0 ; y1 < blockw ; y1++)
		for (x1 = 0 ; x1 < blockw ; x1++)
		{
			src = voxels + x1 + y1 * blockw + z1 * blockw * blockw;
			dst = self->tiles +
				(x * blockw + x1) + 
				(y * blockw + y1) * sectorw + 
				(z * blockw + z1) * sectorw * sectorw;
			*dst = *src;
		}
		lisys_free (voxels);
	}

	/* Invoke callbacks. */
	event.sector[0] = self->sector->x;
	event.sector[1] = self->sector->y;
	event.sector[2] = self->sector->z;
	event.block[0] = x;
	event.block[1] = y;
	event.block[2] = z;
	lical_callbacks_call (self->manager->callbacks, "block-load", lical_marshal_DATA_PTR, &event);

	return 1;
}

static int private_set_voxel (
	LIVoxSector* self,
	int          x,
	int          y,
	int          z,
	LIVoxVoxel*  voxel)
{
	int m;
	LIVoxVoxel* tile;
	LIVoxBlock* block;

	/* Modify terrain. */
	tile = self->tiles + x + y * self->manager->tiles_per_line +
		z * self->manager->tiles_per_line * self->manager->tiles_per_line;
	if (tile->type == voxel->type)
		return 0;
	*tile = *voxel;

	/* Mark block faces dirty. */
	m = self->manager->tiles_per_line / self->manager->blocks_per_line;
	block = livox_sector_get_block (self, x / m, y / m, z / m);
	x %= m;
	y %= m;
	z %= m;
	if (x == 0) block->dirty |= LIVOX_DIRTY_NEGATIVE_X;
	if (x == m - 1) block->dirty |= LIVOX_DIRTY_POSITIVE_X;
	if (y == 0) block->dirty |= LIVOX_DIRTY_NEGATIVE_Y;
	if (y == m - 1) block->dirty |= LIVOX_DIRTY_POSITIVE_Y;
	if (z == 0) block->dirty |= LIVOX_DIRTY_NEGATIVE_Z;
	if (z == m - 1) block->dirty |= LIVOX_DIRTY_POSITIVE_Z;
	block->dirty |= LIVOX_DIRTY_EXPLICIT;
	block->stamp++;

	return 1;
}

/** @} */
/** @} */
