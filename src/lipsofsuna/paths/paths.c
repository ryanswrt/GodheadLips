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
 * \addtogroup LIPth Paths
 * @{
 * \addtogroup LIPthPaths Paths
 * @{
 */

#include "lipsofsuna/system.h"
#include "paths.h"

/**
 * \brief Creates a new paths object.
 *
 * \param path Package root directory or NULL for auto-detect.
 * \param name Module name.
 * \return Paths or NULL.
 */
LIPthPaths* lipth_paths_new (
	const char* path,
	const char* name)
{
	char* tmp;
	LIPthPaths* self;
	LISysStat stat;

	self = lisys_calloc (1, sizeof (LIPthPaths));
	if (self == NULL)
		return NULL;

	/* Set module name. */
	self->module_name = lisys_string_dup (name);
	if (self->module_name == NULL)
		goto error;

	/* Set root directory. */
	if (path != NULL)
	{
		self->root = lisys_string_dup (path);
		if (self->root == NULL)
			goto error;
	}
	else
	{
		self->root = lipth_paths_get_root ();
		if (self->root == NULL)
			goto error;
	}

	/* Get data directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_data = lisys_path_concat (self->root, "data", NULL);
	if (self->global_data == NULL)
		goto error;
#else
	self->global_data = LIDATADIR;
#endif
	self->module_data = lisys_path_concat (self->global_data, name, NULL);
	if (self->module_data == NULL)
		goto error;

	/* Get data override directory. */
	tmp = lisys_paths_get_data_home ();
	if (tmp == NULL)
		goto error;
	self->override_data = lisys_path_concat (tmp, "lipsofsuna", "data", name, NULL);
	if (self->override_data == NULL)
	{
		lisys_free (tmp);
		goto error;
	}

	/* Get save directory. */
	self->global_state = lisys_path_concat (tmp, "lipsofsuna", "save", NULL);
	lisys_free (tmp);
	if (self->global_state == NULL)
		goto error;
	self->module_state = lisys_path_concat (self->global_state, name, NULL);
	if (self->module_state == NULL)
		goto error;

	/* Get extension directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_exts = lisys_path_concat (self->global_data, "lib", "extensions", NULL);
	if (self->global_exts == NULL)
		goto error;
#else
	self->global_exts = LIEXTSDIR;
#endif

	/* Check for valid data directory. */
	if (!lisys_filesystem_stat (self->module_data, &stat))
	{
		lisys_error_set (EIO, "missing data directory `%s'", self->module_data);
		goto error;
	}
	if (stat.type != LISYS_STAT_DIRECTORY && stat.type != LISYS_STAT_LINK)
	{
		lisys_error_set (EIO, "invalid data directory `%s': not a directory", self->module_data);
		goto error;
	}

	return self;

error:
	lipth_paths_free (self);
	return NULL;
}

/**
 * \brief Frees the paths object.
 *
 * \param self Paths object.
 */
void lipth_paths_free (
	LIPthPaths* self)
{
#ifdef LI_RELATIVE_PATHS
	lisys_free (self->global_exts);
	lisys_free (self->global_data);
#endif
	lisys_free (self->global_state);
	lisys_free (self->module_data);
	lisys_free (self->module_name);
	lisys_free (self->module_state);
	lisys_free (self->override_data);
	lisys_free (self->root);
	lisys_free (self);
}

/**
 * \brief Gets the path to a generic data file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char* lipth_paths_get_data (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Try the override path. */
	path = lisys_path_concat (self->override_data, name, NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
		return path;
	lisys_free (path);

	/* Try the real path. */
	return lisys_path_concat (self->module_data, name, NULL);
}

/**
 * \brief Gets the path to a font file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char* lipth_paths_get_font (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Try the override path. */
	path = lisys_path_concat (self->override_data, "fonts", name, NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
		return path;
	lisys_free (path);

	/* Try the real path. */
	return lisys_path_concat (self->module_data, "fonts", name, NULL);
}

/**
 * \brief Gets the path to a graphics file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char* lipth_paths_get_graphics (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Try the override path. */
	path = lisys_path_concat (self->override_data, "graphics", name, NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
		return path;
	lisys_free (path);

	/* Try the real path. */
	return lisys_path_concat (self->module_data, "graphics", name, NULL);
}

/**
 * \brief Gets the path to a script file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char* lipth_paths_get_script (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Try the override path. */
	path = lisys_path_concat (self->override_data, "scripts", name, NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
		return path;
	lisys_free (path);

	/* Try the real path. */
	return lisys_path_concat (self->module_data, "scripts", name, NULL);
}

/**
 * \brief Gets the path to a sound file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char* lipth_paths_get_sound (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Try the override path. */
	path = lisys_path_concat (self->override_data, "sounds", name, NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
		return path;
	lisys_free (path);

	/* Try the real path. */
	return lisys_path_concat (self->module_data, "sounds", name, NULL);
}

/**
 * \brief Gets the path to an SQL database.
 *
 * Calling this function will create the save directory if it doesn't exist
 * yet. If the creation fails or the function runs out of memory, NULL is
 * returned and the error message is set.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Newly allocated absolute path or NULL.
 */
char* lipth_paths_get_sql (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Format the path. */
	path = lisys_path_concat (self->module_state, name, NULL);
	if (path == NULL)
		return NULL;

	/* Check if the save directory exists. */
	if (lisys_filesystem_access (self->module_state, LISYS_ACCESS_EXISTS))
	{
		if (!lisys_filesystem_access (self->module_state, LISYS_ACCESS_WRITE))
		{
			lisys_error_set (EINVAL, "save path `%s' is not writable", path);
			lisys_free (path);
			return NULL;
		}
		return path;
	}

	/* Create the save directory. */
	if (!lisys_filesystem_makepath (self->module_state))
	{
		lisys_free (path);
		return NULL;
	}

	return path;
}

/**
 * \brief Gets the game root directory.
 * \return Path or NULL.
 */
char* lipth_paths_get_root ()
{
#ifdef LI_RELATIVE_PATHS
	return lisys_relative_exedir ();
#else
	return lisys_string_dup (LIDATADIR);
#endif
}

/** @} */
/** @} */
