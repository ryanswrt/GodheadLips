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
 * \addtogroup LIEngEngine Engine
 * @{
 */

#include <stdarg.h>
#include "engine.h"

static int
private_init (LIEngEngine* self);

static void
private_sector_free (void*        self,
                     LIAlgSector* sector);

static void
private_sector_load (void*        self,
                     LIAlgSector* sector);

/*****************************************************************************/

/**
 * \brief Creates a new game engine.
 * \param calls Callback manager.
 * \param sectors Sector manager.
 * \param paths Paths used for loading data files.
 * \return New engine or NULL.
 */
LIEngEngine* lieng_engine_new (
	LICalCallbacks* calls,
	LIAlgSectors*   sectors,
	LIPthPaths*     paths)
{
	LIEngEngine* self;

	self = lisys_calloc (1, sizeof (LIEngEngine));
	if (self == NULL)
		return NULL;
	lialg_random_init (&self->random, lisys_time (NULL));
	self->callbacks = calls;
	self->sectors = sectors;
	self->sectors->sector_free_callback.callback = private_sector_free;
	self->sectors->sector_free_callback.userdata = self;
	self->sectors->sector_load_callback.callback = private_sector_load;
	self->sectors->sector_load_callback.userdata = self;
	self->paths = paths;
	if (!private_init (self))
	{
		lieng_engine_free (self);
		lisys_error_append ("cannot initialize engine");
		return NULL;
	}

	return self;
}

void
lieng_engine_free (LIEngEngine* self)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;

	/* Clear objects. */
	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
		{
			object = iter.value;
			lieng_object_set_realized (object, 0);
		}
		while (self->objects->list)
		{
			object = self->objects->list->value;
			lieng_object_free (object);
		}
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->callbacks, "free", lical_marshal_DATA_PTR, self);

	/* Clear sector data. */
	if (self->sectors != NULL)
	{
		lialg_sectors_remove_content (self->sectors, LIALG_SECTORS_CONTENT_ENGINE);
		self->sectors->sector_free_callback.callback = NULL;
		self->sectors->sector_free_callback.userdata = NULL;
		self->sectors->sector_load_callback.callback = NULL;
		self->sectors->sector_load_callback.userdata = NULL;
	}

	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);
	if (self->models != NULL)
		lialg_u32dic_free (self->models);
	lisys_free (self);
}

/**
 * \brief Find an object by id.
 *
 * \param self Engine.
 * \param id Object number.
 * \return Object or NULL.
 */
LIEngObject*
lieng_engine_find_object (LIEngEngine* self,
                          uint32_t     id)
{
	return lialg_u32dic_find (self->objects, id);
}

/**
 * \brief Updates the scene.
 * \param self Engine.
 * \param secs Number of seconds since the last update.
 */
void lieng_engine_update (
	LIEngEngine* self,
	float        secs)
{
	/* Maintain callbacks. */
	lical_callbacks_update (self->callbacks);
}

/*****************************************************************************/

static int
private_init (LIEngEngine* self)
{
	/* Objects. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
		return 0;

	/* Models. */
	self->models = lialg_u32dic_new ();
	if (self->models == NULL)
		return 0;

	/* Sectors. */
	if (!lialg_sectors_insert_content (self->sectors, LIALG_SECTORS_CONTENT_ENGINE, self,
	     (LIAlgSectorFreeFunc) lieng_sector_free,
	     (LIAlgSectorLoadFunc) lieng_sector_new))
		return 0;

	return 1;
}

static void private_sector_free (
	void*        self,
	LIAlgSector* sector)
{
	LIEngEngine* engine = self;

	/* Invoke callbacks. */
	lical_callbacks_call (engine->callbacks, "sector-free", lical_marshal_DATA_INT, sector->index);
}

static void private_sector_load (
	void*        self,
	LIAlgSector* sector)
{
	LIEngEngine* engine = self;

	/* Invoke callbacks. */
	lical_callbacks_call (engine->callbacks, "sector-load", lical_marshal_DATA_INT, sector->index);
}

/** @} */
/** @} */
