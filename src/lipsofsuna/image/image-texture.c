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
 * \addtogroup LIImg Image
 * @{
 * \addtogroup LIImgTexture Texture
 * @{
 */

#include <string.h>
#include <lipsofsuna/system.h>
#include "image-dds.h"
#include "image-ddsgl.h"
#include "image-texture.h"

/**
 * \brief Creates a new empty texture.
 * \return New texture or NULL.
 */
LIImgTexture* liimg_texture_new ()
{
	LIImgTexture* self;

	self = lisys_calloc (1, sizeof (LIImgTexture));
	if (self == NULL)
		return NULL;

	return self;
}

/**
 * \brief Creates a texture from RGBA data.
 * \param width Width in pixels.
 * \param height Height in pixels.
 * \param pixels Pixel data.
 * \return New texture or NULL.
 */
LIImgTexture* liimg_texture_new_from_rgba (
	int         width,
	int         height,
	const void* pixels)
{
	LIImgTexture* self;

	self = lisys_calloc (1, sizeof (LIImgTexture));
	if (self == NULL)
		return NULL;
	self->width = width;
	self->height = height;
	glGenTextures (1, &self->texture);
	glBindTexture (GL_TEXTURE_2D, self->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap (GL_TEXTURE_2D);

	return self;
}

/**
 * \brief Loads a texture from a file.
 * \param path Path to the texture file.
 * \return New texture or NULL.
 */
LIImgTexture* liimg_texture_new_from_file (
	const char* path)
{
	LIImgTexture* self;

	self = liimg_texture_new ();
	if (self == NULL)
		return NULL;
	if (!liimg_texture_load (self, path))
	{
		liimg_texture_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the texture.
 * \param self Texture.
 */
void liimg_texture_free (
	LIImgTexture* self)
{
	if (self->texture)
		glDeleteTextures (1, &self->texture);
	lisys_free (self);
}

/**
 * \brief Loads texture data from a DDS file.
 * \param self Texture.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int liimg_texture_load (
	LIImgTexture* self,
	const char*   path)
{
	FILE* file;
	GLuint tex;
	LIImgDDS dds;

	/* Load the DDS file. */
	file = fopen (path, "rb");
	if (file == NULL)
	{
		lisys_error_set (EIO, "cannot open file `%s'", path);
		return 0;
	}
	tex = liimg_ddsgl_load_texture (file, &dds);
	fclose (file);
	if (!tex)
	{
		lisys_error_set (EIO, "invalid texture file `%s'", path);
		return 0;
	}
	glDeleteTextures (1, &self->texture);
	if (dds.info.cubemap)
		self->target = GL_TEXTURE_CUBE_MAP;
	else
		self->target = GL_TEXTURE_2D;
	self->texture = tex;
	self->width = dds.header.width;
	self->height = dds.header.height;

	return 1;
}

/** @} */
/** @} */
