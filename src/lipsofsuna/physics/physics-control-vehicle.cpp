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
 * \addtogroup LIPhyPrivate Private
 * @{
 */

#include "physics-object.h"
#include "physics-private.h"

LIPhyControlVehicle::LIPhyControlVehicle (LIPhyObject* object, btCollisionShape* shape) :
	LIPhyControl (object, shape),
	body (object->config.mass, object->motion, shape, btVector3 (0.0, 0.0, 0.0)),
	caster (object->physics->dynamics)
{
	LIMatVector v;
	btVector3 angular (object->config.angular.x, object->config.angular.y, object->config.angular.z);
	btVector3 velocity (object->config.velocity.x, object->config.velocity.y, object->config.velocity.z);

	liphy_object_get_inertia (object, &v);
	this->body.setMassProps (object->config.mass, btVector3 (v.x, v.y, v.z));
	this->body.setUserPointer (&object->pointer);
	this->body.setLinearVelocity (velocity);
	this->body.setAngularVelocity (angular);
	this->body.setActivationState (DISABLE_DEACTIVATION);
	this->body.setCcdMotionThreshold (PRIVATE_CCD_MOTION_THRESHOLD);
	this->vehicle = new btRaycastVehicle (this->tuning, &this->body, &this->caster);
	this->vehicle->setCoordinateSystem (0, 1, 2);
	this->object->physics->dynamics->addVehicle (this->vehicle);
}

LIPhyControlVehicle::~LIPhyControlVehicle ()
{
	this->object->physics->dynamics->removeVehicle (this->vehicle);
	delete this->vehicle;
}

void LIPhyControlVehicle::transform (const btTransform& value)
{
	int i;

	this->body.setCenterOfMassTransform (value);
	this->object->motion->setWorldTransform (value);
	this->vehicle->resetSuspension ();
	for (i = 0 ; i < this->vehicle->getNumWheels () ; i++)
		this->vehicle->updateWheelTransform (i, true);
}

void LIPhyControlVehicle::update ()
{
	const btVector3& velocity = this->body.getLinearVelocity ();

	this->object->config.velocity = limat_vector_init (velocity[0], velocity[1], velocity[2]);
}

void LIPhyControlVehicle::set_collision_group (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->body.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterGroup = mask;
}

void LIPhyControlVehicle::set_collision_mask (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->body.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterMask = mask;
}

void LIPhyControlVehicle::set_contacts (bool value)
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

void LIPhyControlVehicle::get_gravity (btVector3* value)
{
	*value = this->body.getGravity ();
}

void LIPhyControlVehicle::set_gravity (const btVector3& value)
{
	this->body.setGravity (value);
}

void LIPhyControlVehicle::set_mass (float value, const btVector3& inertia)
{
	this->body.setMassProps (value, inertia);
}

btCollisionObject* LIPhyControlVehicle::get_object ()
{
	return &this->body;
}

void LIPhyControlVehicle::set_velocity (const btVector3& value)
{
	this->body.setLinearVelocity (value);
}

/** @} */
/** @} */
