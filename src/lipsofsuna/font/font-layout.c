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
 * \addtogroup LIFnt Font
 * @{
 * \addtogroup LIFntLayout Layout
 * @{
 */

#include "lipsofsuna/render.h"
#include "lipsofsuna/system.h"
#include "font-layout.h"

#define LIFNT_TEXT_DEFAULT_CAPACITY 32

static void
private_layout (LIFntLayout* self);

static int
private_get_line_height (LIFntLayout* self,
                         int          start,
                         int          end);

static int
private_get_line_ascent (LIFntLayout* self,
                         int          start,
                         int          end);

static int
private_get_line_width (LIFntLayout* self,
                        int          start,
                        int*         end);

static int
private_get_white_length (LIFntLayout* self,
                          int          start);

/*****************************************************************************/

LIFntLayout*
lifnt_layout_new ()
{
	LIFntLayout* self;

	/* Allocate self. */
	self = lisys_malloc (sizeof (LIFntLayout));
	if (self == NULL)
		return NULL;
	self->dirty = 0;
	self->limit_width = 0;
	self->n_glyphs = 0;
	self->c_glyphs = LIFNT_TEXT_DEFAULT_CAPACITY;

	/* Allocate glyphs. */
	self->glyphs = lisys_malloc (self->c_glyphs * sizeof (LIFntLayoutGlyph));
	if (self->glyphs == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
lifnt_layout_free (LIFntLayout* self)
{
	lisys_free (self->glyphs);
	lisys_free (self);
}

int
lifnt_layout_append_string (LIFntLayout* self,
                            LIFntFont*   font,
                            const char*  string)
{
	int i;
	int length;
	wchar_t* wstr;
	LIFntLayoutGlyph* glyph;

	/* Convert to wide characters. */
	wstr = lisys_utf8_to_wchar (string);
	if (wstr == NULL)
		return 0;
	length = wcslen (wstr);

	/* Allocate glyphs. */
	if (self->n_glyphs + length > self->c_glyphs)
	{
		glyph = lisys_realloc (self->glyphs, (self->n_glyphs + length) * sizeof (LIFntLayoutGlyph));
		if (glyph == NULL)
		{
			lisys_free (wstr);
			return 0;
		}
		self->glyphs = glyph;
		self->c_glyphs = self->n_glyphs + length;
	}

	/* Set character data. */
	for (i = 0 ; i < length ; i++)
	{
		glyph = self->glyphs + self->n_glyphs;
		glyph->font = font;
		glyph->glyph = wstr[i];
		glyph->advance = lifnt_font_get_advance (font, wstr[i]);
		self->n_glyphs++;
	}
	lisys_free (wstr);

	/* Needs relayouting. */
	self->dirty = 1;
	return 1;
}

void
lifnt_layout_clear (LIFntLayout* self)
{
	self->dirty = 1;
	self->glyphs = lisys_realloc (self->glyphs, LIFNT_TEXT_DEFAULT_CAPACITY * sizeof (LIFntLayoutGlyph));
	self->n_glyphs = 0;
	self->c_glyphs = LIFNT_TEXT_DEFAULT_CAPACITY;
}

int
lifnt_layout_get_height (LIFntLayout* self)
{
	private_layout (self);
	return self->height;
}

int lifnt_layout_get_vertices (
	LIFntLayout* self,
	LIMdlIndex** result_index,
	float**      result_vertex)
{
	int i;
	int j;
	float* vertices;
	LIMdlIndex* indices;
	LIFntLayoutGlyph* glyph;

	/* Layout glyphs. */
	private_layout (self);

	/* Allocate buffer data. */
	if (self->n_glyphs)
	{
		indices = lisys_calloc (6 * self->n_glyphs, sizeof (LIMdlIndex));
		if (indices == NULL)
			return 0;
		vertices = lisys_calloc (20 * self->n_glyphs, sizeof (float));
		if (vertices == NULL)
		{
			lisys_free (indices);
			return 0;
		}
	}
	else
	{
		indices = NULL;
		vertices = NULL;
	}

	/* Create vertices and indices. */
	for (i = 0 ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		indices[6 * i + 0] = 4 * i + 0;
		indices[6 * i + 1] = 4 * i + 1;
		indices[6 * i + 2] = 4 * i + 2;
		indices[6 * i + 3] = 4 * i + 1;
		indices[6 * i + 4] = 4 * i + 2;
		indices[6 * i + 5] = 4 * i + 3;
		lifnt_font_get_vertices (glyph->font, glyph->glyph, vertices + 20 * i);
		for (j = 2 ; j < 20 ; j += 5)
		{
			vertices[20 * i + j + 0] += glyph->x;
			vertices[20 * i + j + 1] += glyph->y;
		}
	}

	/* Return the buffers. */
	*result_index = indices;
	*result_vertex = vertices;

	return 1;
}

int
lifnt_layout_get_width (LIFntLayout* self)
{
	private_layout (self);
	return self->width;
}

int
lifnt_layout_get_width_limit (const LIFntLayout* self)
{
	return self->limit_width;
}

/**
 * \brief Sets the maximum width of the text.
 *
 * Setting the limit to zero disables the width limit.
 *
 * \param self A text object.
 * \param width The new width.
 */
void
lifnt_layout_set_width_limit (LIFntLayout* self,
                              int          width)
{
	self->limit_width = width;
	self->dirty = 1;
}

/*****************************************************************************/

static void
private_layout (LIFntLayout* self)
{
	int i;
	int w;
	int h = 0;
	int x;
	int y = 0;
	int start = 0;
	int end = 0;
	LIFntLayoutGlyph* glyph;

	if (!self->dirty)
		return;
	self->dirty = 0;
	self->width = 0;

	/* Wrap glyphs on lines. */
	for (start = 0 ; start < self->n_glyphs ; start = end + 1)
	{
		x = 0;
		w = private_get_line_width (self, start, &end);
		lisys_assert (end < self->n_glyphs);
		y += private_get_line_height (self, start, end);
		for (i = start ; i <= end ; i++)
		{
			glyph = self->glyphs + i;
			glyph->x = x;
			glyph->y = y;
			x += glyph->advance;
			/* FIXME: No kerning! */
		}
		if (!start)
			self->ascent = private_get_line_ascent (self, start, end);
		if (self->width < x)
			self->width = x;
		if (start)
			h += private_get_line_height (self, start, end);
	}
	self->height = y;
}

static int
private_get_white_length (LIFntLayout* self,
                          int          start)
{
	int i;
	LIFntLayoutGlyph* glyph;

	for (i = 0 ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		if (!iswspace (glyph->glyph))
			break;
	}
	return i;
}

static int
private_get_line_ascent (LIFntLayout* self,
                         int          start,
                         int          end)
{
	int i;
	int asctmp;
	int ascmax = 0;

	lisys_assert (end < self->n_glyphs);
	for (i = start ; i <= end ; i++)
	{
		asctmp = self->glyphs[i].font->font_ascent;
		if (ascmax < asctmp)
			ascmax = asctmp;
	}
	return ascmax;
}

static int
private_get_line_height (LIFntLayout* self,
                         int          start,
                         int          end)
{
	int i;
	int htmp;
	int hmax = 0;

	lisys_assert (end < self->n_glyphs);
	for (i = start ; i <= end ; i++)
	{
		htmp = lifnt_font_get_height (self->glyphs[i].font);
		if (hmax < htmp)
			hmax = htmp;
	}
	return hmax;
}

static int
private_get_line_width (LIFntLayout* self,
                        int          start,
                        int*         end)
{
	int i;
	int w;
	int x;
	int found = 0;
	LIFntLayoutGlyph* glyph;

	/* End of line? */
	for (x = 0, i = start ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		/* FIXME: No kerning. */
		x += glyph->advance;
		*end = i;
		if (glyph->glyph == L'\n')
			break;
	}
	if (!self->limit_width || x < self->limit_width)
	{
		if (i >= self->n_glyphs - 1)
			*end = self->n_glyphs - 1;
		return x;
	}

	/* End of word? */
	for (x = w = 0, i = start ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		if (iswspace (glyph->glyph))
		{
			found = 1;
			*end = i;
			w = x;
		}
		/* FIXME: No kerning. */
		if (x + glyph->advance >= self->limit_width)
			break;
		x += glyph->advance;
	}
	if (found)
		return w;

	/* End of glyph? */
	for (x = 0, i = start ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		/* FIXME: No kerning. */
		if (x + glyph->advance >= self->limit_width)
		{
			if (i > start)
				i--;
			break;
		}
		x += glyph->advance;
	}
	lisys_assert (i < self->n_glyphs);
	*end = i;

	return x;
}

/** @} */
/** @} */
