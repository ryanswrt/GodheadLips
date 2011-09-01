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

#ifndef __ARCHIVE_WRITER_H__
#define __ARCHIVE_WRITER_H__

#include <stdio.h>
#include <stdarg.h>
#include <lipsofsuna/system.h>

typedef struct _LIArcWriter LIArcWriter;
struct _LIArcWriter
{
	int error;
	void (*close)(LIArcWriter*);
	int (*write)(LIArcWriter*, const void*, int);
	struct
	{
		FILE* pointer;
	} file;
	struct
	{
		void* pointer;
	} gzip;
	struct
	{
		char* buffer;
		int length;
		int capacity;
	} memory;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LIArcWriter*, liarc_writer_new, ());

LIAPICALL (LIArcWriter*, liarc_writer_new_file, (
	const char* path));

LIAPICALL (LIArcWriter*, liarc_writer_new_gzip, (
	const char* path));

LIAPICALL (LIArcWriter*, liarc_writer_new_packet, (
	int type));

LIAPICALL (void, liarc_writer_free, (
	LIArcWriter* self));

LIAPICALL (int, liarc_writer_append_file, (
	LIArcWriter* self,
	FILE*        file));

LIAPICALL (int, liarc_writer_append_format, (
	LIArcWriter* self,
	const char*  format,
	             ...)) LISYS_ATTR_FORMAT(2, 3);

LIAPICALL (int, liarc_writer_append_formatv, (
	LIArcWriter* self,
	const char*  format,
	va_list      args));

LIAPICALL (int, liarc_writer_append_string, (
	LIArcWriter* self,
	const char*  string));

LIAPICALL (int, liarc_writer_append_float, (
	LIArcWriter* self,
	float        value));

LIAPICALL (int, liarc_writer_append_int8, (
	LIArcWriter* self,
	int8_t       value));

LIAPICALL (int, liarc_writer_append_int16, (
	LIArcWriter* self,
	int16_t      value));

LIAPICALL (int, liarc_writer_append_int32, (
	LIArcWriter* self,
	int32_t      value));

LIAPICALL (int, liarc_writer_append_nul, (
	LIArcWriter* self));

LIAPICALL (int, liarc_writer_append_uint8, (
	LIArcWriter* self,
	uint8_t      value));

LIAPICALL (int, liarc_writer_append_uint16, (
	LIArcWriter* self,
	uint16_t     value));

LIAPICALL (int, liarc_writer_append_uint32, (
	LIArcWriter* self,
	uint32_t     value));

LIAPICALL (int, liarc_writer_append_raw, (
	LIArcWriter* self,
	const void*  data,
	int          size));

LIAPICALL (void, liarc_writer_clear, (
	LIArcWriter* self));

LIAPICALL (int, liarc_writer_erase, (
	LIArcWriter* self,
	int          size));

LIAPICALL (const char*, liarc_writer_get_buffer, (
	const LIArcWriter* self));

LIAPICALL (int, liarc_writer_get_length, (
	const LIArcWriter* self));

#ifdef __cplusplus
}
#endif

#endif
