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
 * \addtogroup LICliClient Client
 * @{
 */

#include "lipsofsuna/system.h"
#include "client.h"
#include "client-script.h"
#include "client-window.h"

#define ENABLE_GRABS

static void private_grab (
	LICliClient* self,
	int          value);

static int private_init (
	LICliClient*  self,
	LIMaiProgram* program);

static int private_event (
	LICliClient* client,
	SDL_Event*   event);

static void private_server_main (
	LISysThread* thread,
	void*        data);

static void private_server_shutdown (
	LICliClient* self);

static int private_tick (
	LICliClient* self,
	float        secs);

/*****************************************************************************/

LICliClient* licli_client_new (
	LIMaiProgram* program,
	int           width,
	int           height,
	int           fullscreen,
	int           vsync)
{
	LICliClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LICliClient));
	if (self == NULL)
		return NULL;
	self->active = 1;
	self->program = program;

	/* Initialize SDL. */
	if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
	{
		lisys_error_set (ENOTSUP, "initializing SDL failed");
		lisys_free (self);
		return NULL;
	}

	/* Create window. */
	SDL_EnableUNICODE (1);
	self->window = licli_window_new (self, width, height, fullscreen, vsync);
	if (self->window == NULL)
	{
		licli_client_free (self);
		return NULL;
	}
	SDL_ShowCursor (SDL_DISABLE);

	/* Load module. */
	if (!private_init (self, program))
	{
		licli_client_free (self);
		return NULL;
	}

	return self;
}

void licli_client_free (
	LICliClient* self)
{
	/* Invoke callbacks. */
	lical_callbacks_call (self->program->callbacks, "client-free", lical_marshal_DATA);

	/* Remove component. */
	if (self->program != NULL)
		limai_program_remove_component (self->program, "client");

	/* Free the server. */
	private_server_shutdown (self);

	if (self->scene != NULL)
		liren_scene_free (self->scene);
	if (self->render != NULL)
		liren_render_free (self->render);
	if (self->window != NULL)
		licli_window_free (self->window);

	lisys_free (self);
}

/**
 * \brief Starts an embedded server.
 *
 * \param self Client.
 * \param args Arguments to pass to the server.
 * \return Nonzero on success.
 */
int licli_client_host (
	LICliClient* self,
	const char*  args)
{
	/* Kill old thread. */
	private_server_shutdown (self);

	/* Create new server. */
	self->server = licli_server_new (self->program->paths->root, self->program->paths->module_name, args);
	if (self->server == NULL)
		return 0;

	/* Create server thread. */
	self->server_thread = lisys_thread_new (private_server_main, self);
	if (self->server_thread == NULL)
	{
		licli_server_free (self->server);
		self->server = NULL;
	}

	return 1;
}

/**
 * \brief Returns nonzero if movement mode is active.
 *
 * \param self Client.
 * \return Boolean.
 */
int licli_client_get_moving (
	LICliClient* self)
{
	return self->moving;
}

/**
 * \brief Enables or disables movement mode.
 *
 * When the movement mode is enabled, all mouse events are passed directly to
 * the scripts. Otherwise, the events are first passed to the user interface.
 *
 * \param self Client.
 * \param value Nonzero for movement mode, zero for user interface mode
 */
void licli_client_set_moving (
	LICliClient* self,
	int          value)
{
#ifdef ENABLE_GRABS
	self->moving = value;
	if (self->active && value)
		private_grab (self, 1);
	else
		private_grab (self, 0);
#else
	int cx;
	int cy;

	self->moving = value;
	if (value)
	{
		cx = self->window->mode.width / 2;
		cy = self->window->mode.height / 2;
		SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse (cx, cy);
		SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
	}
#endif
}

/*****************************************************************************/

static void private_grab (
	LICliClient* self,
	int          value)
{
#ifdef ENABLE_GRABS
	if (value)
		SDL_WM_GrabInput (SDL_GRAB_ON);
	else
		SDL_WM_GrabInput (SDL_GRAB_OFF);
#endif
}

static int private_init (
	LICliClient*  self,
	LIMaiProgram* program)
{
	/* Initialize renderer. */
	self->render = liren_render_new (self->program->paths);
	if (self->render == NULL)
		return 0;
	self->scene = liren_scene_new (self->render);
	if (self->scene == NULL)
		return 0;

	/* Register component. */
	if (!limai_program_insert_component (self->program, "client", self))
		return 0;

	/* Register classes. */
	liscr_script_set_userdata (program->script, LICLI_SCRIPT_CLIENT, self);
	licli_script_client (program->script);

	/* Register callbacks. */
	lical_callbacks_insert (program->callbacks, "event", -5, private_event, self, NULL);
	lical_callbacks_insert (program->callbacks, "tick", -1000, private_tick, self, NULL);

	return 1;
}

static int private_event (
	LICliClient* self,
	SDL_Event*   event)
{
	char* str = NULL;

	switch (event->type)
	{
		case SDL_JOYAXISMOTION:
			limai_program_event (self->program, "joystickmotion", "axis", LISCR_TYPE_INT, event->jaxis.axis + 1, "value", LISCR_TYPE_FLOAT, event->jaxis.value / 32768.0f, NULL);
			return 0;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			limai_program_event (self->program, (event->type == SDL_JOYBUTTONDOWN)? "joystickpress" : "joystickrelease", "button", LISCR_TYPE_INT, event->jbutton.button, NULL);
			return 0;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			limai_program_event (self->program, (event->type == SDL_MOUSEBUTTONDOWN)? "mousepress" : "mouserelease", "button", LISCR_TYPE_INT, event->button.button, "x", LISCR_TYPE_INT, event->button.x, "y", LISCR_TYPE_INT, event->button.y, NULL);
			return 0;
		case SDL_KEYDOWN:
			if (event->key.keysym.sym == SDLK_F4 &&
			   (event->key.keysym.mod & KMOD_ALT))
				self->program->quit = 1;
			if (event->key.keysym.sym == SDLK_F5 &&
			   (event->key.keysym.mod & KMOD_CTRL) &&
			   (event->key.keysym.mod & KMOD_SHIFT))
			{
				lical_callbacks_call (self->program->callbacks, "context-lost", lical_marshal_DATA_INT, 0);
				lical_callbacks_call (self->program->callbacks, "context-lost", lical_marshal_DATA_INT, 1);
			}
			/* Fall through */
		case SDL_KEYUP:
			if (event->key.keysym.unicode != 0)
				str = lisys_wchar_to_utf8 (event->key.keysym.unicode);
			if (str != NULL)
			{
				limai_program_event (self->program, (event->type == SDL_KEYDOWN)? "keypress" : "keyrelease", "code", LISCR_TYPE_INT, event->key.keysym.sym, "mods", LISCR_TYPE_INT, event->key.keysym.mod, "text", LISCR_TYPE_STRING, str, NULL);
				lisys_free (str);
			}
			else
			{
				limai_program_event (self->program, (event->type == SDL_KEYDOWN)? "keypress" : "keyrelease", "code", LISCR_TYPE_INT, event->key.keysym.sym, "mods", LISCR_TYPE_INT, event->key.keysym.mod, NULL);
			}
			return 0;
		case SDL_MOUSEMOTION:
			limai_program_event (self->program, "mousemotion", "x", LISCR_TYPE_INT, event->motion.x, "y", LISCR_TYPE_INT, event->motion.y, "dx", LISCR_TYPE_INT, event->motion.xrel, "dy", LISCR_TYPE_INT, event->motion.yrel, NULL);
			return 0;
		case SDL_QUIT:
			limai_program_event (self->program, "quit", NULL);
			break;
		case SDL_ACTIVEEVENT:
			if (event->active.state & SDL_APPINPUTFOCUS)
			{
				if (event->active.gain)
				{
					self->active = 1;
					self->program->sleep = 0;
					if (self->moving)
						private_grab (self, 1);
				}
				else
				{
					self->active = 0;
					self->program->sleep = 100000;
					if (self->moving)
						private_grab (self, 0);
				}
			}
			break;
		case SDL_VIDEORESIZE:
			self->window->mode.width = event->resize.w;
			self->window->mode.height = event->resize.h;
			break;
	}

	return 1;
}

static void private_server_main (
	LISysThread* thread,
	void*        data)
{
	LICliClient* self = data;

	if (!licli_server_main (self->server))
		lisys_error_report ();
	licli_server_free (self->server);
	self->server = NULL;
}

static void private_server_shutdown (
	LICliClient* self)
{
	/* Terminate the server if it's running. If the server closed on its own,
	   for example due to an error its scripts, the server program has already
	   been freed by the server thread but the thread still exists. */
	if (self->server != NULL)
		limai_program_shutdown (self->server->program);

	/* Free the server thread. The server program is guaranteed to be freed
	   by the server thread so all we need to do is to wait for the thread to
	   exit. This doesn't take long since we asked the server to quit already. */
	if (self->server_thread != NULL)
	{
		lisys_thread_free (self->server_thread);
		self->server_thread = NULL;
		lisys_assert (self->server == NULL);
	}
}

static int private_tick (
	LICliClient* self,
	float        secs)
{
#ifndef ENABLE_GRABS
	int x;
	int y;
	int cx;
	int cy;
#endif
	SDL_Event event;

	/* Invoke input callbacks. */
	while (SDL_PollEvent (&event))
		lical_callbacks_call (self->program->callbacks, "event", lical_marshal_DATA_PTR, &event);

	/* Pointer warping in movement mode. */
#ifndef ENABLE_GRABS
	if (self->moving)
	{
		cx = self->window->mode.width / 2;
		cy = self->window->mode.height / 2;
		SDL_GetMouseState (&x, &y);
		if (x != cx || y != cy)
		{
			SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouse (cx, cy);
			SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
		}
	}
#endif

	return 1;
}

/** @} */
/** @} */
