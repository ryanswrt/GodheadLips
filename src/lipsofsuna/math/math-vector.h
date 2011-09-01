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
 * \addtogroup LIMatVector Vector
 * @{
 */

#ifndef __MATH_VECTOR_H__
#define __MATH_VECTOR_H__

#include <math.h>

#define LIMAT_VECTOR_EPSILON 0.0001f

/**
 * \brief A three-dimensional vector type.
 */
typedef struct _LIMatVector LIMatVector;
struct _LIMatVector
{
	float x;
	float y;
	float z;
};

/**
 * \brief Returns a vector with the given components.
 *
 * \param x Float.
 * \param y Float.
 * \param z Float.
 * \return Vector.
 */
static inline LIMatVector
limat_vector_init (float x,
                   float y,
                   float z)
{
	LIMatVector result = { x, y, z };
	return result;
}

/**
 * \brief Compares two vertices using a threshold.
 * \param self Vector.
 * \param vector Vector.
 * \param threshold Threshold for each coordinate.
 * \return Vector.
 */
static inline int limat_vector_compare (
	LIMatVector self,
	LIMatVector vector,
	float       threshold)
{
	LIMatVector tmp =
	{
		self.x - vector.x,
		self.y - vector.y,
		self.z - vector.z
	};
	if (-threshold <= tmp.x && tmp.x < threshold &&
	    -threshold <= tmp.y && tmp.y < threshold &&
	    -threshold <= tmp.z && tmp.z < threshold)
		return 1;
	return 0;
}

/**
 * \brief Gets the opposite vector.
 *
 * \param self Vector.
 * \return Vector.
 */
static inline LIMatVector
limat_vector_invert (LIMatVector self)
{
	LIMatVector result = { -self.x, -self.y, -self.z };
	return result;
}

/**
 * \brief Gets the length of the vector.
 *
 * \param self Vector.
 * \return Float.
 */
static inline float
limat_vector_get_length (LIMatVector self)
{
	return sqrt (self.x * self.x + self.y * self.y + self.z * self.z);
}

/**
 * \brief Adds a vector to another.
 *
 * \param self Vector.
 * \param vector Vector.
 * \return Vector.
 */
static inline LIMatVector
limat_vector_add (LIMatVector self,
                  LIMatVector vector)
{
	LIMatVector result;

	result.x = self.x + vector.x;
	result.y = self.y + vector.y;
	result.z = self.z + vector.z;
	return result;
}

/**
 * \brief Subtracts a vector from another.
 *
 * \param self Vector.
 * \param vector Vector.
 * \return Vector.
 */
static inline LIMatVector
limat_vector_subtract (LIMatVector self,
                       LIMatVector vector)
{
	LIMatVector result;

	result.x = self.x - vector.x;
	result.y = self.y - vector.y;
	result.z = self.z - vector.z;
	return result;
}

/**
 * \brief Multiplies the vector by a scalar.
 *
 * \param self Vector.
 * \param scalar Vector.
 * \return Vector.
 */
static inline LIMatVector
limat_vector_multiply (LIMatVector self,
                       float       scalar)
{
	LIMatVector result;

	result.x = self.x * scalar;
	result.y = self.y * scalar;
	result.z = self.z * scalar;
	return result;
}

/**
 * \brief Normalizes the vector.
 *
 * \param self Vector.
 * \return Vector.
 */
static inline LIMatVector
limat_vector_normalize (LIMatVector self)
{
	LIMatVector result;
	float len = limat_vector_get_length (self);

	if (len < LIMAT_VECTOR_EPSILON)
	{
		result.x = 0.0f;
		result.y = 0.0f;
		result.z = 0.0f;
	}
	else
	{
		result.x = self.x / len;
		result.y = self.y / len;
		result.z = self.z / len;
	}
	return result;
}

/**
 * \brief Calculates the scalar product of two vectors.
 *
 * \param self Vector.
 * \param vector Vector.
 * \return Scalar.
 */
static inline float
limat_vector_dot (LIMatVector self,
                  LIMatVector vector)
{
	return self.x * vector.x + self.y * vector.y + self.z * vector.z;
}

/**
 * \brief Calculates the cross product of two vectors.
 *
 * \param self Vector.
 * \param vector Vector.
 * \return Vector.
 */
static inline LIMatVector
limat_vector_cross (LIMatVector self,
                    LIMatVector vector)
{
	LIMatVector result;

	result.x =  (self.y * vector.z - self.z * vector.y);
	result.y = -(self.x * vector.z - self.z * vector.x);
	result.z =  (self.x * vector.y - self.y * vector.x);
	return result;
}

/**
 * \brief Performs linear vector interpolation.
 *
 * \param self First interpolated vector.
 * \param vector Second interpolated vector.
 * \param weight Interpolating scalar.
 * \return Vector.
 */
static inline LIMatVector
limat_vector_lerp (LIMatVector self,
                   LIMatVector vector,
                   float       weight)
{
	float a = weight;
	float b = 1.0f - weight;
	LIMatVector result;

	result.x = a * self.x + b * vector.x;
	result.y = a * self.y + b * vector.y;
	result.z = a * self.z + b * vector.z;

	return result;
}

#endif

/** @} */
/** @} */
