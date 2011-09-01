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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lipsofsuna/image/image-dds.h"
#include "ddstool-image.h"
#include "ddstool-compress.h"

static int private_print_info (
	const char* src)
{
	int mip;
	FILE* file;
	LIImgDDS dds;
	LIImgDDSFormat fmt;

	/* Read header. */
	file = fopen (src, "rb");
	if (file == NULL)
	{
		lisys_error_set (EINVAL, "cannot open file");
		return 0;
	}
	if (!liimg_dds_read_header (&dds, file))
	{
		lisys_error_set (EINVAL, "not a DDS file");
		fclose (file);
		return 0;
	}
	fmt = dds.info;

	/* Print info. */
	mip = (int) dds.header.mipmaps;
	printf ("Width: %d\n", dds.header.width);
	printf ("Height: %d\n", dds.header.height);
	printf ("Mipmaps: %d\n", mip > 0? mip : 1);
	switch (fmt.type)
	{
		case DDS_TYPE_DXT1:
			printf ("Format: DXT1\n");
			break;
		case DDS_TYPE_DXT3:
			printf ("Format: DXT3\n");
			break;
		case DDS_TYPE_DXT5:
			printf ("Format: DXT5\n");
			break;
		default:
			printf ("Format: uncompressed\n");
			break;
	}

	return 1;
}

static int private_convert_to_dds_s3tc (
	const char* src,
	const char* dst)
{
	int ret;
	LIImgImage* image;

	image = liimg_image_new_from_file (src);
	if (image == NULL)
		return 0;
	ret = liimg_image_save_s3tc (image, dst);
	liimg_image_free (image);

	return ret;
}

static int private_convert_to_dds_rgba (
	const char* src,
	const char* dst)
{
	int ret;
	LIImgImage* image;

	image = liimg_image_new_from_file (src);
	if (image == NULL)
		return 0;
	ret = liimg_image_save_rgba (image, dst);
	liimg_image_free (image);

	return ret;
}

int main (int argc, char** argv)
{
	if (argc == 3 && !strcmp (argv[1], "-i"))
	{
		if (!private_print_info (argv[2]))
		{
			lisys_error_report ();
			return 1;
		}
	}
	else if (argc == 4 && !strcmp (argv[1], "-r"))
	{
		if (!private_convert_to_dds_rgba (argv[2], argv[3]))
		{
			lisys_error_report ();
			return 1;
		}
	}
	else if (argc == 4 && !strcmp (argv[1], "-s"))
	{
		if (!private_convert_to_dds_s3tc (argv[2], argv[3]))
		{
			lisys_error_report ();
			return 1;
		}
	}
	else
	{
		printf ("Usage: %s [OPTION] [src] [dst]\n\n", argv[0]);
		printf (" -h   Print this help message.\n");
		printf (" -i   Print image information.\n");
		printf (" -r   Convert to DDS RGBA.\n");
		printf (" -s   Convert to DDS S3TC DXT5.\n");
	}

	return 0;
}
