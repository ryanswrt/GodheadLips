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

/**
 * \addtogroup LIEng Engine
 * @{
 * \addtogroup LIEngSector Sector
 * @{
 */

#include "lipsofsuna/system.h"
#include "engine-sector.h"

/**
 * \brief Creates a new sector.
 * \param sector Sector manager sector.
 * \return New sector or NULL.
 */
LIEngSector* lieng_sector_new (
	LIAlgSector* sector)
{
	LIEngSector* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngSector));
	if (self == NULL)
		return NULL;
	self->engine = lialg_sectors_get_userdata (sector->manager, LIALG_SECTORS_CONTENT_ENGINE);
	self->sector = sector;

	/* Allocate tree. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		lieng_sector_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the sector.
 * \param self Sector.
 */
void lieng_sector_free (
	LIEngSector* self)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;

	/* Remove objects. */
	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
		{
			object = iter.value;
			lieng_object_set_realized (object, 0);
		}
		lialg_u32dic_free (self->objects);
	}

	lisys_free (self);
}

/** @} */
/** @} */
