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
 * \addtogroup LISysUtf8 Utf8
 * @{
 */

#include "system-memory.h"
#include "system-utf8.h"

/* Stolen from Glib. */
#define UTF8_LENGTH(Char)              \
  ((Char) < 0x80 ? 1 :                 \
   ((Char) < 0x800 ? 2 :               \
    ((Char) < 0x10000 ? 3 :            \
     ((Char) < 0x200000 ? 4 :          \
      ((Char) < 0x4000000 ? 5 : 6)))))
#define UNICODE_VALID(Char)                   \
    ((Char) < 0x110000 &&                     \
     (((Char) & 0xFFFFF800) != 0xD800) &&     \
     ((Char) < 0xFDD0 || (Char) > 0xFDEF) &&  \
     ((Char) & 0xFFFE) != 0xFFFE)

/**
 * \brief Gets one full valid wide character from an UTF-8 string.
 *
 * This function is stolen from Glib.
 *
 * \param self String.
 * \param result Return location for the character.
 * \return Nonzero on success.
 */
int lisys_utf8_get_char (
	const char* self,
	wchar_t*    result)
{
	int i;
	int len;
	wchar_t ch;
	wchar_t wc = (unsigned char) *self;

	if (wc < 0x80)
	{
		*result = wc;
		return 1;
	}
	else if (wc < 0xC0)
	{
		return 0;
	}
	else if (wc < 0xE0)
	{
		len = 2;
		wc &= 0x1f;
	}
	else if (wc < 0xF0)
	{
		len = 3;
		wc &= 0x0f;
	}
	else if (wc < 0xF8)
	{
		len = 4;
		wc &= 0x07;
	}
	else if (wc < 0xFC)
	{
		len = 5;
		wc &= 0x03;
	}
	else if (wc < 0xFE)
	{
		len = 6;
		wc &= 0x01;
	}
	else
	{
		return 0;
	}
	for (i = 1 ; i < len ; i++)
	{
		ch = ((unsigned char*) self)[i];
		if ((ch & 0xC0) != 0x80)
			return 0;
		wc <<= 6;
		wc |= (ch & 0x3F);
	}
	if (UTF8_LENGTH (wc) != len)
		return 0;
	if (!(wc & 0x80000000) && !UNICODE_VALID (wc))
		return 0;
	*result = wc;
	return 1;
}

/**
 * \brief Gets the number of characters in an UTF-8 string.
 * \param self String.
 * \return Length or -1 on error.
 */
int lisys_utf8_get_length (
	const char* self)
{
	const char* ptr;
	wchar_t dummy;
	int length = 0;

	for (ptr = self ; *ptr != '\0' ; ptr = lisys_utf8_get_next (ptr))
	{
		if (!lisys_utf8_get_char (ptr, &dummy))
			return -1;
		length++;
	}

	return length;
}

/**
 * \brief Gets the beginning of the next character in an UTF-8 string.
 *
 * This function is stolen from Glib.
 *
 * \warning This function will break if the current character isn't valid.
 *
 * \param self String.
 * \return Pointer within the same string.
 */
char* lisys_utf8_get_next (
	const char* self)
{
	for (self++ ; (*self & 0xC0) == 0x80 ; self++);

	return (char*) self;
}

/**
 * \brief Checks if the UTF-8 string is valid.
 * \param self String.
 * \return Nonzero if valid.
 */
int lisys_utf8_get_valid (
	const char* self)
{
	const char* ptr;
	wchar_t dummy;

	for (ptr = self ; *ptr != '\0' ; ptr = lisys_utf8_get_next (ptr))
	{
		if (!lisys_utf8_get_char (ptr, &dummy))
			return 0;
	}

	return 1;
}

/**
 * \brief Converts an UTF-8 encoded string to wide characters.
 * \param self String.
 * \return Wide string or NULL on error.
 */
wchar_t* lisys_utf8_to_wchar (
	const char* self)
{
	int i = 0;
	int length;
	const char* ptr;
	wchar_t* result;

	/* Get length. */
	length = lisys_utf8_get_length (self);
	if (length == -1)
		return NULL;

	/* Allocate result. */
	result = lisys_malloc ((length + 1) * sizeof (wchar_t));
	if (result == NULL)
		return NULL;

	/* Actual conversion. */
	for (ptr = self, i = 0 ; i < length ; i++)
	{
		lisys_utf8_get_char (ptr, result + i);
		ptr = lisys_utf8_get_next (ptr);
	}
	result[length] = L'\0';

	return result;
}

/**
 * \brief Converts a wide character to a unicode string.
 *
 * This function is stolen from Glib.
 *
 * \param self Wide character.
 * \return New string or NULL.
 */
char* lisys_wchar_to_utf8 (
	wchar_t self)
{
	int i;
	int first;
	int len = 0;    
	char* result;

	if (self < 0x80)
	{
		first = 0;
		len = 1;
	}
	else if (self < 0x800)
	{
		first = 0xc0;
		len = 2;
	}
	else if (self < 0x10000)
	{
		first = 0xe0;
		len = 3;
	}
	else if (self < 0x200000)
	{
		first = 0xf0;
		len = 4;
	}
	else if (self < 0x4000000)
	{
		first = 0xf8;
		len = 5;
	}
	else
	{
		first = 0xfc;
		len = 6;
	}
	result = lisys_calloc (len + 1, sizeof (char));
	if (result == NULL)
		return NULL;
	for (i = len - 1 ; i > 0 ; --i)
	{
		result[i] = (self & 0x3f) | 0x80;
		self >>= 6;
	}
	result[0] = self | first;

	return result;
}

/** @} */
/** @} */
