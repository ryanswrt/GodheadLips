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
 * \addtogroup LIMatFrustum Frustum
 * @{
 */

#ifndef __MATH_FRUSTUM_H__
#define __MATH_FRUSTUM_H__

#include "math-aabb.h"
#include "math-generic.h"
#include "math-matrix.h"
#include "math-plane.h"

typedef struct _LIMatFrustum LIMatFrustum;
struct _LIMatFrustum
{
	LIMatPlane planes[6];
};

/**
 * \brief Initializes a frustum from camera matrices.
 *
 * \param self Frustum.
 * \param modelview Modelview matrix.
 * \param projection Projection matrix.
 */
static inline void
limat_frustum_init (LIMatFrustum*      self,
                    const LIMatMatrix* modelview,
                    const LIMatMatrix* projection)
{
	int i;
	float len;
	LIMatMatrix m = limat_matrix_multiply (*projection, *modelview);

	self->planes[0].x = m.m[3] + m.m[0];
	self->planes[0].y = m.m[7] + m.m[4];
	self->planes[0].z = m.m[11] + m.m[8];
	self->planes[0].w = m.m[15] + m.m[12];
	self->planes[1].x = m.m[3] - m.m[0];
	self->planes[1].y = m.m[7] - m.m[4];
	self->planes[1].z = m.m[11] - m.m[8];
	self->planes[1].w = m.m[15] - m.m[12];
	self->planes[2].x = m.m[3] + m.m[1];
	self->planes[2].y = m.m[7] + m.m[5];
	self->planes[2].z = m.m[11] + m.m[9];
	self->planes[2].w = m.m[15] + m.m[13];
	self->planes[3].x = m.m[3] - m.m[1];
	self->planes[3].y = m.m[7] - m.m[5];
	self->planes[3].z = m.m[11] - m.m[9];
	self->planes[3].w = m.m[15] - m.m[13];
	self->planes[4].x = m.m[3] + m.m[2];
	self->planes[4].y = m.m[7] + m.m[6];
	self->planes[4].z = m.m[11] + m.m[10];
	self->planes[4].w = m.m[15] + m.m[14];
	self->planes[5].x = m.m[3] - m.m[2];
	self->planes[5].y = m.m[7] - m.m[6];
	self->planes[5].z = m.m[11] - m.m[10];
	self->planes[5].w = m.m[15] - m.m[14];
	for (i = 0 ; i < 6 ; i++)
	{
		len = sqrt (self->planes[i].x * self->planes[i].x +
		            self->planes[i].y * self->planes[i].y +
		            self->planes[i].z * self->planes[i].z);
		if (len > LIMAT_EPSILON)
		{
			self->planes[i].x /= len;
			self->planes[i].y /= len;
			self->planes[i].z /= len;
			self->planes[i].w /= len;
		}
	}
}

/**
 * \brief Checks if the point is outside of the frustum.
 *
 * \param self Frustum.
 * \param point Point.
 * \return Nonzero if the point is outside of the frustum.
 */
static inline int
limat_frustum_cull_point (const LIMatFrustum* self,
                          const LIMatVector*  point)
{
	int i;
	float len;

	for (i = 0 ; i < 6 ; i++)
	{
		len = self->planes[i].x * point->x +
		      self->planes[i].y * point->y +
		      self->planes[i].z * point->z +
		      self->planes[i].w;
		if (len < 0.0f)
			return 1;
	}

	return 0;
}

/**
 * \brief Checks if all the points are outside of the frustum.
 *
 * The object is only culled if all the points are on the outer
 * side of one of the frustum planes. This guarantees that any
 * polygon soup constructed of the vertices is also completely
 * outside if nonzero is returned by this function. However, it
 * doesn't guarantee that the object actually intersects the
 * frustum if zero is returned.
 *
 * \param self Frustum.
 * \param points Array of points.
 * \param count Number of points.
 * \return Nonzero if all the points are outside of the frustum.
 */
static inline int
limat_frustum_cull_points (const LIMatFrustum* self,
                           const LIMatVector*  points,
                           int                 count)
{
	int i;
	int j;
	float len;

	for (i = 0 ; i < 6 ; i++)
	{
		for (j = 0 ; j < count ; j++)
		{
			len = self->planes[i].x * points[j].x +
				  self->planes[i].y * points[j].y +
				  self->planes[i].z * points[j].z +
				  self->planes[i].w;
			if (len >= 0.0f)
				break;
		}
		if (j == count)
			return 1;
	}

	return 0;
}

/**
 * \brief Checks if the sphere is completely outside of the frustum.
 *
 * \param self Frustum.
 * \param point Center of the sphere.
 * \param radius Radius of the sphere.
 * \return Nonzero if the sphere is outside of the frustum.
 */
static inline int
limat_frustum_cull_sphere (const LIMatFrustum* self,
                           const LIMatVector*  point,
                           float               radius)
{
	int i;
	float len;

	for (i = 0 ; i < 6 ; i++)
	{
		len = self->planes[i].x * point->x +
		      self->planes[i].y * point->y +
		      self->planes[i].z * point->z +
		      self->planes[i].w;
		if (len < -radius)
			return 1;
	}

	return 1;
}

/**
 * \brief Checks if the bounding box is completely outside of the frustum.
 *
 * This isn't a fully accurate check since objects near intersection
 * points of the frustum planes may be reported to intersect even though
 * they are actually completely outside. The algorithm is guaranteed to
 * always error to direction of reporting too many intersections, though,
 * so it should be usable for frustum culling.
 *
 * \param self Frustum.
 * \param aabb Bounding box.
 * \return Nonzero if the box is outside of the frustum.
 */
static inline int
limat_frustum_cull_aabb (const LIMatFrustum* self,
                         const LIMatAabb*    aabb)
{
	LIMatVector min = aabb->min;
	LIMatVector max = aabb->max;
	LIMatVector points[8] =
	{
		{ min.x, min.y, min.z },
		{ min.x, min.y, max.z },
		{ min.x, max.y, min.z },
		{ min.x, max.y, max.z },
		{ max.x, min.y, min.z },
		{ max.x, min.y, max.z },
		{ max.x, max.y, min.z },
		{ max.x, max.y, max.z },
	};

	return limat_frustum_cull_points (self, points, 8);
}

#endif

/** @} */
/** @} */
