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

#ifndef __SYSTEM_NOTIFY_H__
#define __SYSTEM_NOTIFY_H__

#include "system-compiler.h"

typedef int LISysNotifyFlags;
enum _LISysNotifyFlags
{
	LISYS_NOTIFY_CLOSER = 0x01,
	LISYS_NOTIFY_CLOSEW = 0x02,
	LISYS_NOTIFY_CLOSE = 0x03,
	LISYS_NOTIFY_CREATE = 0x04,
	LISYS_NOTIFY_DELETE = 0x08,
	LISYS_NOTIFY_MODIFY = 0x10,
	LISYS_NOTIFY_ALL = 0xFF
};

typedef struct _LISysNotifyEvent LISysNotifyEvent;
struct _LISysNotifyEvent
{
	int dir;
	int write;
	const char* name;
	LISysNotifyFlags flags;
};

typedef struct _LISysNotify LISysNotify;

LIAPICALL (LISysNotify*, lisys_notify_new, ());

LIAPICALL (void, lisys_notify_free, (
	LISysNotify* self));

LIAPICALL (int, lisys_notify_add, (
	LISysNotify*     self,
	const char*      path,
	LISysNotifyFlags flags));

LIAPICALL (void, lisys_notify_remove, (
	LISysNotify* self,
	const char*  path));

LIAPICALL (LISysNotifyEvent*, lisys_notify_poll, (
	LISysNotify* self));

LIAPICALL (LISysNotifyEvent*, lisys_notify_wait, (
	LISysNotify* self));

#endif
