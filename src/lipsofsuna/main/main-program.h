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

#ifndef __MAIN_PROGRAM_H__
#define __MAIN_PROGRAM_H__

#include <sys/time.h>
#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/script.h>
#include "main-extension.h"
#include "main-message.h"

#define LIMAI_PROGRAM_FPS_TICKS 32

typedef struct _LIMaiProgram LIMaiProgram;
struct _LIMaiProgram
{
	int sleep;
	int ticki;
	int quit;
	float fps;
	float tick;
	float ticks[LIMAI_PROGRAM_FPS_TICKS];
	char* args;
	char* launch_name;
	char* launch_args;
	struct timeval start;
	struct timeval curr_tick;
	struct timeval prev_tick;
	LIAlgRandom random;
	LIAlgSectors* sectors;
	LIAlgStrdic* components;
	LICalCallbacks* callbacks;
	LICalHandle calls[4];
	LIEngEngine* engine;
	LIMaiExtension* extensions;
	LIMaiMessage* messages[LIMAI_MESSAGE_QUEUE_MAX];
	LIMaiProgram* parent;
	LIPthPaths* paths;
	LIScrScript* script;
	LISysMutex* message_mutex;
};

LIAPICALL (LIMaiProgram*, limai_program_new, (
	const char* path,
	const char* name,
	const char* args));

LIAPICALL (void, limai_program_free, (
	LIMaiProgram* self));

LIAPICALL (void, limai_program_event, (
	LIMaiProgram* self,
	const char*   type,
	              ...)) LISYS_ATTR_SENTINEL;

LIAPICALL (void, limai_program_eventva, (
	LIMaiProgram* self,
	const char*   type,
	va_list       args));

LIAPICALL (int, limai_program_execute_script, (
	LIMaiProgram* self,
	const char*   file));

LIAPICALL (int, limai_program_execute_string, (
	LIMaiProgram* self,
	const char*   code));

LIAPICALL (void*, limai_program_find_component, (
	LIMaiProgram* self,
	const char*   name));

LIAPICALL (LIMaiExtension*, limai_program_find_extension, (
	LIMaiProgram* self,
	const char*   name));

LIAPICALL (int, limai_program_insert_component, (
	LIMaiProgram* self,
	const char*   name,
	void*         value));

LIAPICALL (int, limai_program_insert_extension, (
	LIMaiProgram* self,
	const char*   name));

LIAPICALL (LIMaiMessage*, limai_program_pop_message, (
	LIMaiProgram* self,
	int           queue));

LIAPICALL (int, limai_program_push_message, (
	LIMaiProgram* self,
	int           queue,
	int           type,
	const char*   name,
	const void*   data));

LIAPICALL (void, limai_program_remove_component, (
	LIMaiProgram* self,
	const char*   name));

LIAPICALL (void, limai_program_shutdown, (
	LIMaiProgram* self));

LIAPICALL (void, limai_program_unittest, (
	LIMaiProgram* self));

LIAPICALL (int, limai_program_update, (
	LIMaiProgram* self));

LIAPICALL (double, limai_program_get_time, (
	const LIMaiProgram* self));

#endif
