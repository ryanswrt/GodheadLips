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

#ifndef __SCRIPT_PRIVATE_H__
#define __SCRIPT_PRIVATE_H__

#include <lua.h>
#include <lauxlib.h>
#include <string.h>
#include <lipsofsuna/algorithm.h>
#include "script.h"
#include "script-types.h"

#define LISCR_SCRIPT_SELF (NULL + 1)
#define LISCR_SCRIPT_REFS (NULL + 2)
#define LISCR_SCRIPT_LOOKUP_DATA (NULL + 3)

struct _LIScrData
{
	char signature;
	const char* type;
	LIScrScript* script;
	LIScrGCFunc free;
	void* data;
	int refcount;
};

struct _LIScrScript
{
	lua_State* lua;
	LIAlgStrdic* userdata;
};

#endif
