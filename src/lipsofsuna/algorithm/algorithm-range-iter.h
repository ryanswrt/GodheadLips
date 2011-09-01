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

#ifndef __ALGORITHM_RANGE_ITER_H__
#define __ALGORITHM_RANGE_ITER_H__

#include "algorithm-range.h"

#define LIALG_RANGE_FOREACH(iter, range) \
	for (lialg_range_iter_first (&iter, &range) ; iter.more ; \
	     lialg_range_iter_next (&iter))

typedef struct _LIAlgRangeIter LIAlgRangeIter;
struct _LIAlgRangeIter
{
	int index;
	int x;
	int y;
	int z;
	int more;
	LIAlgRange range;
};

static inline int
lialg_range_iter_first (LIAlgRangeIter* self,
                        LIAlgRange*     range)
{
	int r = range->max - range->min;

	/* Find first bin. */
	self->range = *range;
	self->x = range->minx;
	self->y = range->miny;
	self->z = range->minz;
	if (range->minx > range->maxx ||
	    range->miny > range->maxy ||
	    range->minz > range->maxz)
	{
		self->index = 0;
		self->more = 0;
		return 0;
	}

	/* Calculate bin index. */
	self->more = 1;
	self->index =
		(self->x - self->range.min) +
		(self->y - self->range.min) * r +
		(self->z - self->range.min) * r * r;

	return 1;
}

static inline int
lialg_range_iter_next (LIAlgRangeIter* self)
{
	int r = self->range.max - self->range.min;

	/* Find next bin. */
	if (++self->x > self->range.maxx)
	{
		self->x = self->range.minx;
		if (++self->y > self->range.maxy)
		{
			self->y = self->range.miny;
			if (++self->z > self->range.maxz)
			{
				self->more = 0;
				return 0;
			}
		}
	}

	/* Calculate bin index. */
	self->index =
		(self->x - self->range.min) +
		(self->y - self->range.min) * r +
		(self->z - self->range.min) * r * r;

	return 1;
}

#endif
