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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrData Data
 * @{
 */

#include "script.h"
#include "script-data.h"
#include "script-private.h"
#include "script-util.h"

static int private_gc (
	lua_State* lua);

/*****************************************************************************/

/**
 * \brief Allocates a script userdata object.
 *
 * The created userdata is pushed to the top of the Lua stack.
 *
 * \param script Script.
 * \param lua Lua state whose stack will contain the userdata.
 * \param data Wrapped data.
 * \param type Type identifier string.
 * \param free Free function called by garbage collector.
 * \return New script userdata or NULL.
 */
LIScrData* liscr_data_new (
	LIScrScript* script,
	lua_State*   lua,
	void*        data,
	const char*  type,
	LIScrGCFunc  free)
{
	LIScrData* object;

	/* Allocate object. */
	object = lua_newuserdata (lua, sizeof (LIScrData));
	if (object == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	memset (object, 0, sizeof (LIScrData));
	object->signature = 'D';
	object->type = type;
	object->script = script;
	object->data = data;
	object->free = free;
	lua_newtable (lua);
	lua_pushcfunction (lua, private_gc);
	lua_setfield (lua, -2, "__gc");
	lua_setmetatable (lua, -2);

	/* Add to lookup table. */
	lua_pushlightuserdata (lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (lua, -1) == LUA_TTABLE);
	lua_pushlightuserdata (lua, object);
	lua_pushvalue (lua, -3);
	lua_settable (lua, -3);
	lua_pop (lua, 1);

	/* Create private table. */
	lua_newtable (lua);
	lua_setfenv (lua, -2);

	return object;
}

/**
 * \brief Allocates a script userdata object.
 *
 * Like #liscr_data_new but allocates a new wrapped data.
 *
 * \param script Script.
 * \param lua Lua state whose stack will contain the userdata.
 * \param size Wrapped data size.
 * \param type Type identifier string.
 * \return New script userdata or NULL.
 */
LIScrData* liscr_data_new_alloc (
	LIScrScript* script,
	lua_State*   lua,
	size_t       size,
	const char*  type)
{
	void* data;
	LIScrData* self;

	data = lisys_calloc (1, size);
	if (data == NULL)
		return NULL;
	self = liscr_data_new (script, lua, data, type, lisys_free);
	if (self == NULL)
	{
		lisys_free (data);
		return NULL;
	}

	return self;
}

/**
 * \brief Called in the garbage collection routines.
 *
 * All the values referenced by the userdata are garbage
 * collected automatically since they are stored to the
 * private table, which is always collected at the same
 * time with the userdata. What is left for us to do is
 * removing some values from the lookup tables.
 *
 * \param object Script userdata.
 */
void liscr_data_free (
	LIScrData* object)
{
	LIScrScript* script = object->script;

	/* Call free function. */
	if (object->free != NULL)
		object->free (object->data, object);

	/* Remove from lookup table. */
	lua_pushlightuserdata (script->lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_gettable (script->lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (script->lua, -1) == LUA_TTABLE);
	lua_pushlightuserdata (script->lua, object);
	lua_pushnil (script->lua);
	lua_settable (script->lua, -3);
	lua_pop (script->lua, 1);
}

/**
 * \brief Gets the C data stored to the userdata.
 * \param self Script userdata.
 * \return Script.
 */
void* liscr_data_get_data (
	LIScrData* self)
{
	return self->data;
}

/**
 * \brief Gets the script that owns this userdata.
 * \param self Script userdata.
 * \return Script.
 */
LIScrScript* liscr_data_get_script (
	LIScrData* self)
{
	return self->script;
}

/*****************************************************************************/

static int private_gc (
	lua_State* lua)
{
	char* data;

	data = lua_touserdata (lua, 1);
	lisys_assert (data != NULL);
	lisys_assert (*data == 'D');
	liscr_data_free ((void*) data);

	return 0;
}

/** @} */
/** @} */
