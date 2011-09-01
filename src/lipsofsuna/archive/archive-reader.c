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
 * \addtogroup LIArc Archive
 * @{
 * \addtogroup LIArcReader Reader
 * @{
 */

#include "lipsofsuna/system.h"
#include "archive-reader.h"

/**
 * \brief Creates a new stream reader.
 *
 * \param buffer Buffer to read.
 * \param length Length of the buffer.
 * \return New packet reader or NULL if ran out of memory.
 */
LIArcReader*
liarc_reader_new (const char* buffer,
                  int         length)
{
	LIArcReader* self;

	self = lisys_malloc (sizeof (LIArcReader));
	if (self == NULL)
		return NULL;
	self->pos = 0;
	self->mmap = NULL;
	self->length = length;
	self->buffer = buffer;

	return self;
}

/**
 * \brief Creates a new stream reader from a file.
 *
 * \param path Path to the file.
 * \return New reader or NULL.
 */
LIArcReader*
liarc_reader_new_from_file (const char* path)
{
	LIArcReader* self;

	/* Allocate self. */
	self = lisys_malloc (sizeof (LIArcReader));
	if (self == NULL)
		return NULL;
	self->pos = 0;

	/* Memory map the file. */
	self->mmap = lisys_mmap_open (path);
	if (self->mmap == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->length = lisys_mmap_get_size (self->mmap);
	self->buffer = lisys_mmap_get_buffer (self->mmap);

	return self;
}

/**
 * \brief Creates a new stream reader from a NUL terminated string.
 *
 * \param buffer String to read.
 * \return New packet reader or NULL.
 */
LIArcReader*
liarc_reader_new_from_string (const char* buffer)
{
	LIArcReader* self;

	self = lisys_malloc (sizeof (LIArcReader));
	if (self == NULL)
		return NULL;
	self->pos = 0;
	self->mmap = NULL;
	self->length = strlen (buffer) + 1;
	self->buffer = buffer;
	return self;
}

/**
 * \brief Frees the stream reader.
 *
 * \param self Stream reader.
 */
void liarc_reader_free (LIArcReader* self)
{
	if (self->mmap != NULL)
		lisys_mmap_free (self->mmap);
	lisys_free (self);
}

/**
 * \brief Checks if the reader is at its end position.
 *
 * \param self Stream reader.
 * \return Nonzero if the end has been reached.
 */
int
liarc_reader_check_end (LIArcReader* self)
{
	return (self->pos == self->length);
}

/**
 * \brief Checks for a single character.
 *
 * The reader position is only advanced if the read and the check succeed.
 *
 * \param self Packet reader.
 * \param chr Character expected.
 * \return Nonzero on success.
 */
int
liarc_reader_check_char (LIArcReader* self,
                         char         chr)
{
	if (self->pos == self->length)
		return 0;
	if (self->buffer[self->pos] != chr)
		return 0;
	self->pos++;
	return 1;
}

/**
 * \brief Checks for a sequence of bytes.
 *
 * The reader position is only advanced if the read and the check succeed.
 *
 * \param self Stream reader.
 * \param data Expected data.
 * \param length Number of bytes expected.
 * \return Nonzero on success.
 */
int
liarc_reader_check_data (LIArcReader* self,
                         const void*  data,
                         int          length)
{
	/* Check for read errors. */
	if (self->pos + length > self->length)
		return 0;

	/* Compare the data. */
	if (memcmp (self->buffer + self->pos, data, length) != 0)
		return 0;

	/* Advance the position. */
	self->pos += length;
	return 1;
}

/**
 * \brief Checks for a string.
 *
 * The reader position is only advanced if the read and the check succeed.
 *
 * \param self Stream reader.
 * \param data Expected string.
 * \param list List of delimiters.
 * \return Nonzero on success.
 */
int
liarc_reader_check_text (LIArcReader* self,
                         const char*  data,
                         const char*  list)
{
	char* str;
	int tmp = self->pos;

	/* Avoid overwriting error. */
	if (self->pos >= self->length)
		return 0;

	/* Read text. */
	if (!liarc_reader_get_text (self, list, &str))
		return 0;

	/* Compare to the expected value. */
	if (strcmp (str, data))
	{
		lisys_free (str);
		self->pos = tmp;
		return 0;
	}
	lisys_free (str);
	return 1;
}

/**
 * \brief Checks for a key value pair.
 *
 * The reader position is only advanced if the read and the check succeed.
 *
 * \param self Stream reader.
 * \param key Expected key.
 * \param value Expected value.
 * \return Nonzero on success.
 */
int
liarc_reader_check_key_value_pair (LIArcReader* self,
                                   const char*  key,
                                   const char*  value)
{
	char* k;
	char* v;
	int tmp = self->pos;

	/* Avoid overwriting error. */
	liarc_reader_skip_chars (self, " \t");
	if (self->pos >= self->length)
		return 0;

	/* Read the key. */
	if (!liarc_reader_get_text (self, " \t", &k))
		return 0;
	if (strchr (k, '\n'))
	{
		self->pos = tmp;
		lisys_free (k);
		return 0;
	}
	if (strcmp (key, k))
	{
		self->pos = tmp;
		lisys_free (k);
		return 0;
	}
	lisys_free (k);

	/* Avoid overwriting error. */
	liarc_reader_skip_chars (self, " \t");
	if (self->pos >= self->length)
	{
		self->pos = tmp;
		return 0;
	}

	/* Read the value. */
	if (!liarc_reader_get_text (self, "\n", &v))
	{
		self->pos = tmp;
		return 0;
	}
	if (strcmp (value, v))
	{
		self->pos = tmp;
		lisys_free (v);
		return 0;
	}
	lisys_free (v);

	return 1;
}

/**
 * \brief Checks for a signed 32-bit binary value.
 *
 * The reader position is only advanced if the read and the check succeed.
 *
 * \param self Stream reader.
 * \param value Expected value.
 * \return Nonzero on success.
 */
int
liarc_reader_check_uint32 (LIArcReader* self,
                           uint32_t     value)
{
	int pos;
	int ret;
	uint32_t tmp;

	pos = self->pos;
	ret = liarc_reader_get_uint32 (self, &tmp);
	if (!ret || tmp != value)
	{
		lisys_error_get (NULL);
		return 0;
	}

	return 1;
}

/**
 * \brief Reads a single character.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_char (LIArcReader* self,
                       char*        value)
{
	/* Check for read errors. */
	if (self->pos >= self->length)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Read the value. */
	*value = ((char*)(self->buffer + self->pos))[0];
	self->pos++;
	return 1;
}

/**
 * \brief Reads a floating point number.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 */
int
liarc_reader_get_float (LIArcReader* self,
                        float*       value)
{
	union { float f; uint8_t b[4]; } tmp;

	/* Check for read errors. */
	if (self->pos >= self->length - 3)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Read the value. */
	if (lisys_endian_big ())
	{
		tmp.b[0] = ((uint8_t*)(self->buffer + self->pos))[0];
		tmp.b[1] = ((uint8_t*)(self->buffer + self->pos))[1];
		tmp.b[2] = ((uint8_t*)(self->buffer + self->pos))[2];
		tmp.b[3] = ((uint8_t*)(self->buffer + self->pos))[3];
	}
	else
	{
		tmp.b[0] = ((uint8_t*)(self->buffer + self->pos))[3];
		tmp.b[1] = ((uint8_t*)(self->buffer + self->pos))[2];
		tmp.b[2] = ((uint8_t*)(self->buffer + self->pos))[1];
		tmp.b[3] = ((uint8_t*)(self->buffer + self->pos))[0];
	}
	*value = tmp.f;
	self->pos += 4;
	return 1;
}

/**
 * \brief Reads a signed 8-bit binary integer.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_int8 (LIArcReader* self,
                       int8_t*      value)
{
	/* Check for read errors. */
	if (self->pos >= self->length)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Read the value. */
	*value = ((int8_t*)(self->buffer + self->pos))[0];
	self->pos++;
	return 1;
}

/**
 * \brief Reads a signed 16-bit binary integer.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_int16 (LIArcReader* self,
                        int16_t*     value)
{
	int8_t* tmp;

	/* Check for read errors. */
	if (self->pos >= self->length - 1)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Read the value. */
	tmp = (int8_t*)(self->buffer + self->pos);
	*value = (((int16_t) tmp[0]) << 8) |
	         (((int16_t) tmp[1]) << 0);
	self->pos += 2;
	return 1;
}

/**
 * \brief Reads a signed 32-bit binary integer.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_int32 (LIArcReader* self,
                        int32_t*     value)
{
	int8_t* tmp;

	/* Check for read errors. */
	if (self->pos >= self->length - 3)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Read the value. */
	tmp = (int8_t*)(self->buffer + self->pos);
	*value = (((int32_t) tmp[0]) << 24) |
	         (((int32_t) tmp[1]) << 16) |
	         (((int32_t) tmp[2]) << 8) |
	         (((int32_t) tmp[3]) << 0);
	self->pos += 4;
	return 1;
}

/**
 * \brief Gets the current read offset.
 *
 * \param self Stream reader.
 * \return Stream offset.
 */
int
liarc_reader_get_offset (LIArcReader* self)
{
	return self->pos;
}

/**
 * \brief Seeks to the requested offset.
 *
 * \param self Stream reader.
 * \param offset Stream offset.
 * \return Nonzero on success.
 */
int
liarc_reader_set_offset (LIArcReader* self,
                         int          offset)
{
	if (offset < 0 || offset > self->length)
		return 0;
	self->pos = offset;

	return 1;
}

/**
 * \brief Reads an unsigned 8-bit binary integer.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_uint8 (LIArcReader* self,
                        uint8_t*     value)
{
	/* Check for read errors. */
	if (self->pos >= self->length)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Read the value. */
	*value = ((uint8_t*)(self->buffer + self->pos))[0];
	self->pos++;
	return 1;
}

/**
 * \brief Reads an unsigned 16-bit binary integer.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_uint16 (LIArcReader* self,
                         uint16_t*    value)
{
	uint8_t* tmp;

	/* Check for read errors. */
	if (self->pos >= self->length - 1)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Read the value. */
	tmp = (uint8_t*)(self->buffer + self->pos);
	*value = (((uint16_t) tmp[0]) << 8) |
	         (((uint16_t) tmp[1]) << 0);
	self->pos += 2;
	return 1;
}

/**
 * \brief Reads an unsigned 32-bit binary integer.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_uint32 (LIArcReader* self,
                         uint32_t*    value)
{
	uint8_t* tmp;

	/* Check for read errors. */
	if (self->pos >= self->length - 3)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Read the value. */
	tmp = (uint8_t*)(self->buffer + self->pos);
	*value = (((uint32_t) tmp[0]) << 24) |
	         (((uint32_t) tmp[1]) << 16) |
	         (((uint32_t) tmp[2]) << 8) |
	         (((uint32_t) tmp[3]) << 0);
	self->pos += 4;
	return 1;
}

/**
 * \brief Reads a string terminated with a delimiter character or NUL.
 *
 * The final position is one byte after the delimiter and the last
 * character in the returned string is the delimiter if one was found.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param list List of delimiter character.
 * \param value Return location for the newly allocated string.
 * \return Nonzero on success.
 */
int
liarc_reader_get_text (LIArcReader* self,
                       const char*  list,
                       char**       value)
{
	int i;
	int tmp = 0;
	char* string;

	/* Check for read errors. */
	if (self->pos >= self->length)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Get the length of the string. */
	while (self->buffer[self->pos + tmp] != '\0')
	{
		for (i = 0 ; list[i] != '\0' ; i++)
		{
			if (self->buffer[self->pos + tmp] == list[i])
				break;
		}
		if (list[i] != '\0')
			break;
		tmp++;
		if (self->pos + tmp == self->length)
			break;
	}

	/* Allocate a string. */
	string = lisys_malloc (tmp + 1);
	if (string == NULL)
		return 0;
	strncpy (string, self->buffer + self->pos, tmp);
	string[tmp] = '\0';
	self->pos += tmp + 1;
	if (self->pos > self->length)
		self->pos = self->length;
	*value = string;
	return 1;
}

/**
 * \brief Reads an ASCII integer.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_text_int (LIArcReader* self,
                           int*         value)
{
	int tmp = 0;
	int sign = 1;
	*value = 0;

	/* Check for read errors. */
	if (self->pos >= self->length)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Check for sign. */
	if (self->buffer[self->pos] == '-')
	{
		tmp = 1;
		sign = -1;
		if (self->pos + 1 >= self->length)
		{
			lisys_error_set (EINVAL, "unexpected end of stream");
			return 0;
		}
	}

	/* Check for syntax. */
	if (self->buffer[self->pos + tmp] < '0' ||
	    self->buffer[self->pos + tmp] > '9')
	{
		lisys_error_set (EINVAL, "expected a number");
		return 0;
	}
	if (self->pos + tmp + 1 < self->length &&
	    self->buffer[self->pos + tmp] == '0' &&
	    self->buffer[self->pos + tmp + 1] >= '0' &&
	    self->buffer[self->pos + tmp + 1] <= '9')
	{
		lisys_error_set (EINVAL, "multiple leading zeros");
		return 0;
	}

	/* Read in the digits. */
	while (1)
	{
		*value *= 10;
		*value += self->buffer[self->pos + tmp] - '0';
		tmp++;
		if (tmp == self->length - self->pos)
			break;
		if (self->buffer[self->pos + tmp] < '0' ||
		    self->buffer[self->pos + tmp] > '9')
			break;
	}

	/* Advance the position. */
	self->pos += tmp;
	*value *= sign;
	return 1;
}

/**
 * \brief Reads an unsigned ASCII integer.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_text_uint (LIArcReader* self,
                            int*         value)
{
	int tmp = 0;
	*value = 0;

	/* Check for read errors. */
	if (self->pos >= self->length)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Check for syntax. */
	if (self->buffer[self->pos] == '-')
	{
		lisys_error_set (EINVAL, "expected an unsigned value but got a minus sign");
		return 0;
	}
	if (self->buffer[self->pos] < '0' ||
	    self->buffer[self->pos] > '9')
	{
		lisys_error_set (EINVAL, "expected a number");
		return 0;
	}
	if (self->pos + 1 < self->length &&
	    self->buffer[self->pos] == '0' &&
	    self->buffer[self->pos + 1] >= '0' &&
	    self->buffer[self->pos + 1] <= '9')
	{
		lisys_error_set (EINVAL, "multiple leading zeros");
		return 0;
	}

	/* Read in the digits. */
	while (1)
	{
		*value *= 10;
		*value += self->buffer[self->pos + tmp] - '0';
		tmp++;
		if (tmp == self->length - self->pos)
			break;
		if (self->buffer[self->pos + tmp] < '0' ||
		    self->buffer[self->pos + tmp] > '9')
			break;
	}

	/* Advance the position. */
	self->pos += tmp;
	return 1;
}

/**
 * \brief Reads an ASCII floating point number.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param value Return location for the value.
 * \return Nonzero on success.
 */
int
liarc_reader_get_text_float (LIArcReader* self,
                             float*       value)
{
	int tmp = 0;
	int sign = 1;
	float mul;
	*value = 0;

	/* Check for read errors. */
	if (self->pos >= self->length)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Check for sign. */
	if (self->buffer[self->pos] == '-')
	{
		tmp = 1;
		sign = -1;
		if (self->pos + 1 >= self->length)
		{
			lisys_error_set (EINVAL, "unexpected end of stream");
			return 0;
		}
	}

	/* Check for syntax. */
	if (self->buffer[self->pos + tmp] < '0' ||
	    self->buffer[self->pos + tmp] > '9')
	{
		lisys_error_set (EINVAL, "expected a number");
		return 0;
	}
	if (self->pos + tmp + 1 < self->length &&
	    self->buffer[self->pos + tmp] == '0' &&
	    self->buffer[self->pos + tmp + 1] >= '0' &&
	    self->buffer[self->pos + tmp + 1] <= '9')
	{
		lisys_error_set (EINVAL, "multiple leading zeros");
		return 0;
	}

	/* Read in the digits. */
	while (1)
	{
		*value *= 10;
		*value += self->buffer[self->pos + tmp] - '0';
		tmp++;
		if (tmp == self->length - self->pos)
			break;
		if (self->buffer[self->pos + tmp] < '0' ||
		    self->buffer[self->pos + tmp] > '9')
			break;
	}

	/* Stop if there is no decimal part. */
	if (self->pos + tmp + 1 >= self->length ||
	    self->buffer[self->pos + tmp] != '.' ||
		self->buffer[self->pos + tmp + 1] < '0' ||
	    self->buffer[self->pos + tmp + 1] > '9')
	{
		self->pos += tmp;
		*value *= sign;
		return 1;
	}
	tmp++;

	/* Read in the decimals. */
	for (mul = 0.1f ; 1 ; mul *= 0.1f)
	{
		*value += mul * (self->buffer[self->pos + tmp] - '0');
		tmp++;
		if (tmp == self->length - self->pos)
			break;
		if (self->buffer[self->pos + tmp] < '0' ||
		    self->buffer[self->pos + tmp] > '9')
			break;
	}
	
	/* Return the value. */
	self->pos += tmp;
	*value *= sign;
	return 1;
}

/**
 * \brief Parses a key value pair.
 *
 * The reader position is only advanced if the read succeeds.
 *
 * \param self Stream reader.
 * \param key A return location for a newly allocated string.
 * \param value A return location for a newly allocated string.
 * \return Nonzero on success.
 */
int
liarc_reader_get_key_value_pair (LIArcReader* self,
                                 char**       key,
                                 char**       value)
{
	int tmp = self->pos;

	/* Read the key. */
	liarc_reader_skip_chars (self, " \t");
	if (!liarc_reader_get_text (self, " \t", key))
	{
		*key = NULL;
		*value = NULL;
		self->pos = tmp;
		return 0;
	}
	if (strchr (*key, '\n'))
	{
		lisys_free (*key);
		*key = NULL;
		*value = NULL;
		self->pos = tmp;
		lisys_error_set (EINVAL, "newline between a key and a value");
		return 0;
	}

	/* Read the value. */
	liarc_reader_skip_chars (self, " \t");
	if (!liarc_reader_get_text (self, "\n", value))
	{
		lisys_free (*key);
		*key = NULL;
		*value = NULL;
		self->pos = tmp;
		return 0;
	}
	return 1;
}

/**
 * \brief Skips a number of bytes.
 *
 * The reader position is only advanced if the skip succeeds.
 *
 * \param self Stream reader.
 * \param num Number of bytes to skip.
 * \return Nonzero on success.
 */
int liarc_reader_skip_bytes (
	LIArcReader* self,
	int          num)
{
	/* Check for read errors. */
	if (self->pos > self->length - num)
	{
		lisys_error_set (EINVAL, "unexpected end of stream");
		return 0;
	}

	/* Skip bytes. */
	self->pos += num;
	return 1;
}

/**
 * \brief Skips characters appearing in the list.
 *
 * \param self Stream reader.
 * \param list List of characters to skip.
 * \return Number of characters skipped.
 */
int
liarc_reader_skip_chars (LIArcReader* self,
                         const char*  list)
{
	int c = 0;
	const char* p;

	/* Skip characters. */
	while (self->pos < self->length)
	{
		for (p = list ; *p ; p++)
		{
			if (*p == self->buffer[self->pos])
				break;
		}
		if (*p == '\0')
			break;
		self->pos++;
		c++;
	}

	return c;
}

/** @} */
/** @} */
