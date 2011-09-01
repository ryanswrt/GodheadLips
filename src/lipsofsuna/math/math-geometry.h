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
 * \addtogroup LIMatEllipsoid Ellipsoid
 * @{
 */

#ifndef __MATH_GEOMETRY_H__
#define __MATH_GEOMETRY_H__

#include "math-types.h"
#include "math-vector.h"

static inline LIMatVector
limat_ellipsoid_map_point (LIMatVector center,
                           LIMatVector size,
                           LIMatVector value)
{
	LIMatVector result;

	result.x = (value.x - center.x) / size.x;
	result.y = (value.y - center.y) / size.y;
	result.z = (value.z - center.z) / size.z;
	return result;
}

static inline LIMatVector
limat_ellipsoid_map_vector (LIMatVector center,
                            LIMatVector size,
                            LIMatVector value)
{
	LIMatVector result;

	result.x = value.x / size.x;
	result.y = value.y / size.y;
	result.z = value.z / size.z;
	return result;
}

static inline LIMatVector
limat_ellipsoid_unmap_point (LIMatVector center,
                             LIMatVector size,
                             LIMatVector value)
{
	LIMatVector result;

/*
	result.x = (value.x + center.x) * size.x;
	result.y = (value.y + center.y) * size.y;
	result.z = (value.z + center.z) * size.z;
*/
	result.x = value.x * size.x + center.x;
	result.y = value.y * size.y + center.y;
	result.z = value.z * size.z + center.z;
	return result;
}

static inline LIMatVector
limat_ellipsoid_unmap_vector (LIMatVector center,
                              LIMatVector size,
                              LIMatVector value)
{
	LIMatVector result;

	result.x = value.x * size.x;
	result.y = value.y * size.y;
	result.z = value.z * size.z;
	return result;
}

/** @} */
/** @} */

/**
 * \addtogroup LIMat Math
 * @{
 * \addtogroup LIMatGeometry Geometry
 * @{
 */

static inline LIMatVector
limat_nearest_plane_to_origin (LIMatPlane plane)
{
	LIMatVector n;

	n = limat_vector_init (plane.x, plane.y, plane.z);
	return limat_vector_multiply (n, plane.w);
}

static inline LIMatVector
limat_nearest_plane_to_point (LIMatPlane  plane,
                              LIMatVector p)
{
	float d;
	LIMatVector n;

	/* FIXME: Unsure. */
	n = limat_vector_init (plane.x, plane.y, plane.z);
	d = limat_vector_dot (n, p) + plane.w;
	return limat_vector_multiply (n, d);
}

static inline LIMatVector
limat_nearest_segment_to_origin (LIMatVector p0,
                                 LIMatVector p1)
{
	float t;
	LIMatVector v;

	v = limat_vector_subtract (p1, p0);
	t = limat_vector_dot (p0, v);
	if (t <= 0.0f) return p0;
	if (t >= 1.0f) return p1;
	return limat_vector_add (p0, limat_vector_multiply (v, t));
}

static inline LIMatVector
limat_nearest_segment_to_point (LIMatVector p0,
                                LIMatVector p1,
                                LIMatVector p)
{
	float t;
	LIMatVector v;

	v = limat_vector_subtract (p1, p0);
	t = limat_vector_dot (limat_vector_subtract (p0, p), v);
	if (t <= 0.0f) return p0;
	if (t >= 1.0f) return p1;
	return limat_vector_add (p0, limat_vector_multiply (v, t));
}

static inline LIMatVector
limat_nearest_triangle_to_origin (LIMatPlane  plane,
                                  LIMatVector p0,
                                  LIMatVector p1,
                                  LIMatVector p2)
{
	float d0;
	float d1;
	float d2;
	LIMatVector i;
	LIMatVector i0;
	LIMatVector i1;
	LIMatVector i2;

	i = limat_nearest_plane_to_origin (plane);
	if (limat_triangle_intersects_point (&i, &p0, &p1, &p2))
		return i;
	i0 = limat_nearest_segment_to_origin (p0, p1);
	i1 = limat_nearest_segment_to_origin (p1, p2);
	i2 = limat_nearest_segment_to_origin (p2, p0);
	d0 = limat_vector_dot (i0, i0);
	d1 = limat_vector_dot (i1, i1);
	d2 = limat_vector_dot (i2, i2);
	if (d0 < d1 && d0 < d2)
		return i0;
	if (d1 < d2)
		return i1;
	return i2;
}

static inline LIMatVector
limat_nearest_triangle_to_point (LIMatPlane  plane,
                                 LIMatVector p0,
                                 LIMatVector p1,
                                 LIMatVector p2,
                                 LIMatVector p)
{
	float d0;
	float d1;
	float d2;
	LIMatVector i;
	LIMatVector i0;
	LIMatVector i1;
	LIMatVector i2;

	i = limat_nearest_plane_to_point (plane, p);
	if (limat_triangle_intersects_point (&i, &p0, &p1, &p2))
		return i;
	p0 = limat_vector_subtract (p0, p);
	p1 = limat_vector_subtract (p1, p);
	p2 = limat_vector_subtract (p2, p);
	i0 = limat_nearest_segment_to_origin (p0, p1);
	i1 = limat_nearest_segment_to_origin (p1, p2);
	i2 = limat_nearest_segment_to_origin (p2, p0);
	d0 = limat_vector_dot (i0, i0);
	d1 = limat_vector_dot (i1, i1);
	d2 = limat_vector_dot (i2, i2);
	if (d0 < d1 && d0 < d2)
		return limat_vector_add (i0, p);
	if (d1 < d2)
		return limat_vector_add (i1, p);
	return limat_vector_add (i2, p);
}

static inline LIMatVector
limat_nearest_triangle_to_ellipsoid (LIMatPlane  plane,
                                     LIMatVector p0,
                                     LIMatVector p1,
                                     LIMatVector p2,
                                     LIMatVector center,
                                     LIMatVector size)
{
	LIMatVector p;

	p0 = limat_vector_init (p0.x / size.x, p0.y / size.y, p0.z / size.z);
	p1 = limat_vector_init (p1.x / size.x, p1.y / size.y, p1.z / size.z);
	p2 = limat_vector_init (p2.x / size.x, p2.y / size.y, p2.z / size.z);
	center = limat_vector_init (center.x / size.x, center.y / size.y, center.z / size.z);
	p = limat_nearest_triangle_to_point (plane, p0, p1, p2, center);
	p = limat_vector_init (p.x * size.x, p.y * size.y, p.z * size.z);

	return p;
}

static inline LIMatVector
limat_clamp_point_to_triangle (LIMatVector p,
                               LIMatVector p0,
                               LIMatVector p1,
                               LIMatVector p2)
{
	float d0;
	float d1;
	float d2;
	LIMatVector i0;
	LIMatVector i1;
	LIMatVector i2;

	p0 = limat_vector_subtract (p0, p);
	p1 = limat_vector_subtract (p1, p);
	p2 = limat_vector_subtract (p2, p);
	i0 = limat_nearest_segment_to_origin (p0, p1);
	i1 = limat_nearest_segment_to_origin (p1, p2);
	i2 = limat_nearest_segment_to_origin (p2, p0);
	d0 = limat_vector_dot (i0, i0);
	d1 = limat_vector_dot (i1, i1);
	d2 = limat_vector_dot (i2, i2);
	if (d0 < d1 && d0 < d2)
		return limat_vector_add (i0, p);
	if (d1 < d2)
		return limat_vector_add (i1, p);
	return limat_vector_add (i2, p);
}

#endif

/** @} */
/** @} */
