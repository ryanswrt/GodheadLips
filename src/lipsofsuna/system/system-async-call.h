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

#ifndef __SYSTEM_ASYNC_CALL_H__
#define __SYSTEM_ASYNC_CALL_H__

#include "system-memory.h"

typedef struct _LISysAsyncCall LISysAsyncCall;
typedef void (*LISysAsyncFunc)(LISysAsyncCall*, void*);

LIAPICALL (LISysAsyncCall*, lisys_async_call_new, (
	LISysAsyncFunc func,
	LISysAsyncFunc freecb,
	void*          data));

LIAPICALL (void, lisys_async_call_free, (
	LISysAsyncCall* self));

LIAPICALL (int, lisys_async_call_join, (
	LISysAsyncCall* self));

LIAPICALL (void, lisys_async_call_stop, (
	LISysAsyncCall* self));

LIAPICALL (int, lisys_async_call_get_done, (
	LISysAsyncCall* self));

LIAPICALL (float, lisys_async_call_get_progress, (
	LISysAsyncCall* self));

LIAPICALL (void, lisys_async_call_set_progress, (
	LISysAsyncCall* self,
	float           value));

LIAPICALL (int, lisys_async_call_get_result, (
	LISysAsyncCall* self));

LIAPICALL (void, lisys_async_call_set_result, (
	LISysAsyncCall* self,
	int             value));

LIAPICALL (int, lisys_async_call_get_stop, (
	LISysAsyncCall* self));

LIAPICALL (void*, lisys_async_call_get_userdata, (
	LISysAsyncCall* self));

#endif
