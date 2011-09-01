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

#ifndef __CLIENT_WINDOW_H__
#define __CLIENT_WINDOW_H__

#include <lipsofsuna/system.h>
#include <lipsofsuna/video.h>
#include "client-types.h"

struct _LICliWindow
{
	LICliClient* client;
	SDL_Joystick* joystick;
	SDL_Surface* screen;
	struct
	{
		int width;
		int height;
		int fullscreen;
		int vsync;
	} mode;
};

LIAPICALL (LICliWindow*, licli_window_new, (
	LICliClient* client,
	int          width,
	int          height,
	int          fullscreen,
	int          vsync));

LIAPICALL (void, licli_window_free, (
	LICliWindow* self));

LIAPICALL (void, licli_window_get_size, (
	const LICliWindow* self,
	int*               width,
	int*               height));

LIAPICALL (int, licli_window_set_size, (
	LICliWindow* self,
	int          width,
	int          height,
	int          full,
	int          sync));

#endif

