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
 * \addtogroup LISysPath Path
 * @{
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "system-error.h"
#include "system-memory.h"
#include "system-path.h"

static char* private_strdup (
	const char* str);

/*****************************************************************************/

/**
 * \brief Gets the file name without the path component.
 * \param path Path string.
 * \return New string or NULL.
 */
char* lisys_path_basename (
	const char* path)
{
	char* ptr;

	ptr = strrchr (path, '/');
	if (ptr == NULL)
		return private_strdup (path);

	return private_strdup (ptr + 1);
}

/**
 * \brief Checks if the path is a file name with the requested extension.
 * \param path Path string.
 * \param ext File name extension without the leading period.
 * \return Nonzero if the file has the extension.
 */
int lisys_path_check_ext (
	const char* path,
	const char* ext)
{
	int plen;
	int elen;

	/* Check for the length. */
	plen = strlen (path);
	elen = strlen (ext);
	if (plen < elen + 1)
		return 0;

	/* Check for the period. */
	if (path[plen - elen - 1] != '.')
		return 0;

	/* Check for the extension string. */
	if (strcmp (path + plen - elen, ext))
		return 0;
	return 1;
}

/**
 * \brief Concatenates paths.
 *
 * Concatenates the provided paths, adding path separator characters
 * whenever needed.
 *
 * \param path First path component.
 * \param ... NULL terminated list of path components.
 * \return A new string or NULL.
 */
char* lisys_path_concat (
	const char* path,
	            ...)
{
	int len;
	int total;
	char last;
	char* self;
	va_list args;
	const char* ptr;

	/* Calculate the length. */
	total = strlen (path);
	last = total? path[total - 1] : ' ';
	va_start (args, path);
	while ((ptr = va_arg (args, char*)))
	{
		if ((len = strlen (ptr)))
		{
			if (last != '/' && *ptr != '/')
				total++;
			total += len;
			last = ptr[len - 1];
		}
	}
	va_end (args);

	/* Allocate a new string. */
	self = lisys_malloc (total + 1);
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, "not enough memory to construct path");
		return NULL;
	}

	/* Concatenate the elements. */
	strcpy (self, path);
	total = strlen (path);
	last = total? path[total - 1] : ' ';
	va_start (args, path);
	while ((ptr = va_arg (args, char*)))
	{
		if ((len = strlen (ptr)))
		{
			if (last != '/' && *ptr != '/')
				self[total++] = '/';
			strcpy (self + total, ptr);
			total += len;
			last = ptr[len - 1];
		}
	}
	va_end (args);

	return self;
}

/**
 * \brief Formats a path.
 *
 * In addition to any ordinary string being appended to the path as such and a
 * NULL argument terminating the path, the following special instructions are
 * recognized:
 *
 * \li \c LISYS_PATH_BASENAME Appends the base name of the next string.
 * \li \c LISYS_PATH_PATHNAME Appends the path name of the next string.
 * \li \c LISYS_PATH_SEPARATOR Appends a path separator if one isn't present already.
 * \li \c LISYS_PATH_STRIPEXT Removes an extension from the path if one is present.
 * \li \c LISYS_PATH_STRIPEXTS Removes all extensions from the path.
 * \li \c LISYS_PATH_STRIPLAST Removes the last path component.
 *
 * \param format Path format instruction.
 * \param ... Variable array of path format instructions.
 * \return Path string or NULL.
 */
char* lisys_path_format (
	lisysPathFormat format,
	                ...)
{
	int cap;
	int len;
	int pos;
	int error;
	char last;
	char* tmp;
	char* self;
	const char* arg;
	va_list args;

	/* Allocate buffer. */
	pos = 0;
	cap = 64;
	self = lisys_malloc (cap);
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, "not enough memory to construct path");
		return NULL;
	}
	self[0] = '\0';

	/* Fill buffer. */
	va_start (args, format);
	for (error = last = 0 ; format != NULL ; format = va_arg (args, char*))
	{
		/* Identify format code. */
		if (format == LISYS_PATH_BASENAME)
		{
			/* Append base name of the next string. */
			arg = va_arg (args, char*);
			tmp = strrchr (arg, '/');
			if (tmp != NULL)
				arg = tmp + 1;
			len = strlen (arg);
		}
		else if (format == LISYS_PATH_PATHNAME)
		{
			/* Append path name of the next string. */
			arg = va_arg (args, char*);
			if (*arg == '/' && last == '/')
				arg++;
			tmp = strrchr (arg, '/');
			len = tmp? tmp - arg : strlen (arg);
		}
		else if (format == LISYS_PATH_SEPARATOR)
		{
			/* Append separator if not duplicate. */
			arg = (last != '/')? "/" : "";
			len = (last != '/')? 1 : 0;
		}
		else if (format == LISYS_PATH_STRIPEXT)
		{
			/* Strip extension. */
			for (tmp = self + pos - 1 ; tmp > self ; tmp--)
			{
				if (*tmp == '/' || *tmp == '.')
					break;
			}
			if (tmp > self && tmp[0] == '.' && tmp[-1] != '/')
			{
				pos = tmp - self;
				last = tmp[-1];
			}
			continue;
		}
		else if (format == LISYS_PATH_STRIPEXTS)
		{
			/* Strip all extensions. */
			for (tmp = self + pos - 1 ; tmp > self ; tmp--)
			{
				if (*tmp == '/')
					break;
				if (*tmp == '.' && tmp[-1] != '/')
				{
					pos = tmp - self;
					last = tmp[-1];
				}
			}
			continue;
		}
		else if (format == LISYS_PATH_STRIPLAST)
		{
			/* Strip last component. */
			while (1)
			{
				for (tmp = self + pos - 1 ; *tmp != '/' && tmp > self ; tmp--)
					;
				if (tmp > self)
				{
					if (pos > 0)
						last = self[pos - 1];
					else
						last = '\0';
					if (tmp == self + pos - 1)
					{
						pos--;
						continue;
					}
					pos = tmp - self;
				}
				break;
			}
			continue;
		}
		else
		{
			/* Append format as an ordinary string. */
			arg = format;
			len = strlen (arg);
		}

		/* Resize buffer. */
		if (cap <= pos + len)
		{
			while (cap <= pos + len)
				cap <<= 1;
			tmp = lisys_realloc (self, cap);
			if (tmp == NULL)
			{
				error = 1;
				break;
			}
			self = tmp;
		}

		/* Append to buffer. */
		strncpy (self + pos, arg, len);
		pos += len;
		last = arg[len - 1];
	}
	va_end (args);
	self[pos] = '\0';

	/* Check for errors. */
	if (error)
	{
		lisys_error_set (ENOMEM, "not enough memory to construct path");
		free (self);
		return NULL;
	}

	/* Shrink buffer. */
	tmp = lisys_realloc (self, pos + 1);
	if (tmp != NULL)
		self = tmp;

	return self;
}

/**
 * \brief Gets the file name from a path.
 * \param path Path string.
 * \return New string or NULL.
 */
char* lisys_path_filename (
	const char* path)
{
	const char* ptr;

	ptr = strrchr (path, '/');
	if (ptr == NULL)
		return private_strdup (path);

	return private_strdup (ptr + 1);
}

/**
 * \brief Gets the extension from a path.
 * \param path Path string.
 * \return New string or NULL.
 */
char* lisys_path_fileext (
	const char* path)
{
	const char* ptr0;
	const char* ptr1;

	ptr0 = strrchr (path, '.');
	ptr1 = strrchr (path, '/');
	if (ptr0 == NULL)
		return private_strdup ("");
	if (ptr1 != NULL && ptr0 < ptr1)
		return private_strdup ("");
	return private_strdup (ptr0 + 1);
}

/**
 * \brief Gets the base path name from a path.
 * \param path Path string.
 * \return New string or NULL.
 */
char* lisys_path_pathname (
	const char* path)
{
	char* ret;
	const char* ptr;

	ptr = strrchr (path, '/');
	if (ptr == NULL)
		return private_strdup (".");
	if (ptr == path)
		return private_strdup ("/");
	ret = lisys_malloc (ptr - path + 1);
	if (ret == NULL)
		return NULL;
	strncpy (ret, path, ptr - path);
	ret[ptr - path] = '\0';
	return ret;
}

/*****************************************************************************/

static char* private_strdup (
	const char* str)
{
	int len;
	char* ret;

	len = strlen (str);
	ret = lisys_malloc (len);
	if (ret == NULL)
		return NULL;
	strcpy (ret, str);

	return ret;
}

/** @} */
/** @} */
