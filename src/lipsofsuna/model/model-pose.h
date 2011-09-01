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

#ifndef __MODEL_POSE_H__
#define __MODEL_POSE_H__

#include <lipsofsuna/algorithm.h>
#include "model.h"
#include "model-animation.h"
#include "model-bone.h"
#include "model-types.h"
#include "model-vertex.h"

#define LIMDL_POSE_FADE_AUTOMATIC -1.0f

enum _LIMdlPoseChannelState
{
	LIMDL_POSE_CHANNEL_STATE_INVALID,
	LIMDL_POSE_CHANNEL_STATE_PLAYING,
	LIMDL_POSE_CHANNEL_STATE_PAUSED,
};

struct _LIMdlPoseChannel
{
	int additive;
	int state;
	int repeat;
	int repeats;
	int repeat_start;
	float time;
	float time_scale;
	float priority_scale;
	float priority_transform;
	float fade_in;
	float fade_out;
	LIAlgStrdic* weights;
	LIMdlAnimation* animation;
};

struct _LIMdlPoseFade
{
	float time;
	float time_fade;
	float fade_out;
	float priority_scale;
	float priority_transform;
	float current_weight_scale;
	float current_weight_transform;
	LIMdlPoseFade* prev;
	LIMdlPoseFade* next;
	LIMdlAnimation* animation;
};

typedef struct _LIMdlPoseGroup LIMdlPoseGroup;
struct _LIMdlPoseGroup
{
	int enabled;
	float scale_pose;
	LIMatVector head_pose;
	LIMatVector head_rest;
	LIMatQuaternion rotation;
	LIMdlNode* rest_node;
	LIMdlNode* pose_node;
	LIMdlWeightGroup* weight_group;
};

typedef struct _LIMdlPoseVertex LIMdlPoseVertex;
struct _LIMdlPoseVertex
{
	int weight_count;
	float weight_total;
};

struct _LIMdlPose
{
	LIMdlModel* model;
	LIAlgU32dic* channels;
	LIMdlPoseFade* fades;
	struct
	{
		int count;
		LIMdlPoseGroup* array;
	} groups;
	struct
	{
		int count;
		LIMdlNode** array;
	} nodes;
};

LIAPICALL (LIMdlPose*, limdl_pose_new, ());

LIAPICALL (void, limdl_pose_free, (
	LIMdlPose* self));

LIAPICALL (void, limdl_pose_clear_channel_node_priorities, (
	LIMdlPose*  self,
	int         channel));

LIAPICALL (void, limdl_pose_destroy_channel, (
	LIMdlPose* self,
	int        channel));

LIAPICALL (void, limdl_pose_fade_channel, (
	LIMdlPose* self,
	int        channel,
	float      rate));

LIAPICALL (LIMdlNode*, limdl_pose_find_node, (
	const LIMdlPose* self,
	const char*      name));

LIAPICALL (void, limdl_pose_update, (
	LIMdlPose* self,
	float      secs));

LIAPICALL (int, limdl_pose_get_channel_additive, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_additive, (
	LIMdlPose* self,
	int        channel,
	int        value));

LIAPICALL (LIMdlAnimation*, limdl_pose_get_channel_animation, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_animation, (
	LIMdlPose*  self,
	int         channel,
	const char* animation));

LIAPICALL (int, limdl_pose_get_channel_repeat_start, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_repeat_start, (
	LIMdlPose* self,
	int        channel,
	int        value));

LIAPICALL (float, limdl_pose_get_channel_fade_in, (
	LIMdlPose* self,
	int        channel));

LIAPICALL (void, limdl_pose_set_channel_fade_in, (
	LIMdlPose* self,
	int        channel,
	float      value));

LIAPICALL (float, limdl_pose_get_channel_fade_out, (
	LIMdlPose* self,
	int        channel));

LIAPICALL (void, limdl_pose_set_channel_fade_out, (
	LIMdlPose* self,
	int        channel,
	float      value));

LIAPICALL (const char*, limdl_pose_get_channel_name, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (float, limdl_pose_get_channel_position, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_position, (
	LIMdlPose* self,
	int        channel,
	float      value));

LIAPICALL (float*, limdl_pose_get_channel_priority_node, (
	const LIMdlPose* self,
	int              channel,
	const char*      node));

LIAPICALL (int, limdl_pose_set_channel_priority_node, (
	LIMdlPose*  self,
	int         channel,
	const char* node,
	float       value));

LIAPICALL (LIAlgStrdic*, limdl_pose_get_channel_priority_nodes, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (float, limdl_pose_get_channel_priority_scale, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_priority_scale, (
	LIMdlPose* self,
	int        channel,
	float      value));

LIAPICALL (float, limdl_pose_get_channel_priority_transform, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_priority_transform, (
	LIMdlPose* self,
	int        channel,
	float      value));

LIAPICALL (int, limdl_pose_get_channel_repeats, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_repeats, (
	LIMdlPose* self,
	int        channel,
	int        value));

LIAPICALL (LIMdlPoseChannelState, limdl_pose_get_channel_state, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_state, (
	LIMdlPose*            self,
	int                   channel,
	LIMdlPoseChannelState value));

LIAPICALL (float, limdl_pose_get_channel_time_scale, (
	const LIMdlPose* self,
	int              channel));

LIAPICALL (void, limdl_pose_set_channel_time_scale, (
	LIMdlPose* self,
	int        channel,
	float      value));

LIAPICALL (int, limdl_pose_set_channel_transform, (
	LIMdlPose*            self,
	int                   channel,
	int                   frame,
	const char*           node,
	float                 scale,
	const LIMatTransform* transform));

LIAPICALL (int, limdl_pose_set_model, (
	LIMdlPose*  self,
	LIMdlModel* model));

#endif
