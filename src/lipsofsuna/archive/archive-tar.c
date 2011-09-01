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
 * \addtogroup LIArc Archive
 * @{
 * \addtogroup LIArcTar Tar
 * @{
 */

#include <unistd.h>
#include "lipsofsuna/system.h"
#include "archive-tar.h"

#ifdef SUPPORT_DEVICES
#include <sys/sysmacros.h>
#endif

static void
private_header_init (LIArcTar*       self,
                     LIArcTarHeader* header);

static void
private_header_set_device (LIArcTar*       self,
                           LIArcTarHeader* header,
                           dev_t           dev);

static void
private_header_set_link (LIArcTar*       self,
                         LIArcTarHeader* header,
                         const char*     name);

static void
private_header_set_name (LIArcTar*       self,
                         LIArcTarHeader* header,
                         const char*     name);

static void
private_header_set_owner (LIArcTar*      self,
                          LIArcTarHeader* header,
                          int             uid,
                          int             gid);

static void
private_header_set_perm (LIArcTar*       self,
                         LIArcTarHeader* header,
                         int             perm);

static void
private_header_set_type (LIArcTar*       self,
                         LIArcTarHeader* header,
                         char            type,
                         size_t          size);

static void
private_header_set_sum (LIArcTar*       self,
                        LIArcTarHeader* header);

static void
private_header_set_time (LIArcTar*       self,
                         LIArcTarHeader* header,
                         time_t          time);

/*****************************************************************************/

LIArcTar*
liarc_tar_new (LIArcWriter* writer)
{
	LIArcTar* self;

	self = lisys_calloc (1, sizeof (LIArcTar));
	if (self == NULL)
		return NULL;
	self->writer = writer;

	return self;
}

void
liarc_tar_free (LIArcTar* self)
{
	lisys_free (self);
}

int
liarc_tar_write_data (LIArcTar*   self,
                      const char* name,
                      const void* data,
                      int         length)
{
	char block[512];
	LIArcTarHeader header;

	/* Write header. */
	private_header_init (self, &header);
	private_header_set_type (self, &header, LIARC_TAR_FILE, length);
	private_header_set_name (self, &header, name);
	private_header_set_perm (self, &header, 0660);
	private_header_set_time (self, &header, lisys_time (NULL));
	private_header_set_sum (self, &header);
	if (!liarc_writer_append_raw (self->writer, &header, 512))
		return 0;

	/* Write content. */
	if (!liarc_writer_append_raw (self->writer, data, length))
		return 0;
	length = 512 - (length % 512);
	if (length == 512)
		return 1;
	memset (block, 0, length);
	if (!liarc_writer_append_raw (self->writer, block, length))
		return 0;

	return 1;
}

int
liarc_tar_write_directory (LIArcTar*   self,
                           const char* name)
{
	LIArcTarHeader header;

	private_header_init (self, &header);
	private_header_set_type (self, &header, LIARC_TAR_DIRECTORY, 0);
	private_header_set_name (self, &header, name);
	private_header_set_perm (self, &header, 0770);
	private_header_set_time (self, &header, time (NULL));
	private_header_set_sum (self, &header);
	if (!liarc_writer_append_raw (self->writer, &header, 512))
		return 0;

	return 1;
}

int
liarc_tar_write_end (LIArcTar* self)
{
	char pad[1024];

	memset (pad, 0, 1024);
	return liarc_writer_append_raw (self->writer, pad, 1024);
}

int
liarc_tar_write_file (LIArcTar*   self,
                      const char* src,
                      const char* dst)
{
	FILE* file;
	size_t len;
	size_t pos;
	char type;
	char block[512];
	char* link = NULL;
	LISysStat st;
	LIArcTarHeader header;

	/* FIXME */
	int user = 0;

	/* Sanity checks. */
	len = strlen (dst);
	if (len == 0)
	{
		lisys_error_set (EINVAL, "empty file name");
		return 0;
	}
	if (len >= 256)
	{
		lisys_error_set (EINVAL, "file name too long `%s'", dst);
		return 0;
	}

	/* Open file. */
	if (!lisys_filesystem_lstat (src, &st))
	{
		lisys_error_set (EIO, "cannot stat `%s'", src);
		return 0;
	}

	/* Get file type. */
	switch (st.type)
	{
		case LISYS_STAT_FILE: type = LIARC_TAR_FILE; break;
		case LISYS_STAT_DIRECTORY: type = LIARC_TAR_DIRECTORY; break;
		case LISYS_STAT_LINK: type = LIARC_TAR_LINK; break;
#ifdef SUPPORT_DEVICES
		case LISYS_STAT_CHAR: type = LIARC_TAR_CHAR; break;
		case LISYS_STAT_BLOCK: type = LIARC_TAR_BLOCK; break;
#endif
		default:
			return 1;
	}
	if (st.type == LISYS_STAT_LINK)
	{
		link = lisys_filesystem_readlink (src);
		if (link == NULL)
			return 0;
	}

	/* Write header. */
	private_header_init (self, &header);
	private_header_set_type (self, &header, type, st.size);
	private_header_set_name (self, &header, dst);
	private_header_set_perm (self, &header, st.mode);
	private_header_set_time (self, &header, st.mtime);
	if (user) private_header_set_owner (self, &header, st.uid, st.gid);
	if (st.type == LISYS_STAT_LINK)
	{
		private_header_set_link (self, &header, link);
		lisys_free (link);
	}
	if (st.type == LISYS_STAT_CHAR) private_header_set_device (self, &header, st.rdev);
	if (st.type == LISYS_STAT_BLOCK) private_header_set_device (self, &header, st.rdev);
	private_header_set_sum (self, &header);
	
	if (!liarc_writer_append_raw (self->writer, &header, 512))
		return 0;

	/* Write content. */
	if (type != LIARC_TAR_FILE)
		return 1;
	file = fopen (src, "rb");
	if (file == NULL)
	{
		lisys_error_set (EIO, "cannot open `%s'", src);
		return 0;
	}
	for (pos = 0 ; pos < st.size ; pos += len)
	{
		len = fread (block, 1, 512, file);
		if (len < 512)
		{
			if (pos + len != st.size)
			{
				lisys_error_set (EIO, "error while reading `%s'", src);
				fclose (file);
				return 0;
			}
			memset (block + len, 0, 512 - len);
		}
		if (!liarc_writer_append_raw (self->writer, block, 512))
		{
			fclose (file);
			return 0;
		}
	}

	fclose (file);
	return 1;
}

/*****************************************************************************/

static void
private_header_init (LIArcTar*       self,
                     LIArcTarHeader* header)
{
	memset (header, 0, sizeof (LIArcTarHeader));
	snprintf (header->mode, 8, "%07o", (unsigned int)(0660));
	//snprintf (header->uid, 8, "%07o", (unsigned int)(0));
	//snprintf (header->gid, 8, "%07o", (unsigned int)(0));
	snprintf (header->size, 12, "%011lo", (unsigned long)(0));
	//snprintf (header->mtime, 12, "%011lo", (unsigned long)(0));
	(void) strncpy (header->chksum, "        ", 8);
	(void) strncpy (header->magic, "ustar", 5);
	(void) strncpy (header->version, "00", 2);
}

static void
private_header_set_device (LIArcTar*       self,
                           LIArcTarHeader* header,
                           dev_t           dev)
{
#ifdef SUPPORT_DEVICES
	snprintf (header->devmajor, 8, "%07lo", (unsigned long) major (dev));
	snprintf (header->devminor, 8, "%07lo", (unsigned long) minor (dev));
#endif
}

static void
private_header_set_link (LIArcTar*       self,
                         LIArcTarHeader* header,
                         const char*     name)
{
	strncpy (header->linkname, name, 99);
}

static void
private_header_set_name (LIArcTar*       self,
                         LIArcTarHeader* header,
                         const char*     name)
{
	size_t len;
	size_t suf;
	char block[256];

	len = strlen (name);
	memcpy (block, name, len);
	if (header->typeflag[0] == LIARC_TAR_DIRECTORY)
	{
		if (block[len - 1] != '/')
			block[len++] = '/';
	}
	if (len > 100)
	{
		suf = len - 1;
		if (suf > 155)
			suf = 155;
		while (suf > 0 && block[suf - 1] != '/')
			suf--;
		if (len - suf > 100)
			suf = len - 100;
		strncpy (header->name, block + suf, len - suf);
		strncpy (header->prefix, block, suf);
	}
	else
		strncpy (header->name, block, len);
}

static void
private_header_set_owner (LIArcTar*       self,
                          LIArcTarHeader* header,
                          int             uid,
                          int             gid)
{
	char* user;
	char* group;

	snprintf (header->uid, 8, "%07o", uid);
	snprintf (header->gid, 8, "%07o", gid);
	user = lisys_user_get_name (uid);
	if (user != NULL)
	{
		snprintf (header->uname, 32, "%s", user);
		lisys_free (user);
	}
	group = lisys_group_get_name (gid);
	if (group != NULL)
	{
		snprintf (header->gname, 32, "%s", group);
		lisys_free (group);
	}
}

static void
private_header_set_perm (LIArcTar*       self,
                         LIArcTarHeader* header,
                         int             perm)
{
	snprintf (header->mode, 8, "%07o", (unsigned int) perm);
}

static void
private_header_set_type (LIArcTar*       self,
                         LIArcTarHeader* header,
                         char            type,
                         size_t          size)
{
	if (type != LIARC_TAR_FILE)
		size = 0;
	snprintf (header->size, 12, "%011lo", (unsigned long) size);
	header->typeflag[0] = type;
}

static void
private_header_set_sum (LIArcTar*       self,
                        LIArcTarHeader* header)
{
	int i;
	int sum = 0;

	for (i = 0 ; i < 512 ; i++)
		sum += ((unsigned char*) header)[i];
	snprintf (header->chksum, 7, "%06o", sum);
}

static void
private_header_set_time (LIArcTar*       self,
                         LIArcTarHeader* header,
                         time_t          time)
{
	snprintf (header->mtime, 12, "%011lo", time);
}

/** @} */
/** @} */
