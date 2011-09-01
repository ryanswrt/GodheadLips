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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtWatchdog Watchdog
 * @{
 */

#include "ext-module.h"

static void private_worker_thread (
	LISysAsyncCall* call,
	void*           data);

/*****************************************************************************/

LIMaiExtensionInfo liext_watchdog_info =
{
	LIMAI_EXTENSION_VERSION, "Watchdog",
	liext_watchdog_new,
	liext_watchdog_free
};

LIExtModule* liext_watchdog_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Initialize the mutex. */
	self->mutex = lisys_mutex_new ();
	if (self->mutex == NULL)
	{
		liext_watchdog_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_WATCHDOG, self);
	liext_script_watchdog (program->script);

	return self;
}

void liext_watchdog_free (
	LIExtModule* self)
{
	if (self->worker != NULL)
	{
		lisys_async_call_stop (self->worker);
		lisys_async_call_free (self->worker);
	}
	if (self->mutex != NULL)
		lisys_mutex_free (self->mutex);
	lisys_free (self);
}

int liext_watchdog_start (
	LIExtModule* self,
	float        secs)
{
	/* Set the timer. */
	lisys_mutex_lock (self->mutex);
	self->timer = secs;
	lisys_mutex_unlock (self->mutex);

	/* Start the thread. */
	if (self->worker == NULL)
	{
		self->worker = lisys_async_call_new (private_worker_thread, NULL, self);
		if (self->worker == NULL)
			return 0;
	}

	return 1;
}

void liext_watchdog_stop (
	LIExtModule* self)
{
	/* Stop the thread. */
	if (self->worker != NULL)
	{
		lisys_async_call_stop (self->worker);
		lisys_async_call_free (self->worker);
		self->worker = NULL;
	}
}

/*****************************************************************************/

static void private_worker_thread (
	LISysAsyncCall* call,
	void*           data)
{
	float t;
	LIExtModule* self = data;

	while (!lisys_async_call_get_stop (call))
	{
		lisys_mutex_lock (self->mutex);
		self->timer -= 0.05f;
		t = self->timer;
		lisys_mutex_unlock (self->mutex);
		if (t <= 0.0f)
		{
			lisys_error_set (EINVAL, "the game has frozen");
			lisys_error_report ();
			abort();
		}
		lisys_usleep (50000);
	}
}

/** @} */
/** @} */
