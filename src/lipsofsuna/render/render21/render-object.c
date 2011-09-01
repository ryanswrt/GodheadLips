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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenObject21 Object21
 * @{
 */

#include "lipsofsuna/network.h"
#include "lipsofsuna/system.h"
#include "render-object.h"
#include "render-private.h"
#include "render-scene.h"

static void private_lights_clear (
	LIRenObject21* self);

static int private_lights_create (
	LIRenObject21* self,
	LIMdlPose*     pose);

static void private_lights_update (
	LIRenObject21* self);

/*****************************************************************************/

/**
 * \brief Creates a new render object and adds it to the scene.
 * \param scene Scene.
 * \param id Unique identifier.
 * \return New object or NULL.
 */
LIRenObject21* liren_object21_new (
	LIRenScene21* scene,
	int           id)
{
	LIRenObject21* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenObject21));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	self->transform = limat_transform_identity ();
	self->orientation.matrix = limat_matrix_identity ();

	return self;
}

/**
 * \brief Frees the render object.
 *
 * The object is removed from its renderer and its memory is freed.
 *
 * \param self Object.
 */
void liren_object21_free (
	LIRenObject21* self)
{
	private_lights_clear (self);
	lisys_free (self);
}

/**
 * \brief Deforms the object.
 * \param self Object.
 */
void liren_object21_deform (
	LIRenObject21* self)
{
	if (!liren_object21_get_realized (self))
		return;

	/* Deform the mesh. */
	liren_model21_deform (self->model, "skeletal", self->pose);

	/* Update light positions. */
	private_lights_update (self);
}

/**
 * \brief Sets the particle animation state of the object.
 * \param self Object.
 * \param start Animation offset in seconds.
 * \param loop Nonzero to enable looping.
 */
void liren_object21_particle_animation (
	LIRenObject21* self,
	float          start,
	int            loop)
{
}

/**
 * \brief Advances the timer of the object and deforms its mesh.
 * \param self Object.
 * \param secs Number of seconds since the last update.
 */
void liren_object21_update (
	LIRenObject21* self,
	float          secs)
{
	private_lights_update (self);
}

/**
 * \brief Gets the bounding box of the object.
 * \param self Object.
 * \param result Return location for the bounding box.
 */
void liren_object21_get_bounds (
	const LIRenObject21* self,
	LIMatAabb*           result)
{
	*result = self->bounds;
}

/**
 * \brief Gets the model of the object.
 * \param self Object.
 * \return Model or NULL.
 */
LIRenModel21* liren_object21_get_model (
	LIRenObject21* self)
{
	return self->model;
}

/**
 * \brief Sets the model of the object.
 *
 * Changes the model of the object and takes care of automatically registering
 * and unregistering lights to match the new apperance.
 *
 * \param self Object.
 * \param model Model.
 * \return Nonzero on success.
 */
int liren_object21_set_model (
	LIRenObject21* self,
	LIRenModel21*  model)
{
	/* Replace model. */
	self->model = model;

	/* Recreate lights. */
	private_lights_clear (self);
	if (liren_object21_get_realized (self))
		private_lights_create (self, self->pose);

	return 1;
}

/**
 * \brief Sets the pose of the object.
 * \param self Object.
 * \param pose Pose.
 * \return Nonzero on success.
 */
int liren_object21_set_pose (
	LIRenObject21* self,
	LIMdlPose*     pose)
{
	if (self->pose != pose)
	{
		self->pose = pose;
		return liren_object21_set_model (self, self->model);
	}

	return 1;
}

/**
 * \brief Checks if the object is renderable.
 *
 * Returns nonzero if the realized flag of the object is set and the object
 * has a model assigned to it. Otherwise returns zero.
 *
 * \param self Object.
 * \return Nonzero if realized.
 */
int liren_object21_get_realized (
	const LIRenObject21* self)
{
	return self->realized && self->model != NULL;
}

/**
 * \brief Sets the realized flag of the object.
 *
 * If the flag is set and the object has a model set, the model will be rendered
 * in subsequent frames. The light sources that are a part of the model are also
 * added to the scene.
 *
 * If the flag is cleared, the model assigned to the object is not rendered from
 * now on. The light sources associated with the model are also removed from the
 * scene.
 *
 * \param self Object.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
int liren_object21_set_realized (
	LIRenObject21* self,
	int            value)
{
	if (self->realized == value)
		return 1;
	self->realized = value;

	/* Recreate lights. */
	if (!value)
		private_lights_clear (self);
	else
		private_lights_create (self, self->pose);

	return 1;
}

/**
 * \brief Gets the transformation of the object.
 * \param self Object.
 * \param value Return location for the transformation.
 */
void liren_object21_get_transform (
	LIRenObject21*  self,
	LIMatTransform* value)
{
	*value = self->transform;
}

/**
 * \brief Sets the transformation of the object.
 * \param self Object.
 * \param value Transformation.
 */
void liren_object21_set_transform (
	LIRenObject21*        self,
	const LIMatTransform* value)
{
	/* Set transformation. */
	self->transform = *value;
	self->orientation.matrix = limat_convert_transform_to_matrix (*value);

	/* Update light positions. */
	if (liren_object21_get_realized (self))
		private_lights_update (self);
}

/*****************************************************************************/

static void private_lights_clear (
	LIRenObject21* self)
{
	int i;

	for (i = 0 ; i < self->lights.count ; i++)
	{
		if (self->lights.array[i] != NULL)
		{
			liren_scene21_remove_light (self->scene, self->lights.array[i]);
			liren_light21_free (self->lights.array[i]);
		}
	}
	lisys_free (self->lights.array);
	self->lights.array = NULL;
	self->lights.count = 0;
}

static int private_lights_create (
	LIRenObject21* self,
	LIMdlPose*     pose)
{
	int i;
	float scale;
	LIMatTransform transform;
	LIMdlNode* node;
	LIMdlNodeIter iter;
	LIRenLight21* light;

	/* Create light sources. */
	if (pose != NULL)
	{
		LIMDL_FOREACH_NODE (iter, &pose->nodes)
		{
			/* Find a light node. */
			node = iter.value;
			if (node->type != LIMDL_NODE_LIGHT)
				continue;

			/* Create a light source. */
			light = liren_light21_new (self->scene, node->light.ambient,
				node->light.diffuse, node->light.specular, node->light.equation,
				node->light.spot.cutoff, node->light.spot.exponent,
				node->light.flags & LIMDL_LIGHT_FLAG_SHADOW);
			if (light == NULL)
				return 0;
			light->node = node;
			limdl_node_get_world_transform (node, &scale, &transform);
			liren_light21_set_transform (light, &transform);

			/* Add to the object. */
			if (!lialg_array_append (&self->lights, &light))
			{
				liren_light21_free (light);
				return 0;
			}
		}
	}

	/* Register light sources. */
	for (i = 0 ; i < self->lights.count ; i++)
	{
		light = self->lights.array[i];
		if (!liren_scene21_insert_light (self->scene, light))
		{
			while (i--)
			{
				light = self->lights.array[i];
				liren_scene21_remove_light (self->scene, light);
			}
			return 0;
		}
	}

	return 1;
}

static void private_lights_update (
	LIRenObject21* self)
{
	int i;
	float scale;
	LIMatTransform transform;
	LIRenLight21* light;

	for (i = 0 ; i < self->lights.count ; i++)
	{
		light = self->lights.array[i];
		if (light->node != NULL)
		{
			limdl_node_get_world_transform (light->node, &scale, &transform);
			transform = limat_transform_multiply (self->transform, transform);
			liren_light21_set_transform (light, &transform);
		}
	}
}

/** @} */
/** @} */
