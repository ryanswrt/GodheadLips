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
 * \addtogroup LIFntFont Font
 * @{
 */

#include <lipsofsuna/system.h>
#include "font.h"

static LIFntFontGlyph*
private_cache_glyph (LIFntFont* self,
                     wchar_t    glyph);

/*****************************************************************************/

LIFntFont* lifnt_font_new (
	const char* path,
	int         size)
{
	FILE* file;
	SDL_RWops* rw;
	LIFntFont* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIFntFont));
	if (self == NULL)
		return NULL;

	/* Open the font. */
	/* SDL RWops don't seem to work well together with MinGW when the filename
	   contains unicode characters. Since TTF_OpenFont uses RWops internally,
	   we need to work around by creating our own RWops from a file pointer. */
	file = fopen (path, "rb");
	if (file == NULL)
	{
		lisys_error_set (EIO, "cannot open font file `%s'", path);
		return NULL;
	}
	rw = SDL_RWFromFP (file, 1);
	if (rw == NULL)
	{
		lisys_error_set (EIO, "cannot create rwops for font file `%s'", path);
		return NULL;
	}

	/* Read the font. */
	self->font = TTF_OpenFontRW (rw, 1, size);
	if (self->font == NULL)
	{
		lisys_error_set (EIO, "cannot read font `%s': %s", path, TTF_GetError ());
		lifnt_font_free (self);
		return NULL;
	}
	self->font_size = size;
	self->font_height = TTF_FontLineSkip (self->font);
	self->font_ascent = TTF_FontAscent (self->font);
	self->font_descent = TTF_FontDescent (self->font);

	/* Allocate glyph index. */
	self->index = lialg_u32dic_new ();
	if (self->index == NULL)
	{
		lifnt_font_free (self);
		return NULL;
	}

	/* Allocate glyph table. */
	self->table_glyph_width = TTF_FontHeight (self->font); /* FIXME! */
	self->table_glyph_height = TTF_FontHeight (self->font);
	self->table_width = LIFNT_CACHE_WIDTH / self->table_glyph_width;
	self->table_height = LIFNT_CACHE_HEIGHT / self->table_glyph_height;
	self->table_length = self->table_width * self->table_height;
	self->table_filled = 0;
	self->table = lisys_calloc (self->table_length, sizeof (LIFntFontGlyph*));
	if (self->table == NULL)
	{
		lifnt_font_free (self);
		return NULL;
	}

	/* Allocate texture. */
	glGenTextures (1, &self->texture);
	glBindTexture (GL_TEXTURE_2D, self->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, LIFNT_CACHE_WIDTH, LIFNT_CACHE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	return self;
}

void lifnt_font_free (
	LIFntFont* self)
{
	LIAlgU32dicIter iter;

	if (self->texture != 0)
		glDeleteTextures (1, &self->texture);
	if (self->index != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->index)
			lisys_free (iter.value);
		lialg_u32dic_free (self->index);
	}
	if (self->table != NULL)
		lisys_free (self->table);
	if (self->font != NULL)
		TTF_CloseFont (self->font);
	lisys_free (self);
}

/**
 * \brief Reloads the font.
 *
 * This function is called when the video mode changes in Windows. It
 * recreates the glyph texture.
 *
 * \param self Font.
 * \param pass Reload pass.
 */
void lifnt_font_reload (
	LIFntFont* self,
	int        pass)
{
	if (!pass)
	{
		glDeleteTextures (1, &self->texture);
	}
	else
	{
		glGenTextures (1, &self->texture);
		glBindTexture (GL_TEXTURE_2D, self->texture);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, LIFNT_CACHE_WIDTH, LIFNT_CACHE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, self->reload_pixels);
	}
}

/**
 * \brief Gets the horizontal advance of the glyph.
 * \param self A font.
 * \param glyph A wide character.
 * \return The advance in pixels.
 */
int lifnt_font_get_advance (
	LIFntFont* self,
	wchar_t    glyph)
{
	LIFntFontGlyph* cached;

	cached = lialg_u32dic_find (self->index, glyph);
	if (cached == NULL)
	{
		cached = private_cache_glyph (self, glyph);
		if (cached == NULL)
			return 0;
	}

	return cached->advance;
}

int lifnt_font_get_height (
	const LIFntFont* self)
{
	return self->font_height;
}

/**
 * \brief Gets the vertices of a glyph.
 * \param self Font.
 * \param glyph Glyph code.
 * \param result Return location for 20 floats.
 */
void lifnt_font_get_vertices (
	LIFntFont* self,
	wchar_t    glyph,
	float*     result)
{
	float tex_x;
	float tex_y;
	float tex_w;
	float tex_h;
	LIFntFontGlyph* cached;

	/* Get tile. */
	cached = lialg_u32dic_find (self->index, glyph);
	if (cached == NULL)
	{
		cached = private_cache_glyph (self, glyph);
		if (cached == NULL)
			return;
	}

	/* Get texture rectangle. */
	tex_x = cached->table_x * self->table_glyph_width / (float) LIFNT_CACHE_WIDTH;
	tex_y = cached->table_y * self->table_glyph_height / (float) LIFNT_CACHE_HEIGHT;
	tex_w = cached->width / (float) LIFNT_CACHE_WIDTH;
	tex_h = cached->height / (float) LIFNT_CACHE_HEIGHT;

	/* Write texture and vertex coordinates. */
	result[0] = tex_x;
	result[1] = tex_y;
	result[2] = cached->bearing_x;
	result[3] = -cached->bearing_y;
	result[4] = 0.0f;
	result[5] = tex_x + tex_w;
	result[6] = tex_y;
	result[7] = cached->bearing_x + cached->width;
	result[8] = -cached->bearing_y;
	result[9] = 0.0f;
	result[10] = tex_x;
	result[11] = tex_y + tex_h;
	result[12] = cached->bearing_x;
	result[13] = -cached->bearing_y + cached->height;
	result[14] = 0.0f;
	result[15] = tex_x + tex_w;
	result[16] = tex_y + tex_h;
	result[17] = cached->bearing_x + cached->width;
	result[18] = -cached->bearing_y + cached->height;
	result[19] = 0.0f;
}

/*****************************************************************************/

static LIFntFontGlyph* private_cache_glyph (
	LIFntFont* self,
	wchar_t    glyph)
{
	int x;
	int y;
	int y1;
	int index;
	int advance;
	int bearing_x;
	int bearing_y;
	LIFntFontGlyph* cached;
	SDL_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
	SDL_Surface* image;

	/* Render the glyph. */
	image = TTF_RenderGlyph_Blended (self->font, glyph, color);
	if (image == NULL)
		return NULL;

	/* Choose a tile. */
	if (self->table_filled < self->table_length)
	{
		/* Allocate glyph. */
		cached = lisys_malloc (sizeof (LIFntFontGlyph));
		if (cached == NULL)
			return NULL;
		if (!lialg_u32dic_insert (self->index, glyph, cached))
		{
			self->table_filled--;
			SDL_FreeSurface (image);
			lisys_free (cached);
			return NULL;
		}

		/* Select empty tile. */
		for (index = 0 ; index < self->table_length ; index++)
		{
			if (self->table[index] == NULL)
				break;
		}
		lisys_assert (index < self->table_length);
		self->table[index] = cached;
	}
	else
	{
		/* FIXME: Add support for allocating more textures. */
		SDL_FreeSurface (image);
		return NULL;
	}

	/* Store metrics. */
	TTF_GlyphMetrics (self->font, glyph, &bearing_x, NULL, NULL, &bearing_y, &advance);
	cached->glyph = glyph;
	cached->table_index = index;
	cached->table_x = index % self->table_width;
	cached->table_y = index / self->table_width;
	cached->width = image->w;
	cached->height = image->h;
	cached->advance = advance;
	cached->bearing_x = bearing_x;
	cached->bearing_y = bearing_y - self->font_descent + 1;

	/* Upload to the tile. */
	if (image->w > 0 && image->h > 0)
	{
		x = self->table_glyph_width * cached->table_x;
		y = self->table_glyph_height * cached->table_y;
		for (y1 = 0 ; y1 < image->h ; y1++)
		{
			memcpy (self->reload_pixels + 4 * (x + (y + y1) * LIFNT_CACHE_WIDTH),
				image->pixels + 4 * y1 * image->w, 4 * image->w);
		}
		glBindTexture (GL_TEXTURE_2D, self->texture);
		glTexSubImage2D (GL_TEXTURE_2D, 0, x, y, image->w, image->h,
			GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	}
	SDL_FreeSurface (image);

	return cached;
}

/** @} */
/** @} */
