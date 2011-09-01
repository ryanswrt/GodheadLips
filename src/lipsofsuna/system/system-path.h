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

#ifndef __SYSTEM_PATH_H__
#define __SYSTEM_PATH_H__

#include "system-compiler.h"

#define LISYS_PATH_BASENAME ((char*) -1)

#define LISYS_PATH_PATHNAME ((char*) -2)

#define LISYS_PATH_SEPARATOR ((char*) -3)

#define LISYS_PATH_STRIPEXT ((char*) -4)

#define LISYS_PATH_STRIPEXTS ((char*) -5)

#define LISYS_PATH_STRIPLAST ((char*) -6)

typedef const char* lisysPathFormat;

LIAPICALL (char*, lisys_path_basename, (
	const char* path));

LIAPICALL (int, lisys_path_check_ext, (
	const char* path,
	const char* ext));

LIAPICALL (char*, lisys_path_concat, (
	const char* path,
	            ...)) LISYS_ATTR_SENTINEL;

LIAPICALL (char*, lisys_path_format, (
	lisysPathFormat format,
	                ...)) LISYS_ATTR_SENTINEL;

LIAPICALL (char*, lisys_path_filename, (
	const char* path));

LIAPICALL (char*, lisys_path_fileext, (
	const char* path));

LIAPICALL (char*, lisys_path_pathname, (
	const char* path));

#endif
