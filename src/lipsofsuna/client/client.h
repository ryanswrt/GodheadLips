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

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "lipsofsuna/algorithm.h"
#include "lipsofsuna/callback.h"
#include "lipsofsuna/engine.h"
#include "lipsofsuna/main.h"
#include "lipsofsuna/paths.h"
#include "lipsofsuna/render.h"
#include "lipsofsuna/script.h"
#include "lipsofsuna/system.h"
#include "lipsofsuna/video.h"
#include "lipsofsuna/widget.h"
#include "client-server.h"
#include "client-types.h"
#include "client-window.h"

struct _LICliClient
{
	int active;
	int moving;
	LICliWindow* window;
	LIMaiProgram* program;
	LIRenRender* render;
	LIRenScene* scene;
	LICliServer* server;
	LISysThread* server_thread;
};

LIAPICALL (LICliClient*, licli_client_new, (
	LIMaiProgram* program,
	int           width,
	int           height,
	int           fullscreen,
	int           vsync));

LIAPICALL (void, licli_client_free, (
	LICliClient* self));

LIAPICALL (int, licli_client_host, (
	LICliClient* self,
	const char*  args));

LIAPICALL (int, licli_client_get_moving, (
	LICliClient* self));

LIAPICALL (void, licli_client_set_moving, (
	LICliClient* self,
	int          value));

#endif
