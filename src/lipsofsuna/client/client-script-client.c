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
 * \addtogroup LICliScript Script
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/client.h>

static void Client_clear_buffer (LIScrArgs* args)
{
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void Client_host (LIScrArgs* args)
{
	const char* str = "--server";
	LICliClient* client;

	if (!liscr_args_gets_string (args, "args", &str))
	    liscr_args_geti_string (args, 0, &str);

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	if (!licli_client_host (client, str))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

static void Client_screenshot (LIScrArgs* args)
{
	int i;
	int width;
	int height;
	int pitch;
	char* home;
	char* file;
	char* path;
	uint32_t rmask;
	uint32_t gmask;
	uint32_t bmask;
	uint32_t amask;
	uint8_t* pixels;
	SDL_Surface* surface;
	LICliClient* client;

	/* Get window size. */
	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	width = client->window->mode.width;
	height = client->window->mode.height;
	pitch = 4 * width;

	/* Capture pixel data. */
	/* The one extra row we allocate is used for flipping. */
	pixels = calloc ((height + 1) * pitch, sizeof (uint8_t));
	if (pixels == NULL)
		return;
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
	glReadPixels (0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	/* Flip the image vertically. */
	/* We use the extra row as temporary storage. */
	for (i = 0 ; i < height / 2 ; i++)
	{
		memcpy (pixels + pitch * height, pixels + pitch * i, pitch);
		memcpy (pixels + pitch * i, pixels + pitch * (height - i - 1), pitch);
		memcpy (pixels + pitch * (height - i - 1), pixels + pitch * height, pitch);
	}

	/* Create a temporary SDL surface. */
	if (lisys_endian_big ())
	{
		rmask = 0xFF000000;
		gmask = 0x00FF0000;
		bmask = 0x0000FF00;
		amask = 0x000000FF;
	}
	else
	{
		rmask = 0x000000FF;
		gmask = 0x0000FF00;
		bmask = 0x00FF0000;
		amask = 0xFF000000;
	}
	surface = SDL_CreateRGBSurfaceFrom(pixels, width, height,
		32, pitch, rmask, gmask, bmask, amask);
	if (surface == NULL)
	{
		lisys_free (pixels);
		return;
	}

	/* Construct file path. */
	home = lisys_paths_get_home ();
	file = lisys_string_format ("screenshot-%d.bmp", (int) time (NULL));
	if (home == NULL || file == NULL)
	{
		SDL_FreeSurface (surface);
		lisys_free (pixels);
		lisys_free (home);
		lisys_free (file);
		return;
	}
	path = lisys_path_concat (home, file, NULL);
	lisys_free (home);
	if (path == NULL)
	{
		SDL_FreeSurface (surface);
		lisys_free (pixels);
		lisys_free (file);
		return;
	}

	/* Save the surface to a file. */
	SDL_SaveBMP (surface, path);
	SDL_FreeSurface (surface);
	lisys_free (pixels);

	/* Return the file name. */
	liscr_args_seti_string (args, file);
	lisys_free (path);
	lisys_free (file);
}

static void Client_set_video_mode (LIScrArgs* args)
{
	int width = 1024;
	int height = 768;
	int fullscreen = 0;
	int vsync = 0;
	LICliClient* client;

	if (!liscr_args_gets_int (args, "width", &width))
		liscr_args_geti_int (args, 0, &width);
	if (!liscr_args_gets_int (args, "height", &height))
		liscr_args_geti_int (args, 1, &height);
	if (!liscr_args_gets_bool (args, "fullscreen", &fullscreen))
		liscr_args_geti_bool (args, 2, &fullscreen);
	if (!liscr_args_gets_bool (args, "vsync", &vsync))
		liscr_args_geti_bool (args, 3, &vsync);
	width = LIMAT_MAX (320, width);
	height = LIMAT_MAX (240, height);

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	if (!licli_window_set_size (client->window, width, height, fullscreen, vsync))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

static void Client_swap_buffers (LIScrArgs* args)
{
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
	SDL_GL_SwapBuffers ();
}

static void Client_get_cursor_pos (LIScrArgs* args)
{
	int x;
	int y;
	LICliClient* client;
	LIMatVector tmp;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	SDL_GetMouseState (&x, &y);
	tmp = limat_vector_init (x, y, 0.0f);
	liscr_args_seti_vector (args, &tmp);
}

static void Client_get_fps (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_float (args, client->program->fps);
}

static void Client_get_moving (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_bool (args, licli_client_get_moving (client));
}
static void Client_set_moving (LIScrArgs* args)
{
	int value;
	LICliClient* client;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
		licli_client_set_moving (client, value);
	}
}

static void Client_set_title (LIScrArgs* args)
{
	const char* value;
	LICliClient* client;

	if (liscr_args_geti_string (args, 0, &value))
	{
		client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
		SDL_WM_SetCaption (value, value);
	}
}

static void Client_get_video_mode (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	liscr_args_seti_int (args, client->window->mode.width);
	liscr_args_seti_int (args, client->window->mode.height);
	liscr_args_seti_bool (args, client->window->mode.fullscreen);
	liscr_args_seti_bool (args, client->window->mode.vsync);
}

static void Client_get_video_modes (LIScrArgs* args)
{
	int i;
	SDL_Rect** modes;
	LICliClient* client;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	modes = SDL_ListModes (NULL, SDL_OPENGL | SDL_FULLSCREEN);
	if (modes != NULL && modes != (SDL_Rect**) -1)
	{
		for (i = 0 ; modes[i] ; i++)
		{
			lua_newtable (args->lua);
			lua_pushnumber (args->lua, 1);
			lua_pushnumber (args->lua, modes[i]->w);
			lua_settable (args->lua, -3);
			lua_pushnumber (args->lua, 2);
			lua_pushnumber (args->lua, modes[i]->h);
			lua_settable (args->lua, -3);
			liscr_args_seti_stack (args);
		}
	}
}

/*****************************************************************************/

void licli_script_client (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_clear_buffer", Client_clear_buffer);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_host", Client_host);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_screenshot", Client_screenshot);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_set_video_mode", Client_set_video_mode);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_swap_buffers", Client_swap_buffers);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_cursor_pos", Client_get_cursor_pos);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_fps", Client_get_fps);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_moving", Client_get_moving);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_set_moving", Client_set_moving);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_set_title", Client_set_title);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_video_mode", Client_get_video_mode);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_video_modes", Client_get_video_modes);
}

/** @} */
/** @} */

