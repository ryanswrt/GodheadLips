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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtGraphics Graphics
 * @{
 */

#include "ext-module.h"

static void private_initial_videomode (
	LIExtModule* self,
	int*         width,
	int*         height,
	int*         fullscreen,
	int*         vsync);

static void private_context_lost (
	LIExtModule* self,
	int          pass);

/*****************************************************************************/

LIMaiExtensionInfo liext_graphics_info =
{
	LIMAI_EXTENSION_VERSION, "Graphics",
	liext_graphics_new,
	liext_graphics_free
};

LIExtModule* liext_graphics_new (
	LIMaiProgram* program)
{
	int width = 1024;
	int height = 768;
	int fullscreen = 0;
	int vsync = 0;
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Get the initial video mode. */
	private_initial_videomode (self, &width, &height, &fullscreen, &vsync);

	/* Allocate client. */
	self->client = licli_client_new (program, width, height, fullscreen, vsync);
	if (self->client == NULL)
	{
		liext_graphics_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "context-lost", 0, private_context_lost, self, self->calls + 0))
	{
		liext_graphics_free (self);
		return NULL;
	}

	/* Extend scripts. */
	liext_script_graphics (program->script);

	return self;
}

void liext_graphics_free (
	LIExtModule* self)
{
	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	if (self->client != NULL)
		licli_client_free (self->client);
	lisys_free (self);
}

/*****************************************************************************/

static void private_initial_videomode (
	LIExtModule* self,
	int*         width,
	int*         height,
	int*         fullscreen,
	int*         vsync)
{
	lua_State* lua;

	/* Get the videomode table. */
	lua = liscr_script_get_lua (self->program->script);
	lua_getglobal (lua, "__initial_videomode");
	if (lua_type (lua, -1) != LUA_TTABLE)
	{
		lua_pop (lua, 1);
		return;
	}

	/* Get the width. */
	lua_pushnumber (lua, 1);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TNUMBER)
	{
		*width = (int) lua_tonumber (lua, -1);
		*width = LIMAT_CLAMP (*width, 32, 65536);
	}
	lua_pop (lua, 1);

	/* Get the height. */
	lua_pushnumber (lua, 2);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TNUMBER)
	{
		*height = (int) lua_tonumber (lua, -1);
		*height = LIMAT_CLAMP (*height, 32, 65536);
	}
	lua_pop (lua, 1);

	/* Get the fullscreen flag. */
	lua_pushnumber (lua, 3);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TBOOLEAN)
		*fullscreen = (int) lua_toboolean (lua, -1);
	lua_pop (lua, 1);

	/* Get the vsync flag. */
	lua_pushnumber (lua, 4);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TBOOLEAN)
		*vsync = (int) lua_toboolean (lua, -1);
	lua_pop (lua, 1);

	/* Pop the videomode table. */
	lua_pop (lua, 1);
}

static void private_context_lost (
	LIExtModule* self,
	int          pass)
{
	liren_render_reload (self->client->render, pass);
}

/** @} */
/** @} */
