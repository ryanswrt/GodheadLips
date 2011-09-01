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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlPose Pose
 * @{
 */

#include <lipsofsuna/system.h>
#include "model-pose.h"

static void private_channel_free (
	LIMdlPoseChannel* chan);

static void private_clear_node (
	LIMdlPose*       self,
	LIMdlNode*       node,
	const LIMdlNode* rest);

static void private_clear_pose (
	LIMdlPose* self);

static LIMdlPoseChannel* private_create_channel (
	LIMdlPose* self,
	int        channel);

static void private_fade_free (
	LIMdlPoseFade* fade);

static void private_fade_remove (
	LIMdlPose*     self,
	LIMdlPoseFade* fade);

static LIMdlPoseChannel* private_find_channel (
	const LIMdlPose* self,
	int              channel);

static int private_init_pose (
	LIMdlPose*  self,
	LIMdlModel* model);

static int private_play_channel (
	const LIMdlPose*  self,
	LIMdlPoseChannel* channel,
	float             secs);

static void private_transform_node (
	LIMdlPose* self,
	LIMdlNode* node);

static void private_get_channel_weight (
	const LIMdlPose*        self,
	const LIMdlPoseChannel* channel,
	const LIMdlNode*        node,
	float*                  scale,
	float*                  transform);

static float private_smooth_fade (
	float channel_weight,
	float fade_offset,
	float fade_length);

static LIMdlAnimation private_empty_anim =
{
	0, "empty", 0.0f, 0.0f, { 0, NULL }, { 0, NULL }
};

/*****************************************************************************/

/**
 * \brief Creates a new model pose.
 * \return New model pose or NULL.
 */
LIMdlPose* limdl_pose_new ()
{
	LIMdlPose* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlPose));
	if (self == NULL)
		return NULL;

	/* Allocate channel tree. */
	self->channels = lialg_u32dic_new ();
	if (self->channels == NULL)
	{
		limdl_pose_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the model pose.
 * \param self A model pose.
 */
void limdl_pose_free (
	LIMdlPose* self)
{
	private_clear_pose (self);
	if (self->channels != NULL)
		lialg_u32dic_free (self->channels);
	lisys_free (self);
}

void limdl_pose_clear_channel_node_priorities (
	LIMdlPose*  self,
	int         channel)
{
	LIAlgStrdicIter iter;
	LIMdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL || chan->weights == NULL)
		return;
	LIALG_STRDIC_FOREACH (iter, chan->weights)
		lisys_free (iter.value);
	lialg_strdic_free (chan->weights);
	chan->weights = NULL;
}

/**
 * \brief Destroys an animation channel.
 *
 * Channels are destroyed automatically with the pose but it's also possible to
 * destroy a channel explicitly with this function.
 *
 * \param self Model pose.
 * \param channel Channel number.
 */
void limdl_pose_destroy_channel (
	LIMdlPose* self,
	int        channel)
{
	LIMdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL)
		return;
	lialg_u32dic_remove (self->channels, channel);
	private_channel_free (chan);
}

/**
 * \brief Fades out the channel.
 *
 * If an animation is playing or paused on the channel, a new fade sequence
 * is created and played seamlessly in the background. The channel cleared
 * and immediately ready to accept a new animation.
 *
 * If the channel is empty, no fading takes place, so it is safe to call this
 * function every time you set a new animation and want it to blend.
 *
 * \param self Model pose.
 * \param channel Channel number.
 * \param secs Fade out time in seconds or LIMDL_POSE_FADE_AUTOMATIC.
 */
void limdl_pose_fade_channel (
	LIMdlPose* self,
	int        channel,
	float      secs)
{
	LIMdlPoseFade* fade;
	LIMdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL)
		return;

	/* Handle auto rate. */
	if (secs == LIMDL_POSE_FADE_AUTOMATIC)
	{
		if (chan->fade_out < LIMAT_EPSILON)
			return;
		secs = chan->fade_out;
	}

	/* Create a fade sequence. */
	fade = lisys_calloc (1, sizeof (LIMdlPoseFade));
	if (fade == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		return;
	}
	private_get_channel_weight (self, chan, NULL, &fade->priority_scale, &fade->priority_transform);
	fade->fade_out = secs;
	fade->time = chan->time;
	fade->time_fade = 0.0f;
	fade->current_weight_transform = fade->priority_transform;
	fade->current_weight_scale = fade->priority_scale;
	fade->animation = limdl_animation_new_copy (chan->animation);
	if (fade->animation == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		lisys_free (fade);
		return;
	}

	/* Link to fade list. */
	fade->prev = NULL;
	fade->next = self->fades;
	if (self->fades != NULL)
		self->fades->prev = fade;
	self->fades = fade;

	/* Destroy the channel. */
	limdl_pose_destroy_channel (self, channel);
}

/**
 * \brief Finds a node by name.
 *
 * \param self Model pose.
 * \param name Name of the node to find.
 * \return Node or NULL.
 */
LIMdlNode* limdl_pose_find_node (
	const LIMdlPose* self,
	const char*      name)
{
	int i;
	LIMdlNode* node;

	if (self->model == NULL)
		return NULL;

	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		node = limdl_node_find_node (node, name);
		if (node != NULL)
			return node;
	}

	return NULL;
}

/**
 * \brief Progresses the animation.
 *
 * \param self Model pose.
 * \param secs Number of seconds to progress.
 */
void limdl_pose_update (
	LIMdlPose* self,
	float      secs)
{
	int i;
	LIAlgU32dicIter iter;
	LIMatQuaternion quat0;
	LIMatQuaternion quat1;
	LIMdlPoseFade* fade;
	LIMdlPoseFade* fade_next;
	LIMdlPoseChannel* chan;
	LIMdlPoseGroup* group;
	LIMdlNode* node0;
	LIMdlNode* node1;

	if (self->model == NULL)
		return;

	/* Update channels. */
	LIALG_U32DIC_FOREACH (iter, self->channels)
	{
		chan = iter.value;
		switch (chan->state)
		{
			case LIMDL_POSE_CHANNEL_STATE_PAUSED:
				break;
			case LIMDL_POSE_CHANNEL_STATE_PLAYING:
				if (!private_play_channel (self, chan, secs))
				{
					lialg_u32dic_remove (self->channels, iter.key);
					private_channel_free (chan);
				}
				break;
		}
	}

	/* Update fade sequences. */
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		/* Calculate smooth fading. */
		fade->current_weight_scale = private_smooth_fade (
			fade->priority_scale, fade->time_fade, fade->fade_out);
		fade->current_weight_transform = private_smooth_fade (
			fade->priority_transform, fade->time_fade, fade->fade_out);

		/* Update time and weights. */
		fade_next = fade->next;
		fade->time += secs;
		fade->time_fade += secs;

		/* Remove the fade when its influence reaches zero. */
		if (fade->time_fade >= fade->fade_out)
		{
			private_fade_remove (self, fade);
			private_fade_free (fade);
		}
	}

	/* Clear each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node0 = self->nodes.array[i];
		node1 = self->model->nodes.array[i];
		private_clear_node (self, node0, node1);
	}

	/* Transform each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node0 = self->nodes.array[i];
		private_transform_node (self, node0);
	}

	/* Update pose group transformations. */
	for (i = 0 ; i < self->groups.count ; i++)
	{
		group = self->groups.array + i;
		if (group->enabled)
		{
			quat0 = group->rest_node->transform.global.rotation;
			quat1 = group->pose_node->transform.global.rotation;
			quat0 = limat_quaternion_conjugate (quat0);
			group->rotation = limat_quaternion_multiply (quat1, quat0);
			group->head_pose = group->pose_node->transform.global.position;
			group->scale_pose = group->pose_node->transform.global_scale;
		}
		else
		{
			group->rotation = limat_quaternion_identity ();
			group->head_pose = limat_vector_init (0.0f, 0.0f, 0.0f);
			group->scale_pose = 1.0f;
		}
	}
}

int limdl_pose_get_channel_additive (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;
	return chan->additive;
}

void limdl_pose_set_channel_additive (
	LIMdlPose* self,
	int        channel,
	int        value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->additive = value;
}

LIMdlAnimation* limdl_pose_get_channel_animation (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return NULL;
	return chan->animation;
}

void limdl_pose_set_channel_animation (
	LIMdlPose*  self,
	int         channel,
	const char* animation)
{
	LIMdlAnimation* anim;
	LIMdlPoseChannel* chan;

	if (self->model == NULL)
		return;

	/* Create an animation. */
	anim = limdl_model_find_animation (self->model, animation);
	if (anim == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		return;
	}
	anim = limdl_animation_new_copy (anim);
	if (anim == NULL)
		return;

	/* Create a channel. */
	chan = private_create_channel (self, channel);
	if (chan == NULL)
	{
		limdl_animation_free (anim);
		return;
	}
	chan->time = 0.0f;
	chan->fade_in = 0.0f;
	chan->fade_out = 0.0f;
	limdl_animation_free (chan->animation);
	chan->animation = anim;
}

int limdl_pose_get_channel_repeat_start (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;
	return chan->repeat_start;
}

void limdl_pose_set_channel_repeat_start (
	LIMdlPose* self,
	int        channel,
	int        value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->repeat_start = value;
}

float limdl_pose_get_channel_fade_in (
	LIMdlPose* self,
	int        channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->fade_in;
}

void limdl_pose_set_channel_fade_in (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan != NULL)
		chan->fade_in = value;
}

float limdl_pose_get_channel_fade_out (
	LIMdlPose* self,
	int        channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->fade_out;
}

void limdl_pose_set_channel_fade_out (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan != NULL)
		chan->fade_out = value;
}

const char* limdl_pose_get_channel_name (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return "";
	return chan->animation->name;
}

float limdl_pose_get_channel_position (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->time;
}

void limdl_pose_set_channel_position (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->time = value;
	if (chan->time > limdl_animation_get_duration (chan->animation))
		chan->time = limdl_animation_get_duration (chan->animation);
}

float* limdl_pose_get_channel_priority_node (
	const LIMdlPose* self,
	int              channel,
	const char*      node)
{
	float* ptr;
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL || chan->weights == NULL)
		return NULL;
	ptr = lialg_strdic_find (chan->weights, node);

	return ptr;
}

int limdl_pose_set_channel_priority_node (
	LIMdlPose*  self,
	int         channel,
	const char* node,
	float       value)
{
	float* ptr;
	LIMdlPoseChannel* chan;

	/* Find the channel. */
	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;

	/* Make sure the node weight dictionary exists. */
	if (chan->weights == NULL)
	{
		chan->weights = lialg_strdic_new ();
		if (chan->weights == NULL)
			return 0;
	}

	/* Replace or add a node weight. */
	ptr = lialg_strdic_find (chan->weights, node);
	if (ptr == NULL)
	{
		ptr = lisys_calloc (1, sizeof (float));
		if (ptr == NULL)
			return 0;
		if (!lialg_strdic_insert (chan->weights, node, ptr))
		{
			lisys_free (ptr);
			return 0;
		}
	}
	*ptr = value;

	return 1;
}

LIAlgStrdic* limdl_pose_get_channel_priority_nodes (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return NULL;

	return chan->weights;
}

float limdl_pose_get_channel_priority_scale (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->priority_scale;
}

void limdl_pose_set_channel_priority_scale (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->priority_scale = value;
}

float limdl_pose_get_channel_priority_transform (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->priority_transform;
}

void limdl_pose_set_channel_priority_transform (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->priority_transform = value;
}

int limdl_pose_get_channel_repeats (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;
	return chan->repeats;
}

void limdl_pose_set_channel_repeats (
	LIMdlPose* self,
	int        channel,
	int        value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->repeats = value;
}

/**
 * \brief Gets the state of a channel.
 *
 * \param self Model pose.
 * \param channel Channel number.
 * \return Current state.
 */
LIMdlPoseChannelState limdl_pose_get_channel_state (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return LIMDL_POSE_CHANNEL_STATE_INVALID;
	return chan->state;
}

/**
 * \brief Sets the state of a channel.
 *
 * \param self Model pose.
 * \param channel Channel number.
 * \param value New state.
 */
void limdl_pose_set_channel_state (
	LIMdlPose*            self,
	int                   channel,
	LIMdlPoseChannelState value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL || chan->state == value)
		return;
	switch (value)
	{
		case LIMDL_POSE_CHANNEL_STATE_PLAYING:
			chan->state = value;
			break;
		case LIMDL_POSE_CHANNEL_STATE_PAUSED:
			chan->state = value;
			break;
		default:
			lisys_assert (0);
			break;
	}
}

float limdl_pose_get_channel_time_scale (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;
	return chan->time_scale;
}

void limdl_pose_set_channel_time_scale (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->time_scale = value;
}

/**
 * \brief Sets the transformation of a node for the given frame.
 * \param self Pose.
 * \param channel Channel number.
 * \param frame Frame number.
 * \param node Node name.
 * \param scale Scale factor.
 * \param transform Node transformation.
 * \return Nonzero on success.
 */
int limdl_pose_set_channel_transform (
	LIMdlPose*            self,
	int                   channel,
	int                   frame,
	const char*           node,
	float                 scale,
	const LIMatTransform* transform)
{
	LIMdlNode* node_;
	LIMdlPoseChannel* chan;

	/* Find the node. */
	node_ = limdl_pose_find_node (self, node);
	if (node_ == NULL)
		return 0;

	/* Find the channel. */
	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;

	/* Make sure the channel and the frame exist. */
	if (!limdl_animation_insert_channel (chan->animation, node))
		return 0;
	if (chan->animation->length <= frame)
	{
		if (!limdl_animation_set_length (chan->animation, frame + 1))
			return 0;
	}

	/* Set the node transformation of the frame. */
	limdl_animation_set_transform (chan->animation, node, frame, scale, transform);

	return 1;
}

/**
 * \brief Sets the posed model.
 *
 * \param self Model pose.
 * \param model Model to pose.
 * \return Nonzero on success.
 */
int limdl_pose_set_model (
	LIMdlPose*  self,
	LIMdlModel* model)
{
	LIAlgU32dicIter iter;
	LIMdlAnimation* anim;
	LIMdlPose backup;
	LIMdlPoseChannel* chan;
	LIMdlPoseFade* fade;
	LIMdlPoseFade* fade_next;

	/* Backup old data. */
	memcpy (&backup, self, sizeof (LIMdlPose));
	self->channels = NULL;
	self->fades = NULL;
	self->model = NULL;
	self->groups.count = 0;
	self->groups.array = NULL;
	self->nodes.count = 0;
	self->nodes.array = NULL;

	/* Initialize new pose. */
	if (model != NULL)
	{
		if (!private_init_pose (self, model))
		{
			private_clear_pose (self);
			memcpy (self, &backup, sizeof (LIMdlPose));
			return 0;
		}
	}

	/* Clear invalid animations. */
	self->channels = backup.channels;
	backup.channels = NULL;
	LIALG_U32DIC_FOREACH (iter, self->channels)
	{
		chan = iter.value;
		if (model != NULL)
		{
			anim = limdl_model_find_animation (model, chan->animation->name);
			if (anim != NULL)
				anim = limdl_animation_new_copy (anim);
		}
		else
			anim = NULL;
		if (anim != NULL)
		{
			limdl_animation_free (chan->animation);
			chan->animation = anim;
		}
		else
		{
			lialg_u32dic_remove (self->channels, iter.key);
			private_channel_free (chan);
		}
	}

	/* Clear invalid fades. */
	self->fades = backup.fades;
	backup.fades = NULL;
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		fade_next = fade->next;
		if (model != NULL)
			anim = limdl_model_find_animation (model, fade->animation->name);
		else
			anim = NULL;
		if (anim == NULL)
		{
			private_fade_remove (self, fade);
			private_fade_free (fade);
		}
	}

	/* Clear old data. */
	private_clear_pose (&backup);

	/* Rebuild pose channels. */
	limdl_pose_update (self, 0.0f);

	return 1;
}

/*****************************************************************************/

static void private_channel_free (
	LIMdlPoseChannel* chan)
{
	LIAlgStrdicIter iter;

	limdl_animation_free (chan->animation);
	if (chan->weights)
	{
		LIALG_STRDIC_FOREACH (iter, chan->weights)
			lisys_free (iter.value);
		lialg_strdic_free (chan->weights);
	}
	lisys_free (chan);
}

static void private_clear_node (
	LIMdlPose*       self,
	LIMdlNode*       node,
	const LIMdlNode* rest)
{
	int i;
	LIMdlNode* node0;
	LIMdlNode* node1;

	node->transform.global = rest->transform.global;
	for (i = 0 ; i < node->nodes.count ; i++)
	{
		node0 = node->nodes.array[i];
		node1 = rest->nodes.array[i];
		private_clear_node (self, node0, node1);
	}
}

static void private_clear_pose (
	LIMdlPose* self)
{
	int i;
	LIAlgU32dicIter iter;
	LIMdlPoseFade* fade;
	LIMdlPoseFade* fade_next;

	/* Free fades. */
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		fade_next = fade->next;
		private_fade_free (fade);
	}
	self->fades = NULL;

	/* Clear channel tree. */
	if (self->channels != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->channels)
			private_channel_free (iter.value);
		lialg_u32dic_clear (self->channels);
	}

	/* Free nodes. */
	if (self->nodes.array != NULL)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			if (self->nodes.array[i] != NULL)
				limdl_node_free (self->nodes.array[i]);
		}
		lisys_free (self->nodes.array);
	}

	lisys_free (self->groups.array);
}

static LIMdlPoseChannel* private_create_channel (
	LIMdlPose* self,
	int        channel)
{
	LIMdlAnimation* anim;
	LIMdlPoseChannel* chan;

	/* Check for an existing channel. */
	chan = private_find_channel (self, channel);
	if (chan != NULL)
		return chan;

	/* Create an empty animation. */
	anim = limdl_animation_new_copy (&private_empty_anim);
	if (anim == NULL)
		return 0;

	/* Create a new channel. */
	chan = lisys_calloc (1, sizeof (LIMdlPoseChannel));
	if (chan == NULL)
	{
		limdl_animation_free (anim);
		return 0;
	}
	chan->state = LIMDL_POSE_CHANNEL_STATE_PLAYING;
	chan->animation = anim;
	chan->priority_scale = 0.0f;
	chan->priority_transform = 1.0f;
	chan->time_scale = 1.0f;

	/* Register the channel. */
	if (!lialg_u32dic_insert (self->channels, channel, chan))
	{
		limdl_animation_free (chan->animation);
		lisys_free (chan);
		return 0;
	}

	return chan;
}

static void private_fade_free (
	LIMdlPoseFade* fade)
{
	limdl_animation_free (fade->animation);
	lisys_free (fade);
}

static void private_fade_remove (
	LIMdlPose*     self,
	LIMdlPoseFade* fade)
{
	if (fade->next != NULL)
		fade->next->prev = fade->prev;
	if (fade->prev != NULL)
		fade->prev->next = fade->next;
	else
		self->fades = fade->next;
}

static LIMdlPoseChannel* private_find_channel (
	const LIMdlPose* self,
	int              channel)
{
	return lialg_u32dic_find (self->channels, channel);
}

static int private_init_pose (
	LIMdlPose*  self,
	LIMdlModel* model)
{
	int i;
	LIMdlPoseGroup* pose_group;
	LIMdlWeightGroup* weight_group;

	/* Set model. */
	self->model = model;
	self->groups.count = model->weight_groups.count;
	self->nodes.count = model->nodes.count;

	/* Copy nodes. */
	if (self->nodes.count)
	{
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (LIMdlNode*));
		if (self->nodes.array == NULL)
			return 0;
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_copy (model->nodes.array[i]);
			if (self->nodes.array[i] == NULL)
				return 0;
		}
	}

	/* Precalculate weight group information. */
	if (self->groups.count)
	{
		self->groups.array = lisys_calloc (self->groups.count, sizeof (LIMdlPoseGroup));
		if (self->groups.array == NULL)
			return 0;
		for (i = 0 ; i < self->groups.count ; i++)
		{
			weight_group = model->weight_groups.array + i;
			pose_group = self->groups.array + i;
			pose_group->weight_group = weight_group;
			pose_group->rest_node = weight_group->node;
			pose_group->pose_node = limdl_pose_find_node (self, weight_group->bone);
			pose_group->rotation = limat_quaternion_identity ();
			if (pose_group->rest_node != NULL)
				pose_group->head_rest = pose_group->rest_node->transform.global.position;
			if (pose_group->rest_node != NULL && pose_group->pose_node != NULL)
				pose_group->enabled = 1;
		}
	}

	return 1;
}

static int private_play_channel (
	const LIMdlPose*  self,
	LIMdlPoseChannel* channel,
	float             secs)
{
	int cycles;
	float duration;
	float start;

	/* Skip empty. */
	duration = limdl_animation_get_duration (channel->animation);
	if (duration < LIMAT_EPSILON)
	{
		channel->time = 0.0f;
		if (channel->repeats == -1)
			return 1;
		return 0;
	}

	/* Advance time. */
	channel->time += channel->time_scale * secs;

	/* Handle looping. */
	if (channel->time > duration)
	{
		start = LIMAT_CLAMP (channel->repeat_start, 0.0f, duration);
		if (start < duration)
		{
			cycles = (int) floor ((channel->time - start) / (duration - start));
			channel->time = channel->time - (duration - start) * cycles;
			channel->repeat += cycles;
		}
		else
		{
			channel->time = duration;
			channel->repeat++;
		}
	}

	/* Handle ending. */
	if (channel->repeats != -1 && channel->repeat >= channel->repeats)
		return 0;

	return 1;
}

static void private_transform_node (
	LIMdlPose* self,
	LIMdlNode* node)
{
	int i;
	int channels;
	float scale;
	float scale1;
	float total_scale;
	float total_transform;
	float weight;
	float weight1;
	LIAlgU32dicIter iter;
	LIMatQuaternion bonerot;
	LIMatQuaternion rotation;
	LIMatTransform transform;
	LIMatVector bonepos;
	LIMatVector position;
	LIMdlPoseFade* fade;
	LIMdlPoseChannel* chan;

	channels = 0;
	scale = 0.0f;
	total_scale = 0.0f;
	total_transform = 0.0f;
	position = limat_vector_init (0.0f, 0.0f, 0.0f);
	rotation = limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f);

	/* Sum channel weights. */
	LIALG_U32DIC_FOREACH (iter, self->channels)
	{
		chan = iter.value;
		if (chan->additive)
			continue;
		if (limdl_animation_get_channel (chan->animation, node->name) != -1)
		{
			private_get_channel_weight (self, chan, node, &weight, &weight1);
			total_scale += weight;
			total_transform += weight1;
			channels++;
		}
	}

	/* Sum fade weights. */
	for (fade = self->fades ; fade != NULL ; fade = fade->next)
	{
		if (limdl_animation_get_channel (fade->animation, node->name) != -1)
		{
			total_transform += fade->current_weight_transform;
			total_scale += fade->current_weight_scale;
			channels++;
		}
	}

	/* Apply valid transformation influences. */
	if (channels && total_transform >= LIMAT_EPSILON)
	{
		/* Apply channel influences. */
		LIALG_U32DIC_FOREACH (iter, self->channels)
		{
			chan = iter.value;
			if (chan->additive)
				continue;
			if (limdl_animation_get_transform (chan->animation, node->name, chan->time, &scale1, &transform))
			{
				bonepos = transform.position;
				bonerot = transform.rotation;
				private_get_channel_weight (self, chan, node, &weight1, &weight);
				rotation = limat_quaternion_nlerp (bonerot, rotation, weight / total_transform);
				position = limat_vector_lerp (bonepos, position, weight / total_transform);
			}
		}

		/* Apply fade influences. */
		for (fade = self->fades ; fade != NULL ; fade = fade->next)
		{
			if (limdl_animation_get_transform (fade->animation, node->name, fade->time, &scale1, &transform))
			{
				bonepos = transform.position;
				bonerot = transform.rotation;
				weight = fade->current_weight_transform;
				rotation = limat_quaternion_nlerp (bonerot, rotation, weight / total_transform);
				position = limat_vector_lerp (bonepos, position, weight / total_transform);
			}
		}
	}

	/* Apply valid scale influences. */
	if (channels && total_scale >= LIMAT_EPSILON)
	{
		/* Apply channel influences. */
		LIALG_U32DIC_FOREACH (iter, self->channels)
		{
			chan = iter.value;
			if (limdl_animation_get_transform (chan->animation, node->name, chan->time, &scale1, &transform))
			{
				private_get_channel_weight (self, chan, node, &weight, &weight1);
				scale += scale1 * weight / total_scale;
			}
		}

		/* Apply fade influences. */
		for (fade = self->fades ; fade != NULL ; fade = fade->next)
		{
			if (limdl_animation_get_transform (fade->animation, node->name, fade->time, &scale1, &transform))
			{
				weight = fade->current_weight_scale;
				scale += scale1 * weight / total_scale;
			}
		}
	}
	else
		scale = 1.0f;

	/* Apply additive transformations and scaling. */
	/* Additive channels aren't normalized against the total weight but applied as
	   is on top of other transformations. If the weight of an additive channel is
	   1, the blended transformation of other channels is multiplied by its full
	   rotation and scaling. */
	LIALG_U32DIC_FOREACH (iter, self->channels)
	{
		chan = iter.value;
		if (!chan->additive)
			continue;
		if (limdl_animation_get_transform (chan->animation, node->name, chan->time, &scale1, &transform))
		{
			bonepos = transform.position;
			bonerot = transform.rotation;
			private_get_channel_weight (self, chan, node, &weight1, &weight);
			rotation = limat_quaternion_nlerp (bonerot, rotation, weight);
			position = limat_vector_lerp (bonepos, position, weight);
			scale += scale1 * weight1;
		}
	}

	/* Update node transformation. */
	transform = limat_transform_init (position, rotation);
	limdl_node_set_local_transform (node, scale, &transform);
	limdl_node_rebuild (node, 0);

	/* Update child transformations recursively. */
	for (i = 0 ; i < node->nodes.count ; i++)
		private_transform_node (self, node->nodes.array[i]);
}

static void private_get_channel_weight (
	const LIMdlPose*        self,
	const LIMdlPoseChannel* channel,
	const LIMdlNode*        node,
	float*                  scale,
	float*                  transform)
{
	float end;
	float time;
	float duration;
	float weight_scale;
	float weight_transform;
	float* weight_ptr;

	/* Calculate channel offset. */
	duration = limdl_animation_get_duration (channel->animation);
	time = channel->repeat * duration + channel->time;
	end = channel->repeats * duration;

	/* Calculate base weights. */
	weight_scale = channel->priority_scale;
	weight_transform = channel->priority_transform;
	if (node != NULL && channel->weights)
	{
		weight_ptr = lialg_strdic_find (channel->weights, node->name);
		if (weight_ptr != NULL)
			weight_transform = *weight_ptr;
	}

	/* Calculate channel weight. */
	if (!channel->repeat && time < channel->fade_in)
	{
		/* Fade in period. */
		*scale = private_smooth_fade (weight_scale,
			channel->fade_in - time, channel->fade_in);
		*transform = private_smooth_fade (weight_transform,
			channel->fade_in - time, channel->fade_in);
	}
	else if (channel->repeats == -1 || time < end - channel->fade_out)
	{
		/* No fade period. */
		*scale = weight_scale;
		*transform = weight_transform;
	}
	else
	{
		/* Fade out period. */
		*scale = private_smooth_fade (weight_scale,
			time - (end - channel->fade_out), channel->fade_out);
		*transform = private_smooth_fade (weight_transform,
			time - (end - channel->fade_out), channel->fade_out);
	}
}

static float private_smooth_fade (
	float channel_weight,
	float fade_offset,
	float fade_length)
{
	float weight_base;
	float weight_scaled;
	float weight_smoothing;

	/* Calculates smooth fading. */
	/* Linear fading of channels with wildly different weights doesn't
	   look nice because the channel with the largest weight dominates.
	   This is particularly problematic with cross-fading since there's
	   no cross-fading at all without compensating for the difference. */
	/* We reduce the problem by applying smoothstep() to the excess weight.
	   This compensates for the weight difference by smoothly bringing the
	   weight of the sequence closer to the [0,1] range. */
	weight_base = 1.0f - (fade_offset / fade_length);
	weight_scaled = weight_base * channel_weight;
	weight_smoothing = 1.0f - limat_smoothstep (fade_offset, 0.0f, fade_length);
	return weight_base + (weight_scaled - weight_base) * weight_smoothing;
}

/** @} */
/** @} */
