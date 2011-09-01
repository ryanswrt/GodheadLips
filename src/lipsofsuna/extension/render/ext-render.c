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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include <lipsofsuna/render.h>
#include "ext-module.h"

static void Render_get_anisotrophy (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
	liscr_args_seti_int (args, liren_render_get_anisotropy (module->client->render));
}
static void Render_set_anisotrophy (LIScrArgs* args)
{
	int value;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
	if (liscr_args_geti_int (args, 0, &value))
	{
		value = LIMAT_MAX (0, value);
		liren_render_set_anisotropy (module->client->render, value);
	}
}

/*****************************************************************************/

void liext_script_render (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_get_anisotrophy", Render_get_anisotrophy);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_set_anisotrophy", Render_set_anisotrophy);
}

/** @} */
/** @} */
