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

LIPhyControlStatic::LIPhyControlStatic (LIPhyObject* object, btCollisionShape* shape) :
	LIPhyControl (object, shape),
	body (0.0, object->motion, shape, btVector3 (0.0, 0.0, 0.0))
{
	this->body.setUserPointer (&object->pointer);
	this->object->physics->dynamics->addCollisionObject (&this->body,
		this->object->config.collision_group,
		this->object->config.collision_mask);
}

LIPhyControlStatic::~LIPhyControlStatic ()
{
	this->object->physics->dynamics->removeCollisionObject (&this->body);
}

void LIPhyControlStatic::transform (const btTransform& value)
{
	this->object->physics->dynamics->removeRigidBody (&this->body);
	this->body.setCenterOfMassTransform (value);
	this->object->motion->setWorldTransform (value);
	this->object->physics->dynamics->addRigidBody (&this->body,
		this->object->config.collision_group,
		this->object->config.collision_mask);
}

void LIPhyControlStatic::set_contacts (bool value)
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

btCollisionObject* LIPhyControlStatic::get_object ()
{
	return &this->body;
}

/** @} */
/** @} */
