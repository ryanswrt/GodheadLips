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

#ifndef __ALGORITHM_ARRAY_H__
#define __ALGORITHM_ARRAY_H__

#include <lipsofsuna/system.h>

typedef struct _LIAlgArray LIAlgArray;
struct _LIAlgArray
{
	int count;
	void* array;
};

#define lialg_array_append(self, data) \
	lialg_array_append_full ((self), sizeof (*((self)->array)), data, 1)

#define lialg_array_appendv(self, data, count) \
	lialg_array_append_full ((self), sizeof (*((self)->array)), data, count)

#define lialg_array_insert(self, index, data) \
	lialg_array_insert_full ((self), sizeof (*((self)->array)), index, data, 1)

#define lialg_array_insertv(self, index, data, count) \
	lialg_array_insert_full ((self), sizeof (*((self)->array)), index, data, count)

#define lialg_array_prepend(self, data) \
	lialg_array_prepend_full ((self), sizeof (*((self)->array)), data, 1)

#define lialg_array_prependv(self, data, count) \
	lialg_array_prepend_full ((self), sizeof (*((self)->array)), data, count)

#define lialg_array_remove(self, index) \
	lialg_array_remove_full ((self), sizeof (*((self)->array)), index, 1)

#define lialg_array_removev(self, index, count) \
	lialg_array_remove_full ((self), sizeof (*((self)->array)), index, count)

#define lialg_array_resize(self, count) \
	lialg_array_resize_full((self), sizeof (*((self)->array)), count)

#define lialg_array_zero(self, index, count) \
	lialg_array_zero_full((self), sizeof (*((self)->array)), index, count)

LIAPICALL (int, lialg_array_append_full, (
	void*  self,
	size_t size,
	void*  data,
	int    count));

LIAPICALL (int, lialg_array_insert_full, (
	void*  self,
	size_t size,
	int    index,
	void*  data,
	int    count));

LIAPICALL (int, lialg_array_prepend_full, (
	void*  self,
	size_t size,
	void*  data,
	int    count));

LIAPICALL (void, lialg_array_remove_full, (
	void*  self,
	size_t size,
	int    index,
	int    count));

LIAPICALL (int, lialg_array_resize_full, (
	void*  self,
	size_t size,
	int    count));

LIAPICALL (void, lialg_array_zero_full, (
	void*  self,
	size_t size,
	int    index,
	int    count));

#endif
