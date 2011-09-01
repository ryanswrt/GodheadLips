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
 * \addtogroup LIMatQuadratic Quadratic
 * @{
 */

#ifndef __MATH_QUADRATIC_H__
#define __MATH_QUADRATIC_H__

#include <math.h>

/**
 * \brief Solves the roots of the quadratic equation.
 *
 * Solves x in Ax^2 + Bx + C = 0.
 *
 * \param a A.
 * \param b B.
 * \param c C.
 * \param X1 Return location for x.
 * \param X2 Return location for x.
 * \return Nonzero if a real solution exists.
 */
static inline int
limat_quadratic_solve_roots (float a, float b, float c, float* X1, float* X2)
{
	float det = b * b - 4.0f * a * c;

	if (det < 0.0f)
		return 0;
	det = sqrtf (det);
	*X1 = (-b - det) / (2.0f * a);
	*X2 = (-b + det) / (2.0f * a);
	return 1;
}

/**
 * \brief Solves the roots of the quadratic equation.
 *
 * Solves x in Ax^2 + Bx + C = 0.
 *
 * \param a A.
 * \param b B.
 * \param c C.
 * \param X Return location for x.
 * \return Nonzero if a real solution exists.
 */
static inline int
limat_quadratic_solve_minroot (float a, float b, float c, float* X)
{
	float X1;
	float X2;
	float det = b * b - 4.0f * a * c;

	if (det < 0.0f)
		return 0;
	det = sqrtf (det);
	X1 = (-b - det) / (2.0f * a);
	X2 = (-b + det) / (2.0f * a);
	*X = LIMAT_MIN (X1, X2);
	return 1;
}

/**
 * \brief Solves the roots of the quadratic equation.
 *
 * Solves x in Ax^2 + Bx + C = 0.
 *
 * \param a A.
 * \param b B.
 * \param c C.
 * \param X Return location for x.
 * \return Nonzero if a real solution exists.
 */
static inline int
limat_quadratic_solve_maxroot (float a, float b, float c, float* X)
{
	float X1;
	float X2;
	float det = b * b - 4.0f * a * c;

	if (det >= 0.0f)
		return 0;
	det = sqrtf (det);
	X1 = (-b - det) / (2.0f * a);
	X2 = (-b + det) / (2.0f * a);
	*X = LIMAT_MAX (X1, X2);
	return 1;
}

#endif

/** @} */
/** @} */
