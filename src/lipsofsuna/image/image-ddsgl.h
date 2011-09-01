/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * \addtogroup LIImg Image
 * @{
 * \addtogroup LIImgDDSGL DDSGL
 * @{
 */

#ifndef __IMAGE_DDSGL_H__
#define __IMAGE_DDSGL_H__

#include <lipsofsuna/video.h>
#include "image-dds.h"

typedef struct _LIImgDDSGLFormat LIImgDDSGLFormat;
struct _LIImgDDSGLFormat
{
	int compressed;
	GLenum internal;
	GLenum format;
	GLenum type;
};

/**
 * \brief Gets the OpenGL format of the DDS image.
 * \param dds DDS.
 * \param info Return location for DDS info.
 * \return Nonzero on success.
 */
static inline int liimg_ddsgl_get_format (
	LIImgDDS*         self,
	LIImgDDSGLFormat* info)
{
	static const LIImgDDSGLFormat formats[13] =
	{
		{ 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
		{ 0, GL_RGB, GL_BGR, GL_UNSIGNED_BYTE },
		{ 0, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE },
		{ 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE },
		{ 0, GL_RGBA, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV },
		{ 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV },
		{ 0, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },
		{ 0, GL_RGBA16, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV },
		{ 0, GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },
		{ 0, GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT_5_6_5_REV },
		{ 1, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0 },
		{ 1, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0 },
		{ 1, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0 }
	};
	switch (self->info.type)
	{
		case DDS_TYPE_RGBA8888: *info = formats[0]; break;
		case DDS_TYPE_RGB888: *info = formats[1]; break;
		case DDS_TYPE_BGRA8888: *info = formats[2]; break;
		case DDS_TYPE_BGR888: *info = formats[3]; break;
		case DDS_TYPE_ARGB8888: *info = formats[4]; break;
		case DDS_TYPE_ABGR8888: *info = formats[5]; break;
		case DDS_TYPE_RGBA4444: *info = formats[6]; break;
		case DDS_TYPE_ARGB4444: *info = formats[7]; break;
		case DDS_TYPE_RGB565: *info = formats[8]; break;
		case DDS_TYPE_BGR565: *info = formats[9]; break;
		case DDS_TYPE_DXT1: *info = formats[10]; break;
		case DDS_TYPE_DXT3: *info = formats[11]; break;
		case DDS_TYPE_DXT5: *info = formats[12]; break;
		default:
			return 0;
	};
	return 1;
}

/**
 * \brief Loads mipmap levels from a DDS texture file.
 * \param dds DDS information structure.
 * \param format DDS OpenGL information structure.
 * \param file File pointer.
 * \param target Texture target type.
 * \param texture Texture.
 * \return Nonzero on success.
 */
static inline int liimg_ddsgl_load_levels (
	LIImgDDS*         dds,
	LIImgDDSGLFormat* format,
	FILE*             file,
	GLenum            target,
	GLuint            texture)
{
	int level;
	uint8_t* data;
	LIImgDDSLevel lvl;

	for (level = 0 ; !level || level < (int) dds->header.mipmaps ; level++)
	{
		if (!format->compressed)
		{
			/* Uncompressed format. */
			if (!liimg_dds_read_level (dds, file, level, &lvl))
				return 0;
			glTexImage2D (target, level, format->internal, lvl.width,
				lvl.height, 0, format->format, format->type, lvl.data);
			lisys_free (lvl.data);
		}
		else if (GLEW_EXT_texture_compression_s3tc)
		{
			/* Compressed format. */
			if (!liimg_dds_read_level (dds, file, level, &lvl))
				return 0;
			glCompressedTexImage2DARB (target, level, format->format,
				lvl.width, lvl.height, 0, lvl.size, lvl.data);
			lisys_free (lvl.data);
		}
		else
		{
			/* Compressed format, software decompression. */
			if (!liimg_dds_read_level (dds, file, level, &lvl))
				return 0;
			data = lisys_calloc (lvl.width * lvl.height * 4, sizeof (uint8_t));
			if (data != NULL)
				liimg_dds_decompress (lvl.data, data, lvl.width, lvl.height, dds->format.compress);
			glTexImage2D (target, level, GL_RGBA, lvl.width,
				lvl.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			lisys_free (data);
			lisys_free (lvl.data);
		}
	}

	return 1;
}

/**
 * \brief Loads a DDS texture file.
 * \param file File pointer.
 * \param info Return location for DDS info or NULL.
 * \return New texture number or zero.
 */
static inline GLuint liimg_ddsgl_load_texture (
	FILE*     file,
	LIImgDDS* info)
{
	int i;
	GLuint texture;
	GLenum target;
	LIImgDDS dds;
	LIImgDDSGLFormat fmt;

	/* Read header. */
	if (!liimg_dds_read_header (&dds, file) ||
	    !liimg_ddsgl_get_format (&dds, &fmt))
		return 0;
	if (info != NULL)
		*info = dds;

	/* Create a texture. */
	if (dds.info.cubemap)
		target = GL_TEXTURE_CUBE_MAP;
	else
		target = GL_TEXTURE_2D;
	glGenTextures (1, &texture);
	glBindTexture (target, texture);
	glTexParameteri (target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Load the mipmap levels. */
	if (dds.info.cubemap)
	{
		/* Cube map. */
		for (i = 0 ; i < 6 ; i++)
		{
			if (!liimg_ddsgl_load_levels (&dds, &fmt, file, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture))
			{
				glDeleteTextures (1, &texture);
				return 0;
			}
		}
	}
	else
	{
		/* 2D texture. */
		if (!liimg_ddsgl_load_levels (&dds, &fmt, file, GL_TEXTURE_2D, texture))
		{
			glDeleteTextures (1, &texture);
			return 0;
		}
	}

	/* Generate mipmap if not provided by the file. */
	if (dds.header.mipmaps <= 1)
		glGenerateMipmap (target);

	return texture;
}

#endif

/** @} */
/** @} */
