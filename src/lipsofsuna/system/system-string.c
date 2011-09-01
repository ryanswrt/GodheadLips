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
 * \addtogroup LISysString String
 * @{
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iconv.h>
#include "system-memory.h"

/**
 * \brief Creates a duplicate of the string.
 * \param self String.
 * \return New string or NULL.
 */
char* lisys_string_dup (
	const char* self)
{
	int i;
	char* dup;

	for (i = 0 ; self[i] != '\0' ; i++)
		;
	dup = lisys_malloc (i + 1);
	if (dup != NULL)
		memcpy (dup, self, i + 1);

	return dup;
}

/**
 * \brief Creates a duplicate of the string.
 * \param self String.
 * \param count Number of characters to duplicate at most.
 * \return New string or NULL.
 */
char* lisys_string_dupn (
	const char* self,
	int         count)
{
	int i;
	char* dup;

	for (i = 0 ; self[i] != '\0' && i < count ; i++)
		;
	dup = lisys_malloc (i + 1);
	if (dup != NULL)
		memcpy (dup, self, i);
	dup[i] = '\0';

	return dup;
}

/**
 * \brief Concatenates two strings.
 * \param self String.
 * \param string String.
 * \return New string or NULL.
 */
char* lisys_string_concat (
	const char* self,
	const char* string)
{
	int len1;
	int len2;
	char* result;

	len1 = strlen (self);
	len2 = strlen (string);
	result = lisys_malloc (len1 + len2 + 1);
	if (result == NULL)
		return NULL;
	strcpy (result, self);
	strcpy (result + len1, string);

	return result;
}

/**
 * \brief Allocates a string and fills it with printf style format.
 * \param format Format string.
 * \param ... Variable arguments matching the format.
 * \return New string or NULL.
 */
char* lisys_string_format (
	const char* format,
	            ...)
{
	int len;
	int num;
	char* buf;
	char* tmp;
	va_list args;

	len = 64;
	buf = lisys_malloc (len);
	if (buf == NULL)
		return NULL;

	while (1)
	{
		/* Try to write to the buffer. */
		va_start (args, format);
		num = vsnprintf (buf, len - 1, format, args);
		va_end (args);
		if (-1 < num && num < len)
			break;

		/* Try to expand the buffer. */
		tmp = lisys_realloc (buf, len << 1);
		if (tmp == NULL)
		{
			lisys_free (buf);
			return NULL;
		}
		buf = tmp;
		len <<= 1;
	}

	/* Try to shrink the buffer. */
	tmp = lisys_realloc (buf, num + 1);
	if (tmp != NULL)
		buf = tmp;

	return buf;
}

/**
 * \brief Splits the string into substrings.
 * \param self String.
 * \param separator Separator character.
 * \param result Return location for the substring array.
 * \param resultn Return location for the substring count.
 * \return Nonzero on success.
 */
int lisys_string_split (
	const char* self,
	char        separator,
	char***     result,
	int*        resultn)
{
	int i;
	int count;
	int failed;
	char** slices;
	const char* end;
	const char* pos;

	/* Count slices. */
	count = 1;
	for (pos = self ; ; pos = end + 1)
	{
		end = strchr (pos, separator);
		if (end == NULL)
			break;
		count++;
	}

	/* Allocate array. */
	slices = lisys_calloc (count, sizeof (char**));
	if (slices == NULL)
		return 0;

	/* Create slices. */
	i = 0;
	failed = 0;
	for (pos = self ; ; pos = end + 1)
	{
		end = strchr (pos, separator);
		if (end == NULL)
		{
			slices[i] = lisys_string_dup (pos);
			if (slices[i] == NULL)
				failed = 1;
			break;
		}
		slices[i] = lisys_string_dupn (pos, end - pos);
		if (slices[i] == NULL)
		{
			failed = 1;
			break;
		}
		i++;
	}

	/* Handle errors. */
	if (failed)
	{
		for (i-- ; i >= 0 ; i--)
			lisys_free (slices[i]);
		lisys_free (slices);
		return 0;
	}

	/* Set result. */
	*result = slices;
	*resultn = count;

	return 1;
}

char* lisys_string_convert_sys_to_utf8 (
	const char* str)
{
	char* tmp;
	char* inbuf;
	char* outbuf;
	size_t n;
	size_t ninbytes;
	size_t noutbytes;
	iconv_t cd;

	/* Initialize buffers. */
	ninbytes = strlen (str);
	noutbytes = 4 * (ninbytes + 10);
	tmp = lisys_calloc (noutbytes, sizeof (char));
	if (tmp == NULL)
		return NULL;
	inbuf = (char*) str;
	outbuf = tmp;

	/* Initialize iconv. */
	cd = iconv_open ("UTF-8", "char");
	if (cd == (iconv_t) -1)
		cd = iconv_open ("UTF-8", "");
	if (cd == (iconv_t) -1)
	{
		lisys_free (tmp);
		return NULL;
	}

	/* Perform the conversion. */
	n = iconv (cd, &inbuf, &ninbytes, &outbuf, &noutbytes);
	iconv_close (cd);
	if (n == (size_t) -1)
	{
		lisys_free (tmp);
		return NULL;
	}

	return tmp;
}

/** @} */
/** @} */
