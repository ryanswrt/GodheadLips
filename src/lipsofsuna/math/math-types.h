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

#ifndef __MATH_TYPES_H__
#define __MATH_TYPES_H__

#include "math-vector.h"

/**
 * \brief A plane defined by a point and a normal.
 */
typedef struct _LIMatPlane LIMatPlane;
struct _LIMatPlane
{
	float x;
	float y;
	float z;
	float w;
};

typedef struct _LIMatTriangle LIMatTriangle;
struct _LIMatTriangle
{
	LIMatPlane plane;
	LIMatVector vertices[3];
};

static inline void
limat_triangle_set_from_points (LIMatTriangle*     self,
                                const LIMatVector* vertex0,
                                const LIMatVector* vertex1,
                                const LIMatVector* vertex2);

#endif
