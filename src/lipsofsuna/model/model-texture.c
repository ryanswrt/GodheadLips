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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlTexture Texture
 * @{
 */

#include "lipsofsuna/system.h"
#include "model-texture.h"

int
limdl_texture_compare (const LIMdlTexture* self,
                       const LIMdlTexture* texture)
{
	if (self->type != texture->type ||
	    self->flags != texture->flags ||
	    self->width != texture->width ||
	    self->height != texture->height)
		return 0;
	if (strcmp (self->string, texture->string))
		return 0;

	return 1;
}

int
limdl_texture_set_string (LIMdlTexture* self,
                          const char*   value)
{
	char* tmp;

	tmp = lisys_string_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->string);
	self->string = tmp;

	return 1;
}

/** @} */
/** @} */
