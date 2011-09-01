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
 * \addtogroup LIExtAnimation Animation
 * @{
 */

#include "ext-module.h"

static void Object_animate (LIScrArgs* args)
{
	int additive = 0;
	int repeat = 0;
	int repeat_start = 0;
	int channel = -1;
	int keep = 0;
	float fade_in = 0.0f;
	float fade_out = 0.0f;
	float weight = 1.0f;
	float weight_scale = 0.0f;
	float time = 0.0f;
	float time_scale = 1.0f;
	const char* animation = NULL;
	const char* name;
	LIEngObject* self = args->self;

	/* Handle arguments. */
	liscr_args_gets_bool (args, "addivite", &additive);
	liscr_args_gets_string (args, "animation", &animation);
	liscr_args_gets_int (args, "channel", &channel);
	liscr_args_gets_float (args, "fade_in", &fade_in);
	liscr_args_gets_float (args, "fade_out", &fade_out);
	liscr_args_gets_bool (args, "permanent", &repeat);
	liscr_args_gets_int (args, "repeat_start", &repeat_start);
	liscr_args_gets_float (args, "time", &time);
	liscr_args_gets_float (args, "time_scale", &time_scale);
	liscr_args_gets_float (args, "weight", &weight);
	liscr_args_gets_float (args, "weight_scale", &weight_scale);
	if (channel < 1 || channel > 255)
		channel = -1;
	else
		channel--;
	repeat_start = LIMAT_MAX (0, repeat_start);
	time_scale = LIMAT_MAX (0.0f, time_scale);

	/* Avoid restarts in simple cases. */
	/* The position is kept if the animation is repeating and being replaced with
	   the same one but parameters such as fading and weights still need to be reset. */
	if (repeat && channel != -1 && animation != NULL)
	{
		if (limdl_pose_get_channel_state (self->pose, channel) == LIMDL_POSE_CHANNEL_STATE_PLAYING &&
		    limdl_pose_get_channel_repeats (self->pose, channel) == -1)
		{
			name = limdl_pose_get_channel_name (self->pose, channel);
			if (!strcmp (name, animation))
				keep = 1;
		}
	}

	/* Automatic channel assignment. */
	if (channel == -1)
	{
		for (channel = 254 ; channel > 0 ; channel--)
		{
			if (limdl_pose_get_channel_state (self->pose, channel) == LIMDL_POSE_CHANNEL_STATE_INVALID)
				break;
		}
	}

	/* Update or initialize the channel. */
	if (!keep)
	{
		limdl_pose_fade_channel (self->pose, channel, LIMDL_POSE_FADE_AUTOMATIC);
		if (animation != NULL)
		{
			limdl_pose_set_channel_animation (self->pose, channel, animation);
			limdl_pose_set_channel_repeats (self->pose, channel, repeat? -1 : 1);
			limdl_pose_set_channel_position (self->pose, channel, time);
			limdl_pose_set_channel_state (self->pose, channel, LIMDL_POSE_CHANNEL_STATE_PLAYING);
		}
	}
	limdl_pose_set_channel_additive (self->pose, channel, additive);
	limdl_pose_set_channel_repeat_start (self->pose, channel, repeat_start);
	limdl_pose_set_channel_priority_scale (self->pose, channel, weight_scale);
	limdl_pose_set_channel_priority_transform (self->pose, channel, weight);
	limdl_pose_set_channel_time_scale (self->pose, channel, time_scale);
	limdl_pose_set_channel_fade_in (self->pose, channel, fade_in);
	limdl_pose_set_channel_fade_out (self->pose, channel, fade_out);

	/* Handle optional per-node weights. */
	if (animation != NULL)
	{
		limdl_pose_clear_channel_node_priorities (self->pose, channel);
		if (liscr_args_gets_table (args, "node_weights"))
		{
			lua_pushnil (args->lua);
			while (lua_next (args->lua, -2) != 0)
			{
				if (lua_type (args->lua, -2) == LUA_TSTRING &&
				    lua_type (args->lua, -1) == LUA_TNUMBER)
				{
					limdl_pose_set_channel_priority_node (self->pose, channel,
						lua_tostring (args->lua, -2),
						lua_tonumber (args->lua, -1));
				}
				lua_pop (args->lua, 1);
			}
			lua_pop (args->lua, 1);
		}
	}

	liscr_args_seti_bool (args, 1);
}

static void Object_animate_fade (LIScrArgs* args)
{
	int channel = 0;
	float time = LIMDL_POSE_FADE_AUTOMATIC;
	LIEngObject* object;

	if (!liscr_args_gets_int (args, "channel", &channel))
		return;
	if (liscr_args_gets_float (args, "duration", &time))
		time = LIMAT_MAX (0, time);

	if (channel < 1 || channel > 255)
		return;
	channel--;

	object = args->self;
	if (object->pose != NULL)
		limdl_pose_fade_channel (object->pose, channel, time);
}

static void Object_edit_pose (LIScrArgs* args)
{
	int frame = 0;
	int channel = 0;
	float scale = 1.0f;
	const char* node = NULL;
	LIMatTransform transform = limat_transform_identity ();
	LIEngObject* self = args->self;

	if (!liscr_args_gets_string (args, "node", &node))
		return;
	if (liscr_args_gets_int (args, "channel", &channel))
	{
		channel--;
		if (channel < 0) channel = 0;
		if (channel > 254) channel = 254;
	}
	if (liscr_args_gets_int (args, "frame", &frame))
	{
		frame--;
		if (frame < 0)
			return;
	}
	liscr_args_gets_quaternion (args, "rotation", &transform.rotation);
	liscr_args_gets_vector (args, "position", &transform.position);
	liscr_args_gets_float (args, "scale", &scale);
	transform.rotation = limat_quaternion_normalize (transform.rotation);

	limdl_pose_set_channel_transform (self->pose, channel, frame, node, scale, &transform);
}

static void Object_find_node (LIScrArgs* args)
{
	float scale;
	const char* name;
	const char* space = "local";
	LIMatTransform transform;
	LIMatTransform transform1;
	LIMdlNode* node;
	LIEngObject* self;

	if (!liscr_args_gets_string (args, "name", &name))
		return;
	liscr_args_gets_string (args, "space", &space);

	/* Find the node. */
	self = args->self;
	node = limdl_pose_find_node (self->pose, name);
	if (node == NULL)
		return;

	/* Get the transformation. */
	if (!strcmp (space, "world"))
	{
		limdl_node_get_world_transform (node, &scale, &transform);
		lieng_object_get_transform (self, &transform1);
		transform = limat_transform_multiply (transform1, transform);
	}
	else
		limdl_node_get_world_transform (node, &scale, &transform);

	/* Return the transformation. */
	liscr_args_seti_vector (args, &transform.position);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

static void Object_get_animation (LIScrArgs* args)
{
	int chan;
	LIAlgStrdic* weights;
	LIAlgStrdicIter iter;
	LIEngObject* object;
	LIMdlAnimation* anim;

	/* Check arguments. */
	if (!liscr_args_gets_int (args, "channel", &chan))
		return;
	chan--;
	object = args->self;
	anim = limdl_pose_get_channel_animation (object->pose, chan);
	if (anim == NULL)
		return;

	/* Set animation info. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_string (args, "animation", anim->name);
	liscr_args_sets_int (args, "channel", chan + 1);
	liscr_args_sets_float (args, "fade_in", limdl_pose_get_channel_fade_in (object->pose, chan));
	liscr_args_sets_float (args, "fade_out", limdl_pose_get_channel_fade_out (object->pose, chan));
	liscr_args_sets_bool (args, "permanent", limdl_pose_get_channel_repeats (object->pose, chan) == -1);
	liscr_args_sets_float (args, "repeat_start", limdl_pose_get_channel_repeat_start (object->pose, chan));
	liscr_args_sets_float (args, "time", limdl_pose_get_channel_position (object->pose, chan));
	liscr_args_sets_float (args, "time_scale", limdl_pose_get_channel_time_scale (object->pose, chan));
	liscr_args_sets_float (args, "weight", limdl_pose_get_channel_priority_transform (object->pose, chan));
	liscr_args_sets_float (args, "weight_scale", limdl_pose_get_channel_priority_scale (object->pose, chan));

	/* Set node weight info. */
	weights = limdl_pose_get_channel_priority_nodes (object->pose, chan);
	if (weights != NULL)
	{
		lua_newtable (args->lua);
		LIALG_STRDIC_FOREACH (iter, weights)
		{
			lua_pushnumber (args->lua, *((float*) iter.value));
			lua_setfield (args->lua, -2, iter.key);
		}
		liscr_args_sets_stack (args, "node_weights");
	}
}

static void Object_get_animations (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;
	LIMdlPoseChannel* channel;

	object = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, object->pose->channels)
	{
		channel = iter.value;
		if (channel->repeats == -1)
			liscr_args_setf_string (args, iter.key + 1, channel->animation->name);
	}
}

static void Object_update_animations (LIScrArgs* args)
{
	float secs = 1.0f;
	LIEngObject* self;

	self = args->self;
	liscr_args_gets_float (args, "secs", &secs);
	if (self->pose != NULL)
		limdl_pose_update (self->pose, secs);
}

/*****************************************************************************/

void liext_script_object_animation (
	LIScrScript* self)
{
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_animate", Object_animate);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_animate_fade", Object_animate_fade);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_edit_pose", Object_edit_pose);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_find_node", Object_find_node);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_animation", Object_get_animation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_animations", Object_get_animations);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_update_animations", Object_update_animations);
}

/** @} */
/** @} */
