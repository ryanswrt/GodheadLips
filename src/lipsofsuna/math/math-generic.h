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
 * \addtogroup LIMatGeneric Generic
 * @{
 */

#ifndef __MATH_GENERIC_H__
#define __MATH_GENERIC_H__

#include <math.h>

#define LIMAT_ABS(a) (((a)<0)?-(a):(a))
#define LIMAT_MIN(a,b) ((a)<(b)?(a):(b))
#define LIMAT_MAX(a,b) ((a)>(b)?(a):(b))
#define LIMAT_CLAMP(val,min,max) ((val)<(min)?(min):(val)>(max)?(max):(val))
#define LIMAT_EPSILON 1E-20f
#define LIMAT_INFINITE (1.0f/0.0f)

static inline float limat_mix (
	float value0,
	float value1,
	float mix)
{
	return (1.0f - mix) * value0 + mix * value1;
}

static inline float limat_smoothstep (
	float value,
	float edge0,
	float edge1)
{
	float x = (value - edge0) / (edge1 - edge0);
	x = LIMAT_CLAMP (x, 0.0f, 1.0f);
	return x * x * (3.0f - 2.0f * x);
}

static inline float limat_quantize (
	float value,
	float resolution)
{
	float tmp;

	if (resolution <= LIMAT_EPSILON)
		return value;
	tmp = fmod (value, resolution);
	if (tmp >= 0.5f * resolution)
		return value - tmp + resolution;
	else if (tmp <= -0.5f * resolution)
		return value - tmp - resolution;
	else
		return value - tmp;
}

#endif

/** @} */
/** @} */

