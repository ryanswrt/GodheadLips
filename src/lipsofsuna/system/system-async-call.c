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

/**
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysAsyncCall AsyncCall
 * @{
 */

#include <pthread.h>
#include "system-error.h"
#include "system-memory.h"
#include "system-async-call.h"

struct _LISysAsyncCall
{
	pthread_t thread;
	int done;
	int result;
	int stop;
	float progress;
	void* data;
	LISysAsyncFunc func;
	LISysAsyncFunc free;
};

static void* private_thread (
	void* data);

/*****************************************************************************/

LISysAsyncCall* lisys_async_call_new (
	LISysAsyncFunc func,
	LISysAsyncFunc freecb,
	void*          data)
{
	LISysAsyncCall* self;

	self = lisys_calloc (1, sizeof (LISysAsyncCall));
	if (self == NULL)
		return NULL;
	self->data = data;
	self->func = func;
	self->free = freecb;
	if (pthread_create (&self->thread, NULL, private_thread, self) != 0)
	{
		lisys_error_set (ENOMEM, "not enough resources to create thread");
		lisys_free (self);
		return NULL;
	}

	return self;
}

void lisys_async_call_free (
	LISysAsyncCall* self)
{
	lisys_async_call_join (self);
	if (self->free != NULL)
		self->free (self, self->data);
	lisys_free (self);
}

int lisys_async_call_join (
	LISysAsyncCall* self)
{
	pthread_join (self->thread, NULL);
	return self->result;
}

void lisys_async_call_stop (
	LISysAsyncCall* self)
{
	self->stop = 1;
}

int lisys_async_call_get_done (
	LISysAsyncCall* self)
{
	return self->done;
}

float lisys_async_call_get_progress (
	LISysAsyncCall* self)
{
	return self->progress;
}

void lisys_async_call_set_progress (
	LISysAsyncCall* self,
	float           value)
{
	self->progress = value;
}

int lisys_async_call_get_result (
	LISysAsyncCall* self)
{
	return self->result;
}

void lisys_async_call_set_result (
	LISysAsyncCall* self,
	int             value)
{
	self->result = value;
}

int lisys_async_call_get_stop (
	LISysAsyncCall* self)
{
	return self->stop;
}

void* lisys_async_call_get_userdata (
	LISysAsyncCall* self)
{
	return self->data;
}

/*****************************************************************************/

static void* private_thread (
	void* data)
{
	LISysAsyncCall* self;

	self = data;
	self->func (self, self->data);
	self->done = 1;
	return NULL;
}

/** @} */
/** @} */
