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
 * \addtogroup LICli Client
 * @{
 * \addtogroup LICliWindow Window
 * @{
 */

#include <lipsofsuna/system.h>
#include "client.h"
#include "client-window.h"

static int private_init_input (
	LICliWindow* self);

static int private_init_video (
	LICliWindow* self,
	int          width,
	int          height,
	int          fullscreen,
	int          vsync);

static int private_resize (
	LICliWindow* self,
	int          width,
	int          height,
	int          fullscreen,
	int          vsync);

/****************************************************************************/

LICliWindow* licli_window_new (
	LICliClient* client,
	int          width,
	int          height,
	int          fullscreen,
	int          vsync)
{
	LICliWindow* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LICliWindow));
	if (self == NULL)
		return NULL;
	self->client = client;

	/* Initialize subsystems. */
	if (!private_init_input (self) ||
	    !private_init_video (self, width, height, fullscreen, vsync))
	{
		licli_window_free (self);
		return NULL;
	}

	return self;
}

void licli_window_free (
	LICliWindow* self)
{
	if (self->joystick != NULL)
		SDL_JoystickClose (self->joystick);
	if (self->screen != NULL)
		SDL_FreeSurface (self->screen);
	if (TTF_WasInit ())
		TTF_Quit ();
	lisys_free (self);
}

int licli_window_get_fullscreen (
	LICliWindow* self)
{
	return self->mode.fullscreen;
}

void licli_window_get_size (
	const LICliWindow* self,
	int*               width,
	int*               height)
{
	if (width != NULL)
		*width = self->mode.width;
	if (height != NULL)
		*height = self->mode.height;
}

int licli_window_set_size (
	LICliWindow* self,
	int          width,
	int          height,
	int          full,
	int          sync)
{
	if (!private_resize (self, width, height, full, sync))
		return 0;
	return 1;
}

/****************************************************************************/

static int private_init_input (
	LICliWindow* self)
{
	self->joystick = SDL_JoystickOpen (0);
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	return 1;
}

static int private_init_video (
	LICliWindow* self,
	int          width,
	int          height,
	int          fullscreen,
	int          vsync)
{
	/* Create the window. */
	if (!private_resize (self, width, height, fullscreen, vsync))
		return 0;
	if (TTF_Init () == -1)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "cannot initialize SDL_ttf");
		return 0;
	}

	return 1;
}

static int private_resize (
	LICliWindow* self,
	int          width,
	int          height,
	int          fullscreen,
	int          sync)
{
	int i;
	Uint32 flags;
	SDL_Rect* best = NULL;
	SDL_Rect** modes;

	/* Determine screen surface flags. */
	if (fullscreen)
	{
		flags = SDL_OPENGL | SDL_FULLSCREEN;
		modes = SDL_ListModes (NULL, flags);
		if (modes != NULL && modes != (SDL_Rect**) -1)
		{
			/* Determine the best possible fullscreen mode. */
			for (i = 0 ; modes[i] ; i++)
			{
				if (best == NULL ||
				   (LIMAT_ABS (modes[i]->w - width) < LIMAT_ABS (best->w - width) &&
				    LIMAT_ABS (modes[i]->h - height) < LIMAT_ABS (best->h - height)))
					best = modes[i];
			}
		}
		if (best != NULL)
		{
			/* Set the resolution to the best mode found. */
			width = best->w;
			height = best->h;
		}
		else
		{
			/* Revert to windowed mode if no fullscreen modes. */
			flags = SDL_OPENGL | SDL_RESIZABLE;
			fullscreen = 0;
		}
	}
	else
		flags = SDL_OPENGL | SDL_RESIZABLE;

	/* Unload all graphics. */
	/* Since changing the video mode erases the OpenGL context in Windows,
	   we have to unload all textures, shaders, vertex buffers, etc. */
#ifdef WIN32
	if (self->client->render != NULL)
		lical_callbacks_call (self->client->program->callbacks, "context-lost", lical_marshal_DATA_INT, 0);
#endif

	/* Recreate surface. */
	/* This destroys all graphics data in Windows. */
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 0);
	if (sync)
		SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, 1);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	self->screen = SDL_SetVideoMode (width, height, 0, flags);
	if (self->screen == NULL)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "cannot set video mode");
		return 0;
	}

	/* Initialize libraries. */
	if (!livid_video_init ())
		return 0;

	/* Store mode. */
	self->mode.width = width;
	self->mode.height = height;
	self->mode.fullscreen = fullscreen;
	self->mode.vsync = sync;

	/* Reload all graphics. */
	/* Since changing the video mode erases the OpenGL context in Windows,
	   we have to reload all textures, shaders, vertex buffers, etc. */
#ifdef WIN32
	if (self->client->render != NULL)
		lical_callbacks_call (self->client->program->callbacks, "context-lost", lical_marshal_DATA_INT, 1);
#endif

	return 1;
}

/** @} */
/** @} */
