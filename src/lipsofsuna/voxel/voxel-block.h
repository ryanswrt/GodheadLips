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

#ifndef __VOXEL_BLOCK_H__
#define __VOXEL_BLOCK_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "voxel-types.h"

enum
{
	LIVOX_DIRTY_NEGATIVE_X = 0x01,
	LIVOX_DIRTY_POSITIVE_X = 0x02,
	LIVOX_DIRTY_NEGATIVE_Y = 0x04,
	LIVOX_DIRTY_POSITIVE_Y = 0x08,
	LIVOX_DIRTY_NEGATIVE_Z = 0x10,
	LIVOX_DIRTY_POSITIVE_Z = 0x20,
	LIVOX_DIRTY_PROPAGATED = 0x40,
	LIVOX_DIRTY_EXPLICIT = 0x80,
};

typedef struct _LIVoxBlock LIVoxBlock;
typedef struct _LIVoxBlockAddr LIVoxBlockAddr;

struct _LIVoxBlockAddr
{
	uint8_t sector[3];
	uint8_t block[3];
};

LIAPICALL (int, livox_block_get_dirty, (
	const LIVoxBlock* self));

LIAPICALL (void, livox_block_set_dirty, (
	LIVoxBlock* self,
	int         value));

LIAPICALL (int, livox_block_get_stamp, (
	const LIVoxBlock* self));

#endif
