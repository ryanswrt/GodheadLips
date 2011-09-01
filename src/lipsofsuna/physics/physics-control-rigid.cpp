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
 * \addtogroup LIPhyPrivate Private
 * @{
 */

#include "physics-object.h"
#include "physics-private.h"

LIPhyControlRigid::LIPhyControlRigid (LIPhyObject* object, btCollisionShape* shape) :
	LIPhyControl (object, shape),
	body (object->config.mass, object->motion, shape, btVector3 (0.0, 0.0, 0.0))
{
	LIMatVector v;
	btVector3 angular (object->config.angular.x, object->config.angular.y, object->config.angular.z);
	btVector3 gravity (object->config.gravity.x, object->config.gravity.y, object->config.gravity.z);
	btVector3 velocity (object->config.velocity.x, object->config.velocity.y, object->config.velocity.z);

	this->body.setUserPointer (&object->pointer);
	this->body.setLinearVelocity (velocity);
	this->body.setAngularVelocity (angular);
	this->body.setCcdMotionThreshold (PRIVATE_CCD_MOTION_THRESHOLD);
	this->object->physics->dynamics->addRigidBody (&this->body,
		this->object->config.collision_group,
		this->object->config.collision_mask);
	liphy_object_get_inertia (object, &v);
	this->body.setMassProps (object->config.mass, btVector3 (v.x, v.y, v.z));
	this->body.setGravity (gravity);
}

LIPhyControlRigid::~LIPhyControlRigid ()
{
	this->object->physics->dynamics->removeRigidBody (&this->body);
}

void LIPhyControlRigid::apply_impulse (const btVector3& pos, const btVector3& imp)
{
	this->body.activate ();
	this->body.applyImpulse (imp, pos);
}

void LIPhyControlRigid::transform (const btTransform& value)
{
	this->body.setCenterOfMassTransform (value);
	this->object->motion->setWorldTransform (value);
}

void LIPhyControlRigid::update ()
{
	const btVector3& velocity = this->body.getLinearVelocity ();

	this->object->config.velocity = limat_vector_init (velocity[0], velocity[1], velocity[2]);
}

int LIPhyControlRigid::get_activated ()
{
	return this->body.getActivationState () == ACTIVE_TAG;
}

void LIPhyControlRigid::set_activated (int value)
{
	if (value)
		this->body.activate (true);
	else
		this->body.setActivationState (ISLAND_SLEEPING);
}

void LIPhyControlRigid::get_angular (btVector3* value)
{
	*value = this->body.getAngularVelocity ();
}

void LIPhyControlRigid::set_angular (const btVector3& value)
{
	this->body.setAngularVelocity (value);
}

void LIPhyControlRigid::set_collision_group (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->body.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterGroup = mask;
}

void LIPhyControlRigid::set_collision_mask (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->body.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterMask = mask;
}

void LIPhyControlRigid::set_contacts (bool value)
{
	if (value)
	{
		this->body.setCollisionFlags (this->body.getCollisionFlags() |
			btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
	else
	{
		this->body.setCollisionFlags (this->body.getCollisionFlags() &
			~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

void LIPhyControlRigid::get_gravity (btVector3* value)
{
	*value = this->body.getGravity ();
}

void LIPhyControlRigid::set_gravity (const btVector3& value)
{
	this->body.setGravity (value);
}

void LIPhyControlRigid::set_mass (float value, const btVector3& inertia)
{
	this->body.setMassProps (value, inertia);
}

btCollisionObject* LIPhyControlRigid::get_object ()
{
	return &this->body;
}

void LIPhyControlRigid::get_velocity (btVector3* value)
{
	*value = this->body.getLinearVelocity ();
}

void LIPhyControlRigid::set_velocity (const btVector3& value)
{
	this->body.setLinearVelocity (value);
}

/** @} */
/** @} */
