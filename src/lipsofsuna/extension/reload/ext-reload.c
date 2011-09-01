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
 * \addtogroup LIExtReload Reload
 * @{
 */

#include "ext-reload.h"

static int private_callback_tick (
	LIExtReload* self,
	float        secs);

static void private_reload_image (
	LIExtReload* self,
	const char*  name);

static void private_reload_model (
	LIExtReload* self,
	const char*  name);

/*****************************************************************************/

LIMaiExtensionInfo liext_reload_info =
{
	LIMAI_EXTENSION_VERSION, "Reload",
	liext_reload_new,
	liext_reload_free
};

LIExtReload* liext_reload_new (
	LIMaiProgram* program)
{
	LIExtReload* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtReload));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->paths = program->paths;
	self->client = limai_program_find_component (program, "client");

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "tick", 0, private_callback_tick, self, self->calls + 0))
	{
		liext_reload_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_RELOAD, self);
	liext_script_reload (program->script);

	return self;
}

void liext_reload_free (
	LIExtReload* self)
{
	liext_reload_set_enabled (self, 0);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/**
 * \brief Updates the status of the reloader.
 * \param self Reload.
 */
void liext_reload_update (
	LIExtReload* self)
{
	char* name;
	LISysNotifyEvent* event;

	/* Check for monitor events. */
	if (self->notify == NULL)
		return;
	event = lisys_notify_poll (self->notify);
	if (event == NULL)
		return;
	if (!(event->flags & LISYS_NOTIFY_CLOSEW))
		return;

	/* Reload changed models. */
	if (lisys_path_check_ext (event->name, "lmdl"))
	{
		name = lisys_path_format (LISYS_PATH_BASENAME, event->name, LISYS_PATH_STRIPEXTS, NULL);
		if (name != NULL)
		{
			private_reload_model (self, name);
			lisys_free (name);
		}
	}

	/* Reload changed DDS textures. */
	if (lisys_path_check_ext (event->name, "dds"))
	{
		name = lisys_path_format (LISYS_PATH_BASENAME,
			event->name, LISYS_PATH_STRIPEXTS, NULL);
		if (name != NULL)
		{
			private_reload_image (self, name);
			lisys_free (name);
		}
	}
}

int liext_reload_get_enabled (
	const LIExtReload* self)
{
	return (self->notify != NULL);
}

int liext_reload_set_enabled (
	LIExtReload* self,
	int          value)
{
	char* dstdir;

	if ((value != 0) == (self->notify != NULL))
		return 1;
	if (value)
	{
		self->notify = lisys_notify_new ();
		if (self->notify == NULL)
			return 0;
		dstdir = lisys_path_concat (self->paths->module_data, "graphics", NULL);
		if (dstdir == NULL)
		{
			lisys_notify_free (self->notify);
			self->notify = NULL;
			return 0;
		}
		if (!lisys_notify_add (self->notify, dstdir, LISYS_NOTIFY_CLOSEW))
		{
			lisys_notify_free (self->notify);
			self->notify = NULL;
			lisys_free (dstdir);
		}
		lisys_free (dstdir);
	}
	else
	{
		lisys_notify_free (self->notify);
		self->notify = NULL;
	}

	return 1;
}

/*****************************************************************************/

/**
 * \brief Monitors data files for changes if enabled.
 *
 * \param self Reload.
 */
static int private_callback_tick (
	LIExtReload* self,
	float        secs)
{
	liext_reload_update (self);

	return 1;
}

static void private_reload_image (
	LIExtReload* self,
	const char*  name)
{
	liren_render_load_image (self->client->render, name);
	limai_program_event (self->program, "reload-image", "file", LISCR_TYPE_STRING, name, NULL);
}

static void private_reload_model (
	LIExtReload* self,
	const char*  name)
{
	limai_program_event (self->program, "reload-model", "file", LISCR_TYPE_STRING, name, NULL);
}

/** @} */
/** @} */
