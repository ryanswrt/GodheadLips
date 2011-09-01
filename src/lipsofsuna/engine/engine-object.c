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
 * \addtogroup LIEng Engine
 * @{
 * \addtogroup LIEngObject Object
 * @{
 */

#include "lipsofsuna/network.h"
#include "engine-object.h"

static int private_warp (
	LIEngObject*       self,
	const LIMatVector* position);

/*****************************************************************************/

/**
 * \brief Creates a new engine object.
 * \param engine Engine.
 * \return Engine object or NULL.
 */
LIEngObject* lieng_object_new (
	LIEngEngine* engine)
{
	LIEngObject* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngObject));
	if (self == NULL)
		return NULL;
	self->engine = engine;
	self->transform = limat_transform_identity ();

	/* Choose object number. */
	self->id = 0;
	while (!self->id)
	{
		self->id = lialg_random_range (&engine->random, LINET_RANGE_ENGINE_START, LINET_RANGE_ENGINE_END);
		if (lieng_engine_find_object (engine, self->id))
			self->id = 0;
	}

	/* Insert to object list. */
	if (!lialg_u32dic_insert (engine->objects, self->id, self))
	{
		lisys_free (self);
		return 0;
	}

	/* Allocate pose buffer. */
	self->pose = limdl_pose_new ();
	if (self->pose == NULL)
	{
		lialg_u32dic_remove (engine->objects, self->id);
		lisys_free (self);
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, "object-new", lical_marshal_DATA_PTR, self);

	return self;
}

/**
 * \brief Frees the object.
 * \param self Object.
 */
void lieng_object_free (
	LIEngObject* self)
{
	/* Unrealize. */
	/* When the execution gets here, the script data of the object has been
	   garbage collected. Hence, we need to be careful not to generate any
	   script events that refer to it. */
	if (lieng_object_get_realized (self))
		lieng_object_set_realized (self, 0);

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, "object-free", lical_marshal_DATA_PTR, self);

	/* Remove from engine. */
	lialg_u32dic_remove (self->engine->objects, self->id);

	/* Free pose. */
	if (self->pose != NULL)
		limdl_pose_free (self->pose);

	/* Free all memory. */
	lisys_free (self);
}

/**
 * \brief Called when the object has moved.
 * \param self Object.
 */
int lieng_object_moved (
	LIEngObject* self)
{
	LIEngSector* dst;
	LIEngSector* src;
	LIMatTransform transform;

	/* Move between sectors. */
	lieng_object_get_transform (self, &transform);
	src = self->sector;
	dst = lialg_sectors_data_point (self->engine->sectors, LIALG_SECTORS_CONTENT_ENGINE, &transform.position, 1);
	if (dst == NULL)
		return 0;
	if (src != dst)
	{
		if (lialg_u32dic_insert (dst->objects, self->id, self) == NULL)
			return 0;
		if (src != NULL)
			lialg_u32dic_remove (src->objects, self->id);
		self->sector = dst;
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, "object-motion", lical_marshal_DATA_PTR, self);

	return 1;
}

/**
 * \brief Refreshes map around the object.
 *
 * Calling this function prevents the map sectors near the object from
 * unloading. One important use case for this is preventing clients from
 * being swapped out when they stand still.
 *
 * \param self Object.
 * \param radius Refresh radius.
 */
void lieng_object_refresh (
	LIEngObject* self,
	float        radius)
{
	LIMatTransform transform;

	if (lieng_object_get_realized (self))
	{
		lieng_object_get_transform (self, &transform);
		lialg_sectors_refresh_point (self->engine->sectors, &transform.position, radius);
	}
}

/**
 * \brief Gets the bounding box size of the object.
 * \param self Object.
 * \param bounds Return location for the bounding box.
 */
void lieng_object_get_bounds (
	const LIEngObject* self,
	LIMatAabb*         bounds)
{
	if (self->model != NULL && self->model->model != NULL)
		*bounds = self->model->model->bounds;
	else
		limat_aabb_init (bounds);
}

/**
 * \brief Gets the distance between the objects.
 *
 * If either of the objects is in not realized, LIMAT_INFINITE is returned.
 *
 * \param self An object.
 * \param object An object.
 * \return The distance.
 */
float lieng_object_get_distance (
	const LIEngObject* self,
	const LIEngObject* object)
{
	LIMatTransform t0;
	LIMatTransform t1;

	if (!lieng_object_get_realized (self) ||
	    !lieng_object_get_realized (object))
		return LIMAT_INFINITE;
	lieng_object_get_transform (self, &t0);
	lieng_object_get_transform (object, &t1);

	return limat_vector_get_length (limat_vector_subtract (t0.position, t1.position));
}

/**
 * \brief Replaces the current model of the object.
 * \param self Object.
 * \param model Model or NULL.
 * \return Nonzero on success.
 */
int lieng_object_set_model (
	LIEngObject* self,
	LIEngModel*  model)
{
	lua_State* lua;
	LIScrScript* script;

	script = liscr_data_get_script (self->script);
	lua = liscr_script_get_lua (script);

	/* Switch model. */
	if (model != NULL)
		limdl_pose_set_model (self->pose, model->model);
	else
		limdl_pose_set_model (self->pose, NULL);
	self->model = model;

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, "object-model", lical_marshal_DATA_PTR_PTR, self, model);

	return 1;
}

/**
 * \brief Checks if the object is added to the simulation.
 * \param self Object.
 * \return Nonzero if realized.
 */
int lieng_object_get_realized (
	const LIEngObject* self)
{
	if (self->sector == NULL)
		return 0;
	return 1;
}

/**
 * \brief Realizes or unrealizes the object.
 *
 * Unrealized objects don't affect the physics simulation and they
 * are, by default, invisible to clients and don't emit events.
 * Realized objects, on the other hand, contribute to the physics
 * simulation and emit events on state changes.
 *
 * Objects are created unrealized by the engine, but you can realize
 * them with this function if you want to attach them to the world
 * map. Likewise, you can unrealize them if you'd like to remove them
 * from the map.
 *
 * \param self Object.
 * \param value Nonzero if the object should be realized.
 * \return Nonzero on success.
 */
int lieng_object_set_realized (
	LIEngObject* self,
	int          value)
{
	LIMatTransform transform;

	if (value == lieng_object_get_realized (self))
		return 1;
	if (value)
	{
		/* Link to map. */
		lieng_object_get_transform (self, &transform);
		if (!private_warp (self, &transform.position))
			return 0;

		/* Invoke callbacks. */
		lical_callbacks_call (self->engine->callbacks, "object-visibility", lical_marshal_DATA_PTR_INT, self, 1);
	}
	else
	{
		/* Invoke callbacks. */
		lical_callbacks_call (self->engine->callbacks, "object-visibility", lical_marshal_DATA_PTR_INT, self, 0);

		/* Remove from map. */
		lialg_u32dic_remove (self->sector->objects, self->id);
		self->sector = NULL;
	}

	return 1;
}

LIEngSector* lieng_object_get_sector (
	LIEngObject* self)
{
	return self->sector;
}

/**
 * \brief Gets the world space transformation of the object.
 * \param self Object.
 * \param value Return location for the transformation.
 */
void lieng_object_get_transform (
	const LIEngObject* self,
	LIMatTransform*    value)
{
	*value = self->transform;
}

/**
 * \brief Sets the world space transformation of the object.
 * \param self Object.
 * \param value Transformation.
 * \return Nonzero on success.
 */
int lieng_object_set_transform (
	LIEngObject*          self,
	const LIMatTransform* value)
{
	int realized;

	/* Warp to new position. */
	realized = lieng_object_get_realized (self);
	if (realized)
	{
		if (!private_warp (self, &value->position))
			return 0;
	}
	self->transform = *value;

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, "object-transform", lical_marshal_DATA_PTR_PTR, self, value);

	return 1;
}

/*****************************************************************************/

static int private_warp (
	LIEngObject*       self,
	const LIMatVector* position)
{
	LIEngSector* dst;
	LIEngSector* src;

	/* Move between sectors. */
	src = self->sector;
	dst = lialg_sectors_data_point (self->engine->sectors, LIALG_SECTORS_CONTENT_ENGINE, position, 1);
	if (dst == NULL)
		return 0;
	if (src != dst)
	{
		if (lialg_u32dic_insert (dst->objects, self->id, self) == NULL)
			return 0;
		if (src != NULL)
			lialg_u32dic_remove (src->objects, self->id);
		self->sector = dst;
	}

	return 1;
}

/** @} */
/** @} */
