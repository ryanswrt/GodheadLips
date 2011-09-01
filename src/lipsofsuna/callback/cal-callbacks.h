/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef __CAL_CALLBACKS_H__
#define __CAL_CALLBACKS_H__

#include "lipsofsuna/algorithm.h"
#include "lipsofsuna/system.h"
#include "cal-handle.h"
#include "cal-marshal.h"

struct _LICalCallfunc
{
	int prio;
	int removed;
	void* call;
	void* data;
	LICalCallfunc* prev;
	LICalCallfunc* next;
};

struct _LICalCallbacks
{
	LIAlgStrdic* types;
	LICalCallfunc* removed;
};

LIAPICALL (LICalCallbacks*, lical_callbacks_new, ());

LIAPICALL (void, lical_callbacks_free, (
	LICalCallbacks* self));

LIAPICALL (void, lical_callbacks_call, (
	LICalCallbacks* self,
	const char*     type,
	LICalMarshal    marshal,
	                ...));

LIAPICALL (int, lical_callbacks_insert, (
	LICalCallbacks* self,
	const char*     type,
	int             priority,
	void*           call,
	void*           data,
	LICalHandle*    result));

LIAPICALL (void, lical_callbacks_update, (
	LICalCallbacks* self));

#endif
