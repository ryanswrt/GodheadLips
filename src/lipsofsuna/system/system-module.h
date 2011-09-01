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

#include "system-compiler.h"

enum
{
	LISYS_MODULE_FLAG_GLOBAL  = 0x01,
	LISYS_MODULE_FLAG_LIBDIRS = 0x02
};

typedef struct _LISysModule LISysModule;

LIAPICALL (LISysModule*, lisys_module_new, (
	const char* path,
	int         flags));

LIAPICALL (void, lisys_module_free, (
	LISysModule* self));

LIAPICALL (void*, lisys_module_symbol, (
	LISysModule* self,
	const char*  name));

LIAPICALL (void*, lisys_module_global_symbol, (
	const char* module,
	const char* name));
