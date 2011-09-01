/* Dual quaternion functions.

  Copyright (C) 2006-2007 University of Dublin, Trinity College, All Rights Reserved
  Copyright (C) 2011 Lips of Suna development team

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author(s) be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Original author: Ladislav Kavan, kavanl@cs.tcd.ie
  Modified and extended by the Lips of Suna development team
*/

#include "math-quaternion.h"
#include "math-vector.h"

typedef struct _LIMatDualquat LIMatDualquat;
struct _LIMatDualquat
{
	LIMatQuaternion r;
	LIMatQuaternion d;
};

/**
 * \brief Initializes a dual quaternion.
 * \param t Translation vector.
 * \param r Rotation quaternion.
 * \return Dual quaternion.
 */
static inline LIMatDualquat limat_dualquat_init (
	LIMatVector     t,
	LIMatQuaternion r)
{
	LIMatQuaternion tmp0 = limat_quaternion_init (t.x, t.y, t.z, 0.0f);
	LIMatQuaternion tmp1 = limat_quaternion_multiply (tmp0, r);
	LIMatDualquat self = { r, { 0.5f * tmp1.x, 0.5f * tmp1.y, 0.5f * tmp1.z, 0.5f * tmp1.w }};
	return self;
}

/**
 * \brief Initializes a dual quaternion.
 * \param r Rotation quaternion.
 * \return Dual quaternion.
 */
static inline LIMatDualquat limat_dualquat_init_rotation (
	LIMatQuaternion r)
{
	LIMatDualquat self = { r, { 0.0f, 0.0f, 0.0f, 0.0f } };
	return self;
}

/**
 * \brief Initializes a dual quaternion.
 * \param t Translation vector.
 * \return Dual quaternion.
 */
static inline LIMatDualquat limat_dualquat_init_translation (
	LIMatVector     t)
{
	LIMatDualquat self = {
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5 * t.x, 0.5 * t.y, 0.5 * t.z, 0.0f }};
	return self;
}

static inline LIMatDualquat limat_dualquat_add (
	LIMatDualquat self,
	LIMatDualquat dual)
{
	LIMatDualquat result = {
		limat_quaternion_add (self.r, dual.r),
		limat_quaternion_add (self.d, dual.d) };
	return result;
}

static inline LIMatDualquat limat_dualquat_multiply (
	LIMatDualquat self,
	LIMatDualquat dual)
{
	/* A*B = (Ar*Br, Ar*Bd+Ad*Br) */
	LIMatDualquat result = {
		limat_quaternion_multiply (self.r, dual.r),
		limat_quaternion_add (
			limat_quaternion_multiply (self.r, dual.d),
			limat_quaternion_multiply (self.d, dual.r)) };
	return result;
}

static inline LIMatDualquat limat_dualquat_scale (
	LIMatDualquat self,
	float         scalar)
{
	LIMatDualquat result = {
		{ scalar * self.r.x, scalar * self.r.y, scalar * self.r.z, scalar * self.r.w },
		{ scalar * self.d.x, scalar * self.d.y, scalar * self.d.z, scalar * self.d.w }};
	return result;
}

static inline LIMatVector limat_dualquat_transform (
	LIMatDualquat self,
	LIMatVector   vector)
{
	LIMatVector R_xyz = limat_vector_init (self.r.x, self.r.y, self.r.z);
	LIMatVector D_xyz = limat_vector_init (self.d.x, self.d.y, self.d.z);
	/* P = R_xyz CROSS (v * R_w + R_xyz CROSS v) */
	LIMatVector p = limat_vector_cross (R_xyz,
		limat_vector_add (
			limat_vector_multiply (vector, self.r.w),
			limat_vector_cross (R_xyz, vector)));
	/* T = R_xyz CROSS D_xyz + D_xyz * R_w - R_xyz * D_w */
	LIMatVector t = limat_vector_add (
		limat_vector_cross (R_xyz, D_xyz),
		limat_vector_subtract (
			limat_vector_multiply (D_xyz, self.r.w),
			limat_vector_multiply (R_xyz, self.d.w)));
	/* Q*V = V + 2 * (P + T) */
	return limat_vector_add (vector,
		limat_vector_multiply (
			limat_vector_add (p, t), 2.0f));
}
