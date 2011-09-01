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
 * \addtogroup LIPhyConstraint Constraint
 * @{
 */

#include "physics.h"
#include "physics-constraint.h"
#include "physics-object.h"
#include "physics-private.h"

/**
 * \brief Creates a hinge constraint.
 *
 * The constraint will be owned by the physics simulation. It will be freed
 * automatically when the object has been deleted or becomes incompatible with
 * the constraint.
 *
 * \param physics Physics simulation.
 * \param object Object to constrain.
 * \param point Pivot point in object space.
 * \param axis Axis of rotation in object space.
 * \param limit Nonzero if should use rotation limit.
 * \param limit_min Minimum angle in radians.
 * \param limit_max Maximum angle in radians.
 * \return New constraint or NULL.
 */
LIPhyConstraint*
liphy_constraint_new_hinge (LIPhyPhysics*      physics,
                            LIPhyObject*       object,
                            const LIMatVector* point,
                            const LIMatVector* axis,
                            int                limit,
                            float              limit_min,
                            float              limit_max)
{
	LIPhyConstraint* self;
	btCollisionObject* bobject;

	/* Check for collision object. */
	if (object->control == NULL)
	{
		lisys_error_set (EINVAL, "not realized");
		return NULL;
	}
	bobject = object->control->get_object ();
	if (bobject == NULL)
	{
		lisys_error_set (EINVAL, "not realized");
		return NULL;
	}

	/* Allocate self. */
	self = (LIPhyConstraint*) lisys_calloc (1, sizeof (LIPhyConstraint));
	if (self == NULL)
		return NULL;

	/* Calculate displacement. */
	/* Due to the center of mass transform of the collision shape, the
	   hinge may need to be displaced for it to appear in the right place. */
	LIMatVector ctr = object->center_of_mass;
	btVector3 disp = -btVector3 (ctr.x, ctr.y, ctr.z);

	/* Create hinge constraint. */
	btVector3 bpos (point->x, point->y, point->z);
	btVector3 baxis (axis->x, axis->y, axis->z);
	btHingeConstraint* constraint = new btHingeConstraint ((btRigidBody&) *bobject, bpos + disp, baxis);
	if (limit)
		constraint->setLimit (limit_min, limit_max);

	/* FIXME: Why do we get divide by zeros if we don't do this? */
	LIMatTransform t;
	liphy_object_get_transform (object, &t);
	liphy_object_set_transform (object, &t);

	/* Add to simulation. */
	if (!lialg_list_prepend (&physics->constraints, self))
	{
		delete constraint;
		lisys_free (self);
	}
	physics->dynamics->addConstraint (constraint);
	self->physics = physics;
	self->constraint = constraint;
	self->object0 = object;
	self->object1 = NULL;

	return self;
}

void
liphy_constraint_free (LIPhyConstraint* self)
{
	self->physics->dynamics->removeConstraint (self->constraint);
	delete self->constraint;
	lisys_free (self);
}

/** @} */
/** @} */
