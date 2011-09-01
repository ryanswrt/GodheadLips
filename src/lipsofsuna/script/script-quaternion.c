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
 * \addtogroup LIScrQuaternion Quaternion
 * @{
 */

#include "lipsofsuna/script.h"
#include "script-private.h"

static void Quaternion_new (LIScrArgs* args)
{
	float angle;
	LIMatVector axis;
	LIMatVector dir;
	LIMatVector up;
	LIMatQuaternion quat = { 0.0f, 0.0f, 0.0f, 1.0f };

	if (liscr_args_gets_vector (args, "axis", &axis) &&
	    liscr_args_gets_float (args, "angle", &angle))
	{
		quat = limat_quaternion_rotation (angle, axis);
	}
	else if (liscr_args_gets_vector (args, "dir", &dir) &&
	         liscr_args_gets_vector (args, "up", &up))
	{
		quat = limat_quaternion_look (dir, up);
		quat = limat_quaternion_conjugate (quat);
	}
	else
	{
		if (!liscr_args_gets_float (args, "x", &quat.x))
			liscr_args_geti_float (args, 0, &quat.x);
		if (!liscr_args_gets_float (args, "y", &quat.y))
			liscr_args_geti_float (args, 1, &quat.y);
		if (!liscr_args_gets_float (args, "z", &quat.z))
			liscr_args_geti_float (args, 2, &quat.z);
		if (!liscr_args_gets_float (args, "w", &quat.w))
			liscr_args_geti_float (args, 3, &quat.w);
	}
	liscr_args_seti_quaternion (args, &quat);
}

static void Quaternion_new_euler (LIScrArgs* args)
{
	float euler[3] = { 0.0f, 0.0f, 0.0f };
	LIMatQuaternion quat;

	liscr_args_geti_float (args, 0, euler + 0);
	liscr_args_geti_float (args, 1, euler + 1);
	liscr_args_geti_float (args, 2, euler + 2);
	quat = limat_quaternion_euler (euler[0], euler[1], euler[2]);
	liscr_args_seti_quaternion (args, &quat);
}

static void Quaternion_add (LIScrArgs* args)
{
	LIMatQuaternion tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &b))
		return;

	tmp = limat_quaternion_add (*((LIMatQuaternion*) args->self), *((LIMatQuaternion*) b->data));
	liscr_args_seti_quaternion (args, &tmp);
}

static void Quaternion_mul (LIScrArgs* args)
{
	float s;
	LIMatQuaternion q;
	LIMatVector v;
	LIScrData* b;

	if (liscr_args_geti_float (args, 0, &s))
	{
		/* Multiply by scalar. */
		q = limat_quaternion_init (
			((LIMatQuaternion*) args->self)->x * s,
			((LIMatQuaternion*) args->self)->y * s,
			((LIMatQuaternion*) args->self)->z * s,
			((LIMatQuaternion*) args->self)->w * s);
		liscr_args_seti_quaternion (args, &q);
	}
	else if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
	{
		/* Transform vector. */
		v = limat_quaternion_transform (*((LIMatQuaternion*) args->self), *((LIMatVector*) b->data));
		liscr_args_seti_vector (args, &v);
	}
	else if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &b))
	{
		/* Concatenate rotations. */
		q = limat_quaternion_multiply (*((LIMatQuaternion*) args->self), *((LIMatQuaternion*) b->data));
		liscr_args_seti_quaternion (args, &q);
	}
}

static void Quaternion_sub (LIScrArgs* args)
{
	LIMatQuaternion tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &b))
		return;

	tmp = limat_quaternion_subtract (*((LIMatQuaternion*) args->self), *((LIMatQuaternion*) b->data));
	liscr_args_seti_quaternion (args, &tmp);
}

static void Quaternion_tostring (LIScrArgs* args)
{
	char buffer[256];
	LIMatQuaternion* self;

	self = args->self;
	snprintf (buffer, 256, "Quaternion(%g,%g,%g,%g)", self->x, self->y, self->z, self->w);
	liscr_args_seti_string (args, buffer);
}

static void Quaternion_nlerp (LIScrArgs* args)
{
	float val;
	LIMatQuaternion q1;
	LIMatQuaternion q2;

	if (liscr_args_geti_quaternion(args, 0, &q2) &&
	    liscr_args_geti_float (args, 1, &val))
	{
		q1 = *((LIMatQuaternion*) args->self);
		q2 = limat_quaternion_get_nearest (q2, q1);
		q2 = limat_quaternion_nlerp (q1, q2, val);
		liscr_args_seti_quaternion (args, &q2);
	}
}

static void Quaternion_normalize (LIScrArgs* args)
{
	LIMatQuaternion tmp;

	tmp = limat_quaternion_normalize (*((LIMatQuaternion*) args->self));
	liscr_args_seti_quaternion (args, &tmp);
}

static void Quaternion_get_conjugate (LIScrArgs* args)
{
	LIMatQuaternion* data;
	LIMatQuaternion tmp;

	data = args->self;
	tmp = limat_quaternion_conjugate (*data);
	liscr_args_seti_quaternion (args, &tmp);
}

static void Quaternion_get_euler (LIScrArgs* args)
{
	float e[3];
	LIMatQuaternion* data;

	data = args->self;
	limat_quaternion_get_euler (*data, e + 0, e + 1, e + 2);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, e[0]);
	liscr_args_seti_float (args, e[1]);
	liscr_args_seti_float (args, e[2]);
}

static void Quaternion_get_length (LIScrArgs* args)
{
	liscr_args_seti_float (args, limat_quaternion_get_length (*((LIMatQuaternion*) args->self)));
}

static void Quaternion_get_w (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->w);
}
static void Quaternion_set_w (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->w);
}

static void Quaternion_get_x (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->x);
}
static void Quaternion_set_x (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->x);
}

static void Quaternion_get_y (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->y);
}
static void Quaternion_set_y (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->y);
}

static void Quaternion_get_z (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->z);
}
static void Quaternion_set_z (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->z);
}

/*****************************************************************************/

void liscr_script_quaternion (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_new", Quaternion_new);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_new_euler", Quaternion_new_euler);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_add", Quaternion_add);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_mul", Quaternion_mul);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_sub", Quaternion_sub);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_tostring", Quaternion_tostring);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_nlerp", Quaternion_nlerp);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_normalize", Quaternion_normalize);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_conjugate", Quaternion_get_conjugate);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_euler", Quaternion_get_euler);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_length", Quaternion_get_length);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_x", Quaternion_get_x);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_x", Quaternion_set_x);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_y", Quaternion_get_y);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_y", Quaternion_set_y);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_z", Quaternion_get_z);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_z", Quaternion_set_z);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_w", Quaternion_get_w);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_w", Quaternion_set_w);
}

/** @} */
/** @} */
