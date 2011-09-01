/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenImage Image
 * @{
 */

#include "render.h"
#include "render-image.h"
#include "render-private.h"
#include "render21/render-image.h"
#include "render21/render-private.h"
#include "render32/render-image.h"
#include "render32/render-private.h"

/**
 * \brief Creates an empty image.
 * \param render Renderer.
 * \param name Image name.
 * \return Image or NULL.
 */
LIRenImage* liren_image_new (
	LIRenRender* render,
	const char*  name)
{
	LIRenImage* self;

	self = lisys_calloc (1, sizeof (LIRenImage));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_image32_new_from_file (render->v32, name);
		if (self->v32 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}
	else
	{
		self->v21 = liren_image21_new_from_file (render->v21, name);
		if (self->v21 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	/* Add to dictionary. */
	if (!lialg_strdic_insert (self->render->images, name, self))
	{
		liren_image_free (self);
		return 0;
	}

	return self;
}

/**
 * \brief Frees the image.
 * \param self Image.
 */
void liren_image_free (
	LIRenImage* self)
{
	if (self->v32 != NULL)
	{
		lialg_strdic_remove (self->render->images, self->v32->name);
		liren_image32_free (self->v32);
	}
	if (self->v21 != NULL)
	{
		lialg_strdic_remove (self->render->images, self->v21->name);
		liren_image21_free (self->v21);
	}
	lisys_free (self);
}

GLuint liren_image_get_handle (
	const LIRenImage* self)
{
	if (self->v32 != NULL)
		return liren_image32_get_handle (self->v32);
	else
		return liren_image21_get_handle (self->v21);
}

int liren_image_get_height (
	const LIRenImage* self)
{
	if (self->v32 != NULL)
		return liren_image32_get_height (self->v32);
	else
		return liren_image21_get_height (self->v21);
}

int liren_image_get_width (
	const LIRenImage* self)
{
	if (self->v32 != NULL)
		return liren_image32_get_width (self->v32);
	else
		return liren_image21_get_width (self->v21);
}

/** @} */
/** @} */
