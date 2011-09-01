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
 * \addtogroup LIExtConfigFile ConfigFile
 * @{
 */

#include "ext-module.h"

typedef struct _LIExtConfigFile LIExtConfigFile;
struct _LIExtConfigFile
{
	char* path;
	LIAlgStrdic* dict;
};

static LIExtConfigFile* private_new_config (
	LIExtModule* module,
	const char*  name)
{
	char* key;
	char* value;
	LIArcReader* reader;
	LIExtConfigFile* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtConfigFile));
	if (self == NULL)
		return NULL;

	/* Resolve the full path. */
	self->path = lipth_paths_get_sql (module->program->paths, name);
	if (self->path == NULL)
	{
		lisys_error_report ();
		lisys_free (self);
		return NULL;
	}

	/* Allocate the dictionary. */
	self->dict = lialg_strdic_new ();
	if (self->dict == NULL)
	{
		lisys_free (self->path);
		lisys_free (self);
		return NULL;
	}

	/* Read the file. */
	reader = liarc_reader_new_from_file (self->path);
	if (reader == NULL)
		return self;
	while (1)
	{
		liarc_reader_skip_chars (reader, " \t\n");
		if (liarc_reader_check_end (reader))
			break;
		if (!liarc_reader_get_key_value_pair (reader, &key, &value))
		{
			lisys_error_report ();
			break;
		}
		if (lialg_strdic_find (self->dict, key))
		{
			lisys_free (value);
			lisys_free (key);
			continue;
		}
		if (!lialg_strdic_insert (self->dict, key, value))
		{
			lisys_error_report ();
			lisys_free (value);
		}
		lisys_free (key);
	}
	liarc_reader_free (reader);

	return self;
}

static void private_free_config (
	LIExtConfigFile* self)
{
	LIAlgStrdicIter iter;

	if (self->dict == NULL)
		return;
	LIALG_STRDIC_FOREACH (iter, self->dict)
		lisys_free (iter.value);
	lialg_strdic_free (self->dict);
	lisys_free (self->path);
	lisys_free (self);
}

static int private_line_compare (
	const void* a,
	const void* b)
{
	return strcmp (*((char**) a), *((char**) b));
}

/*****************************************************************************/

static void ConfigFile_new (LIScrArgs* args)
{
	int ok;
	const char* ptr;
	const char* name;
	LIExtConfigFile* cfg;
	LIExtModule* module;
	LIScrData* data;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_CONFIG_FILE);

	/* Get and validate the filename. */
	if (!liscr_args_geti_string (args, 0, &name) &&
	    !liscr_args_gets_string (args, "name", &name))
		return;
	for (ptr = name ; *ptr != '\0' ; ptr++)
	{
		if (*ptr >= 'a' && *ptr <= 'z') ok = 1;
		else if (*ptr >= 'A' && *ptr <= 'Z') ok = 1;
		else if (*ptr >= '0' && *ptr <= '9') ok = 1;
		else if (*ptr == '-') ok = 1;
		else if (*ptr == '_') ok = 1;
		else if (*ptr == '.') ok = 1;
		else ok = 0;
		if (!ok)
		{
			lisys_error_set (EINVAL, "invalid config file name `%s'", name);
			lisys_error_report ();
			return;
		}
	}

	/* Create the file. */
	cfg = private_new_config (module, name);
	if (cfg == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, cfg, LIEXT_SCRIPT_CONFIG_FILE, private_free_config);
	if (data == NULL)
	{
		private_free_config (cfg);
		return;
	}
	liscr_args_seti_stack (args);
}

static void ConfigFile_get (LIScrArgs* args)
{
	const char* key;
	const char* value;
	LIExtConfigFile* self;

	self = args->self;
	if (!liscr_args_geti_string (args, 0, &key) &&
	    !liscr_args_gets_string (args, "key", &key))
		return;

	value = lialg_strdic_find (self->dict, key);
	if (value != NULL)
		liscr_args_seti_string (args, value);
}

static void ConfigFile_save (LIScrArgs* args)
{
	int i;
	int error = 0;
	int line_cap;
	int line_num;
	char* line;
	char** tmp;
	char** line_arr;
	LIAlgStrdicIter iter;
	LIArcWriter* writer;
	LIExtConfigFile* self;

	self = args->self;

	/* Collect key-value pairs. */
	line_num = 0;
	line_cap = 1;
	line_arr = lisys_calloc (line_cap, sizeof (char*));
	if (line_arr == NULL)
	{
		lisys_error_report ();
		return;
	}
	LIALG_STRDIC_FOREACH (iter, self->dict)
	{
		if (line_num == line_cap)
		{
			line_cap <<= 1;
			tmp = lisys_realloc (line_arr, line_cap * sizeof (char*));
			if (tmp == NULL)
			{
				error = 1;
				break;
			}
			line_arr = tmp;
		}
		line = lisys_string_format ("%s %s\n", iter.key, (char*) iter.value);
		if (line == NULL)
		{
			error = 1;
			break;
		}
		line_arr[line_num++] = line;
	}
	if (error)
	{
		lisys_error_report ();
		for (i = 0 ; i < line_num ; i++)
			lisys_free (line_arr[i]);
		lisys_free (line_arr);
		return;
	}

	/* Sort the key-value pairs. */
	qsort (line_arr, line_num, sizeof (char*), private_line_compare);

	/* Open the destination file. */
	writer = liarc_writer_new_file (self->path);
	if (writer == NULL)
	{
		lisys_error_report ();
		return;
	}

	/* Write the key-value pairs. */
	for (i = 0 ; i < line_num ; i++)
		liarc_writer_append_string (writer, line_arr[i]);
	liarc_writer_free (writer);

	/* Free the sorted key-value array. */
	for (i = 0 ; i < line_num ; i++)
		lisys_free (line_arr[i]);
	lisys_free (line_arr);
}

static void ConfigFile_set (LIScrArgs* args)
{
	char* tmp;
	const char* key;
	const char* value = NULL;
	LIExtConfigFile* self;

	self = args->self;
	if (!liscr_args_geti_string (args, 0, &key) &&
	    !liscr_args_gets_string (args, "key", &key))
		return;
	if (!liscr_args_geti_string (args, 1, &value))
		liscr_args_gets_string (args, "value", &value);

	/* Remove the old value. */
	tmp = lialg_strdic_find (self->dict, key);
	if (tmp != NULL)
	{
		lisys_free (tmp);
		lialg_strdic_remove (self->dict, key);
	}

	/* Add the new value. */
	if (value != NULL)
	{
		tmp = lisys_string_dup (value);
		if (tmp == NULL)
			return;
		if (!lialg_strdic_insert (self->dict, key, tmp))
			lisys_free (tmp);
	}
}

/*****************************************************************************/

void liext_script_config_file (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_CONFIG_FILE, "config_file_new", ConfigFile_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CONFIG_FILE, "config_file_get", ConfigFile_get);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CONFIG_FILE, "config_file_save", ConfigFile_save);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CONFIG_FILE, "config_file_set", ConfigFile_set);
}

/** @} */
/** @} */
