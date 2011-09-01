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

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <lipsofsuna/system.h>
#include "script-types.h"

LIAPICALL (LIScrScript*, liscr_script_new, ());

LIAPICALL (void, liscr_script_free, (
	LIScrScript* self));

LIAPICALL (int, liscr_script_load_file, (
	LIScrScript* self,
	const char*  path,
	const char*  path_mod,
	const char*  path_core));

LIAPICALL (int, liscr_script_load_string, (
	LIScrScript* self,
	const char*  code,
	const char*  path_mod,
	const char*  path_core));

LIAPICALL (void, liscr_script_insert_cfunc, (
	LIScrScript*  self,
	const char*   clss,
	const char*   name,
	LIScrArgsFunc func));

LIAPICALL (void, liscr_script_insert_mfunc, (
	LIScrScript*  self,
	const char*   clss,
	const char*   name,
	LIScrArgsFunc func));

LIAPICALL (void, liscr_script_update, (
	LIScrScript* self,
	float        secs));

LIAPICALL (void, liscr_script_set_gc, (
	LIScrScript* self,
	int          value));

LIAPICALL (lua_State*, liscr_script_get_lua, (
	LIScrScript* self));

LIAPICALL (void*, liscr_script_get_userdata, (
	LIScrScript* self,
	const char*  key));

LIAPICALL (void, liscr_script_set_userdata, (
	LIScrScript* self,
	const char*  key,
	void*        value));

#endif
