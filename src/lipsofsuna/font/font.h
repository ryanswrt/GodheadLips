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

#ifndef __FONT_H__
#define __FONT_H__

#include <wchar.h>
#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/video.h>

#define LIFNT_CACHE_WIDTH 1024
#define LIFNT_CACHE_HEIGHT 1024

typedef struct _LIFntFontGlyph LIFntFontGlyph;
struct _LIFntFontGlyph
{
	wchar_t glyph;
	int table_index;
	int table_x;
	int table_y;
	int bearing_x;
	int bearing_y;
	int width;
	int height;
	int advance;
};

typedef struct _LIFntFont LIFntFont;
struct _LIFntFont
{
	int font_size;
	int font_height;
	int font_ascent;
	int font_descent;
	int table_length;
	int table_filled;
	int table_width;
	int table_height;
	int table_glyph_width;
	int table_glyph_height;
	GLuint texture;
	LIAlgU32dic* index;
	LIFntFontGlyph** table;
	TTF_Font* font;
	uint8_t reload_pixels[4 * LIFNT_CACHE_WIDTH * LIFNT_CACHE_HEIGHT];
};

LIAPICALL (LIFntFont*, lifnt_font_new, (
	const char* path,
	int         size));

LIAPICALL (void, lifnt_font_free, (
	LIFntFont* self));

LIAPICALL (void, lifnt_font_reload, (
	LIFntFont* self,
	int        pass));

LIAPICALL (int, lifnt_font_get_advance, (
	LIFntFont* self,
	wchar_t    glyph));

LIAPICALL (int, lifnt_font_get_height, (
	const LIFntFont* self));

LIAPICALL (void, lifnt_font_get_vertices, (
	LIFntFont* self,
	wchar_t    glyph,
	float*     result));

#endif
