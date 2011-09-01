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
 * \addtogroup LIWdg Widget
 * @{
 * \addtogroup LIWdgStyle Style
 * @{
 */

#include <lipsofsuna/font.h>
#include <lipsofsuna/image.h>
#include "widget-manager.h"
#include "widget-styles.h"

LIWdgStyles* liwdg_styles_new (
	LIWdgManager* manager,
	LIPthPaths*   paths)
{
	LIWdgStyles* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIWdgStyles));
	if (self == NULL)
		return NULL;
	self->manager = manager;
	self->paths = paths;

	/* Allocate resource lists. */
	self->fonts = lialg_strdic_new ();
	if (self->fonts == NULL)
	{
		liwdg_styles_free (self);
		return NULL;
	}

	return self;
}

void liwdg_styles_free (
	LIWdgStyles* self)
{
	LIAlgStrdicIter iter;

	if (self->fonts != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->fonts)
			lifnt_font_free (iter.value);
		lialg_strdic_free (self->fonts);
	}
	lisys_free (self);
}

LIFntFont* liwdg_styles_load_font (
	LIWdgStyles* self,
	const char*  name,
	const char*  file,
	int          size)
{
	char* path;
	char* file_;
	LIFntFont* font;

	/* Check for existing. */
	font = lialg_strdic_find (self->fonts, name);
	if (font != NULL)
		return font;

	/* Load font. */
	file_ = lisys_string_concat (file, ".ttf");
	if (file == NULL)
		return NULL;
	path = lipth_paths_get_font (self->paths, file_);
	lisys_free (file_);
	if (path == NULL)
		return NULL;
	font = lifnt_font_new (path, size);
	lisys_free (path);
	if (font == NULL)
		return NULL;

	/* Add to list. */
	if (!lialg_strdic_insert (self->fonts, name, font))
	{
		lifnt_font_free (font);
		return NULL;
	}

	return font;
}

/** @} */
/** @} */
