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
 * \addtogroup LIExtRender Render
 * @{
 */

#include "ext-module.h"

LIMaiExtensionInfo liext_render_info =
{
	LIMAI_EXTENSION_VERSION, "Render",
	liext_render_new,
	liext_render_free
};

LIExtModule* liext_render_new (
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
		lisys_error_set (EINVAL, "extension `render' can only be used by the client");
		liext_render_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_LIGHT, self);
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_RENDER, self);
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_SCENE, self);
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_SHADER, self);
	liext_script_light (program->script);
	liext_script_render (program->script);
	liext_script_scene (program->script);
	liext_script_shader (program->script);

	return self;
}

void liext_render_free (
	LIExtModule* self)
{
	lisys_free (self);
}

/** @} */
/** @} */
