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

#ifndef __SYSTEM_DIRECTORY_H__
#define __SYSTEM_DIRECTORY_H__

#include "system-compiler.h"

typedef int (*LISysDirFilter)(const char* dir, const char* name, void* data);
typedef int (*LISysDirSorter)(const char** name0, const char** name1);
typedef struct _LISysDir LISysDir;
struct _LISysDir;

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LISysDir*, lisys_dir_open, (
	const char* path));

LIAPICALL (void, lisys_dir_free, (
	LISysDir* self));

LIAPICALL (int, lisys_dir_scan, (
	LISysDir* self));

LIAPICALL (int, lisys_dir_get_count, (
	const LISysDir* self));

LIAPICALL (const char*, lisys_dir_get_name, (
	const LISysDir* self,
	int             i));

LIAPICALL (char*, lisys_dir_get_path, (
	const LISysDir* self,
	int             i));

LIAPICALL (void, lisys_dir_set_filter, (
	LISysDir*      self,
	LISysDirFilter filter,
	void*          data));

LIAPICALL (void, lisys_dir_set_sorter, (
	LISysDir*      self,
	LISysDirSorter sorter));

LIAPICALL (int, lisys_dir_filter_files, (
	const char* dir,
	const char* name,
	void*       data));

LIAPICALL (int, lisys_dir_filter_dirs, (
	const char* dir,
	const char* name,
	void*       data));

LIAPICALL (int, lisys_dir_filter_hidden, (
	const char* dir,
	const char* name,
	void*       data));

LIAPICALL (int, lisys_dir_filter_visible, (
	const char* dir,
	const char* name,
	void*       data));

LIAPICALL (int, lisys_dir_sorter_alpha, (
	const char** name0,
	const char** name1));

#ifdef __cplusplus
}
#endif

#endif
