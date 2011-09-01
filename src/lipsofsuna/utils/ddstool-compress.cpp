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
 * \addtogroup LIImgCompress Compress
 * @{
 */

#include <squish.h>
#include "ddstool-compress.h"

/**
 * \brief Compresses RGBA pixel data to DXT5 bytes.
 *
 * \param pixels RGBA pixels to compress.
 * \param width Width.
 * \param height Height.
 * \param type DXT format number.
 * \param result Return location for compressed data.
 */
void
liimg_compress_compress (const void* pixels,
                         int         width,
                         int         height,
                         int         type,
                         void*       result)
{
	int flags;

	switch (type)
	{
		case 1: flags = squish::kColourClusterFit | squish::kDxt1; break;
		case 3: flags = squish::kColourClusterFit | squish::kDxt3; break;
		default: flags = squish::kColourClusterFit | squish::kDxt5; break;
	}
	squish::CompressImage ((squish::u8*) pixels, width, height, result, flags);
}

/**
 * \brief Uncompresses DXT5 bytes to RGBA pixel data.
 *
 * \param pixels DXT5 bytes to uncompress.
 * \param width Width.
 * \param height Height.
 * \param type DXT format number.
 * \param result Return location for uncompressed pixels.
 */
void
liimg_compress_uncompress (const void* pixels,
                           int         width,
                           int         height,
                           int         type,
                           void*       result)
{
	int flags;

	switch (type)
	{
		case 1: flags = squish::kColourClusterFit | squish::kDxt1; break;
		case 3: flags = squish::kColourClusterFit | squish::kDxt3; break;
		default: flags = squish::kColourClusterFit | squish::kDxt5; break;
	}
	squish::DecompressImage ((squish::u8*) result, width, height, pixels, flags);
}

/**
 * \brief Gets storage requirements for compressed pixel data.
 *
 * \param width Width.
 * \param height Height.
 * \param type DXT format number.
 * \return Number of bytes.
 */
int
liimg_compress_storage (int width,
                        int height,
                        int type)
{
	int flags;

	switch (type)
	{
		case 1: flags = squish::kColourClusterFit | squish::kDxt1; break;
		case 3: flags = squish::kColourClusterFit | squish::kDxt3; break;
		default: flags = squish::kColourClusterFit | squish::kDxt5; break;
	}
	return squish::GetStorageRequirements(width, height, flags);
}

/** @} */
/** @} */
