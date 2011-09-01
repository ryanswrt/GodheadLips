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
 * \addtogroup LISysMutex Mutex
 * @{
 */

#include <pthread.h>
#include "system-memory.h"
#include "system-mutex.h"

struct _LISysMutex
{
	pthread_mutex_t mutex;
};

LISysMutex* lisys_mutex_new ()
{
	LISysMutex* self;

	self = lisys_calloc (1, sizeof (LISysMutex));
	if (self == NULL)
		return NULL;
	pthread_mutex_init (&self->mutex, NULL);

	return self;
}

void lisys_mutex_free (
	LISysMutex* self)
{
	pthread_mutex_destroy (&self->mutex);
	lisys_free (self);
}

void lisys_mutex_lock (
	LISysMutex* self)
{
	pthread_mutex_lock (&self->mutex);
}

void lisys_mutex_unlock (
	LISysMutex* self)
{
	pthread_mutex_unlock (&self->mutex);
}

/** @} */
/** @} */
