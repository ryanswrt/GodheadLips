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

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenLod32 Lod32
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-private.h"

int liren_lod32_init (
	LIRenLod32* self,
	LIMdlModel* model,
	LIMdlLod*   lod)
{
	int i;
	LIMdlFaces* group;

	memset (self, 0, sizeof (LIRenLod32));

	/* Allocate face groups. */
	self->groups.count = lod->face_groups.count;
	if (self->groups.count)
	{
		self->groups.array = lisys_calloc (self->groups.count, sizeof (LIRenLodGroup32));
		if (self->groups.array == NULL)
			return 0;
	}

	/* Calculate face group offsets. */
	for (i = 0 ; i < self->groups.count ; i++)
	{
		group = lod->face_groups.array + i;
		self->groups.array[i].start = group->start;
		self->groups.array[i].count = group->count;
	}

	/* Initialize the render buffer. */
	if (!liren_mesh32_init (&self->mesh, lod->indices.array, lod->indices.count,
	     model->vertices.array, model->vertices.count))
		return 0;

	return 1;
}

void liren_lod32_clear (
	LIRenLod32* self)
{
	liren_mesh32_clear (&self->mesh);
	lisys_free (self->groups.array);
}
