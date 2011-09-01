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
 * \addtogroup LIScrUtil Util
 * @{
 */

#include "script.h"
#include "script-data.h"
#include "script-private.h"
#include "script-util.h"

#define ALL_USERDATA_IS_OURS

LIScrData* liscr_isanydata (
	lua_State* lua,
	int        arg)
{
#ifdef ALL_USERDATA_IS_OURS
	LIScrData* ptr;
	ptr = lua_touserdata (lua, arg);
	if (ptr == NULL || ptr->signature != 'D')
		return NULL;
	return ptr;
#else
	int ret;
	void* ptr;

	/* Check if userdata. */
	ptr = lua_touserdata (lua, arg);
	if (ptr == NULL)
		return NULL;

	/* Get the data lookup table. */
	lua_pushlightuserdata (lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (lua, -1) == LUA_TTABLE);

	/* Check if in the lookup table. */
	lua_pushlightuserdata (lua, ptr);
	lua_gettable (lua, -2);
	ret = lua_isuserdata (lua, -1);
	lua_pop (lua, 2);
	if (!ret)
		return NULL;

	return ptr;
#endif
}

/**
 * \brief Gets a userdata from stack.
 *
 * If the type check fails, NULL is returned.
 *
 * Consumes: 0.
 * Returns: 0.
 *
 * \param lua Lua state.
 * \param arg Stack index.
 * \param meta Class type.
 * \return Userdata owned by Lua or NULL.
 */
LIScrData* liscr_isdata (
	lua_State*  lua,
	int         arg,
	const char* meta)
{
	LIScrData* data;

	data = liscr_isanydata (lua, arg);
	if (data == NULL)
		return NULL;
	if (strcmp (data->type, meta))
		return NULL;

	return data;
}

/**
 * \brief Pushes the userdata to stack.
 *
 * Consumes: 0.
 * Returns: 1.
 *
 * \param lua Lua state.
 * \param object Pointer to script userdata.
 * \return Nonzero if succeded.
 */
int liscr_pushdata (
	lua_State* lua,
	LIScrData* object)
{
	/* Get the lookup table. */
	lua_pushlightuserdata (lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (lua, -1) == LUA_TTABLE);

	/* Get the userdata. */
	/* In some cases, the object might have been garbage collected when we
	   do this. We let the caller decide whether to catch the error or prevent
	   it by disabling garbage collection. */
	lua_pushlightuserdata (lua, object);
	lua_gettable (lua, -2);
	lua_remove (lua, -2);
	if (!lua_isuserdata (lua, -1))
	{
		lua_pop (lua, 1);
		return 0;
	}

	return 1;
}

/**
 * \brief Returns the current script.
 *
 * \param lua Lua state.
 * \return Script.
 */
LIScrScript* liscr_script (
	lua_State* lua)
{
	LIScrScript* script;

	lua_pushlightuserdata (lua, LISCR_SCRIPT_SELF);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	script = lua_touserdata (lua, -1);
	lua_pop (lua, 1);

	return script;
}

/**
 * \brief Dumpts the stack to the terminal.
 *
 * This function is intended to be used for debugging. It helps with locating
 * stack populatin and cleanup errors.
 *
 * \param lua Lua state.
 */
void liscr_stackdump (
	lua_State* lua)
{
	int i;

	printf ("\nLua stackdump:\n");
	for (i = 1 ; i <= lua_gettop (lua) ; i++)
		printf ("  %d: %s\n", i, lua_typename (lua, lua_type (lua, i)));
	printf ("\n");
}

/**
 * \brief Prints the traceback to the terminal.
 *
 * This function is intended to be used for debugging. It helps with locating
 * crashes and other problems that involve Lua calling the C functions.
 *
 * \param lua Lua state.
 */
void liscr_traceback (
	lua_State* lua)
{
	/* Get debug table. */
	lua_getfield (lua, LUA_GLOBALSINDEX, "debug");
	if (lua_type (lua, -1) != LUA_TTABLE)
	{
		lisys_error_set (EINVAL, "invalid debug table");
		lua_pop (lua, 1);
		return;
	}

	/* Get traceback function. */
	lua_getfield (lua, -1, "traceback");
	lua_remove (lua, -2);
	if (lua_type (lua, -1) != LUA_TFUNCTION)
	{
		lisys_error_set (EINVAL, "invalid traceback function");
		lua_pop (lua, 1);
		return;
	}

	/* Call traceback excluding itself from the trace. */
	lua_pushstring (lua, "");
	lua_pushinteger (lua, 1);
	if (lua_pcall (lua, 2, 1, 0) != 0)
	{
		lisys_error_set (EINVAL, lua_tostring (lua, -1));
		lisys_error_report ();
		lua_pop (lua, 1);
		return;
	}

	/* Print the traceback to the terminal. */
	printf ("\nLua traceback:\n%s\n\n", lua_tostring (lua, -1));
	lua_pop (lua, 1);
}

/** @} */
/** @} */
