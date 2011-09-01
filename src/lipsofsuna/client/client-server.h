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

#ifndef __SERVER_H__
#define __SERVER_H__

#include "lipsofsuna/archive.h"
#include "lipsofsuna/main.h"

typedef struct _LICliServer LICliServer;
struct _LICliServer
{
	/* Server. */
	LIMaiProgram* program;

	/* Program. */
	LIAlgSectors* sectors;
	LICalCallbacks* callbacks;
	LIEngEngine* engine;
	LIPthPaths* paths;
	LIScrScript* script;
};

LIAPICALL (LICliServer*, licli_server_new, (
	const char* path,
	const char* name,
	const char* args));

LIAPICALL (void, licli_server_free, (
	LICliServer* self));

LIAPICALL (int, licli_server_main, (
	LICliServer* self));

#endif
