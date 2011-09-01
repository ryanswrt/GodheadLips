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
 * \addtogroup LIExtFile File
 * @{
 */

#include "ext-module.h"

static void File_read (LIScrArgs* args)
{
	int i;
	int len;
	char* buf;
	char* path_abs;
	const char* path;
	LIArcReader* reader;
	LIExtModule* module;

	/* Construct the path. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_FILE);
	if (!liscr_args_geti_string (args, 0, &path))
		return;
	for (i = 0 ; i < strlen (path) ; i++)
	{
		if ((path[i] < 'a' || path[i] > 'z') && (path[i] != '/'))
			return;
	}
	path_abs = lisys_path_concat (module->program->paths->module_data, path, NULL);

	/* Open the file. */
	/* TODO: Also support additions in the home directory. */
	reader = liarc_reader_new_from_file (path_abs);
	lisys_free (path_abs);
	if (reader == NULL)
		return;

	/* Return the file contents. */
	len = reader->length;
	buf = lisys_calloc (len, sizeof (char));
	if (buf != NULL)
	{
		lua_pushlstring (args->lua, buf, len);
		liscr_args_seti_stack (args);
		lisys_free (buf);
	}
	liarc_reader_free (reader);
}

static void File_scan_directory (LIScrArgs* args)
{
	int i;
	int count;
	char* path_abs;
	const char* name;
	const char* path;
	LIExtModule* module;
	LISysDir* dir;

	/* Construct the path. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_FILE);
	if (!liscr_args_geti_string (args, 0, &path))
		return;
	if (strstr (path, ".."))
		return;
	for (i = 0 ; i < strlen (path) ; i++)
	{
		if ((path[i] < 'a' || path[i] > 'z') && (path[i] < '0' || path[i] > '9') && 
		    (path[i] != '/' && path[i] != '.' && path[i] != '-' && path[i] != '_'))
			return;
	}
	path_abs = lisys_path_concat (module->program->paths->module_data, path, NULL);

	/* Scan the directory. */
	/* TODO: Also support additions in the home directory. */
	dir = lisys_dir_open (path_abs);
	lisys_free (path_abs);
	if (dir == NULL)
		return;
	if (!lisys_dir_scan (dir))
	{
		lisys_dir_free (dir);
		return;
	}

	/* Return the file names. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	count = lisys_dir_get_count (dir);
	for (i = 0 ; i < count ; i++)
	{
		name = lisys_dir_get_name (dir, i);
		if (name[0] != '.')
			liscr_args_seti_string (args, name);
	}

	lisys_dir_free (dir);
}

static void File_write (LIScrArgs* args)
{
	int i;
	char* path_abs;
	const char* path;
	const char* data;
	LIArcWriter* writer;
	LIExtModule* module;

	/* Construct the path. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_FILE);
	if (!liscr_args_geti_string (args, 0, &path) ||
	    !liscr_args_geti_string (args, 1, &data))
		return;
	for (i = 0 ; i < strlen (path) ; i++)
	{
		if ((path[i] < 'a' || path[i] > 'z') && (path[i] < '0' || path[i] > '9') && 
		    (path[i] != '/' && path[i] != '.' && path[i] != '-' && path[i] != '_'))
			return;
	}
	path_abs = lisys_path_concat (module->program->paths->module_data, path, NULL);

	/* Open the file. */
	/* TODO: Also support additions in the home directory. */
	writer = liarc_writer_new_file (path_abs);
	lisys_free (path_abs);
	if (writer == NULL)
		return;

	/* Write the file contents. */
	if (liarc_writer_append_string (writer, data))
		liscr_args_seti_bool (args, 1);
	liarc_writer_free (writer);
}

/*****************************************************************************/

void liext_script_file (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_FILE, "file_read", File_read);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_FILE, "file_scan_directory", File_scan_directory);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_FILE, "file_write", File_write);
}

/** @} */
/** @} */
