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
 * \addtogroup LIImgDDS DDS
 * @{
 */

#ifndef __IMAGE_DDS_H__
#define __IMAGE_DDS_H__

#include <stdio.h>
#include <string.h>
#include <lipsofsuna/system.h>

#define DDS_HEADER_MAGIC 0x20534444
#define DDS_FLAG_CAPS 0x00000001
#define DDS_FLAG_HEIGHT 0x00000002
#define DDS_FLAG_WIDTH 0x00000004
#define DDS_FLAG_PITCH 0x00000008
#define DDS_FLAG_PIXELFORMAT 0x00001000
#define DDS_FLAG_MIPMAPCOUNT 0x00020000
#define DDS_FLAG_LINEARSIZE 0x00080000
#define DDS_FLAG_DEPTH 0x00800000
#define DDS_FORMAT_ALPHAPIXELS 0x00000001
#define DDS_FORMAT_FOURCC 0x00000004
#define DDS_FORMAT_RGB 0x00000040
#define DDS_COMPRESS_DXT1 0x31545844
#define DDS_COMPRESS_DXT3 0x33545844
#define DDS_COMPRESS_DXT5 0x35545844
#define DDS_CAPS_COMPLEX 0x00000008
#define DDS_CAPS_CUBEMAP 0x00000200
#define DDS_CAPS_TEXTURE 0x00001000
#define DDS_CAPS_MIPMAP 0x00400000

enum
{
	DDS_TYPE_RGBA8888,
	DDS_TYPE_RGB888,
	DDS_TYPE_BGRA8888,
	DDS_TYPE_BGR888,
	DDS_TYPE_ARGB8888,
	DDS_TYPE_ABGR8888,
	DDS_TYPE_RGBA4444,
	DDS_TYPE_ARGB4444,
	DDS_TYPE_RGB565,
	DDS_TYPE_BGR565,
	DDS_TYPE_DXT1,
	DDS_TYPE_DXT3,
	DDS_TYPE_DXT5
};

typedef struct _LIImgDDSFormat LIImgDDSFormat;
struct _LIImgDDSFormat
{
	int alpha;
	int bytes;
	int type;
	int cubemap;
};

typedef struct _LIImgDDSLevel LIImgDDSLevel;
struct _LIImgDDSLevel
{
	int width;
	int height;
	int size;
	void* data;
};

typedef struct _LIImgDDS LIImgDDS;
struct _LIImgDDS
{
	/* Loaded from file. */
	struct
	{
		uint32_t magic;
		uint32_t size;
		uint32_t flags;
		uint32_t height;
		uint32_t width;
		uint32_t pitch;
		uint32_t depth;
		uint32_t mipmaps;
		uint32_t reserved[11];
	} header;
	struct
	{
		uint32_t size;
		uint32_t flags;
		uint32_t compress;
		uint32_t rgbbits;
		uint32_t rmask;
		uint32_t gmask;
		uint32_t bmask;
		uint32_t amask;
	} format;
	struct
	{
		uint32_t caps1;
		uint32_t caps2;
		uint32_t ddsx;
		uint32_t reserved[2];
	} caps;

	/* Derived from loaded data. */
	LIImgDDSFormat info;
};

/**
 * \brief Initializes the DDS header for an RGBA image.
 * \param self DDS.
 * \param width Image width.
 * \param height Image height.
 * \param mipmaps Mipmap count.
 */
static inline void liimg_dds_init_rgba (
	LIImgDDS* self,
	int       width,
	int       height,
	int       mipmaps)
{
	memset (self, 0, sizeof (LIImgDDS));
	self->header.magic = DDS_HEADER_MAGIC;
	self->header.size = 124;
	self->header.flags = DDS_FLAG_CAPS | DDS_FLAG_HEIGHT | DDS_FLAG_WIDTH | DDS_FLAG_PIXELFORMAT | DDS_FLAG_PITCH;
	self->header.height = height;
	self->header.width = width;
	self->header.pitch = 4 * width;
	self->header.mipmaps = mipmaps;
	self->format.size = 32;
	self->format.flags = DDS_FORMAT_RGB | DDS_FORMAT_ALPHAPIXELS;
	self->format.rgbbits = 32;
	self->format.rmask = 0xFF000000;
	self->format.gmask = 0x00FF0000;
	self->format.bmask = 0x0000FF00;
	self->format.amask = 0x000000FF;
	self->caps.caps1 = DDS_CAPS_TEXTURE;
	if (mipmaps)
	{
		self->header.flags |= DDS_FLAG_MIPMAPCOUNT;
		self->caps.caps1 |= DDS_CAPS_COMPLEX | DDS_CAPS_MIPMAP;
	}
}

/**
 * \brief Initializes the DDS header for an S3TC image.
 * \param self DDS.
 * \param width Image width.
 * \param height Image height.
 * \param size Size of first mipmap level in bytes.
 * \param mipmaps Mipmap count.
 */
static inline void liimg_dds_init_s3tc (
	LIImgDDS* self,
	int       width,
	int       height,
	int       size,
	int       mipmaps)
{
	memset (self, 0, sizeof (LIImgDDS));
	self->header.magic = DDS_HEADER_MAGIC;
	self->header.size = 124;
	self->header.flags = DDS_FLAG_CAPS | DDS_FLAG_HEIGHT | DDS_FLAG_WIDTH | DDS_FLAG_PIXELFORMAT | DDS_FLAG_LINEARSIZE;
	self->header.height = height;
	self->header.width = width;
	self->header.pitch = size;
	self->header.mipmaps = mipmaps;
	self->format.size = 32;
	self->format.flags = DDS_FORMAT_FOURCC;
	self->format.compress = DDS_COMPRESS_DXT5;
	self->caps.caps1 = DDS_CAPS_TEXTURE;
	if (mipmaps)
	{
		self->header.flags |= DDS_FLAG_MIPMAPCOUNT;
		self->caps.caps1 |= DDS_CAPS_COMPLEX | DDS_CAPS_MIPMAP;
	}
}

/**
 * \brief Used internally for swapping header byte order when necessary.
 * \param self DDS.
 */
static inline void liimg_dds_byteswap (
	LIImgDDS* self)
{
	if (lisys_endian_big ())
	{
	#define BYTESWAP(i) ((((i)&&0xFF)<<24) | (((i)&&0xFF00)<<8) | (((i)&&0xFF0000)>>8) | (((i)&&0xFF000000)>>24))
	self->header.magic = BYTESWAP (self->header.magic);
	self->header.flags = BYTESWAP (self->header.flags);
	self->header.height = BYTESWAP (self->header.height);
	self->header.width = BYTESWAP (self->header.width);
	self->header.pitch = BYTESWAP (self->header.pitch);
	self->header.depth = BYTESWAP (self->header.depth);
	self->header.mipmaps = BYTESWAP (self->header.mipmaps);
	self->format.size = BYTESWAP (self->format.size);
	self->format.flags = BYTESWAP (self->format.flags);
	self->format.compress = BYTESWAP (self->format.compress);
	self->format.rgbbits = BYTESWAP (self->format.rgbbits);
	self->format.rmask = BYTESWAP (self->format.rmask);
	self->format.gmask = BYTESWAP (self->format.gmask);
	self->format.bmask = BYTESWAP (self->format.bmask);
	self->format.amask = BYTESWAP (self->format.amask);
	self->caps.caps1 = BYTESWAP (self->caps.caps1);
	self->caps.caps2 = BYTESWAP (self->caps.caps2);
	self->caps.ddsx = BYTESWAP (self->caps.ddsx);
	#undef BYTESWAP
	}
}

int liimg_dds_decompress (
	const void* input,
	void*       output,
	int         width,
	int         height,
	int         format);

/**
 * \brief Reads the DDS file header.
 * \param self DDS.
 * \param file File.
 * \return Nonzero on success.
 */
static inline int liimg_dds_read_header (
	LIImgDDS* self,
	FILE*     file)
{
	int i;
	int alpha = 0;
	static const struct
	{
		int alpha;
		uint32_t rgbbits;
		uint32_t rmask;
		uint32_t gmask;
		uint32_t bmask;
		uint32_t amask;
		LIImgDDSFormat format;
	}
	formats[] =
	{
		{ 1, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, { 1, 4, DDS_TYPE_RGBA8888, 0 } },
		{ 0, 24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, { 0, 3, DDS_TYPE_RGB888, 0 } },
		{ 1, 32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF, { 1, 4, DDS_TYPE_BGRA8888, 0 } },
		{ 0, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, { 0, 3, DDS_TYPE_BGR888, 0 } },
		{ 1, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, { 1, 4, DDS_TYPE_ARGB8888, 0 } },
		{ 1, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, { 1, 4, DDS_TYPE_ABGR8888, 0 } },
		{ 1, 16, 0xF000, 0x0F00, 0x00F0, 0x000F, { 1, 2, DDS_TYPE_RGBA4444, 0 } },
		{ 1, 16, 0x0F00, 0x00F0, 0x000F, 0xF000, { 1, 2, DDS_TYPE_ARGB4444, 0 } },
		{ 0, 16, 0xF800, 0x07E0, 0x001F, 0x0000, { 0, 2, DDS_TYPE_RGB565, 0 } },
		{ 0, 16, 0x001F, 0x07E0, 0xF800, 0x0000, { 0, 2, DDS_TYPE_BGR565, 0 } }
	};

	/* Read header. */
	if (!fread (&self->header.magic, 4, 1, file) ||
	    !fread (&self->header.size, 4, 1, file) ||
	    !fread (&self->header.flags, 4, 1, file) ||
	    !fread (&self->header.height, 4, 1, file) ||
	    !fread (&self->header.width, 4, 1, file) ||
	    !fread (&self->header.pitch, 4, 1, file) ||
	    !fread (&self->header.depth, 4, 1, file) ||
	    !fread (&self->header.mipmaps, 4, 1, file) ||
	    !fread (self->header.reserved, 44, 1, file) ||
	    !fread (&self->format.size, 4, 1, file) ||
	    !fread (&self->format.flags, 4, 1, file) ||
	    !fread (&self->format.compress, 4, 1, file) ||
	    !fread (&self->format.rgbbits, 4, 1, file) ||
	    !fread (&self->format.rmask, 4, 1, file) ||
	    !fread (&self->format.gmask, 4, 1, file) ||
	    !fread (&self->format.bmask, 4, 1, file) ||
	    !fread (&self->format.amask, 4, 1, file) ||
	    !fread (&self->caps.caps1, 4, 1, file) ||
	    !fread (&self->caps.caps2, 4, 1, file) ||
	    !fread (&self->caps.ddsx, 4, 1, file) ||
	    !fread (self->caps.reserved, 8, 1, file))
		return 0;
	liimg_dds_byteswap (self);
	if (self->header.magic != DDS_HEADER_MAGIC)
		return 0;

	/* Detect pixel format. */
	if (self->format.flags & DDS_FORMAT_FOURCC)
	{
		switch (self->format.compress)
		{
			case DDS_COMPRESS_DXT1:
				self->info.alpha = 0;
				self->info.type = DDS_TYPE_DXT1;
				break;
			case DDS_COMPRESS_DXT3:
				self->info.alpha = 1;
				self->info.type = DDS_TYPE_DXT3;
				break;
			case DDS_COMPRESS_DXT5:
				self->info.alpha = 1;
				self->info.type = DDS_TYPE_DXT5;
				break;
			default:
				return 0;
		}
	}
	else
	{
		if (!(self->format.flags & DDS_FORMAT_RGB))
			return 0;
		alpha = (self->format.flags & DDS_FORMAT_ALPHAPIXELS);
		for (i = 0 ; i < (int)(sizeof (formats) / sizeof (*formats)) ; i++)
		{
			if (formats[i].alpha == alpha &&
			    formats[i].rgbbits == self->format.rgbbits &&
			    formats[i].rmask == self->format.rmask &&
			    formats[i].gmask == self->format.gmask &&
			    formats[i].bmask == self->format.bmask &&
			   (formats[i].amask == self->format.amask || !alpha))
			{
				self->info = formats[i].format;
				break;
			}
		}
	}

	/* Check if this is a cube map. */
	if (self->caps.caps2 & DDS_CAPS_CUBEMAP)
		self->info.cubemap = 1;
	else
		self->info.cubemap = 0;

	return 1;
}

/**
 * \brief Reads a mipmap level from the DDS file.
 * \param self DDS.
 * \param file File.
 * \param level Mipmap level.
 * \param data Return location for pixel data buffer.
 * \param size Return location for pixel data buffer size.
 * \return Nonzero on success.
 */
static inline int liimg_dds_read_level (
	LIImgDDS*      self,
	FILE*          file,
	int            level,
	LIImgDDSLevel* result)
{
	int mult;

	/* Get buffer size. */
	result->width = self->header.width >> level;
	result->width = result->width? result->width : 1;
	result->height = self->header.height >> level;
	result->height = result->height? result->height : 1;
	if (self->format.flags & DDS_FORMAT_FOURCC)
	{
		switch (self->format.compress)
		{
			case DDS_COMPRESS_DXT1: mult = 8; break;
			case DDS_COMPRESS_DXT3: mult = 16; break;
			case DDS_COMPRESS_DXT5: mult = 16; break;
			default: return 0;
		}
		result->size = ((result->width + 3) / 4) * ((result->height + 3) / 4) * mult;
	}
	else
		result->size = self->info.bytes * result->width * result->height;

	/* Load pixel data. */
	result->data = lisys_malloc (result->size);
	if (result->data == NULL)
		return 0;
	if (!fread (result->data, result->size, 1, file))
	{
		lisys_free (result->data);
		return 0;
	}

	return 1;
}

/**
 * \brief Writes the DDS file header.
 * \param self DDS.
 * \param file File.
 * \return Nonzero on success.
 */
static inline int liimg_dds_write_header (
	LIImgDDS* self,
	FILE*     file)
{
	liimg_dds_byteswap (self);
	if (!fwrite (&self->header.magic, 4, 1, file) ||
	    !fwrite (&self->header.size, 4, 1, file) ||
	    !fwrite (&self->header.flags, 4, 1, file) ||
	    !fwrite (&self->header.height, 4, 1, file) ||
	    !fwrite (&self->header.width, 4, 1, file) ||
	    !fwrite (&self->header.pitch, 4, 1, file) ||
	    !fwrite (&self->header.depth, 4, 1, file) ||
	    !fwrite (&self->header.mipmaps, 4, 1, file) ||
	    !fwrite (self->header.reserved, 44, 1, file) ||
	    !fwrite (&self->format.size, 4, 1, file) ||
	    !fwrite (&self->format.flags, 4, 1, file) ||
	    !fwrite (&self->format.compress, 4, 1, file) ||
	    !fwrite (&self->format.rgbbits, 4, 1, file) ||
	    !fwrite (&self->format.rmask, 4, 1, file) ||
	    !fwrite (&self->format.gmask, 4, 1, file) ||
	    !fwrite (&self->format.bmask, 4, 1, file) ||
	    !fwrite (&self->format.amask, 4, 1, file) ||
	    !fwrite (&self->caps.caps1, 4, 1, file) ||
	    !fwrite (&self->caps.caps2, 4, 1, file) ||
	    !fwrite (&self->caps.ddsx, 4, 1, file) ||
	    !fwrite (self->caps.reserved, 8, 1, file))
		return 0;

	return 1;
}

/**
 * \brief Writes a mipmap level to the DDS file.
 * \param self DDS.
 * \param file File.
 * \param level Mipmap level.
 * \param data Pixel data.
 * \param size Pixel data size in bytes.
 * \return Nonzero on success.
 */
static inline int liimg_dds_write_level (
	LIImgDDS* self,
	FILE*     file,
	int       level,
	void*     data,
	int       size)
{
	if (!fwrite (data, size, 1, file))
		return 0;

	return 1;
}

#endif

/** @} */
/** @} */
