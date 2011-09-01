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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysThread Thread
 * @{
 */

#include <pthread.h>
#include "system-error.h"
#include "system-memory.h"
#include "system-thread.h"

struct _LISysThread
{
	pthread_t thread;
	int done;
	void* data;
	LISysThreadFunc func;
};

static void* private_thread (
	void* data);

/*****************************************************************************/

LISysThread* lisys_thread_new (
	LISysThreadFunc func,
	void*           data)
{
	LISysThread* self;

	self = lisys_calloc (1, sizeof (LISysThread));
	if (self == NULL)
		return NULL;
	self->data = data;
	self->func = func;
	if (pthread_create (&self->thread, NULL, private_thread, self) != 0)
	{
		lisys_error_set (ENOMEM, "not enough resources to create thread");
		lisys_free (self);
		return NULL;
	}

	return self;
}

void lisys_thread_free (
	LISysThread* self)
{
	lisys_thread_join (self);
	lisys_free (self);
}

void lisys_thread_join (
	LISysThread* self)
{
	pthread_join (self->thread, NULL);
}

int lisys_thread_call_get_done (
	LISysThread* self)
{
	return self->done;
}

/*****************************************************************************/

static void*
private_thread (void* data)
{
	LISysThread* self;

	self = data;
	self->func (self, self->data);
	self->done = 1;
	return NULL;
}

/** @} */
/** @} */
