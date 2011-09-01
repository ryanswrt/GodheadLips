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
 * \addtogroup LIExtVision Vision
 * @{
 */

#include "ext-module.h"
#include "ext-vision-listener.h"

static void Vision_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIExtVisionListener* self;
	LIScrData* data;

	/* Allocate self. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VISION);
	self = liext_vision_listener_new (module);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_VISION, liext_vision_listener_free);
	if (data == NULL)
	{
		liext_vision_listener_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Vision_clear (LIScrArgs* args)
{
	liext_vision_listener_clear (args->self);
}

static void Vision_update (LIScrArgs* args)
{
	/* Add vision events to a table. */
	liscr_script_set_gc (args->script, 0);
	lua_newtable (args->lua);
	liext_vision_listener_update (args->self, args->lua);
	liscr_args_seti_stack (args);
	liscr_script_set_gc (args->script, 1);
}

static void Vision_get_cone_angle (LIScrArgs* args)
{
	LIExtVisionListener* self;

	self = args->self;
	liscr_args_seti_float (args, self->cone_angle);
}

static void Vision_set_cone_angle (LIScrArgs* args)
{
	float v;
	LIExtVisionListener* self;

	self = args->self;
	if (liscr_args_geti_float (args, 0, &v))
	{
		self->cone_angle = v;
		self->cone_cosine = cos (v);
	}
}

static void Vision_get_cone_factor (LIScrArgs* args)
{
	LIExtVisionListener* self;

	self = args->self;
	liscr_args_seti_float (args, self->cone_factor);
}

static void Vision_set_cone_factor (LIScrArgs* args)
{
	float v;
	LIExtVisionListener* self;

	self = args->self;
	if (liscr_args_geti_float (args, 0, &v))
		self->cone_factor = v;
}

static void Vision_get_direction (LIScrArgs* args)
{
	LIExtVisionListener* self;

	self = args->self;
	liscr_args_seti_vector (args, &self->direction);
}

static void Vision_set_direction (LIScrArgs* args)
{
	LIMatVector v;
	LIExtVisionListener* self;

	self = args->self;
	if (liscr_args_geti_vector (args, 0, &v))
		self->direction = v;
}

static void Vision_get_position (LIScrArgs* args)
{
	LIExtVisionListener* self;

	self = args->self;
	liscr_args_seti_vector (args, &self->position);
}

static void Vision_set_position (LIScrArgs* args)
{
	LIMatVector v;
	LIExtVisionListener* self;

	self = args->self;
	if (liscr_args_geti_vector (args, 0, &v))
		self->position = v;
}

static void Vision_get_radius (LIScrArgs* args)
{
	LIExtVisionListener* self;

	self = args->self;
	liscr_args_seti_float (args, self->scan_radius);
}

static void Vision_set_radius (LIScrArgs* args)
{
	float v;
	LIExtVisionListener* self;

	self = args->self;
	if (liscr_args_geti_float (args, 0, &v))
		self->scan_radius = v;
}

static void Vision_get_threshold (LIScrArgs* args)
{
	LIExtVisionListener* self;

	self = args->self;
	liscr_args_seti_float (args, self->keep_threshold);
}

static void Vision_set_threshold (LIScrArgs* args)
{
	float v;
	LIExtVisionListener* self;

	self = args->self;
	if (liscr_args_geti_float (args, 0, &v))
		self->keep_threshold = v;
}

/*****************************************************************************/

void liext_script_vision (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VISION, "vision_new", Vision_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_clear", Vision_clear);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_update", Vision_update);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_get_cone_angle", Vision_get_cone_angle);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_set_cone_angle", Vision_set_cone_angle);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_get_cone_factor", Vision_get_cone_factor);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_set_cone_factor", Vision_set_cone_factor);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_get_direction", Vision_get_direction);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_set_direction", Vision_set_direction);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_get_position", Vision_get_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_set_position", Vision_set_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_get_radius", Vision_get_radius);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_set_radius", Vision_set_radius);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_get_threshold", Vision_get_threshold);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_VISION, "vision_set_threshold", Vision_set_threshold);
}

/** @} */
/** @} */
