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
 * \addtogroup LIMatMatrix Matrix
 * @{
 */

#ifndef __MATH_MATRIX_H__
#define __MATH_MATRIX_H__

#include <math.h>
#include "math-vector.h"

typedef struct _LIMatMatrix LIMatMatrix;
struct _LIMatMatrix
{
	float m[16];
};

static inline LIMatMatrix
limat_matrix_identity ()
{
	LIMatMatrix self;

	self.m[0] = 1.0f;
	self.m[1] = 0.0f;
	self.m[2] = 0.0f;
	self.m[3] = 0.0f;
	self.m[4] = 0.0f;
	self.m[5] = 1.0f;
	self.m[6] = 0.0f;
	self.m[7] = 0.0f;
	self.m[8] = 0.0f;
	self.m[9] = 0.0f;
	self.m[10] = 1.0f;
	self.m[11] = 0.0f;
	self.m[12] = 0.0f;
	self.m[13] = 0.0f;
	self.m[14] = 0.0f;
	self.m[15] = 1.0f;

	return self;
}

static inline LIMatMatrix
limat_matrix_frustum (float left,
                      float right,
                      float bottom,
                      float top,
                      float nearz,
                      float farz)
{
	LIMatMatrix self;

	self.m[0] = (2.0f * nearz) / (right - left);
	self.m[1] = 0.0f;
	self.m[2] = 0.0f;
	self.m[3] = 0.0f;
	self.m[4] = 0.0f;
	self.m[5] = (2.0f * nearz) / (top - bottom);
	self.m[6] = 0.0f;
	self.m[7] = 0.0f;
	self.m[8] = (right + left) / (right - left);
	self.m[9] = (top + bottom) / (top - bottom);
	self.m[10] = -(farz + nearz) / (farz - nearz);
	self.m[11] = -1.0f;
	self.m[12] = 0.0f;
	self.m[13] = 0.0f;
	self.m[14] = -2.0f * farz * nearz / (farz - nearz);
	self.m[15] = 0.0f;

	return self;
}

static inline LIMatMatrix
limat_matrix_look (float eyex,
                   float eyey,
                   float eyez,
                   float dirx,
                   float diry,
                   float dirz,
                   float upx,
                   float upy,
                   float upz)
{
	float len;
	float sidex;
	float sidey;
	float sidez;
	LIMatMatrix self;

	/* Normalize direction. */
	len = sqrt (dirx * dirx + diry * diry + dirz * dirz);
	dirx = dirx / len;
	diry = diry / len;
	dirz = dirz / len;

	/* Calculate side. */
	sidex =  (diry * upz - dirz * upy);
	sidey = -(dirx * upz - dirz * upx);
	sidez =  (dirx * upy - diry * upx);
	len = sqrt (sidex * sidex + sidey * sidey + sidez * sidez);
	sidex = sidex / len;
	sidey = sidey / len;
	sidez = sidez / len;

	/* Calculate up. */
	upx =  (sidey * dirz - sidez * diry);
	upy = -(sidex * dirz - sidez * dirx);
	upz =  (sidex * diry - sidey * dirx);
	len = sqrt (upx * upx + upy * upy + upz * upz);
	upx = upx / len;
	upy = upy / len;
	upz = upz / len;

	/* Fill matrix. */
	self.m[0] = sidex;
	self.m[1] = upx;
	self.m[2] = -dirx;
	self.m[3] = 0.0f;
	self.m[4] = sidey;
	self.m[5] = upy;
	self.m[6] = -diry;
	self.m[7] = 0.0f;
	self.m[8] = sidez;
	self.m[9] = upz;
	self.m[10] = -dirz;
	self.m[11] = 0.0f;
	self.m[12] = -self.m[0] * eyex - self.m[4] * eyey - self.m[8] * eyez;
	self.m[13] = -self.m[1] * eyex - self.m[5] * eyey - self.m[9] * eyez;
	self.m[14] = -self.m[2] * eyex - self.m[6] * eyey - self.m[10] * eyez;
	self.m[15] = 1.0f;

	return self;
}

static inline LIMatMatrix
limat_matrix_lookat (float eyex,
                     float eyey,
                     float eyez,
                     float centerx,
                     float centery,
                     float centerz,
                     float upx,
                     float upy,
                     float upz)
{
	float dirx = centerx - eyex;
	float diry = centery - eyey;
	float dirz = centerz - eyez;

	return limat_matrix_look (
		eyex, eyey, eyez,
		dirx, diry, dirz,
		upx, upy, upz);
}

static inline LIMatMatrix
limat_matrix_ortho (float left,
                    float right,
                    float bottom,
                    float top,
                    float nearz,
                    float farz)
{
	LIMatMatrix self;

	self.m[0] = 2.0f / (right - left);
	self.m[1] = 0.0f;
	self.m[2] = 0.0f;
	self.m[3] = 0.0f;
	self.m[4] = 0.0f;
	self.m[5] = 2.0f / (top - bottom);
	self.m[6] = 0.0f;
	self.m[7] = 0.0f;
	self.m[8] = 0.0f;
	self.m[9] = 0.0f;
	self.m[10] = -2.0f / (farz - nearz);
	self.m[11] = 0.0f;
	self.m[12] = -(right + left) / (right - left);
	self.m[13] = -(top + bottom) / (top - bottom);
	self.m[14] = -(farz + nearz) / (farz - nearz);
	self.m[15] = 1.0f;

	return self;
}

/**
 * \brief Creates a perspective matrix.
 *
 * \param fov Field of view in radians.
 * \param aspect Aspect ratio.
 * \param nearz Near plane distance.
 * \param farz Far plane distance.
 * \return Perspective matrix.
 */
static inline LIMatMatrix
limat_matrix_perspective (float fov,
                          float aspect,
                          float nearz,
                          float farz)
{
	float d;
	float f;
	LIMatMatrix self;

	d = nearz - farz;
	f = 1.0f / tan (0.5f * fov);
	self.m[0] = f / aspect;
	self.m[1] = 0.0f;
	self.m[2] = 0.0f;
	self.m[3] = 0.0f;
	self.m[4] = 0.0f;
	self.m[5] = f;
	self.m[6] = 0.0f;
	self.m[7] = 0.0f;
	self.m[8] = 0.0f;
	self.m[9] = 0.0f;
	self.m[10] = (farz + nearz) / d;
	self.m[11] = -1.0f;
	self.m[12] = 0.0f;
	self.m[13] = 0.0f;
	self.m[14] = 2.0f * farz * nearz / d;
	self.m[15] = 0.0f;

	return self;
}

static inline LIMatMatrix
limat_matrix_rotation (float radians,
                       float x,
                       float y,
                       float z)
{
	LIMatMatrix self;
	float s0 = sin (-radians);
	float c0 = cos (-radians);
	float c1 = 1 - c0;

	self.m[0] = x * x * c1 + c0;
	self.m[1] = x * y * c1 - z * s0;
	self.m[2] = x * z * c1 + y * s0;
	self.m[3] = 0.0f;
	self.m[4] = x * y * c1 + z * s0;
	self.m[5] = y * y * c1 + c0;
	self.m[6] = y * z * c1 - x * s0;
	self.m[7] = 0.0f;
	self.m[8] = x * z * c1 - y * s0;
	self.m[9] = y * z * c1 + x * s0;
	self.m[10] = z * z * c1 + c0;
	self.m[11] = 0.0f;
	self.m[12] = 0.0f;
	self.m[13] = 0.0f;
	self.m[14] = 0.0f;
	self.m[15] = 1.0f;

	return self;
}

static inline LIMatMatrix
limat_matrix_scale (float x,
                    float y,
                    float z)
{
	LIMatMatrix self;

	self.m[0] = x;
	self.m[1] = 0.0f;
	self.m[2] = 0.0f;
	self.m[3] = 0.0f;
	self.m[4] = 0.0f;
	self.m[5] = y;
	self.m[6] = 0.0f;
	self.m[7] = 0.0f;
	self.m[8] = 0.0f;
	self.m[9] = 0.0f;
	self.m[10] = z;
	self.m[11] = 0.0f;
	self.m[12] = 0.0f;
	self.m[13] = 0.0f;
	self.m[14] = 0.0f;
	self.m[15] = 1.0f;

	return self;
}

/**
 * \brief Multiplies the vector by a matrix.
 *
 * \param self Matrix.
 * \param vector Vector.
 * \return Vector.
 */
static inline LIMatVector
limat_matrix_transform (LIMatMatrix self,
                        LIMatVector vector)
{
	LIMatVector result;

	result.x = self.m[0] * vector.x + self.m[4] * vector.y + self.m[8] * vector.z + self.m[12];
	result.y = self.m[1] * vector.x + self.m[5] * vector.y + self.m[9] * vector.z + self.m[13];
	result.z = self.m[2] * vector.x + self.m[6] * vector.y + self.m[10] * vector.z + self.m[14];
	return result;
}

static inline LIMatMatrix
limat_matrix_translation (float x,
                          float y,
                          float z)
{
	LIMatMatrix self;

	self.m[0] = 1.0f;
	self.m[1] = 0.0f;
	self.m[2] = 0.0f;
	self.m[3] = 0.0f;
	self.m[4] = 0.0f;
	self.m[5] = 1.0f;
	self.m[6] = 0.0f;
	self.m[7] = 0.0f;
	self.m[8] = 0.0f;
	self.m[9] = 0.0f;
	self.m[10] = 1.0f;
	self.m[11] = 0.0f;
	self.m[12] = x;
	self.m[13] = y;
	self.m[14] = z;
	self.m[15] = 1.0f;

	return self;
}

static inline LIMatVector
limat_matrix_get_basis (const LIMatMatrix self,
                        int               basis)
{
	switch (basis)
	{
		case 0: return limat_vector_init (self.m[0], self.m[4], self.m[8]);
		case 1: return limat_vector_init (self.m[1], self.m[5], self.m[9]);
		case 2: return limat_vector_init (self.m[2], self.m[6], self.m[10]);
	}
	return limat_vector_init (0.0f, 0.0f, 0.0f);
}

static inline LIMatMatrix
limat_matrix_get_rotation (const LIMatMatrix self)
{
	LIMatMatrix result;

	result.m[0] = self.m[0];
	result.m[1] = self.m[1];
	result.m[2] = self.m[2];
	result.m[3] = 0.0f;
	result.m[4] = self.m[4];
	result.m[5] = self.m[5];
	result.m[6] = self.m[6];
	result.m[7] = 0.0f;
	result.m[8] = self.m[8];
	result.m[9] = self.m[9];
	result.m[10] = self.m[10];
	result.m[11] = 0.0f;
	result.m[12] = 0.0f;
	result.m[13] = 0.0f;
	result.m[14] = 0.0f;
	result.m[15] = 1.0f;

	return result;
}

static inline int
limat_matrix_get_singular (const LIMatMatrix self)
{
	float det;

	det = self.m[0] * (self.m[5] * self.m[10] - self.m[9] * self.m[6])
	    - self.m[4] * (self.m[1] * self.m[10] - self.m[9] * self.m[2])
	    + self.m[8] * (self.m[1] * self.m[ 6] - self.m[5] * self.m[2]);
	if (-LIMAT_VECTOR_EPSILON <= det && det <= LIMAT_VECTOR_EPSILON)
		return 1;
	return 0;
}

/* Stolen from Mesa. */
static inline LIMatMatrix
limat_matrix_invert (const LIMatMatrix self)
{
	int i;
	float det;
	float inv[16];
	const float* m = self.m;
	LIMatMatrix ret;

	inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
	         + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
	         - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
	         + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
	         - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
	inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
	         - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
	         + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
	         - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
	         + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
	         + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
	         - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
	         + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
	         - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
	         - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
	         + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
	         - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
	         + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];
	det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
	det = 1.0 / det;
	for (i = 0; i < 16; i++)
		ret.m[i] = inv[i] * det;

	return ret;
}

static inline LIMatMatrix
limat_matrix_multiply (const LIMatMatrix self,
                       const LIMatMatrix matrix)
{
	LIMatMatrix result;
	const float* A = self.m;
	const float* B = matrix.m;

	result.m[0] = A[0] * B[0] + A[4] * B[1] + A[8] * B[2] + A[12] * B[3];
	result.m[1] = A[1] * B[0] + A[5] * B[1] + A[9] * B[2] + A[13] * B[3];
	result.m[2] = A[2] * B[0] + A[6] * B[1] + A[10] * B[2] + A[14] * B[3];
	result.m[3] = A[3] * B[0] + A[7] * B[1] + A[11] * B[2] + A[15] * B[3];
	result.m[4] = A[0] * B[4] + A[4] * B[5] + A[8] * B[6] + A[12] * B[7];
	result.m[5] = A[1] * B[4] + A[5] * B[5] + A[9] * B[6] + A[13] * B[7];
	result.m[6] = A[2] * B[4] + A[6] * B[5] + A[10] * B[6] + A[14] * B[7];
	result.m[7] = A[3] * B[4] + A[7] * B[5] + A[11] * B[6] + A[15] * B[7];
	result.m[8] = A[0] * B[8] + A[4] * B[9] + A[8] * B[10] + A[12] * B[11];
	result.m[9] = A[1] * B[8] + A[5] * B[9] + A[9] * B[10] + A[13] * B[11];
	result.m[10] = A[2] * B[8] + A[6] * B[9] + A[10] * B[10] + A[14] * B[11];
	result.m[11] = A[3] * B[8] + A[7] * B[9] + A[11] * B[10] + A[15] * B[11];
	result.m[12] = A[0] * B[12] + A[4] * B[13] + A[8] * B[14] + A[12] * B[15];
	result.m[13] = A[1] * B[12] + A[5] * B[13] + A[9] * B[14] + A[13] * B[15];
	result.m[14] = A[2] * B[12] + A[6] * B[13] + A[10] * B[14] + A[14] * B[15];
	result.m[15] = A[3] * B[12] + A[7] * B[13] + A[11] * B[14] + A[15] * B[15];

	return result;
}

/* Stolen from Mesa. */
static inline LIMatMatrix
limat_matrix_pick (float      x,
                   float      y,
                   float      w,
                   float      h,
                   const int* viewport)
{
	LIMatMatrix m0;
	LIMatMatrix m1;

	m0 = limat_matrix_translation (
		(viewport[2] - 2.0f * (x - viewport[0])) / w,
		(viewport[3] - 2.0f * (y - viewport[1])) / h, 0.0f);
	m1 = limat_matrix_scale (
		viewport[2] / w, viewport[3] / h, 1.0f);

	return limat_matrix_multiply (m0, m1);
}

/**
 * \brief Projects a point from object space to window space.
 *
 * \param projection Projection matrix.
 * \param modelview Modelview matrix.
 * \param viewport Viewport.
 * \param object Vector in object space.
 * \param window Return location for a vector in window space.
 * \return Nonzero on success.
 */
static inline int
limat_matrix_project (const LIMatMatrix  projection,
                      const LIMatMatrix  modelview,
                      const int*         viewport,
                      const LIMatVector* object,
                      LIMatVector*       window)
{
	float w;
	LIMatMatrix m;
	LIMatVector v;
	LIMatVector result;

	/* Multiply by modelview matrix. */
	w = 1.0f;
	v = *object;
	m = modelview;
	result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * w;
	result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * w;
	result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * w;
	w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * w;

	/* Multiply by projection matrix. */
	v = result;
	m = projection;
	result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * w;
	result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * w;
	result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * w;
	w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * w;

	/* Convert to window space. */
	if (-LIMAT_VECTOR_EPSILON < w && w < LIMAT_VECTOR_EPSILON)
		return 0;
	result.x /= w;
	result.y /= w;
	result.z /= w;
	window->x = viewport[0] + viewport[2] * (result.x + 1.0f) / 2.0f;
	window->y = viewport[1] + viewport[3] * (result.y + 1.0f) / 2.0f;
	window->z = (result.z + 1.0f) / 2.0f;

	return 1;
}

/**
 * \brief Unprojects a point from window space to object space.
 *
 * \param projection Projection matrix.
 * \param modelview Modelview matrix.
 * \param viewport Viewport.
 * \param window Vector in window space.
 * \param object Return location for a vector in object space.
 * \return Nonzero on success.
 */
static inline int
limat_matrix_unproject (const LIMatMatrix  projection,
                        const LIMatMatrix  modelview,
                        const int*         viewport,
                        const LIMatVector* window,
                        LIMatVector*       object)
{
	float w;
	LIMatMatrix m;
	LIMatVector v;

	/* Get inverse matrix. */
	m = limat_matrix_multiply (projection, modelview);
	if (limat_matrix_get_singular (m))
		return 0;
	m = limat_matrix_invert (m);

	/* Vector to [-1,1] range. */
	v.x = 2.0f * (window->x - viewport[0]) / viewport[2] - 1.0f;
	v.y = 2.0f * (window->y - viewport[1]) / viewport[3] - 1.0f;
	v.z = 2.0f * window->z - 1.0f;

	/* Multiply by the inverse matrix. */
	w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15];
	if (-LIMAT_VECTOR_EPSILON < w && w < LIMAT_VECTOR_EPSILON)
		return 0;
	object->x = (m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12]) / w;
	object->y = (m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13]) / w;
	object->z = (m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14]) / w;

	return 1;
}

#endif

/** @} */
/** @} */
