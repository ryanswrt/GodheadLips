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

#ifndef __ARCHIVE_READER_H__
#define __ARCHIVE_READER_H__

#include <lipsofsuna/system.h>

typedef struct _LIArcReader LIArcReader;
struct _LIArcReader
{
	int pos;
	int length;
	LISysMmap* mmap;
	const char* buffer;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LIArcReader*, liarc_reader_new, (const char* buffer, int length));
LIAPICALL (LIArcReader*, liarc_reader_new_from_file, (const char* path));
LIAPICALL (LIArcReader*, liarc_reader_new_from_string, (const char* buffer));
LIAPICALL (void, liarc_reader_free, (LIArcReader* self));
LIAPICALL (int, liarc_reader_check_end, (LIArcReader* self));
LIAPICALL (int, liarc_reader_check_char, (LIArcReader* self, char c));
LIAPICALL (int, liarc_reader_check_data, (LIArcReader* self, const void* data, int length));
LIAPICALL (int, liarc_reader_check_text, (LIArcReader* self, const char* data, const char* list));
LIAPICALL (int, liarc_reader_check_key_value_pair, (LIArcReader* self, const char* key, const char* value));
LIAPICALL (int, liarc_reader_check_uint32, (LIArcReader* self, uint32_t value));
LIAPICALL (int, liarc_reader_get_char, (LIArcReader* self, char* value));
LIAPICALL (int, liarc_reader_get_float, (LIArcReader* self, float* value));
LIAPICALL (int, liarc_reader_get_int8, (LIArcReader* self, int8_t* value));
LIAPICALL (int, liarc_reader_get_int16, (LIArcReader* self, int16_t* value));
LIAPICALL (int, liarc_reader_get_int32, (LIArcReader* self, int32_t* value));
LIAPICALL (int, liarc_reader_get_offset, (LIArcReader* self));
LIAPICALL (int, liarc_reader_set_offset, (LIArcReader* self, int offset));
LIAPICALL (int, liarc_reader_get_uint8, (LIArcReader* self, uint8_t* value));
LIAPICALL (int, liarc_reader_get_uint16, (LIArcReader* self, uint16_t* value));
LIAPICALL (int, liarc_reader_get_uint32, (LIArcReader* self, uint32_t* value));
LIAPICALL (int, liarc_reader_get_text, (LIArcReader* self, const char* list, char** value));
LIAPICALL (int, liarc_reader_get_text_int, (LIArcReader* self, int* value));
LIAPICALL (int, liarc_reader_get_text_uint, (LIArcReader* self, int* value));
LIAPICALL (int, liarc_reader_get_text_float, (LIArcReader* self, float* value));
LIAPICALL (int, liarc_reader_get_key_value_pair, (LIArcReader* self, char** key, char** value));
LIAPICALL (int, liarc_reader_skip_bytes, (LIArcReader* self, int num));
LIAPICALL (int, liarc_reader_skip_chars, (LIArcReader* self, const char* list));

#ifdef __cplusplus
}
#endif

#endif
