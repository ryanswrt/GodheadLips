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

#ifndef __SCRIPT_DATA_H__
#define __SCRIPT_DATA_H__

#include "lipsofsuna/system.h"
#include "script-types.h"

LIAPICALL (LIScrData*, liscr_data_new, (
	LIScrScript* script,
	lua_State*   lua,
	void*        data,
	const char*  type,
	LIScrGCFunc  free));

LIAPICALL (LIScrData*, liscr_data_new_alloc, (
	LIScrScript* script,
	lua_State*   lua,
	size_t       size,
	const char*  type));

LIAPICALL (void, liscr_data_free, (
	LIScrData* object));

LIAPICALL (void*, liscr_data_get_data, (
	LIScrData* self));

LIAPICALL (LIScrScript*, liscr_data_get_script, (
	LIScrData* self));

#endif
