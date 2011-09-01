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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/archive.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/system.h>
#include "engine-model.h"
#include "engine-object.h"
#include "engine-sector.h"
#include "engine-types.h"

/*****************************************************************************/

struct _LIEngEngine
{
	LIAlgRandom random;
	LIAlgSectors* sectors;
	LIAlgU32dic* models;
	LIAlgU32dic* objects;
	LICalCallbacks* callbacks;
	LIPthPaths* paths;
};

LIAPICALL (LIEngEngine*, lieng_engine_new, (
	LICalCallbacks* calls,
	LIAlgSectors*   sectors,
	LIPthPaths*     paths));

LIAPICALL (void, lieng_engine_free, (
	LIEngEngine* self));

LIAPICALL (LIEngObject*, lieng_engine_find_object, (
	LIEngEngine* self,
	uint32_t     id));

LIAPICALL (void, lieng_engine_update, (
	LIEngEngine* self,
	float        secs));

#endif
