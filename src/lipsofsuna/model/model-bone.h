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

#ifndef __MODEL_BONE_H__
#define __MODEL_BONE_H__

#include "lipsofsuna/archive.h"
#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"
#include "model-types.h"

struct _LIMdlBone
{
	LIMatVector length;
	LIMatVector tail;
};

LIAPICALL (int, limdl_bone_read, (
	LIMdlNode*   self,
	LIArcReader* reader));

LIAPICALL (int, limdl_bone_write, (
	const LIMdlNode* self,
	LIArcWriter*     writer));

LIAPICALL (void, limdl_bone_get_pose_head, (
	const LIMdlNode* self,
	LIMatVector*     head));

LIAPICALL (void, limdl_bone_get_pose_tail, (
	const LIMdlNode* self,
	LIMatVector*     tail));

#endif
