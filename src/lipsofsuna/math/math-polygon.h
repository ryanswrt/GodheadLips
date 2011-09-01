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
 * \addtogroup LIMatPolygon Polygon
 * @{
 */

#ifndef __MATH_POLYGON_H__
#define __MATH_POLYGON_H__

#include <lipsofsuna/system.h>
#include "math-aabb.h"
#include "math-plane.h"
#include "math-types.h"
#include "math-vertex.h"

typedef struct _LIMatPolygon LIMatPolygon;
struct _LIMatPolygon
{
	const LIMatVtxops* ops;
	void* data;
	LIMatVector normal;
	struct
	{
		int capacity;
		int count;
		void* vertices;
	} vertices;
};

/**
 * \brief Creates a new polygon.
 *
 * \param ops Vertex access operations.
 * \param normal Normal vector.
 * \param vertices Array of vertices.
 * \param count Number of vertices in the array.
 * \return New polygon or NULL.
 */
static inline LIMatPolygon*
limat_polygon_new (const LIMatVtxops* ops,
                   const LIMatVector* normal,
                   const void*        vertices,
                   int                count)
{
	LIMatPolygon* self;

	/* Allocate self. */
	self = (LIMatPolygon*) lisys_calloc (1, sizeof (LIMatPolygon));
	if (self == NULL)
		return NULL;
	self->ops = ops;
	self->normal = *normal;
	if (count == 0)
		return self;

	/* Allocate vertices. */
	self->vertices.capacity = count;
	self->vertices.vertices = lisys_malloc (count * ops->size);
	if (self->vertices.vertices == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Copy vertices. */
	memcpy (self->vertices.vertices, vertices, count * ops->size);
	self->vertices.count = count;

	return self;
}

/**
 * \brief Creates a new polygon with three vertices.
 *
 * \param ops Vertex access operations.
 * \param v0 Vertex.
 * \param v1 Vertex.
 * \param v2 Vertex.
 * \return New polygon or NULL.
 */
static inline LIMatPolygon*
limat_polygon_new_from_triangle (const LIMatVtxops* ops,
                                 const void*        v0,
                                 const void*        v1,
                                 const void*        v2)
{
	LIMatPolygon* self;
	LIMatVector coord0;
	LIMatVector coord1;
	LIMatVector coord2;

	/* Allocate self. */
	self = (LIMatPolygon*) lisys_calloc (1, sizeof (LIMatPolygon));
	if (self == NULL)
		return NULL;
	self->ops = ops;
	ops->getcoord (v0, &coord0);
	ops->getcoord (v1, &coord1);
	ops->getcoord (v2, &coord2);
	self->normal = limat_vector_normalize (
		limat_vector_cross (
		limat_vector_subtract (coord2, coord1),
		limat_vector_subtract (coord1, coord0)));

	/* Allocate vertices. */
	self->vertices.capacity = 3;
	self->vertices.count = 3;
	self->vertices.vertices = lisys_malloc (3 * ops->size);
	if (self->vertices.vertices == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	memcpy ((char*) self->vertices.vertices + 0 * ops->size, v0, ops->size);
	memcpy ((char*) self->vertices.vertices + 1 * ops->size, v1, ops->size);
	memcpy ((char*) self->vertices.vertices + 2 * ops->size, v2, ops->size);

	return self;
}

/**
 * \brief Frees the polygon.
 *
 * \param self Polygon.
 */
static inline void
limat_polygon_free (LIMatPolygon* self)
{
	lisys_free (self->vertices.vertices);
	lisys_free (self);
}

/**
 * \brief Appends vertices to the polygon.
 *
 * \param self Polygon.
 * \param vertices Array of vertices.
 * \param count Number of vertices in the array.
 * \return Nonzero on success.
 */
static inline int
limat_polygon_add_vertices (LIMatPolygon* self,
                            const void*   vertices,
                            int           count)
{
	int num;
	LIMatVector* tmp;
	const LIMatVtxops* ops = self->ops;

	num = self->vertices.count + count;
	if (num > self->vertices.capacity)
	{
		tmp = (LIMatVector*) lisys_realloc (self->vertices.vertices, num * ops->size);
		if (tmp == NULL)
			return 0;
		self->vertices.vertices = tmp;
		self->vertices.capacity = num;
	}
	memcpy ((char*) self->vertices.vertices + self->vertices.count * ops->size, vertices, count * ops->size);
	self->vertices.count += count;

	return 1;
}

/**
 * \brief Returns the part of the polygon in front of the plane.
 *
 * If the algorithm runs out of memory, zero is returned and the returned
 * polygon misses one or more vertices.
 *
 * Uses the Sutherland-Hodgman algorithm.
 *
 * \param self Polygon.
 * \param plane Plane.
 * \param front Return location for the polygon on the front side.
 * \return Nonzero on success.
 */
static inline int
limat_polygon_clip (const LIMatPolygon* self,
                    const LIMatPlane*   plane,
                    LIMatPolygon*       front)
{
	int i;
	int ret = 1;
	int curr_in;
	int prev_in;
	float frac;
	void* curr_vtx;
	void* prev_vtx;
	void* tmp_vtx;
	LIMatVector curr_coord;
	LIMatVector prev_coord;
	LIMatVector tmp_coord;
	const LIMatVtxops* ops = self->ops;

	/* Initialize. */
	front->data = self->data;
	front->normal = self->normal;
	front->vertices.count = 0;
	if (!self->vertices.count)
		return 1;
	prev_vtx = (char*) self->vertices.vertices + (self->vertices.count - 1) * ops->size;
	ops->getcoord (prev_vtx, &prev_coord);
	prev_in = limat_plane_signed_distance_to_point (plane, &prev_coord) >= 0.0f;
	tmp_vtx = lisys_malloc (ops->size);
	if (tmp_vtx == NULL)
		return 0;

	/* Calculate the vertices of the new polygons. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		curr_vtx = (char*) self->vertices.vertices + i * ops->size;
		ops->getcoord (curr_vtx, &curr_coord);
		curr_in = limat_plane_signed_distance_to_point (plane, &curr_coord) >= 0.0f;
		if (curr_in)
		{
			if (!prev_in)
			{
				limat_plane_intersects_segment (plane, &prev_coord, &curr_coord, &tmp_coord);
				frac = limat_vector_get_length (limat_vector_subtract (prev_coord, tmp_coord)) /
				       limat_vector_get_length (limat_vector_subtract (prev_coord, curr_coord));
				ops->setcoord (tmp_vtx, &tmp_coord);
				ops->interpolate (curr_vtx, prev_vtx, frac, tmp_vtx);
				ret &= limat_polygon_add_vertices (front, tmp_vtx, 1);
			}
			ret &= limat_polygon_add_vertices (front, curr_vtx, 1);
		}
		else
		{
			if (prev_in)
			{
				limat_plane_intersects_segment (plane, &prev_coord, &curr_coord, &tmp_coord);
				frac = limat_vector_get_length (limat_vector_subtract (prev_coord, tmp_coord)) /
				       limat_vector_get_length (limat_vector_subtract (prev_coord, curr_coord));
				ops->setcoord (tmp_vtx, &tmp_coord);
				ops->interpolate (curr_vtx, prev_vtx, frac, tmp_vtx);
				ret &= limat_polygon_add_vertices (front, tmp_vtx, 1);
			}
		}
		prev_coord = curr_coord;
		prev_vtx = curr_vtx;
		prev_in = curr_in;
	}

	lisys_free (tmp_vtx);

	return ret;
}

/**
 * \brief Returns the part of the polygon in front of all the planes.
 *
 * If the algorithm runs out of memory, zero is returned and the returned
 * polygon misses one or more vertices.
 *
 * Uses the Sutherland-Hodgman algorithm.
 *
 * \param self Polygon.
 * \param planes Array of planes.
 * \param count Number of planes.
 * \param front Return location for the polygon on the front side.
 * \return Nonzero on success.
 */
static inline int
limat_polygon_clip_with_planes (const LIMatPolygon* self,
                                const LIMatPlane*   planes,
                                int                 count,
                                LIMatPolygon*       front)
{
	int i;
	int ret = 1;
	LIMatPolygon tmp;
	const LIMatVtxops* ops = self->ops;

	/* Special cases. */
	if (count == 0)
	{
		front->vertices.count = 0;
		return limat_polygon_add_vertices (front, self->vertices.vertices, self->vertices.count);
	}
	if (count == 1)
		return limat_polygon_clip (self, planes, front);

	/* Allocate a temporary polygon. */
	tmp.ops = ops;
	tmp.vertices.capacity = self->vertices.count + count;
	tmp.vertices.count = 0;
	tmp.vertices.vertices = lisys_malloc (tmp.vertices.capacity * ops->size);
	if (tmp.vertices.vertices == NULL)
		return 0;

	/* Let each plane clip. */
	if (count % 2)
	{
		ret &= limat_polygon_clip (self, planes, front);
		for (i = 1 ; i < count ; i += 2)
		{
			ret &= limat_polygon_clip (front, planes + i, &tmp);
			ret &= limat_polygon_clip (&tmp, planes + i + 1, front);
		}
	}
	else
	{
		ret &= limat_polygon_clip (self, planes, &tmp);
		ret &= limat_polygon_clip (&tmp, planes + 1, front);
		for (i = 2 ; i < count ; i += 2)
		{
			ret &= limat_polygon_clip (front, planes + i, &tmp);
			ret &= limat_polygon_clip (&tmp, planes + i + 1, front);
		}
	}

	/* Free the temporary polygon. */
	lisys_free (tmp.vertices.vertices);

	return ret;
}

/**
 * \brief Returns the part of the polygon inside the bounding box.
 *
 * If the algorithm runs out of memory, zero is returned and the returned
 * polygon misses one or more vertices.
 *
 * Uses the Sutherland-Hodgman algorithm.
 *
 * \param self Polygon.
 * \param bounds Bounding box.
 * \param front Return location for the polygon inside the box.
 * \return Nonzero on success.
 */
static inline int
limat_polygon_clip_with_aabb (const LIMatPolygon* self,
                              const LIMatAabb*    bounds,
                              LIMatPolygon*       front)
{
	LIMatPlane p[6] =
	{
		{ 1, 0, 0, bounds->min.x }, /* Left. */
		{ -1, 0, 0, -bounds->max.x }, /* Right. */
		{ 0, 1, 0, bounds->min.y }, /* Bottom. */
		{ 0, -1, 0, -bounds->max.y }, /* Top. */
		{ 0, 0, 1, bounds->min.z }, /* Front. */
		{ 0, 0, -1, -bounds->max.z }, /* Back. */
	};

	return limat_polygon_clip_with_planes (self, p, 6, front);
}

/**
 * \brief Splits the polygon in two parts by the plane.
 *
 * If the algorithm runs out of memory, zero is returned and at least one
 * of the resulting polygons misses one or more vertices.
 *
 * Uses the Sutherland-Hodgman algorithm.
 *
 * \param self Polygon.
 * \param front Return location for the polygon on the front side.
 * \param back Return location for the polygon on the back side.
 * \param plane Plane.
 * \return Nonzero on success.
 */
static inline int
limat_polygon_split (const LIMatPolygon* self,
                     const LIMatPlane*   plane,
                     LIMatPolygon*       front,
                     LIMatPolygon*       back)
{
	int i;
	int ret = 1;
	int curr_in;
	int prev_in;
	float frac;
	void* curr_vtx;
	void* prev_vtx;
	void* tmp_vtx;
	LIMatVector curr_coord;
	LIMatVector prev_coord;
	LIMatVector tmp_coord = { 0.0f, 0.0f, 0.0f };
	const LIMatVtxops* ops = self->ops;

	/* Initialize. */
	front->data = self->data;
	front->normal = self->normal;
	back->data = self->data;
	back->normal = self->normal;
	front->vertices.count = 0;
	back->vertices.count = 0;
	if (!self->vertices.count)
		return 1;
	prev_vtx = (char*) self->vertices.vertices + (self->vertices.count - 1) * ops->size;
	ops->getcoord (prev_vtx, &prev_coord);
	prev_in = limat_plane_signed_distance_to_point (plane, &prev_coord) >= 0.0f;
	tmp_vtx = lisys_malloc (ops->size);
	if (tmp_vtx == NULL)
		return 0;

	/* Calculate the vertices of the new polygons. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		curr_vtx = (char*) self->vertices.vertices + i * ops->size;
		ops->getcoord (curr_vtx, &curr_coord);
		curr_in = limat_plane_signed_distance_to_point (plane, &curr_coord) >= 0.0f;
		if (curr_in && !prev_in)
		{
			/* Back to front. */
			limat_plane_intersects_segment (plane, &prev_coord, &curr_coord, &tmp_coord);
			frac = limat_vector_get_length (limat_vector_subtract (prev_coord, tmp_coord)) /
			       limat_vector_get_length (limat_vector_subtract (prev_coord, curr_coord));
			ops->setcoord (tmp_vtx, &tmp_coord);
			ops->interpolate (curr_vtx, prev_vtx, frac, tmp_vtx);
			ret &= limat_polygon_add_vertices (back, tmp_vtx, 1);
			ret &= limat_polygon_add_vertices (front, tmp_vtx, 1);
			ret &= limat_polygon_add_vertices (front, curr_vtx, 1);
		}
		else if (!curr_in && prev_in)
		{
			/* Front to back. */
			limat_plane_intersects_segment (plane, &prev_coord, &curr_coord, &tmp_coord);
			frac = limat_vector_get_length (limat_vector_subtract (prev_coord, tmp_coord)) /
			       limat_vector_get_length (limat_vector_subtract (prev_coord, curr_coord));
			ops->setcoord (tmp_vtx, &tmp_coord);
			ops->interpolate (curr_vtx, prev_vtx, frac, tmp_vtx);
			ret &= limat_polygon_add_vertices (front, tmp_vtx, 1);
			ret &= limat_polygon_add_vertices (back, tmp_vtx, 1);
			ret &= limat_polygon_add_vertices (back, curr_vtx, 1);
		}
		else if (curr_in)
		{
			/* Stay in front. */
			ret &= limat_polygon_add_vertices (front, curr_vtx, 1);
		}
		else
		{
			/* Stay behind. */
			ret &= limat_polygon_add_vertices (back, curr_vtx, 1);
		}
		prev_coord = curr_coord;
		prev_vtx = curr_vtx;
		prev_in = curr_in;
	}

	lisys_free (tmp_vtx);

	return ret;
}

/**
 * \brief Checks if the polygon is completely degenerate.
 *
 * If this function returns nonzero, any attempts to triangulate the polygon
 * will yield zero non-degenerate triangles.
 *
 * \param self Polygon.
 * \return Nonzero if the polygon is degenerate.
 */
static inline int
limat_polygon_get_degenerate (const LIMatPolygon* self)
{
	int i;
	LIMatVector coord[3];
	const LIMatVtxops* ops = self->ops;

	if (self->vertices.count < 3)
		return 1;
	ops->getcoord ((char*) self->vertices.vertices + 0 * ops->size, coord + 0);
	ops->getcoord ((char*) self->vertices.vertices + 1 * ops->size, coord + 1);
	for (i = 2 ; i < self->vertices.count ; i++)
	{
		ops->getcoord ((char*) self->vertices.vertices + i * ops->size, coord + 2);
		if (memcmp (coord + 0, coord + 1, sizeof (LIMatVector)) &&
			memcmp (coord + 1, coord + 2, sizeof (LIMatVector)) &&
			memcmp (coord + 2, coord + 0, sizeof (LIMatVector)))
			return 0;
		memcpy (coord + 1, coord + 2, sizeof (LIMatVector));
	}

	return 1;
}

/**
 * \brief Gets the coordinates of a vertex from the polygon.
 *
 * \param self Polygon.
 * \param index Vertex index.
 * \param coord Return location for a vector.
 */
static inline void
limat_polygon_get_coord (const LIMatPolygon* self,
                         int                 index,
                         LIMatVector*        coord)
{
	const LIMatVtxops* ops = self->ops;

	self->ops->getcoord ((char*) self->vertices.vertices + index * ops->size, coord);
}

/**
 * \brief Gets a vertex from the polygon.
 *
 * \param self Polygon.
 * \param index Vertex index.
 * \return Pointer to vertex.
 */
static inline void*
limat_polygon_get_vertex (const LIMatPolygon* self,
                          int                 index)
{
	const LIMatVtxops* ops = self->ops;

	return (char*) self->vertices.vertices + index * ops->size;
}

#endif

/** @} */
/** @} */
