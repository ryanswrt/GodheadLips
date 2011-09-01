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
 * \addtogroup LIArcWriter Writer
 * @{
 */

#include <zlib.h>
#include "lipsofsuna/system.h"
#include "archive-writer.h"

#define FORMAT_BUFFER_SIZE 128
#define DEFAULT_BUFFER_SIZE 128
#define CLEAR_BUFFER_SIZE 1024

static void
private_file_close (LIArcWriter* self);

static int
private_file_write (LIArcWriter* self,
                    const void*  data,
                    int          size);

static void
private_gzip_close (LIArcWriter* self);

static int
private_gzip_write (LIArcWriter* self,
                    const void*  data,
                    int          size);

static void
private_memory_close (LIArcWriter* self);

static int
private_memory_write (LIArcWriter* self,
                      const void*  data,
                      int          size);

/****************************************************************************/

/**
 * \brief Creates a new writer.
 *
 * \return Writer or NULL if ran out of memory.
 */
LIArcWriter*
liarc_writer_new ()
{
	LIArcWriter* self;

	self = lisys_calloc (1, sizeof (LIArcWriter));
	if (self == NULL)
		return NULL;
	self->close = private_memory_close;
	self->write = private_memory_write;
	self->memory.capacity = DEFAULT_BUFFER_SIZE;
	self->memory.buffer = lisys_malloc (self->memory.capacity);
	if (self->memory.buffer == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->memory.buffer[0] = '\0';

	return self;
}

LIArcWriter*
liarc_writer_new_file (const char* path)
{
	LIArcWriter* self;

	self = lisys_calloc (1, sizeof (LIArcWriter));
	if (self == NULL)
		return NULL;
	self->close = private_file_close;
	self->write = private_file_write;
	self->file.pointer = fopen (path, "wb");
	if (self->file.pointer == NULL)
	{
		lisys_error_set (EIO, NULL);
		return NULL;
	}

	return self;
}

LIArcWriter*
liarc_writer_new_gzip (const char* path)
{
	LIArcWriter* self;

	self = lisys_calloc (1, sizeof (LIArcWriter));
	if (self == NULL)
		return NULL;
	self->close = private_gzip_close;
	self->write = private_gzip_write;
	self->gzip.pointer = gzopen (path, "wb9");
	if (self->gzip.pointer == NULL)
	{
		lisys_error_set (EIO, NULL);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a new packet writer and sets the type of the packet.
 *
 * \param type The type of the packet.
 * \return Writer or NULL if ran out of memory.
 */
LIArcWriter*
liarc_writer_new_packet (int type)
{
	LIArcWriter* self;

	self = lisys_calloc (1, sizeof (LIArcWriter));
	if (self == NULL)
		return NULL;
	self->close = private_memory_close;
	self->write = private_memory_write;
	self->memory.capacity = DEFAULT_BUFFER_SIZE;
	self->memory.buffer = lisys_malloc (self->memory.capacity);
	if (self->memory.buffer == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->memory.buffer[0] = type;
	self->memory.length = 1;

	return self;
}

/**
 * \brief Frees the writer.
 *
 * \param self Writer.
 */
void
liarc_writer_free (LIArcWriter* self)
{
	self->close (self);
	lisys_free (self);
}

/**
 * \brief Appends the contents of the file to the end of the writer.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param file The file to append.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_file (LIArcWriter* self,
                          FILE*        file)
{
	int ret;
	int size;
	char data[1024];

	for (ret = 1 ; !feof (file) ; )
	{
		size = fread (data, sizeof (char), sizeof (data), file);
		if (ferror (file))
		{
			ret = 0;
			break;
		}
		if (!self->write (self, data, size))
			ret = 0;
	}
	self->error = !ret;

	return ret;
}

/**
 * \brief Appends a formatted string to the end of the writer.
 *
 * Appends a formatted string to the end of the writer and places the write
 * position on the terminating NUL character. If you want the NUL character
 * to be written as well, use #liarc_writer_append_nul after this call.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param format Format of the string to append.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_format (LIArcWriter* self,
                            const char*  format,
                                         ...)
{
	int ret;
	int num;
	int size;
	char* tmp;
	char* tmp1;
	va_list args;

	size = FORMAT_BUFFER_SIZE;
	tmp = lisys_malloc (size);
	if (tmp == NULL)
	{
		self->error = 1;
		return 0;
	}
	while (1)
	{
		/* Try to write to the buffer. */
		va_start (args, format);
		num = vsnprintf (tmp, size, format, args);
		va_end (args);
		if (-1 < num && num < size)
		{
			ret = self->write (self, tmp, num);
			break;
		}
		tmp1 = lisys_realloc (tmp, (size <<= 1));
		if (tmp1 == NULL)
		{
			ret = 0;
			break;
		}
		tmp = tmp1;
	}
	lisys_free (tmp);

	return ret;
}

/**
 * \brief Appends a formatted variable argument string to the end of the writer.
 *
 * Appends a formated string to the end of the writer and places the write
 * position on the terminating NUL character. If you want the NUL character
 * to be written as well, use #liarc_writer_append_nul after this call.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param format Format of the string to append.
 * \param args Variable argument list conforming to the format.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_formatv (LIArcWriter* self,
                             const char*  format,
                             va_list      args)
{
	int ret;
	int num;
	int size;
	char* tmp;
	char* tmp1;

	size = FORMAT_BUFFER_SIZE;
	tmp = lisys_malloc (size);
	if (tmp == NULL)
	{
		self->error = 1;
		return 0;
	}
	while (1)
	{
		num = vsnprintf (tmp, size, format, args);
		if (-1 < num && num < size)
		{
			ret = self->write (self, tmp, num);
			break;
		}
		tmp1 = lisys_realloc (tmp, (size <<= 1));
		if (tmp1 == NULL)
		{
			ret = 0;
			break;
		}
		tmp = tmp1;
	}
	lisys_free (tmp);

	return ret;
}

/**
 * \brief Appends a string to the end of the writer.
 *
 * Appends a string to the end of the writer and places the write position on
 * the terminating NUL character. If you want the NUL character to be written
 * as well, use #liarc_writer_append_nul after this call.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param string The string to append.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_string (LIArcWriter* self,
                            const char*  string)
{
	if (!self->write (self, string, strlen (string)))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends a floating point number to the end of the writer.
 */
int
liarc_writer_append_float (LIArcWriter* self,
                           float        value)
{
	uint8_t tmp[4];

	if (lisys_endian_big ())
	{
		tmp[0] = ((uint8_t*) &value)[0];
		tmp[1] = ((uint8_t*) &value)[1];
		tmp[2] = ((uint8_t*) &value)[2];
		tmp[3] = ((uint8_t*) &value)[3];
	}
	else
	{
		tmp[0] = ((uint8_t*) &value)[3];
		tmp[1] = ((uint8_t*) &value)[2];
		tmp[2] = ((uint8_t*) &value)[1];
		tmp[3] = ((uint8_t*) &value)[0];
	}
	if (!self->write (self, tmp, 4))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends a signed 8-bit integer to the end of the writer.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param value Integer.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_int8 (LIArcWriter* self,
                          int8_t       value)
{
	if (!self->write (self, &value, 1))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends a signed 16-bit integer to the end of the writer.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param value Integer.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_int16 (LIArcWriter* self,
                           int16_t      value)
{
	value = lisys_htons (value);
	if (!self->write (self, &value, 2))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends a signed 32-bit integer to the end of the writer.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param value Integer.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_int32 (LIArcWriter* self,
                           int32_t      value)
{
	value = lisys_htonl (value);
	if (!self->write (self, &value, 4))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends a NUL character to the end of the writer.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_nul (LIArcWriter* self)
{
	uint8_t value = 0;

	if (!self->write (self, &value, 1))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends an unsigned 8-bit integer to the end of the writer.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param value Integer.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_uint8 (LIArcWriter* self,
                           uint8_t      value)
{
	if (!self->write (self, &value, 1))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends an unsigned 16-bit integer to the end of the writer.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param value Integer.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_uint16 (LIArcWriter* self,
                            uint16_t     value)
{
	value = lisys_htons (value);
	if (!self->write (self, &value, 2))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends an unsigned 32-bit integer to the end of the writer.
 *
 * Upon failure, the writer is set to error state.
 * \param self Writer.
 * \param value Integer.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_uint32 (LIArcWriter* self,
                            uint32_t     value)
{
	value = lisys_htonl (value);
	if (!self->write (self, &value, 4))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Appends raw data to the writer.
 *
 * Upon failure, the writer is set to error state.
 *
 * \param self Writer.
 * \param data Raw data.
 * \param size Number of bytes to writer.
 * \return One on success, zero on failure.
 */
int
liarc_writer_append_raw (LIArcWriter* self,
                         const void*  data,
                         int          size)
{
	if (!self->write (self, data, size))
	{
		self->error = 1;
		return 0;
	}

	return 1;
}

/**
 * \brief Clears the buffer.
 *
 * Sets the write position to the beginning of the buffer and sets the first
 * byte to zero. If the buffer is very large, it may be shrunk.
 *
 * Attempt to clear a writer with type other than a memory buffer is an error.
 *
 * \param self Writer.
 */
void
liarc_writer_clear (LIArcWriter* self)
{
	char* tmp;

	lisys_assert (self->write == private_memory_write);

	/* Resize the buffer. */
	if (self->memory.capacity > CLEAR_BUFFER_SIZE)
	{
		tmp = lisys_realloc (self->memory.buffer, CLEAR_BUFFER_SIZE);
		if (tmp != NULL)
		{
			self->memory.buffer = tmp;
			self->memory.capacity = CLEAR_BUFFER_SIZE;
		}
	}

	/* Move to the beginning. */
	self->memory.buffer[0] = '\0';
	self->memory.length = 0;
	self->error = 0;
}

/**
 * \brief Erases bytes from the end of the buffer.
 *
 * If the string is shorter than the requested number of bytes,
 * the entire string is discarded and zero is returned.
 *
 * Attempt to erase a writer with type other than a memory buffer is an error.
 *
 * \param self Writer.
 * \param size Number of bytes to discard.
 * \return Nonzero if exactly the requested amount was discarded.
 */
int
liarc_writer_erase (LIArcWriter* self,
                    int          size)
{
	lisys_assert (self->write == private_memory_write);

	if (size < self->memory.length)
	{
		self->memory.length -= size;
		self->memory.buffer[self->memory.length] = '\0';
		return 1;
	}
	else
	{
		self->memory.length = 0;
		self->memory.buffer[0] = '\0';
		return 0;
	}
}

/**
 * \brief Gets the buffer of the writer.
 *
 * Attempt to access writer type other than memory buffer is an error.
 *
 * \param self Writer.
 * \return Write buffer.
 */
const char*
liarc_writer_get_buffer (const LIArcWriter* self)
{
	lisys_assert (self->write == private_memory_write);

	return self->memory.buffer;
}

/**
 * \brief Gets the length of the writer.
 *
 * Attempt to access writer type other than memory buffer is an error.
 *
 * \param self Writer.
 * \return Buffer length.
 */
int
liarc_writer_get_length (const LIArcWriter* self)
{
	lisys_assert (self->write == private_memory_write);

	return self->memory.length;
}

/****************************************************************************/

static void
private_file_close (LIArcWriter* self)
{
	fclose (self->file.pointer);
}

static int
private_file_write (LIArcWriter* self,
                    const void*  data,
                    int          size)
{
	if (size && !fwrite (data, size, 1, self->file.pointer))
	{
		lisys_error_set (EIO, "write error");
		return 0;
	}

	return 1;
}

static void
private_gzip_close (LIArcWriter* self)
{
	gzclose (self->gzip.pointer);
}

static int
private_gzip_write (LIArcWriter* self,
                    const void*  data,
                    int          size)
{
	if (size && !gzwrite (self->gzip.pointer, data, size))
	{
		lisys_error_set (EIO, "write error");
		return 0;
	}

	return 1;
}

static void
private_memory_close (LIArcWriter* self)
{
	lisys_free (self->memory.buffer);
}

static int
private_memory_write (LIArcWriter* self,
                      const void*  data,
                      int          size)
{
	int cap;
	char* tmp;

	/* Try to expand the buffer. */
	if (self->memory.length + size > self->memory.capacity)
	{
		for (cap = self->memory.capacity ; cap < self->memory.length + size ; cap <<= 1);
		tmp = lisys_realloc (self->memory.buffer, cap);
		if (tmp == NULL)
		{
			self->error = 1;
			return 0;
		}
		self->memory.buffer = tmp;
		self->memory.capacity = cap;
	}

	/* Write the data to the buffer. */
	memcpy (self->memory.buffer + self->memory.length, data, size);
	self->memory.length += size;

	return 1;
}

/** @} */
/** @} */

