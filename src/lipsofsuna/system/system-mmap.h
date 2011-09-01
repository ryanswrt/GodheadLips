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

#ifndef __SYSTEM_MMAP_H__
#define __SYSTEM_MMAP_H__

#include "system-compiler.h"

typedef struct _LISysMmap LISysMmap;

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LISysMmap*, lisys_mmap_open, (
	const char* path));

LIAPICALL (void, lisys_mmap_free, (
	LISysMmap* self));

LIAPICALL (void*, lisys_mmap_get_buffer, (
	LISysMmap* self));

LIAPICALL (int, lisys_mmap_get_size, (
	LISysMmap* self));

#ifdef __cplusplus
}
#endif

#endif
