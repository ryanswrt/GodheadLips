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

#ifndef __FONT_LAYOUT_H__
#define __FONT_LAYOUT_H__

#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "font.h"

typedef struct _LIFntLayoutGlyph LIFntLayoutGlyph;
struct _LIFntLayoutGlyph
{
	LIFntFont* font;
	wchar_t glyph;
	int x;
	int y;
	int advance;
};

typedef struct _LIFntLayout LIFntLayout;
struct _LIFntLayout
{
	int dirty;
	int ascent;
	int width;
	int height;
	int limit_width;
	int n_glyphs;
	int c_glyphs;
	LIFntLayoutGlyph* glyphs;
};

LIAPICALL (LIFntLayout*, lifnt_layout_new, ());

LIAPICALL (void, lifnt_layout_free, (
	LIFntLayout* self));

LIAPICALL (void, lifnt_layout_render, (
	LIFntLayout* self,
	int          x,
	int          y));

LIAPICALL (int, lifnt_layout_append_string, (
	LIFntLayout* self,
	LIFntFont*   font,
	const char*  string));

LIAPICALL (void, lifnt_layout_clear, (
	LIFntLayout* self));

LIAPICALL (int, lifnt_layout_get_height, (
	LIFntLayout* self));

LIAPICALL (int, lifnt_layout_get_vertices, (
	LIFntLayout* self,
	LIMdlIndex** result_index,
	float**      result_vertex));

LIAPICALL (int, lifnt_layout_get_width, (
	LIFntLayout* self));

LIAPICALL (int, lifnt_layout_get_width_limit, (
	const LIFntLayout* self));

LIAPICALL (void, lifnt_layout_set_width_limit, (
	LIFntLayout* self,
	int          width));

#endif
