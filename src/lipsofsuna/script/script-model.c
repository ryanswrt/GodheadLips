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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrObject Object
 * @{
 */

#include <lipsofsuna/engine.h>
#include <lipsofsuna/main.h>
#include <lipsofsuna/script.h>
#include "script-private.h"

static void Model_new (LIScrArgs* args)
{
	LIEngModel* self;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);

	/* Allocate model. */
	self = lieng_model_new (program->engine);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->script = liscr_data_new (args->script, args->lua, self, LISCR_SCRIPT_MODEL, lieng_model_free);
	if (self->script == NULL)
	{
		lieng_model_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Model_copy (LIScrArgs* args)
{
	LIEngModel* self;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);

	/* Allocate model. */
	self = lieng_model_new_copy (args->self);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->script = liscr_data_new (args->script, args->lua, self, LISCR_SCRIPT_MODEL, lieng_model_free);
	if (self->script == NULL)
	{
		lieng_model_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Model_calculate_bounds (LIScrArgs* args)
{
	lieng_model_calculate_bounds (args->self);
}

static void Model_changed (LIScrArgs* args)
{
	lieng_model_changed (args->self);
}

static void Model_get_bounding_box (LIScrArgs* args)
{
	LIEngModel* self;
	LIMatVector min;
	LIMatVector max;

	self = args->self;
	if (self->model != NULL)
	{
		min = self->model->bounds.min;
		max = self->model->bounds.max;
	}
	else
	{
		min = limat_vector_init (-0.1f, -0.1f, -0.1f);
		max = limat_vector_init (0.1f, 0.1f, 0.1f);
	}
	liscr_args_seti_vector (args, &min);
	liscr_args_seti_vector (args, &max);
}

static void Model_get_center_offset (LIScrArgs* args)
{
	LIEngModel* self;
	LIMatVector ctr;

	self = args->self;
	if (self->model != NULL)
	{
		ctr = limat_vector_add (self->model->bounds.min, self->model->bounds.max);
		ctr = limat_vector_multiply (ctr, 0.5f);
	}
	else
		ctr = limat_vector_init (0.0f, 0.0f, 0.0f);
	liscr_args_seti_vector (args, &ctr);
}

static void Model_get_memory_used (LIScrArgs* args)
{
	LIEngModel* self;

	self = args->self;
	if (self->model != NULL)
		liscr_args_seti_int (args, limdl_model_get_memory (self->model));
	else
		liscr_args_seti_int (args, 0);
}

static void Model_load (LIScrArgs* args)
{
	int mesh = 1;
	const char* file;

	if (!liscr_args_geti_string (args, 0, &file) &&
	    !liscr_args_gets_string (args, "file", &file))
		return;
	if (!liscr_args_geti_bool (args, 1, &mesh))
		liscr_args_gets_bool (args, "mesh", &mesh);

	if (!lieng_model_load (args->self, file, mesh))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

/*****************************************************************************/

void liscr_script_model (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_MODEL, "model_new", Model_new);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_copy", Model_copy);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_calculate_bounds", Model_calculate_bounds);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_changed", Model_changed);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_get_bounding_box", Model_get_bounding_box);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_get_center_offset", Model_get_center_offset);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_get_memory_used", Model_get_memory_used);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_load", Model_load);
}

/** @} */
/** @} */
