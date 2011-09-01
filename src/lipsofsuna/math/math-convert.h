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
 * \addtogroup LIMatConvert Convert
 * @{
 */

#ifndef __MATH_CONVERT_H__
#define __MATH_CONVERT_H__

#include "math-vector.h"
#include "math-matrix.h"
#include "math-quaternion.h"
#include "math-transform.h"

/**
 * \brief Converts a 4x4 rotation matrix to a quaternion.
 *
 * \param matrix Matrix.
 * \return Quaternion.
 */
static inline LIMatQuaternion
limat_convert_matrix_to_quaternion (LIMatMatrix matrix)
{
	LIMatQuaternion self;

	self.x = 0.5f * sqrt (LIMAT_MAX (0.0f, 1.0f + matrix.m[0] - matrix.m[5] - matrix.m[10]));
	self.y = 0.5f * sqrt (LIMAT_MAX (0.0f, 1.0f - matrix.m[0] + matrix.m[5] - matrix.m[10]));
	self.z = 0.5f * sqrt (LIMAT_MAX (0.0f, 1.0f - matrix.m[0] - matrix.m[5] + matrix.m[10]));
	self.w = 0.5f * sqrt (LIMAT_MAX (0.0f, 1.0f + matrix.m[0] + matrix.m[5] + matrix.m[10]));
	self.x = copysign (self.x, matrix.m[6] - matrix.m[9]);
	self.y = copysign (self.y, matrix.m[8] - matrix.m[2]);
	self.z = copysign (self.z, matrix.m[1] - matrix.m[4]);

	return self;
}

static inline LIMatTransform
limat_convert_matrix_to_transform (LIMatMatrix matrix)
{
	LIMatVector tmp;
	LIMatTransform self;

	tmp = limat_vector_init (0.0f, 0.0f, 0.0f);
	self.position = limat_matrix_transform (matrix, tmp);
	self.rotation = limat_convert_matrix_to_quaternion (matrix);

	return self;
}

/**
 * \brief Converts a quaternion to a 4x4 rotation matrix.
 *
 * The quaternion must be normalized for the results to be meaningful.
 *
 * \param self Quaternion.
 * \return Matrix.
 */
static inline LIMatMatrix
limat_convert_quaternion_to_matrix (LIMatQuaternion self)
{
	LIMatMatrix matrix;

	matrix.m[0] = 1.0f - 2.0f * (self.y * self.y + self.z * self.z);
	matrix.m[1] = 2.0f * (self.x * self.y + self.z * self.w);
	matrix.m[2] = 2.0f * (self.x * self.z - self.y * self.w);
	matrix.m[3] = 0.0f;
	matrix.m[4] = 2.0f * (self.x * self.y - self.z * self.w);
	matrix.m[5] = 1.0f - 2.0f * (self.x * self.x + self.z * self.z);
	matrix.m[6] = 2.0f * (self.y * self.z + self.x * self.w);
	matrix.m[7] = 0.0f;
	matrix.m[8] = 2.0f * (self.x * self.z + self.y * self.w);
	matrix.m[9] = 2.0f * (self.y * self.z - self.x * self.w);
	matrix.m[10] = 1.0f - 2.0f * (self.x * self.x + self.y * self.y);
	matrix.m[11] = 0.0f;
	matrix.m[12] = 0.0f;
	matrix.m[13] = 0.0f;
	matrix.m[14] = 0.0f;
	matrix.m[15] = 1.0f;

	return matrix;
}

static inline LIMatMatrix 
limat_convert_transform_to_matrix (LIMatTransform self)
{
	LIMatMatrix result;
	LIMatQuaternion quat = self.rotation;

	result.m[0] = 1.0f - 2.0f * (quat.y * quat.y + quat.z * quat.z);
	result.m[1] = 2.0f * (quat.x * quat.y + quat.z * quat.w);
	result.m[2] = 2.0f * (quat.x * quat.z - quat.y * quat.w);
	result.m[3] = 0.0f;
	result.m[4] = 2.0f * (quat.x * quat.y - quat.z * quat.w);
	result.m[5] = 1.0f - 2.0f * (quat.x * quat.x + quat.z * quat.z);
	result.m[6] = 2.0f * (quat.y * quat.z + quat.x * quat.w);
	result.m[7] = 0.0f;
	result.m[8] = 2.0f * (quat.x * quat.z + quat.y * quat.w);
	result.m[9] = 2.0f * (quat.y * quat.z - quat.x * quat.w);
	result.m[10] = 1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y);
	result.m[11] = 0.0f;
	result.m[12] = self.position.x;
	result.m[13] = self.position.y;
	result.m[14] = self.position.z;
	result.m[15] = 1.0f;

	return result;
}

static inline LIMatTransform
limat_convert_quaternion_to_transform (LIMatQuaternion rotation)
{
	LIMatTransform self;

	self.position = limat_vector_init (0.0f, 0.0f, 0.0f);
	self.rotation = rotation;

	return self;
}

static inline LIMatTransform
limat_convert_vector_to_transform (LIMatVector position)
{
	LIMatTransform self;

	self.position = position;
	self.rotation = limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f);

	return self;
}

#endif

/** @} */
/** @} */
