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
 * \addtogroup LIMatTriangle Triangle
 * @{
 */

#ifndef __MATH_TRIANGLE_H__
#define __MATH_TRIANGLE_H__

#include <stdio.h>
#include "math-plane.h"
#include "math-rectangle.h"
#include "math-types.h"
#include "math-vector.h"

/**
 * \brief Sets the triangle variables from vertices.
 *
 * \param self Triangle.
 * \param vertex0 Vertex.
 * \param vertex1 Vertex.
 * \param vertex2 Vertex.
 */
static inline void
limat_triangle_set_from_points (LIMatTriangle*     self,
                                const LIMatVector* vertex0,
                                const LIMatVector* vertex1,
                                const LIMatVector* vertex2)
{
	limat_plane_init_from_points (&self->plane, vertex0, vertex1, vertex2);
	self->vertices[0] = *vertex0;
	self->vertices[1] = *vertex1;
	self->vertices[2] = *vertex2;
}

/**
 * \brief Checks if the triangle encloses the point.
 *
 * \param vertex0 Vertex of the triangle.
 * \param vertex1 Vertex of the triangle.
 * \param vertex2 Vertex of the triangle.
 * \param point Point.
 * \return Nonzero if enclosed.
 */
static inline int
limat_triangle_intersects_point (const LIMatVector* vertex0,
                                 const LIMatVector* vertex1,
                                 const LIMatVector* vertex2,
                                 const LIMatVector* point)
{
	LIMatVector p;
	LIMatVector a;
	LIMatVector b;

	/* First edge. */
	p = limat_vector_subtract (*point, *vertex0);
	a = limat_vector_subtract (*vertex1, *vertex0);
	b = limat_vector_subtract (*vertex2, *vertex0);
	if (limat_vector_dot (
		limat_vector_cross (b, a),
		limat_vector_cross (b, p)) < 0)
		return 0;

	/* Second edge. */
	p = limat_vector_subtract (*point, *vertex1);
	a = limat_vector_subtract (*vertex2, *vertex1);
	b = limat_vector_subtract (*vertex0, *vertex1);
	if (limat_vector_dot (
		limat_vector_cross (b, a),
		limat_vector_cross (b, p)) < 0)
		return 0;

	/* Third edge. */
	p = limat_vector_subtract (*point, *vertex2);
	a = limat_vector_subtract (*vertex0, *vertex2);
	b = limat_vector_subtract (*vertex1, *vertex2);
	if (limat_vector_dot (
		limat_vector_cross (b, a),
		limat_vector_cross (b, p)) < 0)
		return 0;

	return 1;
}

static inline int limat_triangle_intersects_ray (
	const LIMatTriangle* self,
	const LIMatVector*   ray0,
	const LIMatVector*   ray1,
	LIMatVector*         result)
{
	if (!limat_plane_intersects_segment (&self->plane, ray0, ray1, result))
		return 0;
	return limat_triangle_intersects_point (self->vertices + 0,
		self->vertices + 1, self->vertices + 2, result);
}

/**
 * \brief Checks if the triangle intersects with another.
 *
 * \param self Triangle.
 * \param triangle Triangle.
 * \param point0 Return location for the intersection or NULL.
 * \param point1 Return location for the intersection or NULL.
 * \return Nonzero if intersects.
 */
static inline int
limat_triangle_intersects_triangle (const LIMatTriangle* self,
                                    const LIMatTriangle* triangle,
                                    LIMatVector*         point0,
                                    LIMatVector*         point1)
{
	float t0;
	float t1;
	float dot0;
	float dot1;
	LIMatVector ref;
	LIMatVector i00;
	LIMatVector i01;
	LIMatVector i10;
	LIMatVector i11;

	/* Get two coinciding intersection line segments. */
	if (!limat_plane_intersects_triangle (&self->plane, triangle, &i00, &i01) ||
	    !limat_plane_intersects_triangle (&triangle->plane, self, &i10, &i11))
		return 0;

	/* Find the intersecting portion of the coinciding segments. */
	ref = limat_vector_subtract (i01, i00);
	dot0 = limat_vector_dot (ref, limat_vector_subtract (i10, i00));
	dot1 = limat_vector_dot (ref, limat_vector_subtract (i11, i00));
	t0 = LIMAT_MAX (0.0f, LIMAT_MIN (1.0f, dot0));
	t1 = LIMAT_MIN (1.0f, LIMAT_MAX (0.0f, dot1));
	if (t0 > t1)
		return 0;
	if (point0 != NULL)
		*point0 = limat_vector_add (i00, limat_vector_multiply (ref, t0));
	if (point1 != NULL)
		*point1 = limat_vector_add (i00, limat_vector_multiply (ref, t1));
	return 1;
}

/**
 * \brief Checks if the triangle intersects an axis-aligned bounding box.
 *
 * \param self Triangle.
 * \param origin Origin on the box.
 * \param size Side lengths of the box.
 * \return Nonzero if intersects.
 */
static inline int
limat_triangle_intersects_aabb (const LIMatTriangle* self,
                                const LIMatVector*   origin,
                                const LIMatVector*   size)
{
	float x0 = origin->x;
	float x1 = origin->x + size->x;
	float y0 = origin->y;
	float y1 = origin->y + size->y;
	float z0 = origin->z;
	float z1 = origin->z + size->z;
	LIMatVector l[2];
	LIMatPlane p[6] =
	{
		{ 1, 0, 0, x0 }, /* Left. */
		{ 1, 0, 0, x1 }, /* Right. */
		{ 0, 1, 0, y0 }, /* Bottom. */
		{ 0, 1, 0, y1 }, /* Top. */
		{ 0, 0, 1, z0 }, /* Front. */
		{ 0, 0, 1, z1 }, /* Back. */
	};
	LIMatRectangle r[3] =
	{
		{ y0, z0, size->y, size->z }, /* Left and right. */
		{ x0, z0, size->x, size->z }, /* Bottom and top. */
		{ x0, y0, size->x, size->y }, /* Front and back. */
	};

	/* Test if a vertex is inside. */
	if (x0 <= self->vertices[0].x && self->vertices[0].x <= x1 &&
	    y0 <= self->vertices[0].y && self->vertices[0].y <= y1 &&
	    z0 <= self->vertices[0].z && self->vertices[0].z <= z1)
		return 1;
	if (x0 <= self->vertices[1].x && self->vertices[1].x <= x1 &&
	    y0 <= self->vertices[1].y && self->vertices[1].y <= y1 &&
	    z0 <= self->vertices[1].z && self->vertices[1].z <= z1)
		return 1;
	if (x0 <= self->vertices[2].x && self->vertices[2].x <= x1 &&
	    y0 <= self->vertices[2].y && self->vertices[2].y <= y1 &&
	    z0 <= self->vertices[2].z && self->vertices[2].z <= z1)
		return 1;

	/* Left and right. */
	if (limat_plane_intersects_triangle (p + 0, self, l + 0, l + 1) &&
	    limat_rectangle_intersects_segment (r + 0, l[0].y, l[0].z, l[1].y, l[1].z))
		return 1;
	if (limat_plane_intersects_triangle (p + 1, self, l + 0, l + 1) &&
	    limat_rectangle_intersects_segment (r + 0, l[0].y, l[0].z, l[1].y, l[1].z))
		return 1;

	/* Bottom and top. */
	if (limat_plane_intersects_triangle (p + 2, self, l + 0, l + 1) &&
	    limat_rectangle_intersects_segment (r + 1, l[0].x, l[0].z, l[1].x, l[1].z))
		return 1;
	if (limat_plane_intersects_triangle (p + 3, self, l + 0, l + 1) &&
	    limat_rectangle_intersects_segment (r + 1, l[0].x, l[0].z, l[1].x, l[1].z))
		return 1;

	/* Front and back. */
	if (limat_plane_intersects_triangle (p + 4, self, l + 0, l + 1) &&
	    limat_rectangle_intersects_segment (r + 2, l[0].x, l[0].y, l[1].x, l[1].y))
		return 1;
	if (limat_plane_intersects_triangle (p + 5, self, l + 0, l + 1) &&
	    limat_rectangle_intersects_segment (r + 2, l[0].x, l[0].y, l[1].x, l[1].y))
		return 1;

	return 0;
}

#endif

/** @} */
/** @} */
