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

#ifndef __MODEL_SHAPE_KEY_H__
#define __MODEL_SHAPE_KEY_H__

#include "lipsofsuna/archive.h"
#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"

typedef struct _LIMdlShapeKeyVertex LIMdlShapeKeyVertex;
struct _LIMdlShapeKeyVertex
{
	LIMatVector coord;
	LIMatVector normal;
};

typedef struct _LIMdlShapeKey LIMdlShapeKey;
struct _LIMdlShapeKey
{
	char* name;
	struct
	{
		int count;
		LIMdlShapeKeyVertex* array;
	} vertices;
};

LIAPICALL (int, limdl_shape_key_init_copy, (
	LIMdlShapeKey*       self,
	const LIMdlShapeKey* key));

LIAPICALL (void, limdl_shape_key_clear, (
	LIMdlShapeKey* self));

LIAPICALL (int, limdl_shape_key_read, (
	LIMdlShapeKey* self,
	LIArcReader*   reader));

LIAPICALL (int, limdl_shape_key_write, (
	const LIMdlShapeKey* self,
	LIArcWriter*         writer));

#endif
