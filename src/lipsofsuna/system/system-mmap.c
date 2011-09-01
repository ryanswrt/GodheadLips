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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysMmap Mmap
 * @{
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include "system.h"
#include "system-error.h"
#include "system-mmap.h"

struct _LISysMmap
{
#ifdef HAVE_SYS_MMAN_H
	int size;
	void* buffer;
#elif defined HAVE_WINDOWS_H
	int size;
	void* buffer;
	HANDLE file;
	HANDLE handle;
#else
#error "Mmap not supported"
#endif
};

/**
 * \brief Creates a read-only memory map of the file.
 * \param path Path.
 * \return New memory map handle or NULL.
 */
LISysMmap* lisys_mmap_open (
	const char* path)
{
#ifdef HAVE_SYS_MMAN_H
	int fd;
	struct stat st;
	LISysMmap* self;

	/* Allocate self. */
	self = malloc (sizeof (LISysMmap));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Open the file. */
	fd = open (path, O_RDONLY);
	if (fd == -1)
	{
		lisys_error_set (EIO, "cannot open `%s'", path);
		free (self);
		return NULL;
	}
	if (fstat (fd, &st) == -1)
	{
		lisys_error_set (EIO, "cannot stat `%s'", path);
		close (fd);
		free (self);
		return NULL;
	}
	self->size = st.st_size;

	/* Memory map the file. */
	if (!self->size)
	{
		self->buffer = NULL;
		return self;
	}
	self->buffer = mmap (NULL, self->size, PROT_READ, MAP_SHARED, fd, 0);
	close (fd);
	if (self->buffer == MAP_FAILED)
	{
		lisys_error_set (EIO, "cannot mmap `%s'", path);
		free (self);
		return NULL;
	}

	return self;
#elif defined HAVE_WINDOWS_H
	LISysMmap* self;

	/* Allocate self. */
	self = malloc (sizeof (LISysMmap));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Open the file. */
	self->file = CreateFile (path, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (self->file == NULL)
	{
		lisys_error_set (EIO, "cannot open `%s'", path);
		free (self);
		return NULL;
	}

	/* Create file mapping. */
	self->handle = CreateFileMapping (self->file, NULL, PAGE_READONLY, 0, 0, NULL);
	if (self->handle == NULL)
	{
		lisys_error_set (EIO, "cannot create mapping for `%s'", path);
		CloseHandle (self->file);
		free (self);
		return NULL;
	}
	self->size = GetFileSize (self->file, NULL);
	if (self->size == INVALID_FILE_SIZE)
	{
		lisys_error_set (EIO, "cannot get size of `%s'", path);
		CloseHandle (self->handle);
		CloseHandle (self->file);
		free (self);
		return NULL;
	}

	/* Memory map the file. */
	if (self->size)
	{
		self->buffer = MapViewOfFile (self->handle, FILE_MAP_READ, 0, 0, self->size);
		if (self->buffer == NULL)
		{
			lisys_error_set (EIO, "cannot map `%s'", path);
			CloseHandle (self->handle);
			CloseHandle (self->file);
			free (self);
			return NULL;
		}
	}
	else
		self->buffer = NULL;

	return self;
#else
#error "Mmap not supported"
	return NULL;
#endif
}

/**
 * \brief Frees a memory map handle.
 * \param self Memory map handle.
 */
void lisys_mmap_free (
	LISysMmap* self)
{
#ifdef HAVE_SYS_MMAN_H
	if (self->buffer != NULL)
		munmap ((void*) self->buffer, self->size);
	free (self);
#elif defined HAVE_WINDOWS_H
	if (self->buffer != NULL)
		UnmapViewOfFile ((void*) self->buffer);
	CloseHandle (self->handle);
	CloseHandle (self->file);
#else
#error "Mmap not supported"
	return NULL;
#endif
}

/**
 * \brief Gets the mapped memory area.
 * \param self Memory map.
 * \return Memory pointer, may be NULL if the file size was zero.
 */
void* lisys_mmap_get_buffer (
	LISysMmap* self)
{
	return self->buffer;
}

/**
 * \brief Gets the size of the mapped file.
 * \param self Memory map.
 * \return Size in bytes.
 */
int lisys_mmap_get_size (
	LISysMmap* self)
{
	return self->size;
}

/** @} */
/** @} */
