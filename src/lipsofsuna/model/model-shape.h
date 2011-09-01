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

#ifndef __MODEL_SHAPE_H__
#define __MODEL_SHAPE_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>

typedef struct _LIMdlShapePart LIMdlShapePart;
struct _LIMdlShapePart
{
	struct
	{
		int count;
		LIMatVector* array;
	} vertices;
};

typedef struct _LIMdlShape LIMdlShape;
struct _LIMdlShape
{
	char* name;
	LIMatAabb bounds;
	LIMatVector center;
	struct
	{
		int count;
		LIMdlShapePart* array;
	} parts;
};

LIAPICALL (int, limdl_shape_init_copy, (
	LIMdlShape*       self,
	const LIMdlShape* shape));

LIAPICALL (void, limdl_shape_clear, (
	LIMdlShape* self));

LIAPICALL (int, limdl_shape_read, (
	LIMdlShape*  self,
	LIArcReader* reader));

LIAPICALL (int, limdl_shape_write, (
	const LIMdlShape* self,
	LIArcWriter*      writer));

LIAPICALL (void, limdl_shape_get_center, (
	const LIMdlShape* self,
	LIMatVector*      result));

#endif
