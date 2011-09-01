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

#ifndef __AI_PATH_H__
#define __AI_PATH_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>

typedef struct _LIAiPath LIAiPath;
struct _LIAiPath
{
	int position;
	struct
	{
		int count;
		LIMatVector* points;
	} points;
};

LIAPICALL (LIAiPath*, liai_path_new, ());

LIAPICALL (void, liai_path_free, (
	LIAiPath* self));

LIAPICALL (int, liai_path_append_point, (
	LIAiPath*          self,
	const LIMatVector* point));

LIAPICALL (void, liai_path_get_point, (
	const LIAiPath* self,
	int             index,
	LIMatVector*    value));

LIAPICALL (int, liai_path_get_length, (
	const LIAiPath* self));

LIAPICALL (int, liai_path_get_position, (
	const LIAiPath* self));

LIAPICALL (void, liai_path_set_position, (
	LIAiPath* self,
	int       index));

#endif
