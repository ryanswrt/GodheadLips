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

#include "lipsofsuna/engine.h"
#include "lipsofsuna/main.h"
#include "lipsofsuna/script.h"
#include "script-private.h"

static void Object_find (LIScrArgs* args)
{
	int id;
	float radius = 32.0f;
	LIAlgU32dicIter iter1;
	LIEngObject* object;
	LIEngSector* sector;
	LIMatVector center;
	LIMatVector diff;
	LIMaiProgram* program;

	/* Find class data. */
	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);

	/* Radial find mode. */
	if (liscr_args_gets_vector (args, "point", &center))
	{
		liscr_args_gets_float (args, "radius", &radius);
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		LIALG_U32DIC_FOREACH (iter1, program->engine->objects)
		{
			object = iter1.value;
			diff = limat_vector_subtract (center, object->transform.position);
			if (limat_vector_get_length (diff) < radius)
				liscr_args_seti_data (args, object->script);
		}
	}

	/* Sector find mode. */
	else if (liscr_args_gets_int (args, "sector", &id))
	{
		sector = lialg_sectors_data_index (program->sectors, LIALG_SECTORS_CONTENT_ENGINE, id, 0);
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		if (sector != NULL)
		{
			LIALG_U32DIC_FOREACH (iter1, sector->objects)
			{
				object = iter1.value;
				liscr_args_seti_data (args, object->script);
			}
		}
	}
}

static void Object_new (LIScrArgs* args)
{
	LIEngObject* self;
	LIMaiProgram* program;

	/* Find class data. */
	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);

	/* Allocate object. */
	self = lieng_object_new (program->engine);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->script = liscr_data_new (args->script, args->lua, self, LISCR_SCRIPT_OBJECT, lieng_object_free);
	if (self->script == NULL)
	{
		lieng_object_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Object_refresh (LIScrArgs* args)
{
	float radius = 32.0f;

	liscr_args_gets_float (args, "radius", &radius);
	lieng_object_refresh (args->self, radius);
}

static void Object_get_model (LIScrArgs* args)
{
	LIEngObject* self = args->self;

	if (self->model != NULL)
		liscr_args_seti_data (args, self->model->script);
}

static void Object_set_model (LIScrArgs* args)
{
	LIScrData* value;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &value))
		lieng_object_set_model (args->self, value->data);
	else
		lieng_object_set_model (args->self, NULL);
}

static void Object_get_position (LIScrArgs* args)
{
	LIMatTransform tmp;

	lieng_object_get_transform (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp.position);
}

static void Object_set_position (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
	{
		lieng_object_get_transform (args->self, &transform);
		transform.position = vector;
		lieng_object_set_transform (args->self, &transform);
	}
}

static void Object_get_realized (LIScrArgs* args)
{
	liscr_args_seti_bool (args, lieng_object_get_realized (args->self));
}

static void Object_set_realized (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		lieng_object_set_realized (args->self, value);
}

static void Object_get_rotation (LIScrArgs* args)
{
	LIMatTransform tmp;

	lieng_object_get_transform (args->self, &tmp);
	liscr_args_seti_quaternion (args, &tmp.rotation);
}

static void Object_set_rotation (LIScrArgs* args)
{
	LIMatTransform transform;
	LIScrData* quat;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &quat))
	{
		lieng_object_get_transform (args->self, &transform);
		transform.rotation = *((LIMatQuaternion*) quat->data);
		lieng_object_set_transform (args->self, &transform);
	}
}

static void Object_get_sector (LIScrArgs* args)
{
	LIEngObject* self = args->self;

	if (self->sector != NULL)
		liscr_args_seti_int (args, self->sector->sector->index);
}

/*****************************************************************************/

void liscr_script_object (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_OBJECT, "object_find", Object_find);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_OBJECT, "object_new", Object_new);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_refresh", Object_refresh);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_model", Object_get_model);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_model", Object_set_model);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_position", Object_get_position);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_position", Object_set_position);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_realized", Object_get_realized);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_realized", Object_set_realized);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_rotation", Object_get_rotation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_rotation", Object_set_rotation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_sector", Object_get_sector);
}

/** @} */
/** @} */
