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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "system-error.h"
#include "system-memory.h"

#ifdef LI_ENABLE_MEMDEBUG
#define __USE_GNU 1
#include <pthread.h>
#include <execinfo.h>
#include "lipsofsuna/algorithm.h"
#define MEMDEBUG_BTSIZE 32
#define MEMDEBUG_CLRCHAR 0xFF
#define MEMDEBUG_PADCHAR 0xFF
#define MEMDEBUG_PADSIZE 128
typedef struct _LISysMemrec LISysMemrec;
struct _LISysMemrec { size_t size; size_t bt_size; void* bt[MEMDEBUG_BTSIZE]; };
static int memdebug_skip = 0;
static LIAlgPtrdic* memdebug_dict = NULL;
static pthread_mutex_t memdebug_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static void private_mem_add (void* mem, size_t size);
static size_t private_mem_del (void* mem);
static void private_rec_check (LISysMemrec* rec, void* mem);
static void private_rec_error (LISysMemrec* rec, void* mem);
#endif

/*****************************************************************************/

void* lisys_calloc (
	size_t num,
	size_t size)
{
	void* mem;

#ifndef LI_ENABLE_MEMDEBUG
	mem = calloc (num, size);
#else
	mem = calloc (num * size + MEMDEBUG_PADSIZE, 1);
	private_mem_add (mem, num * size);
#endif
	if (mem == NULL)
		lisys_error_set (ENOMEM, NULL);

	return mem;
}

void* lisys_malloc (
	size_t size)
{
	void* mem;

#ifndef LI_ENABLE_MEMDEBUG
	mem = malloc (size);
#else
	mem = malloc (size + MEMDEBUG_PADSIZE);
	memset (mem, MEMDEBUG_CLRCHAR, size);
	private_mem_add (mem, size);
#endif
	if (mem == NULL)
		lisys_error_set (ENOMEM, NULL);

	return mem;
}

void* lisys_realloc (
	void*  mem,
	size_t size)
{
	void* mem1;

#ifndef LI_ENABLE_MEMDEBUG
	mem1 = realloc (mem, size);
#else
	size_t size1 = private_mem_del (mem);
	mem1 = realloc (mem, size? size + MEMDEBUG_PADSIZE : 0);
	if (size1 < size)
		memset (mem1 + size1, MEMDEBUG_CLRCHAR, size - size1);
	private_mem_add (mem1, size);
#endif
	if (mem1 == NULL && size)
		lisys_error_set (ENOMEM, NULL);

	return mem1;
}

void lisys_free (void* mem)
{
#ifdef LI_ENABLE_MEMDEBUG
	private_mem_del (mem);
#endif
	free (mem);
}

/**
 * \brief Checks the validity of the memory block when compiled with memory debugging.
 * \param mem Pointer to memory.
 */
void lisys_memchk (
	void*  mem)
{
#ifdef LI_ENABLE_MEMDEBUG
	LISysMemrec* rec;

	pthread_mutex_lock (&memdebug_mutex);
	if (!memdebug_skip && mem != NULL)
	{
		rec = lialg_ptrdic_find (memdebug_dict, mem);
		private_rec_check (rec, mem);
	}
	pthread_mutex_unlock (&memdebug_mutex);
#endif
}

/**
 * \brief Dumps all the allocated memory blocks when compiled with memory debugging.
 */
void lisys_memstats ()
{
#ifdef LI_ENABLE_MEMDEBUG
	size_t i;
	char** names;
	LIAlgPtrdicIter iter;
	LISysMemrec* rec;

	pthread_mutex_lock (&memdebug_mutex);
	if (memdebug_dict != NULL)
	{
		LIALG_PTRDIC_FOREACH (iter, memdebug_dict)
		{
			rec = iter.value;
			printf ("memdebug: %zdB at %p\n", rec->size, iter.key);
			names = backtrace_symbols (rec->bt, rec->bt_size);
			for (i = 0 ; i < rec->bt_size ; i++)
				printf ("  %s\n", names[i]);
			free (names);
		}
	}
	pthread_mutex_unlock (&memdebug_mutex);
#endif
}

/*****************************************************************************/

#ifdef LI_ENABLE_MEMDEBUG
static void private_mem_add (void* mem, size_t size)
{
	size_t i;
	LISysMemrec* rec;

	pthread_mutex_lock (&memdebug_mutex);
	if (!memdebug_skip && mem != NULL)
	{
		/* Check for duplication. */
		/* The most likely cause is that the region was allocated with
		   lisys_malloc() but freed with ordinary free(). */
		if (memdebug_dict != NULL)
		{
			rec = lialg_ptrdic_find (memdebug_dict, mem);
			if (rec != NULL)
				private_rec_error (rec, mem);
		}

		/* Allocate the record. */
		rec = calloc (1, sizeof (LISysMemrec));
		rec->size = size;
		rec->bt_size = backtrace (rec->bt, MEMDEBUG_BTSIZE);

		/* Add to the dictionary. */
		memdebug_skip++;
		if (memdebug_dict == NULL)
		{
			memdebug_dict = lialg_ptrdic_new ();
			atexit (lisys_memstats);
		}
		lialg_ptrdic_insert (memdebug_dict, mem, rec);
		memdebug_skip--;

		/* Add the padding. */
		for (i = 0 ; i < MEMDEBUG_PADSIZE ; i++)
			((unsigned char*) mem)[size + i] = MEMDEBUG_PADCHAR;

		private_rec_check (rec, mem);
	}
	pthread_mutex_unlock (&memdebug_mutex);
}

static size_t private_mem_del (void* mem)
{
	size_t size = 0;
	LISysMemrec* rec;

	pthread_mutex_lock (&memdebug_mutex);
	if (!memdebug_skip && mem != NULL)
	{
		/* Validate the record. */
		rec = lialg_ptrdic_find (memdebug_dict, mem);
		private_rec_check (rec, mem);
		size = rec->size;

		/* Remove from the dictionary. */
		memdebug_skip++;
		lialg_ptrdic_remove (memdebug_dict, mem);
		memdebug_skip--;
		free (rec);
	}
	pthread_mutex_unlock (&memdebug_mutex);

	return size;
}

static void private_rec_check (LISysMemrec* rec, void* mem)
{
	int i;

	/* Validate the pointer. */
	lisys_assert (rec != NULL);

	/* Validate the padding. */
	for (i = MEMDEBUG_PADSIZE - 1 ; i >= 0 ; i--)
	{
		if (((unsigned char*) mem)[rec->size + i] != MEMDEBUG_PADCHAR)
		{
			printf ("memdebug: buffer overflow of %dB\n", (int)(i + 1));
			private_rec_error (rec, mem);
		}
	}
}

static void private_rec_error (LISysMemrec* rec, void* mem)
{
	size_t i;
	char** names;

	/* Print the allocation backtrace. */
	printf ("memdebug: stack trace:\n");
	names = backtrace_symbols (rec->bt, rec->bt_size);
	for (i = 0 ; i < rec->bt_size ; i++)
		printf ("  %s\n", names[i]);
	free (names);

	abort ();
}
#endif
