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

LIPhyMotionState::LIPhyMotionState (
	LIPhyObject*       object,
	const btTransform& transform)
{
	this->object = object;
	this->current = transform;
	this->previous = transform;
}

void LIPhyMotionState::getWorldTransform (
	btTransform& transform) const
{
	transform = this->current;
}

void LIPhyMotionState::setWorldTransform (
	const btTransform& transform)
{
	this->current = transform;
	if (this->object->control != NULL)
	{
		this->previous = this->current;
		this->object->control->update ();
		lical_callbacks_call (this->object->physics->callbacks, "physics-transform", lical_marshal_DATA_PTR, this->object);
	}
}

/** @} */
/** @} */
