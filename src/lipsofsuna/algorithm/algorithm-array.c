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
 * \addtogroup LIAlg Algorithm
 * @{
 * \addtogroup LIAlgArray Array
 * @{
 */

#include "lipsofsuna/system.h"
#include "algorithm-array.h"

int
lialg_array_append_full (void*  self,
                         size_t size,
                         void*  data,
                         int    count)
{
	int num;
	void* tmp;
	LIAlgArray* array = (LIAlgArray*) self;

	lisys_assert (count >= 0);
	lisys_assert (size > 0);

	num = array->count + count;
	if (num > array->count)
	{
		tmp = lisys_realloc (array->array, num * size);
		if (tmp == NULL)
			return 0;
		array->array = tmp;
	}
	memcpy ((char*) array->array + array->count * size, data, count * size);
	array->count += count;
	return 1;
}

int
lialg_array_insert_full (void*  self,
                         size_t size,
                         int    index,
                         void*  data,
                         int    count)
{
	int num;
	void* tmp;
	LIAlgArray* array = (LIAlgArray*) self;

	lisys_assert (count >= 0);
	lisys_assert (size > 0);

	num = array->count + count;
	if (num > array->count)
	{
		tmp = lisys_realloc (array->array, num * size);
		if (tmp == NULL)
			return 0;
		array->array = tmp;
	}
	memmove ((char*) array->array + (index + count) * size,
	         (char*) array->array + index * size,
	         (array->count - index) * size);
	memcpy ((char*) array->array + index * size, data, count * size);
	array->count += count;
	return 1;
}

int
lialg_array_prepend_full (void*  self,
                          size_t size,
                          void*  data,
                          int    count)
{
	int num;
	void* tmp;
	LIAlgArray* array = (LIAlgArray*) self;

	lisys_assert (count >= 0);
	lisys_assert (size > 0);

	num = array->count + count;
	if (num > array->count)
	{
		tmp = lisys_realloc (array->array, num * size);
		if (tmp == NULL)
			return 0;
		array->array = tmp;
	}
	memmove ((char*) array->array + count * size,
	         (char*) array->array,
	         array->count * size);
	memcpy ((char*) array->array, data, count * size);
	array->count += count;
	return 1;
}

void
lialg_array_remove_full (void*  self,
                         size_t size,
                         int    index,
                         int    count)
{
	LIAlgArray* array = (LIAlgArray*) self;

	lisys_assert (size > 0);
	lisys_assert (index >= 0);
	lisys_assert (count <= array->count);
	lisys_assert (index + count <= array->count);

	memmove ((char*) array->array + index * size,
	         (char*) array->array + (index + count) * size,
	         (array->count - index - count) * size);
	array->count -= count;
}

int
lialg_array_resize_full (void*  self,
                         size_t size,
                         int    count)
{
	void* tmp;
	LIAlgArray* array = (LIAlgArray*) self;

	lisys_assert (size > 0);
	lisys_assert (count >= 0);

	tmp = lisys_realloc (array->array, size * count);
	if (tmp == NULL)
		return 0;
	array->count = count;
	array->array = tmp;
	return 1;
}

void
lialg_array_zero_full (void*  self,
                       size_t size,
                       int    index,
                       int    count)
{
	LIAlgArray* array = (LIAlgArray*) self;

	lisys_assert (size > 0);
	lisys_assert (index >= 0);
	lisys_assert (count <= array->count);
	lisys_assert (index + count <= array->count);

	memset ((char*) array->array + index * size, 0, count * size);
}

/** @} */
/** @} */
