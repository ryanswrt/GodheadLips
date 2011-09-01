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
 * \addtogroup LICliMain Main
 * @{
 */

#include "SDL.h"
#include "SDL_main.h"
#include "main-program.h"

static int private_parse_arguments (
	int    argc,
	char** argv,
	char** name,
	char** args)
{
	int i;
	char* tmp;

	for (i = 1 ; i < argc ; i++)
	{
		if (i == 1 && argv[i][0] != '-')
		{
			/* The first argument is the module name unless it begins with '-'. */
			*name = argv[i];
		}
		else if (*args == NULL)
		{
			/* Copy the first switch to the argument list. */
			*args = lisys_string_dup (argv[i]);
			if (*args == NULL)
				return 0;
		}
		else
		{
			/* Append the remaining switches to the argument list. */
			tmp = lisys_string_format ("%s %s", *args, argv[i]);
			if (tmp == NULL)
			{
				lisys_free (*args);
				return 0;
			}
			lisys_free (*args);
			*args = tmp;
		}
	}

	return 1;
}

int main (int argc, char** argv)
{
	char* path;
	char* launch_name;
	char* launch_args;
	LIMaiProgram* program;

	/* Resolve game directory. */
	path = lipth_paths_get_root ();
	if (path == NULL)
	{
		lisys_error_report ();
		return 1;
	}

	/* Parse command line arguments. */
	launch_name = "default";
	launch_args = NULL;
	if (!private_parse_arguments (argc, argv, &launch_name, &launch_args))
	{
		lisys_error_report ();
		lisys_free (path);
		return 1;
	}

	/* Start the program. */
	program = limai_program_new (path, launch_name, launch_args);
	lisys_free (launch_args);
	if (program == NULL)
	{
		lisys_error_report ();
		lisys_free (path);
		return 1;
	}

	/* Execute mods until one exits without starting a new one. */
	while (program != NULL)
	{
		/* Execute the module until the script exits. */
		if (!limai_program_execute_script (program, "main.lua"))
		{
			lisys_error_report ();
			break;
		}

		/* Check if the module started another one. */
		launch_name = program->launch_name;
		launch_args = program->launch_args;
		if (launch_name == NULL)
			break;
		program->launch_name = NULL;
		program->launch_args = NULL;
		limai_program_free (program);

		/* Unload the old module and load a new one. */
		program = limai_program_new (path, launch_name, launch_args);
		lisys_free (launch_name);
		lisys_free (launch_args);
		if (program == NULL)
		{
			lisys_error_report ();
			break;
		}
	}

	/* Free all resources. */
	limai_program_free (program);
	lisys_free (path);
	SDL_Quit ();

	return 0;
}

/** @} */
/** @} */
