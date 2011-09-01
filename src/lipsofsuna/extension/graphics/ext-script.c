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
 * \addtogroup LIExtGraphics Graphics
 * @{
 */

#include "ext-module.h"

static void Program_get_opengl_version (LIScrArgs* args)
{
	if (GLEW_VERSION_3_2)
		liscr_args_seti_float (args, 3.2f);
	else
		liscr_args_seti_float (args, 2.1f);
}

/*****************************************************************************/

void liext_script_graphics (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_opengl_version", Program_get_opengl_version);
}

/** @} */
/** @} */
