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

#ifndef __MODEL_HAIR_H__
#define __MODEL_HAIR_H__

#include "lipsofsuna/archive.h"
#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"
#include "model-types.h"

struct _LIMdlHairNode
{
	LIMatVector position;
	float size;
};

struct _LIMdlHair
{
	int count;
	LIMdlHairNode* nodes;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (int, limdl_hair_read, (
	LIMdlHair*   self,
	LIArcReader* reader));

LIAPICALL (int, limdl_hair_write, (
	LIMdlHair*   self,
	LIArcWriter* writer));

#ifdef __cplusplus
}
#endif

struct _LIMdlHairs
{
	int material;
	int count;
	LIMdlHair* hairs;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (void, limdl_hairs_free, (
	LIMdlHairs* self));

LIAPICALL (int, limdl_hairs_read, (
	LIMdlHairs*  self,
	LIArcReader* reader));

LIAPICALL (int, limdl_hairs_write, (
	LIMdlHairs*  self,
	LIArcWriter* writer));

#ifdef __cplusplus
}
#endif

#endif
