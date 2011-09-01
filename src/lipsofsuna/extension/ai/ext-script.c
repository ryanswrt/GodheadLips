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
 * \addtogroup LIExtAi Ai
 * @{
 */

#include "ext-module.h"

static void Ai_solve_path (LIScrArgs* args)
{
	int i;
	LIAiPath* path;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector start;
	LIMatVector end;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_AI);
	if (!liscr_args_gets_vector (args, "start", &start))
		return;
	if (!liscr_args_gets_vector (args, "start", &start) ||
	    !liscr_args_gets_vector (args, "target", &end))
		return;

	/* Solve the path. */
	path = liai_manager_solve_path (module->ai, &start, &end);
	if (path == NULL)
		return;

	/* Store the path to a table. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	for (i = 0 ; i < liai_path_get_length (path) ; i++)
	{
		liai_path_get_point (path, i, &point);
		liscr_args_seti_vector (args, &point);
	}
	liai_path_free (path);
}

/*****************************************************************************/

void liext_script_ai (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_AI, "ai_solve_path", Ai_solve_path);
}

/** @} */
/** @} */
