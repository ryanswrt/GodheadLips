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
 * \addtogroup LIMat Math
 * @{
 * \addtogroup LIMatTransform Transform
 * @{
 */

#ifndef __MATH_TRANSFORM_H__
#define __MATH_TRANSFORM_H__

#include "math-matrix.h"
#include "math-quaternion.h"
#include "math-vector.h"

typedef struct _LIMatTransform LIMatTransform;
struct _LIMatTransform
{
	LIMatVector position;
	LIMatQuaternion rotation;
};

static inline LIMatTransform
limat_transform_init (LIMatVector     position,
                      LIMatQuaternion rotation)
{
	LIMatTransform self;

	self.position = position;
	self.rotation = rotation;

	return self;
}

static inline LIMatTransform
limat_transform_identity ()
{
	LIMatTransform self;

	self.position = limat_vector_init (0.0f, 0.0f, 0.0f);
	self.rotation = limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f);

	return self;
}

static inline LIMatTransform
limat_transform_look (LIMatVector position,
                      LIMatVector direction,
                      LIMatVector up)
{
	LIMatTransform self;

	self.rotation = limat_quaternion_look (direction, up);
	self.position = limat_quaternion_transform (
		self.rotation, limat_vector_invert (position));

	return self;
}

static inline LIMatTransform
limat_transform_lookat (LIMatVector position,
                        LIMatVector center,
                        LIMatVector up)
{
	LIMatTransform self;
	LIMatVector direction;

	direction = limat_vector_subtract (position, center);
	direction = limat_vector_normalize (position);
	self.rotation = limat_quaternion_look (direction, up);
	self.position = limat_quaternion_transform (
		self.rotation, limat_vector_invert (position));

	return self;
}

static inline LIMatTransform
limat_transform_invert (LIMatTransform self)
{
	LIMatVector tmp;
	LIMatTransform result;

	tmp = limat_vector_invert (self.position);
	result.rotation = limat_quaternion_conjugate (self.rotation);
	result.position = limat_quaternion_transform (result.rotation, tmp);

	return result;
}

static inline LIMatTransform
limat_transform_conjugate (LIMatTransform self)
{
	LIMatTransform result;

	result.position = self.position;
	result.rotation = limat_quaternion_conjugate (self.rotation);

	return result;
}

static inline LIMatTransform
limat_transform_multiply (LIMatTransform self,
                          LIMatTransform transform)
{
	LIMatTransform result;

	result.position = limat_quaternion_transform (self.rotation, transform.position);
	result.position = limat_vector_add (self.position, result.position);
	result.rotation = limat_quaternion_multiply (self.rotation, transform.rotation);

	return result;
}

/**
 * \brief Gets the second transformation relative to the first one.
 *
 * Moves `transform' from global coordinate space to the coordinate space
 * of `self'.
 *
 * \param self Transformation.
 * \param transform Transformation.
 * \return Transformation.
 */
static inline LIMatTransform
limat_transform_relative (LIMatTransform self,
                          LIMatTransform transform)
{
	LIMatQuaternion inverse;
	LIMatQuaternion rotation;
	LIMatVector position;

	inverse = limat_quaternion_conjugate (self.rotation);
	position = limat_vector_subtract (transform.position, self.position);
	position = limat_quaternion_transform (inverse, position);
	rotation = limat_quaternion_multiply (inverse, transform.rotation);

	return limat_transform_init (position, rotation);
}

static inline LIMatVector
limat_transform_transform (LIMatTransform self,
                           LIMatVector    vector)
{
	LIMatVector result;

	result = limat_quaternion_transform (self.rotation, vector);
	result = limat_vector_add (result, self.position);

	return result;
}

static inline LIMatTransform
limat_transform_snap (LIMatTransform self,
                      LIMatTransform child)
{
	LIMatTransform result;
	LIMatTransform inverse;

	inverse = limat_transform_invert (child);
	result = limat_transform_multiply (self, inverse);

	return result;
}

static inline LIMatMatrix
limat_transform_get_modelview (LIMatTransform self)
{
	LIMatVector pos;
	LIMatVector dir;
	LIMatVector up;

	pos = self.position;
	dir = limat_quaternion_get_basis (self.rotation, 2);
	dir = limat_vector_multiply (dir, -1.0f);
	up = limat_quaternion_get_basis (self.rotation, 1);

	return limat_matrix_look (pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, up.x, up.y, up.z);
}

#endif

/** @} */
/** @} */
