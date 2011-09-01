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
 * \addtogroup LIAi Ai
 * @{
 * \addtogroup LIAiManager Manager
 * @{
 */

#include <lipsofsuna/system.h>
#include "ai-manager.h"
#include "ai-sector.h"

static float private_astar_cost (
	LIAiManager*  self,
	void*         object,
	LIAiWaypoint* start,
	LIAiWaypoint* end);

static float private_astar_heuristic (
	LIAiManager*  self,
	void*         object,
	LIAiWaypoint* start,
	LIAiWaypoint* end);

static int private_astar_passable (
	LIAiManager*  self,
	void*         object,
	LIAiWaypoint* start,
	LIAiWaypoint* end);

static void* private_astar_successor (
	LIAiManager*  self,
	void*         object,
	LIAiWaypoint* node,
	int           index);

static LIAiPath* private_solve_path (
	LIAiManager*  self,
	LIAiWaypoint* start,
	LIAiWaypoint* end);

/*****************************************************************************/

/**
 * \brief Creates a new AI manager.
 * \param callbacks Callbacks.
 * \param sectors Sector manager.
 * \param voxels Voxel manager or NULL.
 * \return New AI manager or NULL.
 */
LIAiManager* liai_manager_new (
	LICalCallbacks* callbacks,
	LIAlgSectors*   sectors,
	LIVoxManager*   voxels)
{
	LIAiManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIAiManager));
	if (self == NULL)
		return NULL;
	self->callbacks = callbacks;
	self->sectors = sectors;
	self->voxels = voxels;

	/* Register sector content. */
	if (!lialg_sectors_insert_content (self->sectors, LIALG_SECTORS_CONTENT_AI, self,
		(LIAlgSectorFreeFunc) liai_sector_free,
		(LIAlgSectorLoadFunc) liai_sector_new))
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate path solver. */
	self->astar = lialg_astar_new (
		(LIAlgAstarCost) private_astar_cost,
		(LIAlgAstarHeuristic) private_astar_heuristic,
		(LIAlgAstarPassable) private_astar_passable,
		(LIAlgAstarSuccessor) private_astar_successor);
	if (self->astar == NULL)
	{
		lialg_sectors_remove_content (self->sectors, LIALG_SECTORS_CONTENT_AI);
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the AI manager.
 * \param self AI manager.
 */
void liai_manager_free (
	LIAiManager* self)
{
	/* Unregister sector content. */
	if (self->sectors != NULL)
		lialg_sectors_remove_content (self->sectors, LIALG_SECTORS_CONTENT_AI);

	/* Free path solver. */
	if (self->astar != NULL)
		lialg_astar_free (self->astar);

	lisys_free (self);
}

/**
 * \brief Finds a waypoint.
 * \param self AI manager.
 * \param point Point in tiles.
 * \return Waypoint or NULL.
 */
LIAiWaypoint* liai_manager_find_waypoint (
	LIAiManager*       self,
	const LIMatVector* point)
{
	int x;
	int y;
	int z;
	int sx;
	int sy;
	int sz;
	LIAiSector* sector;

	x = (int)(point->x + 0.5);
	y = (int)(point->y + 0.5);
	z = (int)(point->z + 0.5);
	sx = x / self->voxels->tiles_per_line;
	sy = y / self->voxels->tiles_per_line;
	sz = z / self->voxels->tiles_per_line;
	x %= self->voxels->tiles_per_line;
	y %= self->voxels->tiles_per_line;
	z %= self->voxels->tiles_per_line;

	sector = lialg_sectors_data_offset (self->sectors, LIALG_SECTORS_CONTENT_AI, sx, sy, sz, 0);
	if (sector == NULL)
		return NULL;

	return liai_sector_get_waypoint (sector, x, y, z);
}

/**
 * \brief Solves path from the starting point to the end point.
 * \param self AI manager.
 * \param start Start position.
 * \param end Target position.
 * \return New path or NULL.
 */
LIAiPath* liai_manager_solve_path (
	LIAiManager*       self,
	const LIMatVector* start,
	const LIMatVector* end)
{
	LIAiWaypoint* wp0;
	LIAiWaypoint* wp1;

	wp0 = liai_manager_find_waypoint (self, start);
	wp1 = liai_manager_find_waypoint (self, end);
	if (wp0 == NULL || wp1 == NULL)
		return NULL;

	return private_solve_path (self, wp0, wp1);
}

int liai_manager_update_block (
	LIAiManager* self,
	int          x,
	int          y,
	int          z,
	int          sx,
	int          sy,
	int          sz)
{
	int ssize;
	int boff[3];
	int soff[3];
	LIAiSector* ai;
	LIAiSector* ai1;
	LIAlgSector* sector;
	LIVoxSector* voxel;

	/* Calculate the block size and offset. */
	ssize = self->voxels->tiles_per_line;
	soff[0] = x / ssize;
	soff[1] = y / ssize;
	soff[2] = z / ssize;
	boff[0] = x - soff[0] * ssize;
	boff[1] = y - soff[1] * ssize;
	boff[2] = z - soff[2] * ssize;

	/* Find or create sector. */
	sector = lialg_sectors_sector_offset (self->sectors, soff[0], soff[1], soff[2], 1);
	if (sector == NULL)
		return 1;
	ai = sector->content[LIALG_SECTORS_CONTENT_AI];
	voxel = sector->content[LIALG_SECTORS_CONTENT_VOXEL];
	if (ai == NULL || voxel == NULL)
		return 1;

	/* Build waypoints. */
	liai_sector_build_area (ai, voxel, boff[0], boff[1], boff[2], sx, sy, sz);

	/* Update walkability flags at sector borders. */
	if (!boff[1] && soff[1])
	{
		sector = lialg_sectors_sector_offset (self->sectors, soff[0], soff[1] - 1, soff[2], 0);
		if (sector != NULL)
		{
			ai1 = sector->content[LIALG_SECTORS_CONTENT_AI];
			if (ai1 != NULL)
				liai_sector_build_border (ai, ai1, boff[0], boff[2], sx, sz);
		}
	}
	else if (boff[1] == self->voxels->tiles_per_line - sy)
	{
		sector = lialg_sectors_sector_offset (self->sectors, soff[0], soff[1] + 1, soff[2], 0);
		if (sector != NULL)
		{
			ai1 = sector->content[LIALG_SECTORS_CONTENT_AI];
			if (ai1 != NULL)
				liai_sector_build_border (ai1, ai, boff[0], boff[2], sx, sz);
		}
	}

	return 1;
}

/*****************************************************************************/

static float private_astar_cost (
	LIAiManager*  self,
	void*         object,
	LIAiWaypoint* start,
	LIAiWaypoint* end)
{
	LIMatVector diff;

	diff = limat_vector_subtract (start->position, end->position);

	/* FIXME: Gives lots of penalty for climbing. */
	return limat_vector_get_length (diff) + 50.0f * LIMAT_MAX (0, diff.y);
}

static float private_astar_heuristic (
	LIAiManager*  self,
	void*         object,
	LIAiWaypoint* start,
	LIAiWaypoint* end)
{
	return LIMAT_ABS (start->position.x - end->position.x) +
	       LIMAT_ABS (start->position.y - end->position.y) +
	       LIMAT_ABS (start->position.z - end->position.z);
}

static int private_astar_passable (
	LIAiManager*  self,
	void*         object,
	LIAiWaypoint* start,
	LIAiWaypoint* end)
{
	/* FIXME: Check for object size. */
	return 1;
}

static void* private_astar_successor (
	LIAiManager*  self,
	void*         object,
	LIAiWaypoint* node,
	int           index)
{
	int i;
	int x;
	int y;
	int z;
	int sx;
	int sy;
	int sz;
	int pos;
	LIAiWaypoint* wp;
	LIAiSector* sector;
	static const int rel[26][3] =
	{
		{ -1,  0, -1 },
		{  0,  0, -1 },
		{  1,  0, -1 },
		{ -1,  0,  0 },
		{  1,  0,  0 },
		{ -1,  0,  1 },
		{  0,  0,  1 },
		{  1,  0,  1 },
		{ -1, -1, -1 },
		{  0, -1, -1 },
		{  1, -1, -1 },
		{ -1, -1,  0 },
		{  0, -1,  0 },
		{  1, -1,  0 },
		{ -1, -1,  1 },
		{  0, -1,  1 },
		{  1, -1,  1 },
		{ -1,  1, -1 },
		{  0,  1, -1 },
		{  1,  1, -1 },
		{ -1,  1,  0 },
		{  0,  1,  0 },
		{  1,  1,  0 },
		{ -1,  1,  1 },
		{  0,  1,  1 },
		{  1,  1,  1 }
	};

	for (i = pos = 0 ; i < 26 ; i++)
	{
		/* Find next node. */
		x = node->x + rel[i][0];
		y = node->y + rel[i][1];
		z = node->z + rel[i][2];
		sx = node->sector->sector->x;
		sy = node->sector->sector->y;
		sz = node->sector->sector->z;
		if (x < 0) { x += self->voxels->tiles_per_line; sx--; }
		if (y < 0) { y += self->voxels->tiles_per_line; sy--; }
		if (z < 0) { z += self->voxels->tiles_per_line; sz--; }
		if (x >= self->voxels->tiles_per_line) { x -= self->voxels->tiles_per_line; sx++; }
		if (y >= self->voxels->tiles_per_line) { y -= self->voxels->tiles_per_line; sy++; }
		if (z >= self->voxels->tiles_per_line) { z -= self->voxels->tiles_per_line; sz++; }

		/* Find next sector. */
		sector = node->sector;
		if (sx != sector->sector->x || sy != sector->sector->y || sz != sector->sector->z)
		{
			sector = lialg_sectors_data_offset (sector->sector->manager, LIALG_SECTORS_CONTENT_AI, sx, sy, sz, 0);
			if (sector == NULL)
				continue;
		}

		/* Get next node. */
		/* FIXME: No support for flying monsters. */
		/* FIXME: No support for larger monsters. */
		wp = liai_sector_get_waypoint (sector, x, y, z);
		if (wp->flags & LIAI_WAYPOINT_FLAG_WALKABLE)
		{
			if (pos++ == index)
				return wp;
		}
	}

	return NULL;
}

/**
 * \brief Solves path from the starting point to the end point.
 * \param self AI manager.
 * \param start Start waypoint.
 * \param end Target waypoint.
 * \return New path or NULL.
 */
static LIAiPath* private_solve_path (
	LIAiManager*  self,
	LIAiWaypoint* start,
	LIAiWaypoint* end)
{
	int i;
	LIAiPath* path;
	LIAiWaypoint* point;
	LIAlgAstarResult* result;

	/* Solve path, */
	result = lialg_astar_solve (self->astar, self, NULL, start, end);
	if (result == NULL)
		return NULL;

	/* Allocate path. */
	path = liai_path_new ();
	if (path == NULL)
	{
		lialg_astar_result_free (result);
		return NULL;
	}

	/* Convert results. */
	for (i = 0 ; i < result->length ; i++)
	{
		point = result->nodes[i];
		if (!liai_path_append_point (path, &point->position))
		{
			lialg_astar_result_free (result);
			liai_path_free (path);
			return NULL;
		}
	}
	lialg_astar_result_free (result);

	return path;
}

/** @} */
/** @} */
