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

/*****************************************************************************/

LIPhyControl::LIPhyControl (LIPhyObject* object, btCollisionShape* shape) :
	object (object),
	contact_controller (NULL)
{
}

LIPhyControl::~LIPhyControl ()
{
}

void LIPhyControl::apply_impulse (const btVector3& pos, const btVector3& imp)
{
}

void LIPhyControl::transform (const btTransform& value)
{
}

void LIPhyControl::update ()
{
}

int LIPhyControl::get_activated ()
{
	return 0;
}

void LIPhyControl::set_activated (int value)
{
}

void LIPhyControl::get_angular (btVector3* value)
{
	*value = btVector3 (0.0, 0.0, 0.0);
}

void LIPhyControl::set_angular (const btVector3& value)
{
}

void LIPhyControl::set_collision_group (int mask)
{
}

void LIPhyControl::set_collision_mask (int mask)
{
}

void LIPhyControl::set_contacts (bool value)
{
}

void LIPhyControl::get_gravity (btVector3* value)
{
	*value = btVector3 (0.0, 0.0, 0.0);
}

void LIPhyControl::set_gravity (const btVector3& value)
{
}

bool LIPhyControl::get_ground ()
{
	return false;
}

void LIPhyControl::set_mass (float value, const btVector3& inertia)
{
}

btCollisionObject* LIPhyControl::get_object ()
{
	return NULL;
}

void LIPhyControl::get_velocity (btVector3* value)
{
	*value = btVector3 (0.0, 0.0, 0.0);
}

void LIPhyControl::set_velocity (const btVector3& value)
{
}

/** @} */
/** @} */
