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

#ifndef __ALGORITHM_LIST_H__
#define __ALGORITHM_LIST_H__

#include <lipsofsuna/system.h>

typedef struct _LIAlgList LIAlgList;
struct _LIAlgList
{
	LIAlgList* next;
	LIAlgList* prev;
	void* data;
};

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*LIAlgListForeach)(void*);

typedef int (*LIAlgListCompare)(const void*, const void*);

LIAPICALL (void, lialg_list_free, (
	LIAlgList* self));

LIAPICALL (int, lialg_list_get_length, (
	LIAlgList* self));

LIAPICALL (void, lialg_list_foreach, (
	LIAlgList**      self,
	LIAlgListForeach call));

LIAPICALL (int, lialg_list_append, (
	LIAlgList** self,
	void*       data));

LIAPICALL (void, lialg_list_append_node, (
	LIAlgList** self,
	LIAlgList*  node));

LIAPICALL (int, lialg_list_prepend, (
	LIAlgList** self,
	void*       data));

LIAPICALL (void, lialg_list_prepend_node, (
	LIAlgList** self,
	LIAlgList*  node));

LIAPICALL (int, lialg_list_prepend_sorted, (
	LIAlgList**      self,
	void*            data,
	LIAlgListCompare cmp));

LIAPICALL (void, lialg_list_detach_node, (
	LIAlgList** self,
	LIAlgList*  node));

LIAPICALL (void, lialg_list_remove, (
	LIAlgList** self,
	LIAlgList*  node));

#ifdef __cplusplus
}
#endif

#endif
