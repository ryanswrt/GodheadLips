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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysPaths Paths
 * @{
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_WINDOWS_H
#define _WIN32_IE 0x0400
#include <windows.h>
#include <shlobj.h>
#endif
#include "system.h"
#include "system-error.h"
#include "system-path.h"
#include "system-paths.h"

/**
 * \brief Gets the home directory.
 *
 * If no home directory is specified, the current
 * working directory is returned instead.
 *
 * \return New string or NULL if out of memory.
 */
char* lisys_paths_get_home ()
{
#ifdef __WIN32__
	int i;
	char tmp[MAX_PATH];

	if (!SHGetSpecialFolderPath (NULL, tmp, CSIDL_PROFILE, TRUE))
		return NULL;
	for (i = 0 ; tmp[i] != '\0' ; i++)
	{
		if (tmp[i] == '\\')
			tmp[i] = '/';
	}
	return strdup (tmp);
#else
	char* home;

	home = getenv ("HOME");
	if (home != NULL)
		return strdup (home);
	return strdup (".");
#endif
}

/**
 * \brief Gets the home data directory.
 *
 * Follows the XDG Base Directory Specification:
 * http://www.freedesktop.org/Standards/basedir-spec
 *
 * \return New string or NULL.
 */
char* lisys_paths_get_data_home ()
{
#ifdef __WIN32__
	int i;
	char tmp[MAX_PATH];

	if (!SHGetSpecialFolderPath (NULL, tmp, CSIDL_PERSONAL, TRUE))
		return NULL;
	for (i = 0 ; tmp[i] != '\0' ; i++)
	{
		if (tmp[i] == '\\')
			tmp[i] = '/';
	}
	return lisys_path_concat (tmp, "My Games", NULL);
#else
	char* ret;
	char* tmp;
	const char* dir;

	dir = getenv ("XDG_DATA_HOME");
	if (dir != NULL && dir[0] != '\0')
		return strdup (dir);
	tmp = lisys_paths_get_home ();
	if (tmp == NULL)
		return NULL;
	ret = lisys_path_concat (tmp, ".local/share", NULL);
	free (tmp);

	return ret;
#endif
}

/**
 * \brief Gets the home config directory.
 *
 * Follows the XDG Base Directory Specification:
 * http://www.freedesktop.org/Standards/basedir-spec
 *
 * \return New string or NULL.
 */
char* lisys_paths_get_config_home ()
{
#ifdef __WIN32__
	return lisys_paths_get_data_home ();
#else
	char* ret;
	char* tmp;
	const char* dir;

	dir = getenv ("XDG_CONFIG_HOME");
	if (dir != NULL && dir[0] != '\0')
		return strdup (dir);
	tmp = lisys_paths_get_home ();
	if (tmp == NULL)
		return NULL;
	ret = lisys_path_concat (tmp, ".config", NULL);
	free (tmp);

	return ret;
#endif
}

/**
 * \brief Gets the home cache directory.
 *
 * Follows the XDG Base Directory Specification:
 * http://www.freedesktop.org/Standards/basedir-spec
 *
 * \return New string or NULL.
 */
char* lisys_paths_get_cache_home ()
{
#ifdef __WIN32__
	return lisys_paths_get_data_home ();
#else
	char* ret;
	char* tmp;
	const char* dir;

	dir = getenv ("XDG_CACHE_HOME");
	if (dir != NULL && dir[0] != '\0')
		return strdup (dir);
	tmp = lisys_paths_get_home ();
	if (tmp == NULL)
		return NULL;
	ret = lisys_path_concat (tmp, ".cache", NULL);
	free (tmp);

	return ret;
#endif
}

/** @} */
/** @} */
