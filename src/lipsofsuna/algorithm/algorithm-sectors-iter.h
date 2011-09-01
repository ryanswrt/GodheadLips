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

#ifndef __ALGORITHM_SECTORS_ITER_H__
#define __ALGORITHM_SECTORS_ITER_H__

#include "algorithm-range.h"
#include "algorithm-range-iter.h"
#include "algorithm-sectors.h"
#include "algorithm-u32dic.h"

typedef struct _LIAlgSectorsIter LIAlgSectorsIter;
struct _LIAlgSectorsIter
{
	LIAlgU32dicIter all;
	LIAlgRangeIter range;
	LIAlgSectors* sectors;
	LIAlgSector* sector;
	int next;
};

#define LIALG_SECTORS_FOREACH(iter, sectors) \
	for (lialg_sectors_iter_first_all (&iter, sectors) ; iter.sector != NULL ; \
	     lialg_sectors_iter_next (&iter))

#define LIALG_SECTORS_FOREACH_OFFSET(iter, sectors, x, y, z, radius) \
	for (lialg_sectors_iter_first_offset (&iter, sectors, x, y, z, radius) ; iter.sector != NULL ; \
	     lialg_sectors_iter_next (&iter))

#define LIALG_SECTORS_FOREACH_POINT(iter, sectors, point, radius) \
	for (lialg_sectors_iter_first_point (&iter, sectors, point, radius) ; iter.sector != NULL ; \
	     lialg_sectors_iter_next (&iter))

static inline int
lialg_sectors_iter_next_all (LIAlgSectorsIter* self)
{
	lialg_u32dic_iter_next (&self->all);
	self->sector = (LIAlgSector*) self->all.value;

	return self->sector != NULL;
}

static inline int
lialg_sectors_iter_next_range (LIAlgSectorsIter* self)
{
	/* Find next non-empty sector. */
	while (self->range.more)
	{
		self->sector = lialg_sectors_sector_index (self->sectors, self->range.index, 0);
		lialg_range_iter_next (&self->range);
		if (self->sector != NULL)
			return 1;
	}
	self->sector = NULL;

	return 0;
}

static inline int
lialg_sectors_iter_next (LIAlgSectorsIter* self)
{
	switch (self->next)
	{
		case 0: return lialg_sectors_iter_next_all (self);
		case 1: return lialg_sectors_iter_next_range (self);
	}
	return 0;
}

static inline int
lialg_sectors_iter_first_all (LIAlgSectorsIter* self,
                              LIAlgSectors*     sectors)
{
	/* Initialize self. */
	memset (self, 0, sizeof (LIAlgSectorsIter));
	self->sectors = sectors;
	self->next = 0;

	/* Find first sector. */
	lialg_u32dic_iter_start (&self->all, sectors->sectors);
	self->sector = (LIAlgSector*) self->all.value;

	return self->sector != NULL;
}

static inline int
lialg_sectors_iter_first_offset (LIAlgSectorsIter* self,
                                 LIAlgSectors*     sectors,
                                 int               x,
                                 int               y,
                                 int               z,
                                 int               radius)
{
	LIAlgRange range;

	/* Initialize self. */
	memset (self, 0, sizeof (LIAlgSectorsIter));
	self->sectors = sectors;
	self->next = 1;
	range = lialg_range_new_from_center (x, y, z, radius);
	range = lialg_range_clamp (range, 0, sectors->count - 1);
	if (!lialg_range_iter_first (&self->range, &range))
		return 0;

	/* Find first non-empty sector. */
	while (self->range.more)
	{
		self->sector = lialg_sectors_sector_index (sectors, self->range.index, 0);
		lialg_range_iter_next (&self->range);
		if (self->sector != NULL)
			return 1;
	}

	return 0;
}

static inline int
lialg_sectors_iter_first_point (LIAlgSectorsIter*  self,
                                LIAlgSectors*      sectors,
                                const LIMatVector* point,
                                float              radius)
{
	LIAlgRange range;

	/* Initialize self. */
	memset (self, 0, sizeof (LIAlgSectorsIter));
	self->sectors = sectors;
	self->next = 1;
	range = lialg_range_new_from_sphere (point, radius, sectors->width);
	range = lialg_range_clamp (range, 0, sectors->count - 1);
	if (!lialg_range_iter_first (&self->range, &range))
		return 0;

	/* Find first non-empty sector. */
	while (self->range.more)
	{
		self->sector = lialg_sectors_sector_index (sectors, self->range.index, 0);
		lialg_range_iter_next (&self->range);
		if (self->sector != NULL)
			return 1;
	}

	return 0;
}

#endif
