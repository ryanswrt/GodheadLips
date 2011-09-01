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

#ifndef __ALGORITHM_RANGE_H__
#define __ALGORITHM_RANGE_H__

#include <lipsofsuna/math.h>

#define LIALG_RANGE_DEFAULT_SIZE 1024

typedef struct _LIAlgRange LIAlgRange;
struct _LIAlgRange
{
	int min;
	int max;
	int minx;
	int miny;
	int minz;
	int maxx;
	int maxy;
	int maxz;
};

static inline LIAlgRange
lialg_range_new (int xmin,
                 int ymin,
                 int zmin,
                 int xmax,
                 int ymax,
                 int zmax)
{
	LIAlgRange self;

	self.min = 0;
	self.max = LIALG_RANGE_DEFAULT_SIZE + 1;
	self.minx = xmin;
	self.miny = ymin;
	self.minz = zmin;
	self.maxx = xmax;
	self.maxy = ymax;
	self.maxz = zmax;

	return self;
}

static inline LIAlgRange
lialg_range_new_from_aabb (const LIMatVector* min,
                           const LIMatVector* max,
                           float              unit)
{
	LIAlgRange self;

	self.min = 0;
	self.max = LIALG_RANGE_DEFAULT_SIZE + 1;
	self.minx = (int)(min->x / unit);
	self.miny = (int)(min->y / unit);
	self.minz = (int)(min->z / unit);
	self.maxx = (int)(max->x / unit);
	self.maxy = (int)(max->y / unit);
	self.maxz = (int)(max->z / unit);

	return self;
}

static inline LIAlgRange
lialg_range_new_from_center (int x,
                             int y,
                             int z,
                             int radius)
{
	LIAlgRange self;

	self.min = 0;
	self.max = LIALG_RANGE_DEFAULT_SIZE + 1;
	self.minx = x - radius;
	self.miny = y - radius;
	self.minz = z - radius;
	self.maxx = x + radius;
	self.maxy = y + radius;
	self.maxz = z + radius;

	return self;
}

static inline LIAlgRange
lialg_range_new_from_index (int index,
                            int radius,
                            int mini,
                            int maxi)
{
	int size;
	LIAlgRange self;

	size = maxi - mini;
	self.min = mini;
	self.max = maxi + 1;
	self.minx = index % size;
	self.miny = index / size % size;
	self.minz = index / size / size;
	self.maxx = LIMAT_MIN (self.minx + radius, maxi);
	self.maxy = LIMAT_MIN (self.miny + radius, maxi);
	self.maxz = LIMAT_MIN (self.minz + radius, maxi);
	self.minx = LIMAT_MAX (self.minx - radius, mini);
	self.miny = LIMAT_MAX (self.miny - radius, mini);
	self.minz = LIMAT_MAX (self.minz - radius, mini);

	return self;
}

static inline LIAlgRange
lialg_range_new_from_sphere (const LIMatVector* center,
                             float              radius,
                             float              unit)
{
	LIMatVector min;
	LIMatVector max;
	LIAlgRange self;

	min = limat_vector_subtract (*center, limat_vector_init (radius, radius, radius));
	max = limat_vector_add (*center, limat_vector_init (radius, radius, radius));
	self = lialg_range_new_from_aabb (&min, &max, unit);

	return self;
}

static inline LIAlgRange
lialg_range_clamp (const LIAlgRange self,
                   int              min,
                   int              max)
{
	LIAlgRange ret;

	ret.min = min;
	ret.max = max + 1;
	ret.minx = LIMAT_MAX (self.minx, min);
	ret.miny = LIMAT_MAX (self.miny, min);
	ret.minz = LIMAT_MAX (self.minz, min);
	ret.maxx = LIMAT_MIN (self.maxx, max);
	ret.maxy = LIMAT_MIN (self.maxy, max);
	ret.maxz = LIMAT_MIN (self.maxz, max);

	return ret;
}

static inline int
lialg_range_contains (const LIAlgRange* self,
                      int               x,
                      int               y,
                      int               z)
{
	if (self->minx <= x && x <= self->maxx &&
	    self->miny <= y && y <= self->maxy &&
	    self->minz <= z && z <= self->maxz)
		return 1;

	return 0;
}

static inline int
lialg_range_contains_index (const LIAlgRange* self,
                            int               index)
{
	int x;
	int y;
	int z;
	int size;

	size = self->max - self->min;
	x = index % size;
	y = index / size % size;
	z = index / size / size;

	return lialg_range_contains (self, x, y, z);
}

#endif
