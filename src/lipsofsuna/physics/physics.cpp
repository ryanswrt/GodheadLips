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
 * \addtogroup LIPhy Physics
 * @{
 * \addtogroup LIPhyPhysics Physics
 * @{
 */

#include "lipsofsuna/system.h"
#include "physics.h"
#include "physics-collision-configuration.hpp"
#include "physics-constraint.h"
#include "physics-model.h"
#include "physics-object.h"
#include "physics-private.h"
#include "physics-terrain.h"

#define MAXPROXIES 1000000

static bool private_contact_processed (
	btManifoldPoint& point,
	void*            body0,
	void*            body1);

static void private_internal_tick (
	btDynamicsWorld* dynamics,
	btScalar         step);

/*****************************************************************************/

/**
 * \brief Creates a new physics simulation.
 * \param callbacks Callback manager.
 * \return New physics simulation or NULL.
 */
LIPhyPhysics* liphy_physics_new (
	LICalCallbacks* callbacks)
{
	LIPhyPhysics* self;
	btVector3 vmin (-65535, -65535, -65535);
	btVector3 vmax ( 65535,  65535,  65535);

	/* Allocate self. */
	self = (LIPhyPhysics*) lisys_calloc (1, sizeof (LIPhyPhysics));
	if (self == NULL)
		return NULL;
	self->callbacks = callbacks;

	/* Allocation dictionaries. */
	self->models = lialg_u32dic_new ();
	if (self->models == NULL)
	{
		liphy_physics_free (self);
		return NULL;
	}
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		liphy_physics_free (self);
		return NULL;
	}

	/* Create simulation. */
	try
	{
		self->ghostcallback = new btGhostPairCallback ();
#ifdef LIPHY_BROADPHASE_DBVT
		self->broadphase = new btDbvtBroadphase ();
#else
		self->broadphase = new bt32BitAxisSweep3 (vmin, vmax, MAXPROXIES);
#endif
		self->broadphase->getOverlappingPairCache ()->setInternalGhostPairCallback (self->ghostcallback);
		self->configuration = new LIPhyCollisionConfiguration ();
		self->dispatcher = new btCollisionDispatcher (self->configuration);
		self->solver = new btSequentialImpulseConstraintSolver ();
		self->dynamics = new LIPhyDynamicsWorld (self->dispatcher, self->broadphase, self->solver, self->configuration);
		self->dynamics->setInternalTickCallback (private_internal_tick, self);
	}
	catch (...)
	{
		liphy_physics_free (self);
		return NULL;
	}

	extern ContactProcessedCallback gContactProcessedCallback;
	gContactProcessedCallback = private_contact_processed;

	return self;
}

/**
 * \brief Frees the physics simulation.
 * \param self Physics simulation.
 */
void liphy_physics_free (
	LIPhyPhysics* self)
{
	LIAlgU32dicIter iter;

	lisys_assert (self->constraints == NULL);

	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
			liphy_object_free ((LIPhyObject*) iter.value);
	}
	if (self->models != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->models)
			liphy_model_free ((LIPhyModel*) iter.value);
	}

	delete self->dynamics;
	delete self->solver;
	delete self->configuration;
	delete self->dispatcher;
	delete self->broadphase;
	delete self->ghostcallback;
	lialg_u32dic_free (self->models);
	lialg_u32dic_free (self->objects);
	lialg_list_free (self->controllers);
	lisys_free (self);
}

/**
 * \brief Casts a ray to the scene.
 *
 * If the ray hits no obstacle, result is set to the ray end point, normal is
 * set to ray direction, and zero is returned. Otherwise, nonzero is returned
 * and result and normal hold information on the collision point.
 *
 * \param self Physics simulation.
 * \param start Ray start point.
 * \param end Ray end point.
 * \param group Collision group.
 * \param mask Collision mask.
 * \param ignore_array Array of ignored objects.
 * \param ignore_count Number of ignored objects.
 * \param result Return location for collision data or NULL.
 * \return Nonzero if a collision occurred.
 */
int liphy_physics_cast_ray (
	const LIPhyPhysics* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	int                 group,
	int                 mask,
	LIPhyObject**       ignore_array,
	int                 ignore_count,
	LIPhyCollision*     result)
{
	btVector3 src (start->x, start->y, start->z);
	btVector3 dst (end->x, end->y, end->z);
	btCollisionWorld* collision = self->dynamics->getCollisionWorld ();

	/* Cast the ray. */
	LIPhyRaycastWorld test (ignore_array, ignore_count, src, dst);
	test.m_closestHitFraction = 1.0f;
	test.m_collisionFilterGroup = group;
	test.m_collisionFilterMask = mask;
	collision->rayTest (src, dst, test);
	if (test.m_closestHitFraction >= 1.0f)
		return 0;

	/* Inspect results. */
	if (result != NULL)
	{
		result->fraction = test.m_closestHitFraction;
		result->point = limat_vector_init (test.m_hitPointWorld[0], test.m_hitPointWorld[1], test.m_hitPointWorld[2]);
		result->normal = limat_vector_init (test.m_hitNormalWorld[0], test.m_hitNormalWorld[1], test.m_hitNormalWorld[2]);
		result->object = test.object;
		result->terrain = test.terrain;
		result->terrain_tile[0] = test.terrain_tile[0];
		result->terrain_tile[1] = test.terrain_tile[1];
		result->terrain_tile[2] = test.terrain_tile[2];
	}

	return 1;
}

/**
 * \brief Casts a shape to the scene.
 *
 * If the shape hits no obstacle, result is set to the cast end point, normal is
 * set to cast direction, and zero is returned. Otherwise, nonzero is returned
 * and result and normal hold information on the collision point.
 *
 * \param self Physics simulation.
 * \param start Cast start transform.
 * \param end Cast end transform.
 * \param shape Cast shape.
 * \param group Collision group.
 * \param mask Collision mask.
 * \param ignore_array Array of ignored objects.
 * \param ignore_count Number of ignore objects.
 * \param result Return location for collision data.
 * \return Nonzero if a collision occurred.
 */
int liphy_physics_cast_shape (
	const LIPhyPhysics*   self,
	const LIMatTransform* start,
	const LIMatTransform* end,
	const LIPhyShape*     shape,
	int                   group,
	int                   mask,
	LIPhyObject**         ignore_array,
	int                   ignore_count,
	LIPhyCollision*       result)
{
	int i;
	float best;
	btCollisionWorld* collision;
	btConvexShape* btshape;
	LIPhyConvexcastWorld test (ignore_array, ignore_count);
	btTransform btstart (
		btQuaternion (start->rotation.x, start->rotation.y, start->rotation.z, start->rotation.w),
		btVector3 (start->position.x, start->position.y, start->position.z));
	btTransform btend (
		btQuaternion (end->rotation.x, end->rotation.y, end->rotation.z, end->rotation.w),
		btVector3 (end->position.x, end->position.y, end->position.z));

	/* Initialize sweep. */
	test.m_closestHitFraction = 1.0f;
	test.m_collisionFilterGroup = group;
	test.m_collisionFilterMask = mask;
	collision = self->dynamics->getCollisionWorld ();
	result->fraction = best = 1.0f;

	/* Sweep the shape. */
	/* TODO: Compound shape support when the shape class supports them. */
	for (i = 0 ; i < shape->shape->getNumChildShapes () ; i++)
	{
		btshape = (btConvexShape*) shape->shape->getChildShape (i);
		collision->convexSweepTest (btshape, btstart, btend, test);
		if (test.m_closestHitFraction <= best && test.m_hitCollisionObject != NULL)
		{
			best = test.m_closestHitFraction;
			result->fraction = test.m_closestHitFraction;
			result->point = limat_vector_init (test.m_hitPointWorld[0], test.m_hitPointWorld[1], test.m_hitPointWorld[2]);
			result->normal = limat_vector_init (test.m_hitNormalWorld[0], test.m_hitNormalWorld[1], test.m_hitNormalWorld[2]);
			result->object = test.object;
			result->terrain = test.terrain;
			result->terrain_tile[0] = test.terrain_tile[0];
			result->terrain_tile[1] = test.terrain_tile[1];
			result->terrain_tile[2] = test.terrain_tile[2];
		}
	}

	return result->fraction < 1.0f;
}

/**
 * \brief Casts a sphere to the scene.
 *
 * If the sphere hits no obstacle, result is set to the cast end point, normal is
 * set to cast direction, and zero is returned. Otherwise, nonzero is returned
 * and result and normal hold information on the collision point.
 *
 * \param self Physics simulation.
 * \param start Cast start point.
 * \param end Cast end point.
 * \param radius Sphere radius.
 * \param group Collision group.
 * \param mask Collision mask.
 * \param ignore_array Array of ignored objects.
 * \param ignore_count Number of ignore objects.
 * \param result Return location for collision data.
 * \return Nonzero if a collision occurred.
 */
int liphy_physics_cast_sphere (
	const LIPhyPhysics* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	float               radius,
	int                 group,
	int                 mask,
	LIPhyObject**       ignore_array,
	int                 ignore_count,
	LIPhyCollision*     result)
{
	btCollisionWorld* collision;
	btTransform btstart (btQuaternion (0.0, 0.0, 0.0, 1.0), btVector3 (start->x, start->y, start->z));
	btTransform btend (btQuaternion (0.0, 0.0, 0.0, 1.0), btVector3 (end->x, end->y, end->z));
	btSphereShape shape (radius);
	LIPhyPointer* pointer;

	/* Initialize sweep. */
	LIPhyConvexcastWorld test (ignore_array, ignore_count);
	test.m_closestHitFraction = 1.0f;
	test.m_collisionFilterGroup = group;
	test.m_collisionFilterMask = mask;

	/* Sweep the shape. */
	collision = self->dynamics->getCollisionWorld ();
	collision->convexSweepTest (&shape, btstart, btend, test);
	if (test.m_hitCollisionObject != NULL && test.m_closestHitFraction < 1.0f)
	{
		result->fraction = test.m_closestHitFraction;
		result->normal.x = test.m_hitNormalWorld[0];
		result->normal.y = test.m_hitNormalWorld[1];
		result->normal.z = test.m_hitNormalWorld[2];
		result->point.x = test.m_hitPointWorld[0];
		result->point.y = test.m_hitPointWorld[1];
		result->point.z = test.m_hitPointWorld[2];
		pointer = (LIPhyPointer*) test.m_hitCollisionObject->getUserPointer ();
		if (pointer->object)
		{
			result->object = (LIPhyObject*) pointer->pointer;
			result->terrain = NULL;
			result->terrain_tile[0] = 0;
			result->terrain_tile[1] = 0;
			result->terrain_tile[2] = 0;
		}
		else
		{
			result->object = NULL;
			result->terrain = (LIPhyTerrain*) pointer->pointer;
			result->terrain_tile[0] = pointer->tile[0];
			result->terrain_tile[1] = pointer->tile[1];
			result->terrain_tile[2] = pointer->tile[2];
			liphy_terrain_cast_sphere (result->terrain, start, end, radius, result);
		}
		return 1;
	}

	return 0;
}

/**
 * \brief Clears all constraints affecting the object.
 *
 * Used by the object class to remove invalid constraints.
 *
 * \param self Physics.
 * \param object Object.
 */
void liphy_physics_clear_constraints (
		LIPhyPhysics* self,
		LIPhyObject*  object)
{
	LIAlgList* ptr;
	LIAlgList* next;
	LIPhyConstraint* constraint;

	for (ptr = self->constraints ; ptr != NULL ; ptr = next)
	{
		next = ptr->next;
		constraint = (LIPhyConstraint*) ptr->data;
		if (constraint->object0 == object || constraint->object1 == object)
		{
			lialg_list_remove (&self->constraints, ptr);
			liphy_constraint_free (constraint);
		}
	}
}

/**
 * \brief Finds a physics model by ID.
 * \param self Physics simulation.
 * \param id Model ID.
 * \return Model or NULL.
 */
LIPhyModel* liphy_physics_find_model (
	LIPhyPhysics* self,
	uint32_t      id)
{
	return (LIPhyModel*) lialg_u32dic_find (self->models, id);
}

/**
 * \brief Finds a physics object by ID.
 * \param self Physics simulation.
 * \param id Object ID.
 * \return Object or NULL.
 */
LIPhyObject* liphy_physics_find_object (
	LIPhyPhysics* self,
	uint32_t      id)
{
	return (LIPhyObject*) lialg_u32dic_find (self->objects, id);
}

/**
 * \brief Removes the model from all objects.
 * \param self Physics simulation.
 * \param model Physics model.
 */
void liphy_physics_remove_model (
	LIPhyPhysics* self,
	LIPhyModel*   model)
{
	LIAlgU32dicIter iter;
	LIPhyObject* object;

	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		object = (LIPhyObject*) iter.value;
		if (object->model == model)
			liphy_object_set_model (object, NULL);
	}
}

/**
 * \brief Updates the physics simulation.
 * \param self Physics simulation.
 * \param secs Tick length in seconds.
 */
void liphy_physics_update (
	LIPhyPhysics* self,
	float         secs)
{
	/* Step simulation. */
	self->updating = 1;
	self->dynamics->stepSimulation (secs, 20);
	self->updating = 0;
}

/**
 * \brief Gets the userdata of the physics engine.
 * \param self Physics simulation.
 * \return Userdata.
 */
void* liphy_physics_get_userdata (
	LIPhyPhysics* self)
{
	return self->userdata;
}

/**
 * \brief Sets the userdata of the physics engine.
 * \param self Physics simulation.
 * \param data Userdata.
 */
void liphy_physics_set_userdata (
	LIPhyPhysics* self,
	void*         data)
{
	self->userdata = data;
}

/*****************************************************************************/

static bool private_contact_processed (
	btManifoldPoint& point,
	void*            body0,
	void*            body1)
{
	LIMatVector momentum0;
	LIMatVector momentum1;
	LIPhyObject* object;
	LIPhyPhysics* physics = NULL;
	LIPhyPointer* pointer0;
	LIPhyPointer* pointer1;
	LIPhyContact contact;

	/* Get contact information. */
	memset (&contact, 0, sizeof (LIPhyContact));
	pointer0 = (LIPhyPointer*)((btCollisionObject*) body0)->getUserPointer ();
	pointer1 = (LIPhyPointer*)((btCollisionObject*) body1)->getUserPointer ();
	if (pointer0->object)
	{
		contact.object0 = (LIPhyObject*) pointer0->pointer;
		physics = contact.object0->physics;
	}
	else
	{
		contact.terrain = (LIPhyTerrain*) pointer0->pointer;
		memcpy (contact.terrain_tile, pointer0->tile, 3 * sizeof (int));
	}
	if (pointer1->object)
	{
		contact.object1 = (LIPhyObject*) pointer1->pointer;
		physics = contact.object1->physics;
	}
	else
	{
		contact.terrain = (LIPhyTerrain*) pointer1->pointer;
		memcpy (contact.terrain_tile, pointer1->tile, 3 * sizeof (int));
	}

	/* Make sure that the contact involved at least one object with a contact
	   callback before inserting the contact to the queue. */
	if ((contact.object0 == NULL || !contact.object0->config.contact_events) &&
	    (contact.object1 == NULL || !contact.object1->config.contact_events))
		return false;

	/* Get collision point. */
	const btVector3& pt = point.getPositionWorldOnB ();
	const btVector3& nm = point.m_normalWorldOnB;
	contact.point = limat_vector_init (pt[0], pt[1], pt[2]);
	contact.normal = limat_vector_init (nm[0], nm[1], nm[2]);

	/* Calculate impulse. */
	/* FIXME: This is sloppy. */
	if (contact.object0 != NULL)
	{
		liphy_object_get_velocity (contact.object0, &momentum0);
		momentum0 = limat_vector_multiply (momentum0, liphy_object_get_mass (contact.object0));
	}
	else
		momentum1 = limat_vector_init (0.0f, 0.0f, 0.0f);
	if (contact.object1 != NULL)
	{
		liphy_object_get_velocity (contact.object1, &momentum1);
		momentum1 = limat_vector_multiply (momentum1, liphy_object_get_mass (contact.object1));
	}
	else
		momentum1 = limat_vector_init (0.0f, 0.0f, 0.0f);
	contact.impulse = limat_vector_get_length (limat_vector_subtract (momentum0, momentum1));

	/* Invoke callbacks. */
	if (contact.object0 != NULL && contact.object0->config.contact_events)
		lical_callbacks_call (physics->callbacks, "object-contact", lical_marshal_DATA_PTR, &contact);
	if (contact.object1 != NULL && contact.object1->config.contact_events)
	{
		object = contact.object0;
		contact.object0 = contact.object1;
		contact.object1 = object;
		lical_callbacks_call (physics->callbacks, "object-contact", lical_marshal_DATA_PTR, &contact);
	}

	return false;
}

static void private_internal_tick (
	btDynamicsWorld* dynamics,
	btScalar         step)
{
}

/** @} */
/** @} */
