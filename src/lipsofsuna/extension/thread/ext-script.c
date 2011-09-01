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

static void Thread_new (LIScrArgs* args)
{
	const char* args_ = "";
	const char* file = "main.lua";
	const char* code = NULL;
	LIExtModule* module;

	/* Read arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_THREAD);
	if (!liscr_args_geti_string (args, 0, &file))
	    liscr_args_gets_string (args, "file", &file);
	if (!liscr_args_geti_string (args, 1, &args_))
	    liscr_args_gets_string (args, "args", &args_);
	if (!liscr_args_geti_string (args, 2, &code))
	    liscr_args_gets_string (args, "code", &code);

	/* Create the thread to the stack. */
	if (liext_thread_inst_new (module->program, args->lua, file, code, args_) != NULL)
		liscr_args_seti_stack (args);
}

static void Thread_pop_message (LIScrArgs* args)
{
	LIMaiMessage* message;
	LIExtThread* self;
	LIEngModel* model;

	/* Pop the name. */
	self = args->self;
	message = limai_program_pop_message (self->program, LIMAI_MESSAGE_QUEUE_PROGRAM);
	if (message == NULL)
		return;

	/* Return the message. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_string (args, "name", message->name);
	switch (message->type)
	{
		case LIMAI_MESSAGE_TYPE_EMPTY:
			liscr_args_sets_string (args, "type", "empty");
			break;
		case LIMAI_MESSAGE_TYPE_MODEL:
			liscr_args_sets_string (args, "type", "model");
			model = lieng_model_new_model (self->program->parent->engine, message->model);
			if (model != NULL)
			{
				message->model = NULL;
				model->script = liscr_data_new (args->script, args->lua, model, LISCR_SCRIPT_MODEL, lieng_model_free);
				if (model->script != NULL)
					liscr_args_sets_stack (args, "model");
				else
					lieng_model_free (model);
			}
			break;
		case LIMAI_MESSAGE_TYPE_STRING:
			liscr_args_sets_string (args, "type", "string");
			liscr_args_sets_string (args, "string", message->string);
			break;
	}

	/* Free the message. */
	limai_message_free (message);
}

static void Thread_push_message (LIScrArgs* args)
{
	const char* name = "";
	const char* string = NULL;
	LIScrData* data;
	LIEngModel* emodel;
	LIExtThread* self;

	/* Read the name. */
	self = args->self;
	if (!liscr_args_geti_string (args, 0, &name))
		liscr_args_gets_string (args, "name", &name);

	/* Read and push the data. */
	if (liscr_args_geti_string (args, 1, &string) ||
	    liscr_args_gets_string (args, "string", &string))
	{
		if (limai_program_push_message (self->program, LIMAI_MESSAGE_QUEUE_THREAD, LIMAI_MESSAGE_TYPE_STRING, name, string))
			liscr_args_seti_bool (args, 1);
	}
	else if (liscr_args_geti_data (args, 1, LISCR_SCRIPT_MODEL, &data) ||
	         liscr_args_gets_data (args, "model", LISCR_SCRIPT_MODEL, &data))
	{
		emodel = liscr_data_get_data (data);
		if (limai_program_push_message (self->program, LIMAI_MESSAGE_QUEUE_THREAD, LIMAI_MESSAGE_TYPE_MODEL, name, emodel->model))
			liscr_args_seti_bool (args, 1);
	}
	else
	{
		if (limai_program_push_message (self->program, LIMAI_MESSAGE_QUEUE_THREAD, LIMAI_MESSAGE_TYPE_EMPTY, name, NULL))
			liscr_args_seti_bool (args, 1);
	}
}

static void Thread_set_quit (LIScrArgs* args)
{
	liext_thread_inst_set_quit (args->self);
}

/*****************************************************************************/

void liext_script_thread (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_THREAD, "thread_new", Thread_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_THREAD, "thread_pop_message", Thread_pop_message);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_THREAD, "thread_push_message", Thread_push_message);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_THREAD, "thread_set_quit", Thread_set_quit);
}

/** @} */
/** @} */
