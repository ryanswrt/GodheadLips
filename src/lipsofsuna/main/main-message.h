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

#ifndef __MAIN_MESSAGE_H__
#define __MAIN_MESSAGE_H__

#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"

enum
{
	LIMAI_MESSAGE_QUEUE_PROGRAM,
	LIMAI_MESSAGE_QUEUE_THREAD,
	LIMAI_MESSAGE_QUEUE_MAX
};

enum
{
	LIMAI_MESSAGE_TYPE_EMPTY,
	LIMAI_MESSAGE_TYPE_MODEL,
	LIMAI_MESSAGE_TYPE_STRING
};

typedef struct _LIMaiMessage LIMaiMessage;
struct _LIMaiMessage
{
	int type;
	char* name;
	LIMaiMessage* next;
	LIMaiMessage* prev;
	union
	{
		char* string;
		LIMdlModel* model;
	};
};

LIAPICALL (LIMaiMessage*, limai_message_new, (
	int         type,
	const char* name,
	const void* data));

LIAPICALL (void, limai_message_free, (
	LIMaiMessage* self));

#endif
