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

/**
 * \addtogroup LIAi Ai
 * @{
 * \addtogroup LIAiSector Sector
 * @{
 */

#include <lipsofsuna/system.h>
#include "ai-manager.h"
#include "ai-sector.h"

/**
 * \brief Creates a new AI sector.
 * \param sector Sector manager sector.
 * \return New sector or NULL.
 */
LIAiSector* liai_sector_new (
	LIAlgSector* sector)
{
	int i;
	int j;
	int k;
	int l;
	int tpl;
	LIAiSector* self;
	LIAiWaypoint* wp;
	LIMatVector off;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIAiSector));
	if (self == NULL)
		return NULL;
	self->manager = lialg_sectors_get_userdata (sector->manager, LIALG_SECTORS_CONTENT_AI);
	self->sector = sector;

	/* Allocate tiles. */
	tpl = self->manager->voxels->tiles_per_line;
	self->points = calloc (tpl * tpl * tpl, sizeof (LIAiWaypoint));
	if (self->points == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Calculate the tile offset. */
	off = limat_vector_init (sector->x, sector->y, sector->z);
	off = limat_vector_multiply (off, tpl);

	/* Set waypoint positions. */
	for (k = l = 0 ; k < tpl ; k++)
	for (j = 0 ; j < tpl ; j++)
	for (i = 0 ; i < tpl ; i++, l++)
	{
		wp = self->points + l;
		wp->x = i;
		wp->y = j;
		wp->z = k;
		wp->sector = self;
		wp->position = limat_vector_add (off, limat_vector_init (i, j, k));
	}

	return self;
}

/**
 * \brief Frees the sector.
 * \param self Sector.
 */
void liai_sector_free (
	LIAiSector* self)
{
	lisys_free (self->points);
	lisys_free (self);
}

/**
 * \brief Rebuilds the waypoints for the sector.
 * \param self Sector.
 * \param voxels Voxel sector or NULL.
 */
void liai_sector_build (
	LIAiSector*  self,
	LIVoxSector* voxels)
{
	int tpl;

	tpl = self->manager->voxels->tiles_per_line;
	liai_sector_build_area (self, voxels, 0, 0, 0, tpl, tpl, tpl);
}

/**
 * \brief Rebuilds the waypoints for the given area.
 * \param self Sector.
 * \param voxels Voxel sector or NULL.
 * \param x Start waypoint.
 * \param y Start waypoint.
 * \param z Start waypoint.
 * \param xs Waypoint count.
 * \param ys Waypoint count.
 * \param zs Waypoint count.
 */
void liai_sector_build_area (
	LIAiSector*  self,
	LIVoxSector* voxels,
	int          x,
	int          y,
	int          z,
	int          xs,
	int          ys,
	int          zs)
{
	int i;
	int j;
	int k;
	LIAiWaypoint* wp;
	LIAiWaypoint* wp1;
	LIVoxVoxel* voxel;

	if (voxels != NULL)
	{
		/* Mark flyable waypoints. */
		for (k = z ; k < z + zs ; k++)
		for (j = y ; j < y + ys ; j++)
		for (i = x ; i < x + xs ; i++)
		{
			wp = liai_sector_get_waypoint (self, i, j, k);
			voxel = livox_sector_get_voxel (voxels, i, j, k);
			if (voxel->type)
				wp->flags = 0;
			else
				wp->flags = LIAI_WAYPOINT_FLAG_FLYABLE;
		}

		/* Mark walkable waypoints. */
		for (k = z ; k < z + zs ; k++)
		for (j = LIMAT_MAX (y, 1) ; j < y + ys ; j++)
		for (i = x ; i < x + xs ; i++)
		{
			wp = liai_sector_get_waypoint (self, i, j, k);
			wp1 = liai_sector_get_waypoint (self, i, j - 1, k);
			if ((wp ->flags & LIAI_WAYPOINT_FLAG_FLYABLE) &&
			   !(wp1->flags & LIAI_WAYPOINT_FLAG_FLYABLE))
				wp->flags |= LIAI_WAYPOINT_FLAG_WALKABLE;
		}
	}
	else
	{
		/* No waypoint data. */
		for (k = z ; k < z + zs ; k++)
		for (j = y ; j < y + ys ; j++)
		for (i = x ; i < x + xs ; i++)
		{
			wp = liai_sector_get_waypoint (self, i, j, k);
			wp->flags = 0;
		}
	}
}

/**
 * \brief Rebuilds the waypoints for the given border area.
 * \param self Sector.
 * \param above AI sector above the updated one.
 * \param x Start waypoint.
 * \param z Start waypoint.
 * \param xs Waypoint count.
 * \param zs Waypoint count.
 */
void liai_sector_build_border (
	LIAiSector* self,
	LIAiSector* above,
	int         x,
	int         z,
	int         xs,
	int         zs)
{
	int i;
	int k;
	LIAiWaypoint* wp;
	LIAiWaypoint* wp1;

	/* Mark walkable for border waypoints. */
	for (k = z ; k < z + zs ; k++)
	for (i = x ; i < x + xs ; i++)
	{
		wp = liai_sector_get_waypoint (self, i, self->manager->voxels->tiles_per_line - 1, k);
		wp1 = liai_sector_get_waypoint (above, i, 0, k);
		if ((wp ->flags & LIAI_WAYPOINT_FLAG_FLYABLE) &&
		   !(wp1->flags & LIAI_WAYPOINT_FLAG_FLYABLE))
			wp->flags |= LIAI_WAYPOINT_FLAG_WALKABLE;
	}
}

/**
 * \brief Gets a waypoint by offset.
 * \param self Sector.
 * \param x Waypoint offset.
 * \param y Waypoint offset.
 * \param z Waypoint offset.
 * \return Waypoint.
 */
LIAiWaypoint* liai_sector_get_waypoint (
	LIAiSector* self,
	int         x,
	int         y,
	int         z)
{
	int tpl;

	tpl = self->manager->voxels->tiles_per_line;
	return self->points + x + (y + z * tpl) * tpl;
}

/** @} */
/** @} */
