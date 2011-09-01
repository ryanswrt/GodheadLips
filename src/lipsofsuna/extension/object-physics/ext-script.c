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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtObjectPhysics ObjectPhysics
 * @{
 */

#include "ext-module.h"

static void Object_approach (LIScrArgs* args)
{
	float dist = 0.0f;
	float speed = 1.0f;
	LIMatVector vector;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_gets_vector (args, "point", &vector))
	{
		liscr_args_gets_float (args, "dist", &dist);
		liscr_args_gets_float (args, "speed", &speed);
		liphy_object_approach (object, &vector, speed, dist);
	}
}

static void Object_impulse (LIScrArgs* args)
{
	LIMatVector impulse;
	LIMatVector point = { 0.0f, 0.0f, 0.0f };
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &impulse) ||
	    liscr_args_gets_vector (args, "impulse", &impulse))
	{
		if (!liscr_args_geti_vector (args, 1, &point))
			liscr_args_gets_vector (args, "point", &point);
		liphy_object_impulse (object, &point, &impulse);
	}
}

static void Object_insert_hinge_constraint (LIScrArgs* args)
{
	LIMatVector pos;
	LIMatVector axis = { 0.0f, 1.0f, 0.0f };
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &pos) ||
	    liscr_args_gets_vector (args, "position", &pos))
	{
		if (!liscr_args_geti_vector (args, 1, &axis))
			liscr_args_gets_vector (args, "axis", &axis);
		liphy_constraint_new_hinge (module->physics, object, &pos, &axis, 0, 0.0f, 0.0f);
	}
}

static void Object_jump (LIScrArgs* args)
{
	LIMatVector impulse;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &impulse))
		liphy_object_jump (object, &impulse);
}

static void Object_get_activated (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_bool (args, liphy_object_get_activated (object));
}
static void Object_set_activated (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_bool (args, 0, &value))
		liphy_object_set_activated (object, value);
}

static void Object_get_angular (LIScrArgs* args)
{
	LIMatVector tmp;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_angular (object, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_set_angular (LIScrArgs* args)
{
	LIMatVector vector;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_angular (object, &vector);
}

static void Object_get_bounding_box_physics (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatAabb aabb;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_bounds (object, &aabb);
	liscr_args_seti_vector (args, &aabb.min);
	liscr_args_seti_vector (args, &aabb.max);
}

static void Object_get_center_offset_physics (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatAabb aabb;
	LIMatVector ctr;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_bounds (object, &aabb);
	ctr = limat_vector_add (aabb.min, aabb.max);
	ctr = limat_vector_multiply (ctr, 0.5f);
	liscr_args_seti_vector (args, &ctr);
}

static void Object_get_collision_group (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_int (args, liphy_object_get_collision_group (object));
}
static void Object_set_collision_group (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_int (args, 0, &value))
		liphy_object_set_collision_group (object, value);
}

static void Object_get_collision_mask (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_int (args, liphy_object_get_collision_mask (object));
}
static void Object_set_collision_mask (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_int (args, 0, &value))
		liphy_object_set_collision_mask (object, value);
}

static void Object_get_contact_events (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_bool (args, liphy_object_get_contact_events (object));
}
static void Object_set_contact_events (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_bool (args, 0, &value))
		liphy_object_set_contact_events (object, value);
}

static void Object_get_gravity (LIScrArgs* args)
{
	LIMatVector tmp;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_gravity (object, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_set_gravity (LIScrArgs* args)
{
	LIMatVector vector;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_gravity (object, &vector);
}

static void Object_get_friction_liquid (LIScrArgs* args)
{
	float tmp;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	tmp = liphy_object_get_friction_liquid (object);
	liscr_args_seti_float (args, tmp);
}
static void Object_set_friction_liquid (LIScrArgs* args)
{
	float tmp;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &tmp))
		liphy_object_set_friction_liquid (object, tmp);
}

static void Object_get_gravity_liquid (LIScrArgs* args)
{
	LIMatVector tmp;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_gravity_liquid (object, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_set_gravity_liquid (LIScrArgs* args)
{
	LIMatVector vector;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_gravity_liquid (object, &vector);
}

static void Object_get_ground (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_bool (args, liphy_object_get_ground (object));
}

static void Object_get_mass (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_float (args, liphy_object_get_mass (object));
}
static void Object_set_mass (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		liphy_object_set_mass (object, value);
}

static void Object_get_movement (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_float (args, liphy_object_get_movement (object));
}
static void Object_set_movement (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &value))
		liphy_object_set_movement (object, value);
}

static void Object_get_physics (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	switch (liphy_object_get_control_mode (object))
	{
		case LIPHY_CONTROL_MODE_NONE:
			liscr_args_seti_string (args, "none");
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			liscr_args_seti_string (args, "kinematic");
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			liscr_args_seti_string (args, "rigid");
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			liscr_args_seti_string (args, "static");
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			liscr_args_seti_string (args, "vehicle");
			break;
		default:
			lisys_assert (0 && "invalid physics control mode");
			break;
	}
}
static void Object_set_physics (LIScrArgs* args)
{
	const char* str;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_string (args, 0, &str))
	{
		if (!strcmp (str, "none"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_NONE);
		else if (!strcmp (str, "kinematic"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_CHARACTER);
		else if (!strcmp (str, "rigid"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_RIGID);
		else if (!strcmp (str, "static"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_STATIC);
		else if (!strcmp (str, "vehicle"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_VEHICLE);
	}
}

static void Object_get_shape (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_string (args, liphy_object_get_shape (object));
}
static void Object_set_shape (LIScrArgs* args)
{
	const char* value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_string (args, 0, &value))
		liphy_object_set_shape (object, value);
}

static void Object_get_speed (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_float (args, liphy_object_get_speed (object));
}
static void Object_set_speed (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		liphy_object_set_speed (object, value);
}

static void Object_get_strafing (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_float (args, liphy_object_get_strafing (object));
}
static void Object_set_strafing (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &value))
		liphy_object_set_strafing (object, value);
}

static void Object_get_velocity (LIScrArgs* args)
{
	LIMatVector tmp;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_velocity (object, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_set_velocity (LIScrArgs* args)
{
	LIMatVector vector;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_velocity (object, &vector);
}

/*****************************************************************************/

void liext_script_physics_object (
	LIScrScript* self)
{
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_approach", Object_approach);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_impulse", Object_impulse);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_insert_hinge_constraint", Object_insert_hinge_constraint);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_jump", Object_jump);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_activated", Object_get_activated);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_activated", Object_set_activated);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_angular", Object_get_angular);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_angular", Object_set_angular);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_bounding_box_physics", Object_get_bounding_box_physics);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_center_offset_physics", Object_get_center_offset_physics);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_collision_group", Object_get_collision_group);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_collision_group", Object_set_collision_group);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_collision_mask", Object_get_collision_mask);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_collision_mask", Object_set_collision_mask);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_contact_events", Object_get_contact_events);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_contact_events", Object_set_contact_events);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_friction_liquid", Object_get_friction_liquid);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_friction_liquid", Object_set_friction_liquid);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_gravity", Object_get_gravity);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_gravity", Object_set_gravity);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_gravity_liquid", Object_get_gravity_liquid);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_gravity_liquid", Object_set_gravity_liquid);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_ground", Object_get_ground);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_mass", Object_get_mass);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_mass", Object_set_mass);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_movement", Object_get_movement);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_movement", Object_set_movement);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_physics", Object_get_physics);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_physics", Object_set_physics);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_shape", Object_get_shape);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_shape", Object_set_shape);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_speed", Object_get_speed);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_speed", Object_set_speed);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_strafing", Object_get_strafing);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_strafing", Object_set_strafing);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_velocity", Object_get_velocity);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_velocity", Object_set_velocity);
}

/** @} */
/** @} */
