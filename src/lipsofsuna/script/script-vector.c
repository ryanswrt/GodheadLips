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
 * \addtogroup LIScrVector Vector
 * @{
 */

#include "lipsofsuna/script.h"
#include "script-private.h"

static void Vector_new (LIScrArgs* args)
{
	LIMatVector vec = { 0.0f, 0.0f, 0.0f };

	liscr_args_seti_vector (args, &vec);
}

static void Vector_add (LIScrArgs* args)
{
	LIMatVector tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
		return;

	tmp = limat_vector_add (*((LIMatVector*) args->self), *((LIMatVector*) b->data));
	liscr_args_seti_vector (args, &tmp);
}

static void Vector_mul (LIScrArgs* args)
{
	float s;
	LIMatVector tmp;

	if (!liscr_args_geti_float (args, 0, &s))
		return;

	tmp = limat_vector_multiply (*((LIMatVector*) args->self), s);
	liscr_args_seti_vector (args, &tmp);
}

static void Vector_sub (LIScrArgs* args)
{
	LIMatVector tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
		return;

	tmp = limat_vector_subtract (*((LIMatVector*) args->self), *((LIMatVector*) b->data));
	liscr_args_seti_vector (args, &tmp);
}

static void Vector_tostring (LIScrArgs* args)
{
	char buffer[256];
	LIMatVector* self;

	self = args->self;
	snprintf (buffer, 256, "Vector(%g,%g,%g)", self->x, self->y, self->z);
	liscr_args_seti_string (args, buffer);
}

static void Vector_cross (LIScrArgs* args)
{
	LIMatVector tmp;
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &data))
	{
		tmp = limat_vector_cross (*((LIMatVector*) args->self), *((LIMatVector*) data->data));
		liscr_args_seti_vector (args, &tmp);
	}
}

static void Vector_dot (LIScrArgs* args)
{
	float tmp;
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &data))
	{
		tmp = limat_vector_dot (*((LIMatVector*) args->self), *((LIMatVector*) data->data));
		liscr_args_seti_float (args, tmp);
	}
}

static void Vector_normalize (LIScrArgs* args)
{
	LIMatVector tmp;

	tmp = limat_vector_normalize (*((LIMatVector*) args->self));
	liscr_args_seti_vector (args, &tmp);
}

static void Vector_get_length (LIScrArgs* args)
{
	liscr_args_seti_float (args, limat_vector_get_length (*((LIMatVector*) args->self)));
}

static void Vector_get_x (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatVector*) args->self)->x);
}
static void Vector_set_x (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatVector*) args->self)->x);
}

static void Vector_get_y (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatVector*) args->self)->y);
}
static void Vector_set_y (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatVector*) args->self)->y);
}

static void Vector_get_z (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatVector*) args->self)->z);
}
static void Vector_set_z (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatVector*) args->self)->z);
}

/*****************************************************************************/

void liscr_script_vector (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_VECTOR, "vector_new", Vector_new);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_add", Vector_add);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_mul", Vector_mul);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_sub", Vector_sub);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_tostring", Vector_tostring);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_cross", Vector_cross);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_dot", Vector_dot);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_normalize", Vector_normalize);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_get_length", Vector_get_length);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_get_x", Vector_get_x);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_set_x", Vector_set_x);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_get_y", Vector_get_y);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_set_y", Vector_set_y);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_get_z", Vector_get_z);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_set_z", Vector_set_z);
}

/** @} */
/** @} */
