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
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include "ext-module.h"

static void private_context_lost (
	LIExtModule* self,
	int          pass);

static void private_widget_allocation (
	LIExtModule* module,
	LIWdgWidget* widget);

static void private_widget_paint (
	LIExtModule* module,
	LIWdgWidget* widget);

static int private_widget_tick (
	LIExtModule* module,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_widgets_info =
{
	LIMAI_EXTENSION_VERSION, "Widgets",
	liext_widgets_new,
	liext_widgets_free
};

LIExtModule* liext_widgets_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->client = limai_program_find_component (program, "client");
	if (self->client == NULL)
	{
		lisys_error_set (EINVAL, "extension `widgets' can only be used by the client");
		liext_widgets_free (self);
		return NULL;
	}

	/* Allocate the widget manager. */
	self->widgets = liwdg_manager_new (self->client->render, self->program->callbacks, self->program->paths);
	if (self->widgets == NULL)
	{
		liext_widgets_free (self);
		return NULL;
	}
	liwdg_manager_set_size (self->widgets, self->client->window->mode.width,
		self->client->window->mode.height);

	/* Register component. */
	if (!limai_program_insert_component (program, "widgets", self->widgets))
	{
		liext_widgets_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "context-lost", 0, private_context_lost, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, "tick", 1, private_widget_tick, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, "widget-allocation", 5, private_widget_allocation, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, "widget-paint", 5, private_widget_paint, self, self->calls + 3))
	{
		liext_widgets_free (self);
		return 0;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_WIDGET, self);
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_WIDGETS, self);
	liext_script_widget (program->script);
	liext_script_widgets (program->script);

	return self;
}

void liext_widgets_free (
	LIExtModule* self)
{
	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Unregister component. */
	if (self->widgets != NULL)
		limai_program_remove_component (self->program, "widgets");

	/* Free the widget manager. */
	if (self->widgets != NULL)
		liwdg_manager_free (self->widgets);

	lisys_free (self);
}

/*****************************************************************************/

static void private_context_lost (
	LIExtModule* self,
	int          pass)
{
	liwdg_manager_reload (self->widgets, pass);
}

static void private_widget_allocation (
	LIExtModule* module,
	LIWdgWidget* widget)
{
	LIScrScript* script = module->program->script;
	lua_State* lua = liscr_script_get_lua (script);

	/* Call a global function. */
	lua_getglobal (lua, "__widget_reshape");
	if (lua_type (lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (lua, 1);
		return;
	}
	if (!liscr_pushdata (lua, widget->script))
	{
		lua_pop (lua, 1);
		return;
	}
	if (lua_pcall (lua, 1, 0, 0) != 0)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Widget.reshaped: %s", lua_tostring (lua, -1));
		lisys_error_report ();
		lua_pop (lua, 1);
	}
}

static void private_widget_paint (
	LIExtModule* module,
	LIWdgWidget* widget)
{
	LIScrData* data = widget->script;
	LIScrScript* script = liscr_data_get_script (data);
	lua_State* lua = liscr_script_get_lua (script);

	/* Call a global function. */
	lua_getglobal (lua, "__widget_render");
	if (lua_type (lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (lua, 1);
		return;
	}
	if (!liscr_pushdata (lua, data))
	{
		lua_pop (lua, 1);
		return;
	}
	if (lua_pcall (lua, 1, 0, 0) != 0)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Widget.render: %s", lua_tostring (lua, -1));
		lisys_error_report ();
		lua_pop (lua, 1);
	}
}

static int private_widget_tick (
	LIExtModule* module,
	float        secs)
{
	int w;
	int h;

	/* Update widgets. */
	liwdg_manager_update (module->widgets, secs);

	/* Update dimensions. */
	licli_window_get_size (module->client->window, &w, &h);
	liwdg_manager_set_size (module->widgets, w, h);

	return 1;
}

/** @} */
/** @} */
/** @} */
