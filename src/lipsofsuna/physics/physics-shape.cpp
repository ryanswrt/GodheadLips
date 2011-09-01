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
 * \addtogroup LIPhyShape Shape
 * @{
 */

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <lipsofsuna/system.h>
#include "physics-shape.h"
#include "physics-private.h"

#define VERTEX_WELD_EPSILON 0.05

static int private_add_shape (
	LIPhyShape*           self,
	btConvexShape*        shape,
	const LIMatTransform* transform);

static btConvexHullShape* private_create_convex (
	LIPhyShape* self,
	btScalar*   vertices,
	int         count);

static btConvexHullShape* private_create_from_box (
	LIPhyShape*      self,
	const LIMatAabb* aabb);

static btConvexHullShape* private_create_from_empty (
	LIPhyShape* self);

static btConvexHullShape* private_create_from_vertices (
	LIPhyShape*        self,
	const LIMatVector* vertices,
	int                count,
	float              scale);

/*****************************************************************************/

/**
 * \brief Creates an empty collision shape.
 * \param physics Physics engine.
 * \return New collision shape or NULL.
 */
LIPhyShape* liphy_shape_new (
	LIPhyPhysics* physics)
{
	LIPhyShape* self;

	self = (LIPhyShape*) lisys_calloc (1, sizeof (LIPhyShape));
	if (self == NULL)
		return NULL;
	try
	{
		self->physics = physics;
		self->shape = new btCompoundShape (false);
		self->shape->setUserPointer (self);
	}
	catch (...)
	{
		lisys_error_set (ENOMEM, NULL);
		liphy_shape_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the collision shape.
 * \param self Collision shape.
 */
void liphy_shape_free (
	LIPhyShape* self)
{
	if (self->shape != NULL)
	{
		liphy_shape_clear (self);
		delete self->shape;
	}
	lisys_free (self);
}

int liphy_shape_add_aabb (
	LIPhyShape*           self,
	const LIMatAabb*      aabb,
	const LIMatTransform* transform)
{
	btConvexShape* shape;

	shape = private_create_from_box (self, aabb);
	if (shape == NULL)
		return 0;
	if (!private_add_shape (self, shape, transform))
	{
		delete shape;
		return 0;
	}

	return 1;
}

int liphy_shape_add_convex (
	LIPhyShape*           self,
	const LIMatVector*    vertices,
	int                   count,
	const LIMatTransform* transform)
{
	btConvexShape* shape;

	shape = private_create_from_vertices (self, vertices, count, 1.0f);
	if (shape == NULL)
		return 0;
	if (!private_add_shape (self, shape, transform))
	{
		delete shape;
		return 0;
	}

	return 1;
}

int liphy_shape_add_model_shape (
	LIPhyShape*           self,
	const LIMdlShape*     shape,
	const LIMatTransform* transform,
	float                 scale)
{
	int i;
	int empty = 1;
	btConvexHullShape* ret;

	/* Add the shape parts to the physics shape. */
	for (i = 0 ; i < shape->parts.count ; i++)
	{
		if (!shape->parts.array[i].vertices.count)
			continue;
		ret = private_create_from_vertices (self,
			shape->parts.array[i].vertices.array,
			shape->parts.array[i].vertices.count, scale);
		if (ret == NULL)
			return 0;
		if (!private_add_shape (self, ret, transform))
		{
			delete ret;
			return 0;
		}
		empty = 0;
	}

	/* Ensure that something is always created. */
	if (empty)
	{
		ret = private_create_from_empty (self);
		if (ret == NULL)
			return 0;
		if (!private_add_shape (self, ret, transform))
		{
			delete ret;
			return 0;
		}
	}

	return 1;
}

/**
 * \brief Merges a collision shape to this one.
 * \param self Object.
 * \param shape Collision shape.
 * \param transform Shape transformation or NULL for identity.
 */
int liphy_shape_add_shape (
	LIPhyShape*           self,
	LIPhyShape*           shape,
	const LIMatTransform* transform)
{
	int i;
	btConvexHullShape* child;

	for (i = 0 ; i < shape->shape->getNumChildShapes () ; i++)
	{
		child = (btConvexHullShape*) shape->shape->getChildShape (i);
		child = new btConvexHullShape (*child);
		// FIXME: Take child transformation into account.
		if (!private_add_shape (self, child, transform))
			delete child;
	}

	return 1;
}

/**
 * \brief Clears the shape.
 * \param self Shape.
 */
void liphy_shape_clear (
	LIPhyShape* self)
{
	int i;
	btCollisionShape* child;

	for (i = 0 ; i < self->shape->getNumChildShapes () ; i++)
	{
		child = self->shape->getChildShape (i);
		delete child;
	}
	while (self->shape->getNumChildShapes ())
		self->shape->removeChildShapeByIndex (0);
}

/**
 * \brief Gets the local inertia vector of the shape.
 *
 * \param self Collision shape.
 * \param mass Mass.
 * \param result Return location for the inertia vector.
 */
void liphy_shape_get_inertia (
	const LIPhyShape* self,
	float             mass,
	LIMatVector*      result)
{
	btVector3 inertia;

	/* TODO: Could this be precalculated? */
	self->shape->calculateLocalInertia (mass, inertia);
	result->x = inertia[0];
	result->y = inertia[1];
	result->z = inertia[2];
}

/**
 * \brief Sets the center of mass of the shape.
 *
 * The center of mass will only be applied to the shape parts added
 * since calling this. Be sure that the shape has been cleared before
 * calling this or else the results won't make sense.
 *
 * \param self Collision shape.
 * \param center Center of mass position vector.
 */
void liphy_shape_set_center_of_mass (
	LIPhyShape*        self,
	const LIMatVector* center)
{
	self->center_of_mass = *center;
}

/*****************************************************************************/

static int private_add_shape (
	LIPhyShape*           self,
	btConvexShape*        shape,
	const LIMatTransform* transform)
{
	btVector3 p(0.0f, 0.0f, 0.0f);
	btQuaternion r(0.0f, 0.0f, 0.0f, 1.0f);
	btTransform center_of_mass(r, btVector3 (
		self->center_of_mass.x, self->center_of_mass.y, self->center_of_mass.z));

	if (transform != NULL)
	{
		p = btVector3 (transform->position.x, transform->position.y,
		               transform->position.z);
		r = btQuaternion(transform->rotation.x, transform->rotation.y,
		                 transform->rotation.z, transform->rotation.w);
	}
	try
	{
		self->shape->addChildShape (center_of_mass.inverse() * btTransform (r, p), shape);
	}
	catch (...)
	{
		return 0;
	}

	/* Update the bounding box. */
	btVector3 min;
	btVector3 max;
	self->shape->getAabb(btTransform::getIdentity (), min, max);
	self->bounds.min = limat_vector_init (min[0], min[1], min[2]);
	self->bounds.max = limat_vector_init (max[0], max[1], max[2]);

	return 1;
}

static btConvexHullShape* private_create_convex (
	LIPhyShape* self,
	btScalar*   vertices,
	int         count)
{
	return new btConvexHullShape (vertices, count, 4 * sizeof (btScalar));
}

static btConvexHullShape* private_create_from_box (
	LIPhyShape*      self,
	const LIMatAabb* aabb)
{
	btScalar tmp[32] =
	{
		aabb->min.x, aabb->min.y, aabb->min.z, 0.0,
		aabb->min.x, aabb->min.y, aabb->max.z, 0.0,
		aabb->min.x, aabb->max.y, aabb->min.z, 0.0,
		aabb->min.x, aabb->max.y, aabb->max.z, 0.0,
		aabb->max.x, aabb->min.y, aabb->min.z, 0.0,
		aabb->max.x, aabb->min.y, aabb->max.z, 0.0,
		aabb->max.x, aabb->max.y, aabb->min.z, 0.0,
		aabb->max.x, aabb->max.y, aabb->max.z, 0.0
	};

	return new btConvexHullShape (tmp, 8, 4 * sizeof (btScalar));
}

static btConvexHullShape* private_create_from_empty (
	LIPhyShape* self)
{
	const float w = 0.1f;
	const btScalar tmp[32] =
	{
		-w, -w, -w, 0.0,
		-w, -w, w, 0.0,
		-w, w, -w, 0.0,
		-w, w, w, 0.0,
		w, -w, -w, 0.0,
		w, -w, w, 0.0,
		w, w, -w, 0.0,
		w, w, w, 0.0
	};

	return new btConvexHullShape (tmp, 8, 4 * sizeof (btScalar));
}

static btConvexHullShape* private_create_from_vertices (
	LIPhyShape*        self,
	const LIMatVector* vertices,
	int                count,
	float              scale)
{
	int i;
	btConvexHullShape* ret;
	btScalar* tmp;
	LIMatVector vertex;

	/* Use a default empty model if there are no vertices. */
	if (!count)
		return private_create_from_empty (self);

	/* Create a temporary copy of the vertices. */
	tmp = (btScalar*) lisys_calloc (4 * count, sizeof (btScalar));
	if (tmp == NULL)
		return NULL;
	for (i = 0 ; i < count ; i++)
	{
		vertex = vertices[i];
		tmp[4 * i + 0] = vertex.x;
		tmp[4 * i + 1] = vertex.y;
		tmp[4 * i + 2] = vertex.z;
		tmp[4 * i + 3] = 0.0;
	}

	/* Scale the vertices. */
	if (scale != 1.0f)
	{
		for (i = 0 ; i < 4 * count ; i++)
			tmp[i] *= scale;
	}

	/* Create a convex shape. */
	ret = private_create_convex (self, tmp, count);
	lisys_free (tmp);

	return ret;
}

/** @} */
/** @} */
