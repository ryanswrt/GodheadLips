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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtThread Thread
 * @{
 */

#include "ext-module.h"

static void private_thread_main (
	LISysThread* thread,
	void*        data);

/*****************************************************************************/

LIExtThread* liext_thread_inst_new (
	LIMaiProgram* program,
	lua_State*    lua,
	const char*   file,
	const char*   code,
	const char*   args)
{
	LIExtThread* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtThread));
	if (self == NULL)
		return NULL;
	if (code != NULL)
	{
		self->code = lisys_string_dup (code);
		if (self->code == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}
	else if (file != NULL)
	{
		self->file = lisys_string_dup (file);
		if (self->file == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	/* Allocate the program. */
	self->program = limai_program_new (program->paths->root, program->paths->module_name, args);
	if (self->program == NULL)
	{
		lisys_free (self->file);
		lisys_free (self->code);
		lisys_free (self);
		return NULL;
	}
	self->program->parent = program;

	/* Allocate the host userdata. */
	self->script = liscr_data_new (program->script, lua, self, LIEXT_SCRIPT_THREAD, liext_thread_inst_free);
	if (self->script == NULL)
	{
		limai_program_free (self->program);
		lisys_free (self->file);
		lisys_free (self->code);
		lisys_free (self);
		return NULL;
	}

	/* Start the thread. */
	self->thread = lisys_thread_new (private_thread_main, self);
	if (self->thread == NULL)
		return self;

	return self;
}

void liext_thread_inst_free (
	LIExtThread* self)
{
	liext_thread_inst_set_quit (self);
	if (self->thread != NULL)
		lisys_thread_free (self->thread);
	if (self->program != NULL)
		limai_program_free (self->program);
	lisys_free (self->file);
	lisys_free (self->code);
	lisys_free (self);
}

void liext_thread_inst_set_quit (
	LIExtThread* self)
{
	if (self->program != NULL)
	{
		/* The script doesn't normally write to this so race conditions should
		   be unlikely. However, in theory we should protect this somehow. */
		self->program->quit = 1;
	}
}

/*****************************************************************************/

static void private_thread_main (
	LISysThread* thread,
	void*        data)
{
	LIExtThread* self = data;

	if (self->code != NULL)
	{
		if (!limai_program_execute_string (self->program, self->code))
			lisys_error_report ();
	}
	else
	{
		if (!limai_program_execute_script (self->program, self->file))
			lisys_error_report ();
	}
}

/** @} */
/** @} */
