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
 * \addtogroup LIMatRectangle Rectangle
 * @{
 */

#ifndef __MATH_RECTANGLE_H__
#define __MATH_RECTANGLE_H__

#include "math-generic.h"

/**
 * \brief A class respresenting a rectangle.
 */
typedef struct _LIMatRectangle LIMatRectangle;
struct _LIMatRectangle
{
	float x;
	float y;
	float width;
	float height;
};

/**
 * \brief Sets the rectangle from integer values.
 *
 * \param self Rectangle.
 * \param x Integer.
 * \param y An integer.
 * \param width An integer.
 * \param height An integer.
 */
static inline void
limat_rectangle_set (LIMatRectangle* self,
                     int             x,
                     int             y,
                     int             width,
                     int             height)
{
	self->x = x;
	self->y = y;
	self->width = width;
	self->height = height;
}

/**
 * \brief Calculates the union of the two rectangles and stores it in self.
 *
 * \param self Rectangle.
 * \param rect0 Rectangle.
 * \param rect1 Rectangle.
 * \return Nonzero if the rectangle has an area greater than zero.
 */
static inline int
limat_rectangle_union (LIMatRectangle*       self,
                       const LIMatRectangle* rect0,
                       const LIMatRectangle* rect1)
{
	float x0;
	float y0;
	float x1;
	float y1;

	x0 = LIMAT_MIN (rect0->x, rect1->x);
	y0 = LIMAT_MIN (rect0->y, rect1->y);
	x1 = LIMAT_MAX (rect0->x + rect0->width, rect1->x + rect1->width);
	y1 = LIMAT_MAX (rect0->y + rect0->height, rect1->y + rect1->height);
	self->x = x0;
	self->y = y0;
	self->width = x1 - x0;
	self->height = y1 - y0;

	return self->width > 0 && self->height > 0;
}

/**
 * \brief Calculates the intersection of the two rectangles and stores it in self.
 *
 * \param self Rectangle.
 * \param rect0 Rectangle.
 * \param rect1 Rectangle.
 * \return Nonzero if the rectangle has an area greater than zero.
 */
static inline int
limat_rectangle_intersection (LIMatRectangle*       self,
                              const LIMatRectangle* rect0,
                              const LIMatRectangle* rect1)
{
	float x0;
	float y0;
	float x1;
	float y1;

	x0 = LIMAT_MAX (rect0->x, rect1->x);
	y0 = LIMAT_MAX (rect0->y, rect1->y);
	x1 = LIMAT_MIN (rect0->x + rect0->width, rect1->x + rect1->width);
	y1 = LIMAT_MIN (rect0->y + rect0->height, rect1->y + rect1->height);
	self->x = x0;
	self->y = y0;
	self->width = x1 - x0;
	self->height = y1 - y0;

	return self->width > 0 && self->height > 0;
}

/**
 * \brief Discards parts of the line segment outside the rectangle.
 *
 * \param self Rectangle.
 * \param x0 First end point of the line segment.
 * \param y0 First end point of the line segment.
 * \param x1 Second end point of the line segment.
 * \param y1 Second end point of the line segment.
 * \return Zero if the segment was completely eliminated.
 */
static inline int
limat_rectangle_clip_segment (const LIMatRectangle* self,
                              float*                x0,
                              float*                y0,
                              float*                x1,
                              float*                y1)
{
	float t;
	float t0;
	float t1;
	float x;
	float y;
	float vx;
	float vy;
	float xmin = self->x;
	float xmax = self->x + self->width;
	float ymin = self->y;
	float ymax = self->y + self->height;

	t0 = 0.0f;
	t1 = 1.0f;
	vx = *x1 - *x0;
	vy = *y1 - *y0;
	if (LIMAT_ABS (vx) > LIMAT_EPSILON)
	{
		/* Left edge. */
		t = (xmin - *x0) / vx;
		y = t * vy + *y0;
		if (0.0f <= t && t <= 1.0f && ymin <= y && y <= ymax)
		{
			if (vx > 0)
				t0 = LIMAT_MAX (t0, t);
			else
				t1 = LIMAT_MIN (t1, t);
		}

		/* Right edge. */
		t = (xmax - *x0) / vx;
		y = t * vy + *y0;
		if (0.0f <= t && t <= 1.0f && ymin <= y && y <= ymax)
		{
			if (vx < 0)
				t0 = LIMAT_MAX (t0, t);
			else
				t1 = LIMAT_MIN (t1, t);
		}
	}
	if (LIMAT_ABS (vy) > LIMAT_EPSILON)
	{
		/* Bottom edge. */
		t = (ymin - *y0) / vy;
		x = t * vx + *x0;
		if (0.0f <= t && t <= 1.0f && xmin <= x && x <= xmax)
		{
			if (vy > 0)
				t0 = LIMAT_MAX (t0, t);
			else
				t1 = LIMAT_MIN (t1, t);
		}

		/* Top edge. */
		t = (ymax - *y0) / vy;
		x = t * vx + *x0;
		if (0.0f <= t && t <= 1.0f && xmin <= x && x <= xmax)
		{
			if (vy < 0)
				t0 = LIMAT_MAX (t0, t);
			else
				t1 = LIMAT_MIN (t1, t);
		}
	}
	if (t0 > t1)
		return 0;
	*x1 = *x0 + t1 * vx;
	*y1 = *y0 + t1 * vy;
	*x0 = *x0 + t0 * vx;
	*y0 = *y0 + t0 * vy;
	return 1;
}

/**
 * \brief Checks if a rectangle intersects a line segment.
 *
 * This function returns no intersection for conciding lines.
 *
 * \param self Rectangle.
 * \param x0 First end point of the line segment.
 * \param y0 First end point of the line segment.
 * \param x1 Second end point of the line segment.
 * \param y1 Second end point of the line segment.
 * \return Nonzero if intersects.
 */
static inline int
limat_rectangle_intersects_segment (const LIMatRectangle* self,
                                    float                 x0,
                                    float                 y0,
                                    float                 x1,
                                    float                 y1)
{
	float t;
	float x;
	float y;
	float vx;
	float vy;
	float xmin = self->x;
	float xmax = self->x + self->width;
	float ymin = self->y;
	float ymax = self->y + self->height;

	/*
	 * We use the parametric presentation to solve
	 * the intersection points of the edges:
	 *
	 * x(t) = vx(t) + x0 = t*(x1-x0)+x0 = t*vx+x0
	 * y(t) = vy(t) + y0 = t*(y1-y0)+y0 = t*vy+y0
	 *
	 * x(t) = X
	 * t*vx+x0 = X
	 * t = (X-x0)/vx
	 *
	 * y(t) = Y
	 * t*vy+y0 = Y
	 * t = (Y-y0)/vy
	 */

	/* Check if an end point is inside. */
	if (xmin <= x0 && x0 <= xmax &&
	    ymin <= y0 && y0 <= ymax)
		return 1;
	if (xmin <= x1 && x1 <= xmax &&
	    ymin <= y1 && y1 <= ymax)
		return 1;

	vx = x1 - x0;
	vy = y1 - y0;

	/* Check if edges intersect. */
	if (LIMAT_ABS (vx) > LIMAT_EPSILON)
	{
		/* Left edge. */
		t = (xmin - x0) / vx;
		y = t * vy + y0;
		if (0.0f <= t && t <= 1.0f && ymin <= y && y <= ymax)
			return 1;

		/* Right edge. */
		t = (xmax - x0) / vx;
		y = t * vy + y0;
		if (0.0f <= t && t <= 1.0f && ymin <= y && y <= ymax)
			return 1;
	}
	if (LIMAT_ABS (vy) > LIMAT_EPSILON)
	{
		/* Bottom edge. */
		t = (ymin - y0) / vy;
		x = t * vx + x0;
		if (0.0f <= t && t <= 1.0f && xmin <= x && x <= xmax)
			return 1;

		/* Top edge. */
		t = (ymax - y0) / vy;
		x = t * vx + x0;
		if (0.0f <= t && t <= 1.0f && xmin <= x && x <= xmax)
			return 1;
	}

	return 0;
}

#endif

/** @} */
/** @} */
