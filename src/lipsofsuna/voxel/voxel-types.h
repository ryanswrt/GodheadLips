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

#ifndef __VOXEL_TYPES_H__
#define __VOXEL_TYPES_H__

#include <lipsofsuna/system.h>

enum
{
	/* Slope hints. */
	/* The terrain physics algorithm depends on the order. */
	LIVOX_HINT_SLOPE_CORNER00 = 0x01,
	LIVOX_HINT_SLOPE_CORNER10 = 0x02,
	LIVOX_HINT_SLOPE_CORNER01 = 0x04,
	LIVOX_HINT_SLOPE_CORNER11 = 0x08,
	LIVOX_HINT_SLOPE_CORNERALL = 0x0F,
	LIVOX_HINT_SLOPE_SPECIAL1 = 0x10,
	LIVOX_HINT_SLOPE_SPECIAL2 = 0x20,
	LIVOX_HINT_SLOPE_FACEUP = 0x40,
	LIVOX_HINT_SLOPE_FACEDOWN = 0x80
};

typedef struct _LIVoxVoxel LIVoxVoxel;
struct _LIVoxVoxel
{
	uint8_t type;
	uint8_t hint;
};

typedef struct _LIVoxManager LIVoxManager;
typedef struct _LIVoxMaterial LIVoxMaterial;
typedef struct _LIVoxSector LIVoxSector;

#endif
