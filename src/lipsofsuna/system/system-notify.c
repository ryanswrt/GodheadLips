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
 * \addtogroup LISysNotify Notify
 * @{
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#ifdef HAVE_POLL_H
#include <poll.h>
#endif
#ifdef HAVE_SYS_INOTIFY_H
#include <sys/inotify.h>
#define HAVE_INOTIFY
#endif
#ifdef HAVE_INOTIFYTOOLS_INOTIFY_H
#include <inotifytools/inotify.h>
#define HAVE_INOTIFY
#endif
#include "system-module.h"
#include "system-notify.h"
#include "system-types.h"

#ifdef LI_ENABLE_ERROR
#include "system-error.h"
#define error_memory() lisys_error_set (ENOMEM, NULL)
#define error_open() lisys_error_set (EIO, "cannot initialize inotify")
#define error_support() lisys_error_set (ENOTSUP, "file monitor not supported")
#else
#define error_memory()
#define error_open()
#define error_support()
#endif

struct _LISysNotify
{
	int fd;
#ifdef HAVE_INOTIFY
	struct
	{
		int (*inotify_init)();
		int (*inotify_add_watch)(int, const char*, uint32_t);
		int (*inotify_rm_watch)(int, int);
	} calls;
#endif
	struct
	{
		int pos;
		int length;
		int capacity;
		void* buffer;
	} buffer;
	LISysNotifyEvent event;
};

#ifdef HAVE_INOTIFY
static int
private_process (LISysNotify* self);

static int
private_read (LISysNotify* self);
#endif

/*****************************************************************************/

LISysNotify*
lisys_notify_new ()
{
#ifdef HAVE_INOTIFY
	LISysNotify* self;

	/* Allocate self. */
	self = calloc (1, sizeof (LISysNotify));
	if (self == NULL)
	{
		error_memory ();
		return NULL;
	}

	/* Find symbols. */
	self->calls.inotify_init = lisys_module_global_symbol ("c", "inotify_init");
	self->calls.inotify_add_watch = lisys_module_global_symbol ("c", "inotify_add_watch");
	self->calls.inotify_rm_watch = lisys_module_global_symbol ("c", "inotify_rm_watch");
	if (self->calls.inotify_init == NULL ||
	    self->calls.inotify_add_watch == NULL ||
	    self->calls.inotify_rm_watch == NULL)
	{
		error_support ();
		free (self);
		return NULL;
	}

	/* Allocate buffer. */
	self->buffer.capacity = sizeof (struct inotify_event) + 1024;
	self->buffer.buffer = malloc (self->buffer.capacity);
	if (self->buffer.buffer == NULL)
	{
		error_memory ();
		free (self);
		return NULL;
	}

	/* Initialize monitor. */
	self->fd = self->calls.inotify_init ();
	if (self->fd == -1)
	{
		error_open ();
		free (self->buffer.buffer);
		free (self);
		return NULL;
	}

	return self;
#else
	error_support ();
	return NULL;
#endif
}

void
lisys_notify_free (LISysNotify* self)
{
	close (self->fd);
	free (self->buffer.buffer);
	free (self);
}

int
lisys_notify_add (LISysNotify*     self,
                  const char*      path,
                  LISysNotifyFlags flags)
{
#ifdef HAVE_INOTIFY
	int f = 0;

	if (flags & LISYS_NOTIFY_CLOSER)
		f |= IN_CLOSE_NOWRITE;
	if (flags & LISYS_NOTIFY_CLOSEW)
		f |= IN_CLOSE_WRITE;
	if (flags & LISYS_NOTIFY_CREATE)
		f |= IN_CREATE;
	if (flags & LISYS_NOTIFY_DELETE)
		f |= IN_DELETE;
	if (flags & LISYS_NOTIFY_MODIFY)
		f |= IN_MODIFY;
	self->calls.inotify_add_watch (self->fd, path, f);
	/* FIXME: No error check. */

	return 1;
#else
	return 0;
#endif
}

void
lisys_notify_remove (LISysNotify* self,
                     const char*  path)
{
	/* FIXME: Not implemented. */
}

LISysNotifyEvent*
lisys_notify_poll (LISysNotify* self)
{
#ifdef HAVE_INOTIFY
	struct pollfd fds = { self->fd, POLLIN, 0 };

	if (private_process (self))
		return &self->event;
	while (1)
	{
		if (poll (&fds, 1, 0) <= 0)
			return NULL;
		if (!private_read (self))
			return NULL;
		if (private_process (self))
			return &self->event;
	}
#else
	return NULL;
#endif
}

LISysNotifyEvent*
lisys_notify_wait (LISysNotify* self)
{
#ifdef HAVE_INOTIFY
	if (private_process (self))
		return &self->event;
	while (1)
	{
		if (!private_read (self))
			return NULL;
		if (private_process (self))
			return &self->event;
	}
#else
	return NULL;
#endif
}

/*****************************************************************************/

#ifdef HAVE_INOTIFY
static int
private_process (LISysNotify* self)
{
	int i;
	struct inotify_event* event;

	while (self->buffer.pos < self->buffer.length)
	{
		event = self->buffer.buffer + self->buffer.pos;
		self->buffer.pos += sizeof (struct inotify_event) + event->len;
		if (event->len > 0)
		{
			/* FIXME: Why do we sometimes get an extra trailing '@'? */
			for (i = event->len - 1 ; i >= 0 ; i--)
			{
				if (event->name[i] != '\0')
				{
					if (event->name[i] == '@')
						event->name[i] = '\0';
					else
						break;
				}
			}

			if (event->mask & IN_CLOSE_NOWRITE)
			{
				self->event.name = event->name;
				self->event.flags = LISYS_NOTIFY_CLOSER;
				self->event.dir = ((event->mask & IN_ISDIR) != 0);
				return 1;
			}
			else if (event->mask & IN_CLOSE_WRITE)
			{
				self->event.name = event->name;
				self->event.flags = LISYS_NOTIFY_CLOSEW;
				self->event.dir = ((event->mask & IN_ISDIR) != 0);
				return 1;
			}
			else if (event->mask & IN_CREATE)
			{
				self->event.name = event->name;
				self->event.flags = LISYS_NOTIFY_CREATE;
				self->event.dir = ((event->mask & IN_ISDIR) != 0);
				self->event.write = 0;
				return 1;
			}
			else if (event->mask & IN_DELETE)
			{
				self->event.name = event->name;
				self->event.flags = LISYS_NOTIFY_DELETE;
				self->event.dir = ((event->mask & IN_ISDIR) != 0);
				self->event.write = 0;
				return 1;
			}
			else if (event->mask & IN_MODIFY)
			{
				self->event.name = event->name;
				self->event.flags = LISYS_NOTIFY_MODIFY;
				self->event.dir = ((event->mask & IN_ISDIR) != 0);
				self->event.write = 0;
				return 1;
			}
		}
	}

	return 0;
}

static int
private_read (LISysNotify* self)
{
	self->buffer.pos = 0;
	self->buffer.length = read (self->fd, self->buffer.buffer, self->buffer.capacity);
	if (self->buffer.length <= 0)
	{
		self->buffer.length = 0;
		return 0;
	}

	return 1;
}
#endif

/** @} */
/** @} */
