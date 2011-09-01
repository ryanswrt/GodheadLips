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
 * \addtogroup LIMat Math
 * @{
 * \addtogroup LIMatQuaternion Quaternion
 * @{
 */

#ifndef __MATH_QUATERNION_H__
#define __MATH_QUATERNION_H__

#include <math.h>
#include "math-vector.h"

typedef struct _LIMatQuaternion LIMatQuaternion;
struct _LIMatQuaternion
{
	float x;
	float y;
	float z;
	float w;
};

/**
 * \brief Initializes a new quaternion from floating point values.
 * \param x Float.
 * \param y Float.
 * \param z Float.
 * \param w Float.
 * \return Quaternion.
 */
static inline LIMatQuaternion limat_quaternion_init (
	float x,
	float y,
	float z,
	float w)
{
	LIMatQuaternion self;

	self.x = x;
	self.y = y;
	self.z = z;
	self.w = w;

	return self;
}

/**
 * \brief Returns the shortest rotation from the first vector to the second.
 * \param v1 Vector.
 * \param v2 Vector.
 * \return Quaternion.
 */
static inline LIMatQuaternion limat_quaternion_init_vectors (
	LIMatVector v1,
	LIMatVector v2)
{
	LIMatQuaternion self;
	LIMatVector c;

	c = limat_vector_cross (v1, v2);
	self.x = c.x;
	self.y = c.y;
	self.z = c.z;
	self.w = sqrtf (limat_vector_dot (v1, v1) + limat_vector_dot (v2, v2)) + limat_vector_dot (v1, v2);

	return self;
}

/**
 * \brief Returns an identity quaternion.
 * \return Quaternion.
 */
static inline LIMatQuaternion limat_quaternion_identity ()
{
	LIMatQuaternion self;

	self.x = 0.0f;
	self.y = 0.0f;
	self.z = 0.0f;
	self.w = 1.0f;

	return self;
}

/**
 * \brief Creates a rotation quaternion from euler angles.
 * \param phi Rotation around X-axis.
 * \param theta Rotation around Y-axis.
 * \param psi Rotation around Z-axis.
 * \return Quaternion.
 */
static inline LIMatQuaternion limat_quaternion_euler (
	float phi,
	float theta,
	float psi)
{
	float c1 = cos (0.5f * phi);
	float c2 = cos (0.5f * theta);
	float c3 = cos (0.5f * psi);
	float s1 = sin (0.5f * phi);
	float s2 = sin (0.5f * theta);
	float s3 = sin (0.5f * psi);
	LIMatQuaternion self;

	self.w = c1 * c2 * c3 - s1 * s2 * s3;
	self.x = s1 * s2 * c3 + c1 * c2 * s3;
	self.y = s1 * c2 * c3 + c1 * s2 * s3;
	self.z = c1 * s2 * s3 - s1 * c2 * s3;

	return self;
}

/**
 * \brief Creates a quaternion from forwards and up vectors.
 * \param dir Forwards vector.
 * \param up Up vector.
 * \return Quaternion.
 */
static inline LIMatQuaternion limat_quaternion_look (
	LIMatVector dir,
	LIMatVector up)
{
	float len;
	LIMatQuaternion self;
	LIMatVector side;

	/* Normalize direction. */
	len = sqrt (dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	dir.x = dir.x / len;
	dir.y = dir.y / len;
	dir.z = dir.z / len;

	/* Calculate side. */
	side.x =  (dir.y * up.z - dir.z * up.y);
	side.y = -(dir.x * up.z - dir.z * up.x);
	side.z =  (dir.x * up.y - dir.y * up.x);
	len = sqrt (side.x * side.x + side.y * side.y + side.z * side.z);
	side.x = side.x / len;
	side.y = side.y / len;
	side.z = side.z / len;

	/* Calculate up. */
	up.x =  (side.y * dir.z - side.z * dir.y);
	up.y = -(side.x * dir.z - side.z * dir.x);
	up.z =  (side.x * dir.y - side.y * dir.x);
	len = sqrt (up.x * up.x + up.y * up.y + up.z * up.z);
	up.x = up.x / len;
	up.y = up.y / len;
	up.z = up.z / len;

	/* Convert to quaternion. */
	/* FIXME: Copied from math-convert. */
	/* FIXME: Surely there is a more direct way to do this. */
	self.x = 0.5f * sqrt (LIMAT_MAX (0.0f, 1.0f + side.x - up.y + dir.z));
	self.y = 0.5f * sqrt (LIMAT_MAX (0.0f, 1.0f - side.x + up.y + dir.z));
	self.z = 0.5f * sqrt (LIMAT_MAX (0.0f, 1.0f - side.x - up.y - dir.z));
	self.w = 0.5f * sqrt (LIMAT_MAX (0.0f, 1.0f + side.x + up.y - dir.z));
	self.x = copysign (self.x, -dir.y - up.z);
	self.y = copysign (self.y, side.z + dir.x);
	self.z = copysign (self.z, up.x - side.y);

	return self;
}

/**
 * \brief Creates a new lookat quaternion.
 *
 * Naturally, only the rotation part of the lookat setup is stored to the
 * quaternion. The eye position is only used for calculating the direction
 * and then discarded.
 *
 * \param position Eye position.
 * \param center Center position.
 * \param up Up vector.
 * \return Quaternion.
 */
static inline LIMatQuaternion limat_quaternion_lookat (
	LIMatVector position,
	LIMatVector center,
	LIMatVector up)
{
	LIMatVector direction;

	direction = limat_vector_subtract (position, center);
	direction = limat_vector_normalize (direction);

	return limat_quaternion_look (direction, up);
}

/**
 * \brief Creates a rotation quaternion.
 * \param angle Amount of rotation in radians.
 * \param axis Axis of rotation.
 * \return Quaternion.
 */
static inline LIMatQuaternion limat_quaternion_rotation (
	float       angle,
	LIMatVector axis)
{
	LIMatQuaternion self;
	float w = cos (angle / 2);
	float q = sin (angle / 2);
	float len = sqrtf (axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);

	if (len >= LIMAT_EPSILON)
	{
		axis.x /= len;
		axis.y /= len;
		axis.z /= len;
	}
	self.x = q * axis.x;
	self.y = q * axis.y;
	self.z = q * axis.z;
	self.w = w;

	return self;
}

/**
 * \brief Gets the conjugate of the quaternion.
 *
 * \param self Quaternion.
 * \return Quaternion.
 */
static inline LIMatQuaternion
limat_quaternion_conjugate (LIMatQuaternion self)
{
	LIMatQuaternion result;

	result.x = -self.x;
	result.y = -self.y;
	result.z = -self.z;
	result.w = self.w;
	return result;
}

/**
 * \brief Adds a quaternion to another.
 *
 * \param self First quaternion.
 * \param quat Second quaternion.
 * \return Quaternion.
 */
static inline LIMatQuaternion
limat_quaternion_add (LIMatQuaternion self,
                      LIMatQuaternion quat)
{
	LIMatQuaternion a = self;
	LIMatQuaternion b = quat;
	LIMatQuaternion result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.w = a.w + b.w;
	return result;
}

/**
 * \brief Subtracts a quaternion from another.
 *
 * \param self First quaternion.
 * \param quat Second quaternion.
 * \return Quaternion.
 */
static inline LIMatQuaternion
limat_quaternion_subtract (LIMatQuaternion self,
                           LIMatQuaternion quat)
{
	LIMatQuaternion a = self;
	LIMatQuaternion b = quat;
	LIMatQuaternion result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	result.w = a.w - b.w;
	return result;
}

/**
 * \brief Calculates the Grassham product of the quaternions.
 *
 * \param self First multiplied quaternion.
 * \param quat Second multiplied quaternion.
 * \return Quaternion.
 */
static inline LIMatQuaternion
limat_quaternion_multiply (LIMatQuaternion self,
                           LIMatQuaternion quat)
{
	LIMatQuaternion a = self;
	LIMatQuaternion b = quat;
	LIMatQuaternion result;

	result.x = (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y);
	result.y = (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x);
	result.z = (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w);
	result.w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);
	return result;
}

/**
 * \brief Calculates the scalar product for the quaternion as if it was a 4D vector.
 *
 * \param self Quaternion.
 * \param quat Quaternion.
 * \return Scalar.
 */
static inline float
limat_quaternion_dot (LIMatQuaternion self,
                      LIMatQuaternion quat)
{
	return self.w * quat.w + self.x * quat.x + self.y * quat.y + self.z * quat.z;
}

/**
 * \brief Calculates the unit quaternion of the passed quaternion.
 *
 * \param self Quaternion.
 * \return Quaternion.
 */
static inline LIMatQuaternion
limat_quaternion_normalize (LIMatQuaternion self)
{
	LIMatQuaternion result;
	float len = sqrt (self.w * self.w +
	                  self.x * self.x +
	                  self.y * self.y +
	                  self.z * self.z);

	if (len < LIMAT_EPSILON)
	{
		result.x = 0.0f;
		result.y = 0.0f;
		result.z = 0.0f;
		result.w = 1.0f;
	}
	else
	{
		result.x = self.x / len;
		result.y = self.y / len;
		result.z = self.z / len;
		result.w = self.w / len;
	}
	return result;
}

/**
 * \brief Performs normalized linear quaternion interpolation.
 *
 * \param self First interpolated quaternion.
 * \param quat Second interpolated quaternion.
 * \param weight Interpolating scalar.
 * \return Quaternion.
 */
static inline LIMatQuaternion
limat_quaternion_nlerp (LIMatQuaternion self,
                        LIMatQuaternion quat,
                        float           weight)
{
	float a = weight;
	float b = 1.0f - weight;
	LIMatQuaternion result;

	result.w = a * self.w + b * quat.w;
	result.x = a * self.x + b * quat.x;
	result.y = a * self.y + b * quat.y;
	result.z = a * self.z + b * quat.z;

	return limat_quaternion_normalize (result);
}

/**
 * \brief Transforms the vector by the quaternion.
 *
 * This is analoguous to multiplying the vector by a rotation matrix.
 * The quaternion needs to be normalized in order to get the desired results.
 *
 * \param self Quaternion.
 * \param vector Vector.
 * \return Vector.
 */
static inline LIMatVector
limat_quaternion_transform (LIMatQuaternion self,
                            LIMatVector     vector)
{
	LIMatVector result;
	LIMatQuaternion q1 = self;
	LIMatQuaternion q2 = self;
	LIMatQuaternion q3 = { vector.x, vector.y, vector.z, 0.0f };

	q2 = limat_quaternion_conjugate (q2);
	q3 = limat_quaternion_multiply (q1, q3);
	q3 = limat_quaternion_multiply (q3, q2);
	result.x = q3.x;
	result.y = q3.y;
	result.z = q3.z;

	return result;
}

/**
 * \brief Gets one of the basis vectors of the quaternion.
 *
 * \param self Quaternion.
 * \param basis 0 for X, 1 for Y, 2 for Z.
 * \return Unit length vector.
 */
static inline LIMatVector
limat_quaternion_get_basis (LIMatQuaternion self,
                            int             basis)
{
	LIMatVector result;

	switch (basis)
	{
		case 0:
			result.x = 1.0f - 2.0f * (self.y * self.y + self.z * self.z);
			result.y = 2.0f * (self.x * self.y - self.z * self.w);
			result.z = 2.0f * (self.x * self.z + self.y * self.w);
			break;
		case 1:
			result.x = 2.0f * (self.x * self.y + self.z * self.w);
			result.y = 1.0f - 2.0f * (self.x * self.x + self.z * self.z);
			result.z = 2.0f * (self.y * self.z - self.x * self.w);
			break;
		case 2:
			result.x = 2.0f * (self.x * self.z - self.y * self.w);
			result.y = 2.0f * (self.y * self.z + self.x * self.w);
			result.z = 1.0f - 2.0f * (self.x * self.x + self.y * self.y);
			break;
		default:
			result.x = 0.0f;
			result.y = 0.0f;
			result.z = 0.0f;
			break;
	}

	return result;
}

/**
 * \brief Gets the Euler angle presentation of the quaternion.
 *
 * The quaternion must be normalized for the results to be meaningful.
 *
 * \param self Quaternion.
 * \param phi Return location for the rotation around X-axis.
 * \param theta Return location for the rotation around Y-axis.
 * \param psy Return location for the rotation around Z-axis.
 */
static inline void
limat_quaternion_get_euler (LIMatQuaternion self,
                            float*          phi,
                            float*          theta,
                            float*          psi)
{
	float tmp;
	float xx = self.x * self.x;
	float yy = self.y * self.y;
	float zz = self.z * self.z;

	tmp = self.x * self.y + self.z * self.w;
	if (tmp >= 0.5f - LIMAT_EPSILON)
	{
		*phi = 2 * atan2 (self.x, self.w);
		*theta = 0.5f * M_PI;
		*psi = 0.0f;
	}
	else if (tmp <= -0.5f + LIMAT_EPSILON)
	{
		*phi = -2 * atan2 (self.x, self.w);
		*theta = -0.5f * M_PI;
		*psi = 0.0f;
	}
	else
	{
		*phi = atan2 (2 * (self.y * self.w - self.x * self.z), 1 - 2 * (yy + zz));
		*theta = asin (2 * tmp);
		*psi = atan2 (2 * (self.x * self.w - self.y * self.z), 1 - 2 * (xx + zz));
	}
}

/**
 * \brief Gets the length of the quaternion.
 *
 * \param self Quaternion.
 * \return Float.
 */
static inline float
limat_quaternion_get_length (LIMatQuaternion self)
{
	return sqrt (self.w * self.w +
	             self.x * self.x +
	             self.y * self.y +
	             self.z * self.z);
}

/**
 * \brief Gets the variant of self that gives the shortest rotation towards quat.
 *
 * For every nonzero quaternion Q, there is a quaternion -Q that performs the
 * exact same transformation as Q but via a different path. The choice of path
 * makes a difference when interpolating quaternions, in which case you often
 * want to choose the shorter path as returned by this function.
 *
 * \param self Quaternion.
 * \param quat Quaternion.
 * \return Quaternion.
 */
static inline LIMatQuaternion
limat_quaternion_get_nearest (const LIMatQuaternion self,
                              const LIMatQuaternion quat)
{
	LIMatQuaternion result = self;

	if (self.w * quat.w + self.x * quat.x + self.y * quat.y + self.z * quat.z < 0.0f)
	{
		result.w *= -1.0f;
		result.x *= -1.0f;
		result.y *= -1.0f;
		result.z *= -1.0f;
	}

	return result;
}

/**
 * \brief Gets the angle and axis of a rotation quaternion.
 *
 * \param self Quaternion.
 * \param axis Return location for the axis or NULL.
 * \return Angle in radians.
 */
static inline float
limat_quaternion_get_rotation (const LIMatQuaternion self,
                               LIMatVector*          axis)
{
	if (axis != NULL)
		*axis = limat_vector_normalize (limat_vector_init (self.x, self.y, self.z));

	return 2.0f * acos (self.w);
}

#endif

/** @} */
/** @} */
