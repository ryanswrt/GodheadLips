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

#ifndef __AI_MANAGER_H__
#define __AI_MANAGER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/voxel.h>
#include "ai-path.h"
#include "ai-types.h"

struct _LIAiManager
{
	LIAlgAstar* astar;
	LIAlgSectors* sectors;
	LICalCallbacks* callbacks;
	LIVoxManager* voxels;
};

LIAPICALL (LIAiManager*, liai_manager_new, (
	LICalCallbacks* callbacks,
	LIAlgSectors*   sectors,
	LIVoxManager*   voxels));

LIAPICALL (void, liai_manager_free, (
	LIAiManager* self));

LIAPICALL (LIAiWaypoint*, liai_manager_find_waypoint, (
	LIAiManager*       self,
	const LIMatVector* point));

LIAPICALL (int, liai_manager_load_sector, (
	LIAiManager* self,
	int          sx,
	int          sy,
	int          sz));

LIAPICALL (LIAiPath*, liai_manager_solve_path, (
	LIAiManager*       self,
	const LIMatVector* start,
	const LIMatVector* end));

LIAPICALL (int, liai_manager_update_block, (
	LIAiManager* self,
	int          x,
	int          y,
	int          z,
	int          sx,
	int          sy,
	int          sz));

#endif
