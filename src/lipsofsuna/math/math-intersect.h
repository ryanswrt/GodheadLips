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
 * \addtogroup LIMatIntersect Intersect
 * @{
 */

#ifndef __MATH_INTERSECT_H__
#define __MATH_INTERSECT_H__

#include "math-aabb.h"
#include "math-vector.h"

/**
 * \brief Gets the intersection points of an AABB and a line.
 * \param self Axis-aligned bounding box.
 * \param p0 Point on the line.
 * \param p1 Point on the line.
 * \param ret Return location for 6 points.
 * \return Number of intersections.
 */
static inline int limat_intersect_aabb_line (
	const LIMatAabb*   self,
	const LIMatVector* p0,
	const LIMatVector* p1,
	LIMatVector*       ret)
{
	int hits;
	float d;
	LIMatVector dir;
	LIMatVector pt;

	hits = 0;
	dir = limat_vector_subtract (*p1, *p0);
	dir = limat_vector_normalize (dir);
	if (dir.x >  LIMAT_VECTOR_EPSILON ||
	    dir.x < -LIMAT_VECTOR_EPSILON)
	{
		/* Left. */
		d = (self->min.x - p0->x) / dir.x;
		pt = limat_vector_multiply (dir, d);
		pt = limat_vector_add (pt, *p0);
		if (pt.y >= self->min.y && pt.y < self->max.y &&
		    pt.z >= self->min.z && pt.z < self->max.z)
			ret[hits++] = limat_vector_init (self->min.x, pt.y, pt.z);

		/* Right. */
		d = (self->max.x - p0->x) / dir.x;
		pt = limat_vector_multiply (dir, d);
		pt = limat_vector_add (pt, *p0);
		if (pt.y >= self->min.y && pt.y < self->max.y &&
		    pt.z >= self->min.z && pt.z < self->max.z)
			ret[hits++] = limat_vector_init (self->max.x, pt.y, pt.z);
	}
	if (dir.y >  LIMAT_VECTOR_EPSILON ||
	    dir.y < -LIMAT_VECTOR_EPSILON)
	{
		/* Bottom. */
		d = (self->min.y - p0->y) / dir.y;
		pt = limat_vector_multiply (dir, d);
		pt = limat_vector_add (pt, *p0);
		if (pt.x >= self->min.x && pt.x < self->max.x &&
		    pt.z >= self->min.z && pt.z < self->max.z)
			ret[hits++] = limat_vector_init (pt.x, self->min.y, pt.z);

		/* Top. */
		d = (self->max.y - p0->y) / dir.y;
		pt = limat_vector_multiply (dir, d);
		pt = limat_vector_add (pt, *p0);
		if (pt.x >= self->min.x && pt.x < self->max.x &&
		    pt.z >= self->min.z && pt.z < self->max.z)
			ret[hits++] = limat_vector_init (pt.x, self->max.y, pt.z);
	}
	if (dir.z >  LIMAT_VECTOR_EPSILON ||
	    dir.z < -LIMAT_VECTOR_EPSILON)
	{
		/* Near. */
		d = (self->min.z - p0->z) / dir.z;
		pt = limat_vector_multiply (dir, d);
		pt = limat_vector_add (pt, *p0);
		if (pt.x >= self->min.x && pt.x < self->max.x &&
		    pt.y >= self->min.y && pt.y < self->max.y)
			ret[hits++] = limat_vector_init (pt.x, pt.y, self->min.z);

		/* Far. */
		d = (self->max.z - p0->z) / dir.z;
		pt = limat_vector_multiply (dir, d);
		pt = limat_vector_add (pt, *p0);
		if (pt.x >= self->min.x && pt.x < self->max.x &&
		    pt.y >= self->min.y && pt.y < self->max.y)
			ret[hits++] = limat_vector_init (pt.x, pt.y, self->max.z);
	}

	return hits;
}

/**
 * \brief Gets the intersection points of an AABB and a line.
 * \param self Axis-aligned bounding box.
 * \param p0 Point on the line.
 * \param p1 Point on the line.
 * \return Nonzero if intersects.
 */
static inline int limat_intersect_aabb_line_fast (
	const LIMatAabb*   self,
	const LIMatVector* p0,
	const LIMatVector* p1)
{
	float d;
	LIMatVector dir;
	LIMatVector pt;

	dir = limat_vector_subtract (*p1, *p0);
	dir = limat_vector_normalize (dir);
	if (dir.x >  LIMAT_VECTOR_EPSILON ||
	    dir.x < -LIMAT_VECTOR_EPSILON)
	{
		/* Left. */
		d = (self->min.x - p0->x) / dir.x;
		pt.y = dir.y * d + p0->x;
		pt.z = dir.z * d + p0->y;
		if (pt.y >= self->min.y && pt.y < self->max.y &&
		    pt.z >= self->min.z && pt.z < self->max.z)
			return 1;

		/* Right. */
		d = (self->max.x - p0->x) / dir.x;
		pt.y = dir.y * d + p0->x;
		pt.z = dir.z * d + p0->y;
		if (pt.y >= self->min.y && pt.y < self->max.y &&
		    pt.z >= self->min.z && pt.z < self->max.z)
			return 1;
	}
	if (dir.y >  LIMAT_VECTOR_EPSILON ||
	    dir.y < -LIMAT_VECTOR_EPSILON)
	{
		/* Bottom. */
		d = (self->min.y - p0->y) / dir.y;
		pt.x = dir.x * d + p0->x;
		pt.z = dir.z * d + p0->z;
		if (pt.x >= self->min.x && pt.x < self->max.x &&
		    pt.z >= self->min.z && pt.z < self->max.z)
			return 1;

		/* Top. */
		d = (self->max.y - p0->y) / dir.y;
		pt.x = dir.x * d + p0->x;
		pt.z = dir.z * d + p0->z;
		if (pt.x >= self->min.x && pt.x < self->max.x &&
		    pt.z >= self->min.z && pt.z < self->max.z)
			return 1;
	}
	if (dir.z >  LIMAT_VECTOR_EPSILON ||
	    dir.z < -LIMAT_VECTOR_EPSILON)
	{
		/* Near. */
		d = (self->min.z - p0->z) / dir.z;
		pt.x = dir.x * d + p0->x;
		pt.y = dir.y * d + p0->y;
		if (pt.x >= self->min.x && pt.x < self->max.x &&
		    pt.y >= self->min.y && pt.y < self->max.y)
			return 1;

		/* Far. */
		d = (self->max.z - p0->z) / dir.z;
		pt.x = dir.x * d + p0->x;
		pt.y = dir.y * d + p0->y;
		if (pt.x >= self->min.x && pt.x < self->max.x &&
		    pt.y >= self->min.y && pt.y < self->max.y)
			return 1;
	}

	return 0;
}

/**
 * \brief Gets the furthest intersection point of an AABB and an infinite ray.
 *
 * This is a picking function that can be used for picking a point on an AABB
 * furthest from the camera.
 *
 * \param self Axis-aligned bounding box.
 * \param eye Eye position vector.
 * \param end End position vector.
 * \param ret Return location for one point.
 * \return Nonzero if found an intersection.
 */
static inline int limat_intersect_aabb_line_far (
	const LIMatAabb*   self,
	const LIMatVector* eye,
	const LIMatVector* end,
	LIMatVector*       ret)
{
	int i;
	int hit;
	int num;
	float d;
	float dist;
	LIMatVector dir;
	LIMatVector point[6];

	num = limat_intersect_aabb_line (self, eye, end, point);
	if (num == 0)
		return 0;
	dist = 0.0f;
	hit = 0;
	dir = limat_vector_subtract (*end, *eye);
	dir = limat_vector_normalize (dir);
	for (i = 0 ; i < num ; i++)
	{
		d = limat_vector_dot (limat_vector_subtract (point[i], *eye), dir);
		if (dist < d)
		{
			hit = 1;
			dist = d;
			*ret = point[i];
		}
	}

	return hit;
}

/**
 * \brief Gets the nearest intersection point of an AABB and an infinite ray.
 *
 * This is a picking function that can be used for picking a point on an AABB
 * nearest to the camera.
 *
 * \param self Axis-aligned bounding box.
 * \param eye Eye position vector.
 * \param end End position vector.
 * \param ret Return location for one point or NULL.
 * \return Nonzero if found an intersection.
 */
static inline int limat_intersect_aabb_line_near (
	const LIMatAabb*   self,
	const LIMatVector* eye,
	const LIMatVector* end,
	LIMatVector*       ret)
{
	int i;
	int hit;
	int num;
	float d;
	float dist;
	LIMatVector dir;
	LIMatVector point[6];

	num = limat_intersect_aabb_line (self, eye, end, point);
	if (num == 0)
		return 0;
	dist = 0.0f;
	hit = 0;
	dir = limat_vector_subtract (*end, *eye);
	dir = limat_vector_normalize (dir);
	for (i = 0 ; i < num ; i++)
	{
		d = limat_vector_dot (limat_vector_subtract (point[i], *eye), dir);
		if (dist >= 0.0f && (!hit || dist < d))
		{
			hit = 1;
			dist = d;
			*ret = point[i];
		}
	}

	return hit;
}

#endif

/** @} */
/** @} */
