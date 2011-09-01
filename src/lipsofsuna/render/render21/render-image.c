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
 * \addtogroup LIRenImage21 Image21
 * @{
 */

#include "render.h"
#include "render-image.h"
#include "render-private.h"

static int private_init (
	LIRenImage21* self,
	const char*   name);

/*****************************************************************************/

/**
 * \brief Creates an image from a file.
 * \param render Renderer.
 * \param name Image name.
 * \return Image or NULL.
 */
LIRenImage21* liren_image21_new_from_file (
	LIRenRender21* render,
	const char*    name)
{
	LIRenImage21* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenImage21));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Set name and path. */
	if (!private_init (self, name))
	{
		liren_image21_free (self);
		return NULL;
	}

	/* Load texture. */
	if (!liren_image21_load (self))
	{
		liren_image21_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the image.
 * \param self Image.
 */
void liren_image21_free (
	LIRenImage21* self)
{
	if (self->texture != NULL)
		liimg_texture_free (self->texture);
	lisys_free (self->name);
	lisys_free (self->path);
	lisys_free (self);
}

/**
 * \brief Loads or reloads the image.
 * \param self Image.
 * \return Nonzero on success.
 */
int liren_image21_load (
	LIRenImage21* self)
{
	LIImgTexture* texture;

	/* Load to a temporary texture. */
	texture = liimg_texture_new_from_file (self->path);
	if (texture == NULL)
		return 0;

	/* Use the loaded texture on success. */
	if (self->texture != NULL)
		liimg_texture_free (self->texture);
	self->texture = texture;

	return 1;
}

/**
 * \brief Reloads the image.
 * \param self Image.
 * \param pass Reload pass
 * \return Nonzero on success.
 */
void liren_image21_reload (
	LIRenImage21* self,
	int           pass)
{
	if (!pass)
	{
		if (self->texture != NULL)
		{
			liimg_texture_free (self->texture);
			self->texture = NULL;
		}
	}
	else
		self->texture = liimg_texture_new_from_file (self->path);
}

GLuint liren_image21_get_handle (
	const LIRenImage21* self)
{
	return self->texture->texture;
}

int liren_image21_get_height (
	const LIRenImage21* self)
{
	return self->texture->height;
}

int liren_image21_get_width (
	const LIRenImage21* self)
{
	return self->texture->width;
}

/*****************************************************************************/

static int private_init (
	LIRenImage21* self,
	const char*   name)
{
	char* file;

	/* Allocate name. */
	self->name = lisys_string_dup (name);
	if (self->name == NULL)
		return 0;

	/* Allocate path. */
	file = lisys_string_concat (name, ".dds");
	if (file == NULL)
		return 0;
	self->path = lipth_paths_get_graphics (self->render->paths, file);
	free (file);
	if (self->path == NULL)
		return 0;

	return 1;
}

/** @} */
/** @} */
