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
 * \addtogroup LIPhy Physics
 * @{
 * \addtogroup LIPhyObject Object
 * @{
 */

#include "physics-object.h"
#include "physics-model.h"
#include "physics-private.h"

#define PRIVATE_REALIZED 0x0200

static LIPhyShape* private_get_shape (
	LIPhyObject* self);

static void private_update_state (
	LIPhyObject* self);

/*****************************************************************************/

/**
 * \brief Creates a new physics object.
 *
 * \param physics Physics system.
 * \param id Object ID.
 * \param model Physics model or NULL.
 * \param control_mode Simulation mode.
 * \return New object or NULL.
 */
LIPhyObject* liphy_object_new (
	LIPhyPhysics*    physics,
	uint32_t         id,
	LIPhyModel*      model,
	LIPhyControlMode control_mode)
{
	LIPhyObject* self;
	btVector3 position (0.0f, 0.0f, 0.0f);
	btQuaternion orientation (0.0f, 0.0f, 0.0f, 1.0f);

	self = (LIPhyObject*) lisys_calloc (1, sizeof (LIPhyObject));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->id = id;
	self->model = model;
	self->pointer.object = 1;
	self->pointer.pointer = self;
	self->control_mode = control_mode;
	self->config.friction_liquid = 0.5f;
	self->config.gravity = limat_vector_init (0.0f, -10.0f, 0.0f);
	self->config.gravity_liquid = limat_vector_init (0.0f, -2.0f, 0.0f);
	self->config.mass = 10.0f;
	self->config.speed = LIPHY_DEFAULT_SPEED;
	self->config.character_step = 0.35;
	self->config.collision_group = LIPHY_DEFAULT_COLLISION_GROUP;
	self->config.collision_mask = LIPHY_DEFAULT_COLLISION_MASK;
	self->shape_name = lisys_string_dup ("default");
	if (self->shape_name == NULL)
	{
		liphy_object_free (self);
		return NULL;
	}
	try
	{
		self->motion = new LIPhyMotionState (self, btTransform (orientation, position));
	}
	catch (...)
	{
		liphy_object_free (self);
		return NULL;
	}

	/* Add to dictionary. */
	if (self->id)
	{
		lisys_assert (!lialg_u32dic_find (physics->objects, id));
		if (!lialg_u32dic_insert (physics->objects, id, self))
		{
			liphy_object_free (self);
			return NULL;
		}
	}

	return self;
}

/**
 * \brief Frees the physics object.
 *
 * \param self Object.
 */
void liphy_object_free (
	LIPhyObject* self)
{
	/* Remove from dictionary. */
	if (self->id)
		lialg_u32dic_remove (self->physics->objects, self->id);

	/* Unrealize. */
	self->flags &= ~PRIVATE_REALIZED;
	private_update_state (self);

	/* Free self. */
	delete self->motion;
	lisys_free (self->shape_name);
	lisys_free (self);
}

/**
 * \brief Configures the object to move towards the given target.
 *
 * Configures the physics state of the object so that it heads directly
 * towards the specified target point.
 *
 * \param self Object.
 * \param target Target position vector.
 * \param speed Movement speed.
 * \param dist Tolerance in position for the goal check.
 * \return Nonzero if reached the goal.
 */
int liphy_object_approach (
	LIPhyObject*       self,
	const LIMatVector* target,
	float              speed,
	float              dist)
{
	float len;
	LIMatVector tmp;
	LIMatQuaternion dir;
	LIMatTransform transform;

	if (!liphy_object_get_realized (self))
		return 1;

	/* Get direction to target. */
	liphy_object_get_transform (self, &transform);
	tmp = limat_vector_subtract (*target, transform.position);
	tmp.y = 0.0f;
	len = limat_vector_get_length (tmp);

	/* Set look direction. */
	if (len > 0.1f)
	{
		dir = limat_quaternion_look (tmp, limat_vector_init (0.0f, 1.0f, 0.0f));
		transform.rotation = limat_quaternion_conjugate (dir);
		liphy_object_set_transform (self, &transform);
	}

	/* Move towards target. */
	if (len > dist)
	{
		liphy_object_set_movement (self, speed);
		return 0;
	}
	liphy_object_set_movement (self, 0.0f);

	return 1;
}

/**
 * \brief Performs a relative ray cast test.
 * \param self Object.
 * \param relsrc Sweep start point, in object space.
 * \param reldst Sweep end point, in object space.
 * \param result Return location for collision information.
 * \return Nonzero if hit something.
 */
int liphy_object_cast_ray (
	LIPhyObject*       self,
	const LIMatVector* relsrc,
	const LIMatVector* reldst,
	LIPhyCollision*    result)
{
	LIMatTransform transform;
	LIMatVector start;
	LIMatVector end;

	if (self->control == NULL)
		return 0;
	liphy_object_get_transform (self, &transform);
	start = limat_transform_transform (transform, *relsrc);
	end = limat_transform_transform (transform, *reldst);

	return liphy_physics_cast_ray (self->physics, &start, &end,
		btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter, &self, 1, result);
}

/**
 * \brief Performs a relative sphere sweep test.
 *
 * This function is a handy for things like melee attack checks since
 * you can place the sphere at the damaging point of the weapon and
 * project it to the direction of the attack.
 *
 * \param self Object.
 * \param relsrc Sweep start point, in object space.
 * \param reldst Sweep end point, in object space.
 * \param radius Radius of the swept sphere.
 * \param result Return location for collision information.
 * \return Nonzero if hit something.
 */
int liphy_object_cast_sphere (
	LIPhyObject*       self,
	const LIMatVector* relsrc,
	const LIMatVector* reldst,
	float              radius,
	LIPhyCollision*    result)
{
	LIMatTransform transform;
	LIMatVector start;
	LIMatVector end;

	if (self->control == NULL)
		return 0;
	liphy_object_get_transform (self, &transform);
	start = limat_transform_transform (transform, *relsrc);
	end = limat_transform_transform (transform, *reldst);

	return liphy_physics_cast_sphere (self->physics, &start, &end, radius,
		btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter, &self, 1, result);
}

/**
 * \brief Modifies the velocity of the object with an impulse.
 * \param self Object.
 * \param point Impulse point relative to the body.
 * \param impulse Impulse force.
 */
void liphy_object_impulse (
	LIPhyObject*       self,
	const LIMatVector* point,
	const LIMatVector* impulse)
{
	btVector3 pos (point->x, point->y, point->z);
	btVector3 imp (impulse->x, impulse->y, impulse->z);

	if (self->control != NULL)
		self->control->apply_impulse (pos, imp);
}

/**
 * \brief Adds a new wheel to a vehicle object.
 *
 * \param self Object.
 * \param point Chassis connection point.
 * \param axle Axle direction vector.
 * \param radius Wheel radius.
 * \param susplen Length of suspension spring.
 * \param turning Nonzero if the wheel is controllable.
 */
void liphy_object_insert_wheel (
	LIPhyObject*       self,
	const LIMatVector* point,
	const LIMatVector* axle,
	float              radius,
	float              susplen,
	int                turning)
{
#if 0
	/* FIXME */
	if (self->flags & PRIVATE_ADDED_VEHICLE)
	{
		btVector3 bpoint (point->x, point->y, point->z);
		btVector3 bdir (0.0f, -1.0f, 0.0f);
		btVector3 baxle (axle->x, axle->y, axle->z);
		btWheelInfo& wheel = self->vehicle->addWheel (bpoint, bdir, baxle, susplen, radius, *self->vehicle_tuning, turning);
		/* FIXME: Should be configurable. */
		wheel.m_suspensionStiffness = 20.0f;
		wheel.m_wheelsDampingRelaxation = 2.3f;
		wheel.m_wheelsDampingCompression = 4.4f;
		wheel.m_frictionSlip = 1000.0f;
		wheel.m_rollInfluence = 0.1f;
	}
#endif
}

/**
 * \brief Causes the object to jump.
 *
 * Does the same as #liphy_object_impulse with origin as the position vector.
 *
 * \param self Object.
 * \param impulse Jump force.
 */
void liphy_object_jump (
	LIPhyObject*       self,
	const LIMatVector* impulse)
{
	LIMatVector o = { 0.0f, 0.0f, 0.0f };

	liphy_object_impulse (self, &o, impulse);
}

/**
 * \brief Gets the activation state of the object.
 * \param self Object.
 * \return Activation state.
 */
int liphy_object_get_activated (
	LIPhyObject* self)
{
	if (self->control != NULL)
		return self->control->get_activated ();
	return 0;
}

/**
 * \brief Sets the activation state of the object.
 * \param self Object.
 * \param value Activation state.
 */
void liphy_object_set_activated (
	LIPhyObject* self,
	int          value)
{
	if (self->control != NULL)
		self->control->set_activated (value);
}

/**
 * \brief Gets the angular velocity of the object.
 * \param self Object.
 * \param value Return location for the angular velocity vector.
 */
void liphy_object_get_angular (
	const LIPhyObject* self,
	LIMatVector*       value)
{
	btVector3 velocity;

	if (self->control != NULL)
	{
		self->control->get_angular (&velocity);
		value->x = velocity[0];
		value->y = velocity[1];
		value->z = velocity[2];
	}
	else
	{
		value->x = 0.0f;
		value->y = 0.0f;
		value->z = 0.0f;
	}
}

/**
 * \brief Sets the angular velocity of the object.
 *
 * \param self Object.
 * \param value Angular velocity vector.
 */
void liphy_object_set_angular (
	LIPhyObject*       self,
	const LIMatVector* value)
{
	btVector3 velocity (value->x, value->y, value->z);

	self->config.angular = *value;
	if (self->control != NULL)
		self->control->set_angular (velocity);
}

/**
 * \brief Gets the local bounding box of the object.
 * \param self Object.
 * \param result Return location for the bounding box.
 */
void liphy_object_get_bounds (
	const LIPhyObject* self,
	LIMatAabb*         result)
{
	if (self->shape != NULL)
	{
		*result = self->shape->bounds;
		result->min = limat_vector_add (result->min, self->shape->center_of_mass);
		result->max = limat_vector_add (result->max, self->shape->center_of_mass);
	}
	else
		limat_aabb_init (result);
}

/**
 * \brief Gets the local bounding box of the object without center of mass applied.
 * \param self Object.
 * \param result Return location for the bounding box.
 */
void liphy_object_get_bounds_internal (
	const LIPhyObject* self,
	LIMatAabb*         result)
{
	if (self->shape != NULL)
		*result = self->shape->bounds;
	else
		limat_aabb_init (result);
}

/**
 * \brief Gets the collision group of the object.
 *
 * \param self Object.
 * \return Group mask.
 */
int liphy_object_get_collision_group (
	const LIPhyObject* self)
{
	return self->config.collision_group;
}

/**
 * \brief Sets the collision group of the object.
 *
 * Two objects can collide with each other if the mask of the first object has
 * common bits with the group of the second object and the mask of the second
 * object has common bits with the group of the first object.
 *
 * \param self Object.
 * \param mask Collision group mask.
 */
void liphy_object_set_collision_group (
	LIPhyObject* self,
	int          mask)
{
	self->config.collision_group = mask;
	if (self->control != NULL)
		self->control->set_collision_group (mask);
}

/**
 * \brief Gets the collision mask of the object.
 *
 * \param self Object.
 * \return Collision mask.
 */
int liphy_object_get_collision_mask (
	const LIPhyObject* self)
{
	return self->config.collision_mask;
}

/**
 * \brief Sets the collision mask of the object.
 *
 * Two objects can collide with each other if the mask of the first object has
 * common bits with the group of the second object and the mask of the second
 * object has common bits with the group of the first object.
 *
 * \param self Object.
 * \param mask Collision mask.
 */
void liphy_object_set_collision_mask (
	LIPhyObject* self,
	int          mask)
{
	self->config.collision_mask = mask;
	if (self->control != NULL)
		self->control->set_collision_mask (mask);
}

/**
 * \brief Gets the contact event generation status of the object.
 * \param self Object.
 * \return Nonzero if enabled, zero if disabled.
 */
int liphy_object_get_contact_events (
	LIPhyObject* self)
{
	return self->config.contact_events;
}

/**
 * \brief Enables or disables contect events for the object.
 * \param self Object.
 * \param value Contact callback.
 */
void liphy_object_set_contact_events (
	LIPhyObject* self,
	int          value)
{
	self->config.contact_events = value;
	if (self->control != NULL)
		self->control->set_contacts (value);
}

/**
 * \brief Gets the simulation mode of the object.
 *
 * \param self Object.
 * \return Simulation mode.
 */
LIPhyControlMode liphy_object_get_control_mode (
	const LIPhyObject* self)
{
	return self->control_mode;
}

/**
 * \brief Sets the simulation mode of the object.
 *
 * \param self Object.
 * \param value Simulation mode.
 */
void liphy_object_set_control_mode (
	LIPhyObject*     self,
	LIPhyControlMode value)
{
	if (self->control_mode == value)
		return;
	self->control_mode = value;
	private_update_state (self);
}

/**
 * \brief Gets the physics engine for which this object was created.
 * 
 * \param self Object.
 * \return Physics engine.
 */
LIPhyPhysics* liphy_object_get_engine (
	LIPhyObject* self)
{
	return self->physics;
}

/**
 * \brief Returns the friction coefficient of the object in liquid.
 * \param self Object.
 * \return Friction coefficient.
 */
float liphy_object_get_friction_liquid (
	const LIPhyObject* self)
{
	return self->config.friction_liquid;
}

/**
 * \brief Sets the friction coefficient of the object in liquid.
 * \param self Object.
 * \param value Friction coefficient.
 */
void liphy_object_set_friction_liquid (
	LIPhyObject* self,
	float        value)
{
	self->config.friction_liquid = value;
}

/**
 * \brief Returns the gravity acceleration vector of the object.
 * \param self Object.
 * \param value Return location for the gravity vector.
 */
void liphy_object_get_gravity (
	const LIPhyObject* self,
	LIMatVector*       value)
{
	*value = self->config.gravity;
}

/**
 * \brief Sets the gravity acceleration vector of the object.
 * \param self Object.
 * \param value Gravity vector.
 */
void liphy_object_set_gravity (
	LIPhyObject*       self,
	const LIMatVector* value)
{
	self->config.gravity = *value;
	if (self->control != NULL)
		self->control->set_gravity (btVector3 (value->x, value->y, value->z));
}

/**
 * \brief Returns the gravity acceleration vector of the object in liquid.
 * \param self Object.
 * \param value Return location for the gravity vector.
 */
void liphy_object_get_gravity_liquid (
	const LIPhyObject* self,
	LIMatVector*       value)
{
	*value = self->config.gravity_liquid;
}

/**
 * \brief Sets the gravity acceleration vector of the object in liquid.
 * \param self Object.
 * \param value Gravity vector.
 */
void liphy_object_set_gravity_liquid (
	LIPhyObject*       self,
	const LIMatVector* value)
{
	self->config.gravity_liquid = *value;
}

/**
 * \brief Returns nonzero if the object is standing on ground.
 *
 * This only works for character objects. Other types always return zero.
 *
 * \param self Object.
 * \return Nonzero if standing on ground.
 */
int liphy_object_get_ground (
	const LIPhyObject* self)
{
	if (self->control != NULL)
		return self->control->get_ground ();

	return 0;
}

/**
 * \brief Gets the inertia of the object.
 *
 * \param self Object.
 * \param result Return location for the inertia vector.
 */
void liphy_object_get_inertia (
	LIPhyObject* self,
	LIMatVector* result)
{
	LIPhyShape* shape;

	shape = private_get_shape (self);
	if (shape != NULL)
		liphy_shape_get_inertia (shape, self->config.mass, result);
	else
		*result = limat_vector_init (0.0f, 0.0f, 0.0f);
}

/**
 * \brief Gets the mass of the object.
 *
 * \param self Object.
 * \return Mass.
 */
float liphy_object_get_mass (
	const LIPhyObject* self)
{
	return self->config.mass;
}

/**
 * \brief Sets the mass of the object.
 *
 * \param self Object.
 * \param value Mass.
 */
void liphy_object_set_mass (
	LIPhyObject* self,
	float        value)
{
	LIMatVector v;
	btVector3 inertia(0.0, 0.0, 0.0);

	self->config.mass = value;
	if (self->control != NULL)
	{
		liphy_object_get_inertia (self, &v);
		inertia[0] = v.x;
		inertia[1] = v.y;
		inertia[2] = v.z;
		self->control->set_mass (value, inertia);
	}
}

LIPhyModel* liphy_object_get_model (
	LIPhyObject* self)
{
	return self->model;
}

void liphy_object_set_model (
	LIPhyObject* self,
	LIPhyModel*  model)
{
	if (model != self->model)
	{
		self->model = model;
		private_update_state (self);
	}
}

/**
 * \brief Gets the movement force of the object.
 *
 * \param self Object.
 * \return Movement force.
 */
float liphy_object_get_movement (
	const LIPhyObject* self)
{
	return self->config.movement;
}

/**
 * \brief Sets the movement force of the object.
 *
 * \param self Object.
 * \param value Movement force.
 */
void liphy_object_set_movement (
	LIPhyObject* self,
	float        value)
{
	self->config.movement = value;
#if 0
	if (self->control_mode == LIPHY_CONTROL_MODE_VEHICLE)
	{
		for (i = 0 ; i < self->vehicle->getNumWheels () ; i++)
		{
			btWheelInfo& wheel = self->vehicle->getWheelInfo (i);
			wheel.m_engineForce = value;
		}
	}
#endif
}

/**
 * \brief Returns nonzero if the object is a part of the physics simulation.
 *
 * \param self Object.
 * \return Nonzero if realized.
 */
int liphy_object_get_realized (
	const LIPhyObject* self)
{
	return (self->flags & PRIVATE_REALIZED) != 0;
}

/**
 * \brief Realizes or unrealizes the object.
 *
 * \param self Object.
 * \param value Nonzero if should realize.
 * \return Nonzero if realized.
 */
int liphy_object_set_realized (
	LIPhyObject* self,
	int          value)
{
	if ((value != 0) == ((self->flags & PRIVATE_REALIZED) != 0))
		return 1;
	if (value)
		self->flags |= PRIVATE_REALIZED;
	else
		self->flags &= ~PRIVATE_REALIZED;
	private_update_state (self);

	return 1;
}

/**
 * \brief Sets the angular factor of the object.
 * 
 * \param self Object.
 * \param value Angular factor
 */
void liphy_object_set_rotating (
	LIPhyObject* self,
	float        value)
{
#if 0
	if (self->body)
	{
		if (self->control_mode != LIPHY_CONTROL_MODE_STATIC)
			self->body->setAngularFactor (value);
	}
#endif
}

const char* liphy_object_get_shape (
	const LIPhyObject* self)
{
	return self->shape_name;
}

void liphy_object_set_shape (
	LIPhyObject* self,
	const char*  value)
{
	char* tmp;

	if (!strcmp (self->shape_name, value))
		return;
	tmp = lisys_string_dup (value);
	if (tmp != NULL)
	{
		lisys_free (self->shape_name);
		self->shape_name = tmp;
	}
	private_update_state (self);
}

/**
 * \brief Gets the movement speed of the object.
 *
 * \param self Object.
 * \return Movement speed.
 */
float liphy_object_get_speed (
	const LIPhyObject* self)
{
	return self->config.speed;
}

/**
 * \brief Sets the movement speed of the object.
 *
 * \param self Object.
 * \param value Movement speed.
 */
void liphy_object_set_speed (
	LIPhyObject* self,
	float        value)
{
	self->config.speed = value;
}

/**
 * \brief Gets the strafing force of the object.
 *
 * \param self Object.
 * \return Strafing force.
 */
float liphy_object_get_strafing (
	const LIPhyObject* self)
{
	return self->config.strafing;
}

/**
 * \brief Sets the strafing force of the object.
 *
 * \param self Object.
 * \param value Strafing force.
 */
void liphy_object_set_strafing (
	LIPhyObject* self,
	float        value)
{
	self->config.strafing = value;
}

/**
 * \brief Gets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Return location for the transformation.
 */
void liphy_object_get_transform (
	const LIPhyObject* self,
	LIMatTransform*    value)
{
	btTransform trans;
	btQuaternion rotation;
	btVector3 position;

	/* Get the raw transformation. */
	self->motion->getWorldTransform (trans);

	/* Calculate displacement. */
	/* Due to the center of mass transform of the collision shape, the
	   object may need to displaced for it to appear in the right place. */
	if (self->shape != NULL)
	{
		LIMatVector ctr = self->center_of_mass;
		btVector3 disp = btVector3 (ctr.x, ctr.y, ctr.z);
		trans = trans * btTransform (btQuaternion::getIdentity (), disp).inverse();
	}

	rotation = trans.getRotation ();
	position = trans.getOrigin ();
	value->position.x = position[0];
	value->position.y = position[1];
	value->position.z = position[2];
	value->rotation.x = rotation[0];
	value->rotation.y = rotation[1];
	value->rotation.z = rotation[2];
	value->rotation.w = rotation[3];
}

/**
 * \brief Sets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Quaternion.
 */
void liphy_object_set_transform (
	LIPhyObject*          self,
	const LIMatTransform* value)
{
	btVector3 origin (value->position.x, value->position.y, value->position.z);
	btQuaternion rotation (value->rotation.x, value->rotation.y, value->rotation.z, value->rotation.w);
	btTransform transform (rotation, origin);

	/* Calculate displacement. */
	/* Due to the center of mass transform of the collision shape, the
	   object may need to be displaced for it to appear in the right place. */
	if (self->shape != NULL)
	{
		LIMatVector ctr = self->center_of_mass;
		btVector3 disp = btVector3 (ctr.x, ctr.y, ctr.z);
		transform = transform * btTransform (btQuaternion::getIdentity (), disp);
	}

	/* Update the transformation of the body. */
	if (self->control != NULL)
		self->control->transform (transform);
	else
		self->motion->setWorldTransform (transform);
}

/**
 * \brief Gets the user pointer stored to the object.
 *
 * \param self Object.
 * \return User pointer.
 */
void* liphy_object_get_userdata (
	LIPhyObject* self)
{
	return self->config.userdata;
}

/**
 * \brief Stores a user pointer to the object.
 *
 * \param self Object.
 * \param value User pointer.
 */
void liphy_object_set_userdata (
	LIPhyObject* self,
	void*        value)
{
	self->config.userdata = value;
}

/**
 * \brief Gets the current linear velocity of the object.
 *
 * \param self Object.
 * \param value Return location for the velocity.
 */
void liphy_object_get_velocity (
	LIPhyObject* self,
	LIMatVector* value)
{
	btVector3 velocity;

	if (self->control != NULL)
	{
		self->control->get_velocity (&velocity);
		*value = limat_vector_init (velocity[0], velocity[1], velocity[2]);
	}
	else
		*value = self->config.velocity;
}

/**
 * \brief Sets the linear velocity of the object.
 *
 * \param self Object.
 * \param value Linear velocity vector.
 */
void liphy_object_set_velocity (
	LIPhyObject*       self,
	const LIMatVector* value)
{
	btVector3 velocity (value->x, value->y, value->z);

	self->config.velocity = *value;
	if (self->control != NULL)
		self->control->set_velocity (velocity);
}

/*****************************************************************************/

static LIPhyShape* private_get_shape (
	LIPhyObject* self)
{
	LIPhyShape* shape;

	if (self->model == NULL)
		return NULL;
	shape = liphy_model_find_shape (self->model, self->shape_name);
	if (shape != NULL)
		return shape;
	shape = liphy_model_find_shape (self->model, "default");

	return shape;
}

static void private_update_state (
	LIPhyObject* self)
{
	btCollisionShape* shape;
	LIMatTransform transform;
	LIPhyShape* shape_;

	/* Bullet can't handle objects being deleted during the simulation tick.
	   To avoid problems, the code should never get here during it. */
	assert (!self->physics->updating);

	/* Remove all constraints involving us. */
	liphy_physics_clear_constraints (self->physics, self);
	liphy_object_get_transform (self, &transform);

	/* Remove old controller. */
	if (self->control != NULL)
	{
		delete self->control;
		self->control = NULL;
	}
	self->shape = NULL;

	/* Create new controller. */
	if (self->flags & PRIVATE_REALIZED)
	{
		shape_ = private_get_shape (self);
		if (shape_ == NULL)
			return;
		shape = shape_->shape;
		self->shape = shape_;
		switch (self->control_mode)
		{
			case LIPHY_CONTROL_MODE_NONE:
				break;
			case LIPHY_CONTROL_MODE_CHARACTER:
				self->control = new LIPhyCharacterControl (self, shape);
				break;
			case LIPHY_CONTROL_MODE_RIGID:
				self->control = new LIPhyControlRigid (self, shape);
				break;
			case LIPHY_CONTROL_MODE_STATIC:
				self->control = new LIPhyControlStatic (self, shape);
				break;
			case LIPHY_CONTROL_MODE_VEHICLE:
				self->control = new LIPhyControlVehicle (self, shape);
				break;
			default:
				lisys_assert (0);
				break;
		}
		if (self->control != NULL)
		{
			self->center_of_mass = self->shape->center_of_mass;
			self->control->set_contacts (self->config.contact_events);
			liphy_object_set_transform (self, &transform);
		}
	}
}

/** @} */
/** @} */
