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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysRelative Relative
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
#ifdef linux
#include <linux/limits.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include "system-relative.h"
#include "system-memory.h"

/**
 * \brief Gets the file name of the calling executable.
 * \return New string or NULL.
 */
char*
lisys_relative_exename ()
{
#if defined linux
	int i;
	FILE* f;
	char* tmp;
	char* path;
	size_t buf_size;
	ssize_t size;
	struct stat stat_buf;

	/* Allocate memory. */
	buf_size = PATH_MAX + 128;
	path = (char*) lisys_malloc (buf_size);
	if (path == NULL)
		return NULL;
	tmp = (char*) lisys_malloc (buf_size);
	if (tmp == NULL)
	{
		lisys_free (path);
		return NULL;
	}

	/* Try to resolve /proc/self/exe. */
	strncpy (tmp, "/proc/self/exe", buf_size - 1);
	while (1)
	{
		/* Read the link target. */
		size = readlink (tmp, path, buf_size - 1);
		if (size == -1)
		{
			lisys_free (tmp);
			break;
		}
		path[size] = '\0';

		/* Check if another link. */
		i = stat (path, &stat_buf);
		if (i == -1)
		{
			lisys_free (tmp);
			break;
		}
		if (S_ISLNK (stat_buf.st_mode))
		{
			strncpy (path, tmp, buf_size - 1);
			continue;
		}

		/* Return the final path. */
		lisys_free (tmp);
		return path;
	}

	/* Try to read from /proc/self/maps. */
	if (1)
	{
		/* Open the file. */
		f = fopen ("/proc/self/maps", "r");
		if (f == NULL)
		{
			lisys_free (path);
			return NULL;
		}

		/* Read in the first line. */
		if (fgets (path, (int) buf_size, f) == NULL)
		{
			fclose (f);
			lisys_free (path);
			return NULL;
		}

		/* Remove trailing newline. */
		buf_size = strlen (path);
		if (buf_size <= 0)
		{
			fclose (f);
			lisys_free (path);
			return NULL;
		}
		if (path[buf_size - 1] == 10)
			path[buf_size - 1] = 0;

		/* Find the executable name. */
		tmp = strchr (path, '/');
		if (tmp == NULL)
		{
			fclose (f);
			lisys_free (path);
			return NULL;
		}
	}

	/* Construct the final string. */
	lisys_free (path);
	path = lisys_calloc (strlen (tmp) + 1, sizeof (char));
	if (path == NULL)
		return NULL;
	strcpy (path, tmp);
	fclose (f);

	return path;
#elif defined WIN32
	char* tmp;
	char name[MAX_PATH];

	/* Get executable name. */
	name[0] = '\0';
	GetModuleFileNameA (NULL, name, MAX_PATH);
	if (name[0] == '\0')
		return NULL;
	name[MAX_PATH - 1] = '\0';

	/* Fix backslashes. */
	for (tmp = name ; *tmp != '\0' ; tmp++)
	{
		if (*tmp == '\\')
			*tmp = '/';
	}

	/* Construct the final string. */
	tmp = lisys_calloc (strlen (name) + 1, sizeof (char));
	if (tmp == NULL)
		return NULL;
	strcpy (tmp, name);

	return tmp;
#else
#warning "Not supported."
	return NULL;
#endif
}

/**
 * \brief Gets the directory in which the calling executable is.
 * \return New string or NULL.
 */
char* lisys_relative_exedir ()
{
#if defined WIN32 || defined linux
	char* tmp;
	char* ptr;

	tmp = lisys_relative_exename ();
	if (tmp == NULL)
		return NULL;
	ptr = strrchr (tmp, '/');
	if (ptr == NULL)
	{
		lisys_free (tmp);
		return NULL;
	}
	*(ptr + 1) = '\0';
	return tmp;
#else
#warning "Not supported."
	return NULL;
#endif
}

/** @} */
/** @} */
