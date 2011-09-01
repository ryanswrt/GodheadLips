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
 * \addtogroup LIMatAabb Aabb
 * @{
 */

#ifndef __MATH_AABB_H__
#define __MATH_AABB_H__

#include <string.h>
#include "math-matrix.h"
#include "math-triangle.h"
#include "math-vector.h"

typedef struct _LIMatAabb LIMatAabb;
struct _LIMatAabb
{
	LIMatVector min;
	LIMatVector max;
};

static inline void limat_aabb_init (
	LIMatAabb* self)
{
	memset (self, 0, sizeof (LIMatAabb));
}

/**
 * \brief Initializes an axis-aligned bounding box from a center point and size.
 *
 * \param self Axis-aligned bounding box.
 * \param center Center of the bounding box.
 * \param size Size of the bounding box.
 */
static inline void limat_aabb_init_from_center (
	LIMatAabb*         self,
	const LIMatVector* center,
	const LIMatVector* size)
{
	LIMatVector tmp;

	tmp = limat_vector_multiply (*size, 0.5f);
	self->min = limat_vector_subtract (*center, tmp);
	self->max = limat_vector_add (*center, tmp);
}

/**
 * \brief Initializes an axis-aligned bounding box from minimum and maximum points.
 * \param self Axis-aligned bounding box.
 * \param min Minimum point of the bounding box.
 * \param max Maximum point of the bounding box.
 */
static inline void limat_aabb_init_from_points (
	LIMatAabb*         self,
	const LIMatVector* min,
	const LIMatVector* max)
{
	self->min = *min;
	self->max = *max;
}

/**
 * \brief Updates the bounding box to also enclose a point.
 * \param self Axis-aligned bounding box.
 * \param point Position vector.
 */
static inline void limat_aabb_add_point (
	LIMatAabb*         self,
	const LIMatVector* point)
{
	if (self->min.x > point->x) self->min.x = point->x;
	if (self->min.y > point->y) self->min.y = point->y;
	if (self->min.z > point->z) self->min.z = point->z;
	if (self->max.x < point->x) self->max.x = point->x;
	if (self->max.y < point->y) self->max.y = point->y;
	if (self->max.z < point->z) self->max.z = point->z;
}

/**
 * \brief Creates the maximum bounding box for a transformed box.
 * \param self Axis-aligned bounding box.
 * \param matrix Matrix.
 * \return Axis-aligned bounding box.
 */
static inline LIMatAabb limat_aabb_transform (
	const LIMatAabb    self,
	const LIMatMatrix* matrix)
{
	int i;
	LIMatAabb result;
	LIMatVector v[7];
	LIMatVector min;
	LIMatVector max;

	min = max = limat_matrix_transform (*matrix, self.min);
	v[0] = limat_vector_init (self.min.x, self.min.y, self.max.z);
	v[1] = limat_vector_init (self.min.x, self.max.y, self.min.z);
	v[2] = limat_vector_init (self.min.x, self.max.y, self.max.z);
	v[3] = limat_vector_init (self.max.x, self.min.y, self.min.z);
	v[4] = limat_vector_init (self.max.x, self.min.y, self.max.z);
	v[5] = limat_vector_init (self.max.x, self.max.y, self.min.z);
	v[6] = limat_vector_init (self.max.x, self.max.y, self.max.z);
	for (i = 0 ; i < 7 ; i++)
	{
		v[i] = limat_matrix_transform (*matrix, v[i]);
		if (min.x > v[i].x)
			min.x = v[i].x;
		if (min.y > v[i].y)
			min.y = v[i].y;
		if (min.z > v[i].z)
			min.z = v[i].z;
		if (max.x < v[i].x)
			max.x = v[i].x;
		if (max.y < v[i].y)
			max.y = v[i].y;
		if (max.z < v[i].z)
			max.z = v[i].z;
	}
	limat_aabb_init_from_points (&result, &min, &max);

	return result;
}

/**
 * \brief Lets an axis-aligned bounding box clip a triangle.
 *
 * Uses the Sutherland-Hodgman algorithm with six iterations.
 *
 * \param self Axis-aligned bounding box.
 * \param triangle Triangle to be clipped.
 * \param result Return location with room for at least 9 points.
 * \return Number vertices in the clipped triangle.
 */
static inline int limat_aabb_clip_triangle (
	const LIMatAabb*     self,
	const LIMatTriangle* triangle,
	LIMatVector*         result)
{
	int curr;
	int prev;
	int currin;
	int previn;
	int currlen;
	int prevlen;
	int plane;
	LIMatVector buffer[18];
	LIMatVector* tmpbuf;
	LIMatVector* prevbuf = buffer + 0;
	LIMatVector* currbuf = buffer + 9;
	LIMatPlane p[6] =
	{
		{ 1, 0, 0, self->min.x }, /* Left. */
		{ -1, 0, 0, -self->max.x }, /* Right. */
		{ 0, 1, 0, self->min.y }, /* Bottom. */
		{ 0, -1, 0, -self->max.y }, /* Top. */
		{ 0, 0, 1, self->min.z }, /* Front. */
		{ 0, 0, -1, -self->max.z }, /* Back. */
	};

	prevlen = 3;
	prevbuf[0] = triangle->vertices[0];
	prevbuf[1] = triangle->vertices[1];
	prevbuf[2] = triangle->vertices[2];
	for (plane = 0 ; plane < 6 ; plane++)
	{
		curr = prevlen;
		currlen = 0;
		prev = 0;
		previn = limat_plane_signed_distance_to_point (p + plane, prevbuf) >= 0.0f;
		while (curr--)
		{
			currin = limat_plane_signed_distance_to_point (p + plane, prevbuf + curr) >= 0.0f;
			if (currin)
			{
				if (!previn)
				{
					limat_plane_intersects_segment (p + plane,
						prevbuf + prev, prevbuf + curr, currbuf + currlen++);
				}
				currbuf[currlen++] = prevbuf[curr];
			}
			else if (previn)
			{
				limat_plane_intersects_segment (p + plane,
					prevbuf + prev, prevbuf + curr, currbuf + currlen++);
			}
			previn = currin;
			prev = curr;
		}
		if (!currlen)
			return 0;
		tmpbuf = currbuf;
		currbuf = prevbuf;
		prevbuf = tmpbuf;
		prevlen = currlen;
	}
	memcpy (result, prevbuf, prevlen * sizeof (LIMatVector));
	return prevlen;
}

/**
 * \brief Checks if two axis-aligned bounding boxes intersect.
 * \param self Axis-aligned bounding box.
 * \param aabb Axis-aligned bounding box.
 * \return Nonzero if the boxes intersect.
 */
static inline int limat_aabb_intersects_aabb (
	const LIMatAabb* self,
	const LIMatAabb* aabb)
{
	if (self->min.x >= aabb->max.x ||
	    self->min.y >= aabb->max.y ||
	    self->min.z >= aabb->max.z)
		return 0;
	if (aabb->min.x >= self->max.x ||
	    aabb->min.y >= self->max.y ||
	    aabb->min.z >= self->max.z)
		return 0;
	return 1;
}

/**
 * \brief Calculates a bounding box that encloses both of the passed AABBs.
 * \param self Axis-aligned bounding box.
 * \param aabb Axis-aligned bounding box.
 * \return Axis-aligned bounding box.
 */
static inline LIMatAabb limat_aabb_union (
	const LIMatAabb self,
	const LIMatAabb aabb)
{
	LIMatAabb ret;

	ret.min.x = self.min.x < aabb.min.x? self.min.x : aabb.min.x;
	ret.min.y = self.min.y < aabb.min.y? self.min.y : aabb.min.y;
	ret.min.z = self.min.z < aabb.min.z? self.min.z : aabb.min.z;
	ret.max.x = self.max.x > aabb.max.x? self.max.x : aabb.max.x;
	ret.max.y = self.max.y > aabb.max.y? self.max.y : aabb.max.y;
	ret.max.z = self.max.z > aabb.max.z? self.max.z : aabb.max.z;

	return ret;
}

/**
 * \brief Gets the center of the AABB.
 * \param self Axis-aligned bounding box.
 * \param result Return location for the center.
 */
static inline void limat_aabb_get_center (
	const LIMatAabb* self,
	LIMatVector*     result)
{
	result->x = 0.5f * (self->min.x + self->max.x);
	result->y = 0.5f * (self->min.y + self->max.y);
	result->z = 0.5f * (self->min.z + self->max.z);
}

#endif

/** @} */
/** @} */
