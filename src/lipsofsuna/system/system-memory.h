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

#ifndef __SYSTEM_MEMORY_H__
#define __SYSTEM_MEMORY_H__

#include <stdlib.h>
#include "system-compiler.h"

LIAPICALL (void*, lisys_calloc, (
	size_t num,
	size_t size));

LIAPICALL (void*, lisys_malloc, (
	size_t size));

LIAPICALL (void*, lisys_realloc, (
	void*  mem,
	size_t size));

LIAPICALL (void, lisys_free, (
	void* mem));

LIAPICALL (void, lisys_memchk, (
	void*  mem));

LIAPICALL (void, lisys_memstats, ());

#endif
