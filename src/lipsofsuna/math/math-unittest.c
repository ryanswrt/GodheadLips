/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#include "lipsofsuna/math.h"

#define EPSILON 0.05

static inline int check_matrix (LIMatMatrix m0, LIMatMatrix m1)
{
	int i;

	for (i = 0 ; i < 15 ; i++)
	{
		if (LIMAT_ABS (m0.m[i] - m1.m[i]) > EPSILON)
			return 0;
	}
	return 1;
}

static inline int check_quaternion (LIMatQuaternion q0, LIMatQuaternion q1)
{
	if (q0.w < 0.0f ||
	   (q0.w == 0.0f && q0.z < 0.0f) ||
	   (q0.w == 0.0f && q0.z == 0.0f && q0.y < 0.0f) ||
	   (q0.w == 0.0f && q0.z == 0.0f && q0.y == 0.0f && q0.x < 0.0f))
		q0 = limat_quaternion_init (-q0.x, -q0.y, -q0.z, -q0.w);
	if (q1.w < 0.0f ||
	   (q1.w == 0.0f && q1.z < 0.0f) ||
	   (q1.w == 0.0f && q1.z == 0.0f && q1.y < 0.0f) ||
	   (q1.w == 0.0f && q1.z == 0.0f && q1.y == 0.0f && q1.x < 0.0f))
		q1 = limat_quaternion_init (-q1.x, -q1.y, -q1.z, -q1.w);
	if (LIMAT_ABS (q0.x - q1.x) > EPSILON ||
	    LIMAT_ABS (q0.y - q1.y) > EPSILON ||
	    LIMAT_ABS (q0.z - q1.z) > EPSILON ||
	    LIMAT_ABS (q0.w - q1.w) > EPSILON)
		return 0;
	return 1;
}

static inline int check_vector (LIMatVector v0, LIMatVector v1)
{
	if (LIMAT_ABS (v0.x - v1.x) > EPSILON ||
	    LIMAT_ABS (v0.y - v1.y) > EPSILON ||
	    LIMAT_ABS (v0.z - v1.z) > EPSILON)
		return 0;
	return 1;
}

static inline int check_transform (LIMatTransform t0, LIMatTransform t1)
{
	return check_vector (t0.position, t1.position) &&
	       check_quaternion (t0.rotation, t1.rotation);
}

static inline void print_quaternion (const char* s, LIMatQuaternion q)
{
	printf ("%s: Quaternion({%f,%f,%f,%f})\n",
		s, q.x, q.y, q.z, q.w);
}

static inline void print_vector (const char* s, LIMatVector v)
{
	printf ("%s: Vector({%f,%f,%f})\n",
		s, v.x, v.y, v.z);
}

static inline void print_transform (const char* s, LIMatTransform t)
{
	printf ("%s: Transform({%f,%f,%f}, {%f,%f,%f,%f})\n",
		s, t.position.x, t.position.y, t.position.z,
		t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w);
}

static inline void print_matrix (const char* s, LIMatMatrix m)
{
	printf ("%s: Matrix({%f,%f,%f,%f}, {%f,%f,%f,%f}, {%f,%f,%f,%f}, {%f,%f,%f,%f})\n",
		s, m.m[0], m.m[1], m.m[2], m.m[3],
		m.m[4], m.m[5], m.m[6], m.m[7],
		m.m[8], m.m[9], m.m[10], m.m[11],
		m.m[12], m.m[13], m.m[14], m.m[15]);
}

static inline void print_dualquat (const char* s, LIMatDualquat dq)
{
	printf ("%s: Dualquat({%f,%f,%f,%f}, {%f,%f,%f,%f})\n",
		s, dq.r.x, dq.r.y, dq.r.z, dq.r.w, dq.d.x, dq.d.y, dq.d.z, dq.d.w);
}

/*****************************************************************************/

static void convert_quaternion_matrix ()
{
	LIMatMatrix m0;
	LIMatMatrix m1;
	LIMatQuaternion q0;
	LIMatQuaternion q1;

	printf ("Testing quaternion-matrix conversion...\n");
	m0 = limat_matrix_rotation (2.0f, 1.0f, 0.0f, 0.0f);
	q0 = limat_convert_matrix_to_quaternion (m0);
	m1 = limat_convert_quaternion_to_matrix (q0);
	if (!check_matrix (m0, m1))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m0);
		print_quaternion ("M->Q", q0);
		print_matrix ("Q->M", m1);
	}
	q0 = limat_quaternion_conjugate (q0);
	m0 = limat_convert_quaternion_to_matrix (q0);
	q1 = limat_convert_matrix_to_quaternion (m0);
	if (!check_quaternion (q0, q1))
	{
		printf ("2: FAILED!\n");
		print_quaternion ("Q", q0);
		print_matrix ("Q->M", m0);
		print_quaternion ("M->Q", q1);
	}
}

static void convert_transform_matrix ()
{
	LIMatMatrix m0;
	LIMatMatrix m1;
	LIMatTransform t0;
	LIMatTransform t1;

	printf ("Testing transform-matrix conversion...\n");
	m0 = limat_matrix_rotation (2.0f, 1.0f, 0.0f, 0.0f);
	m1 = limat_matrix_translation (100.0f, 0.0f, -10.0f);
	m0 = limat_matrix_multiply (m0, m1);
	t0 = limat_convert_matrix_to_transform (m0);
	m1 = limat_convert_transform_to_matrix (t0);
	if (!check_matrix (m0, m1))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m0);
		print_transform ("M->T", t0);
		print_matrix ("T->M", m1);
	}
	t0 = limat_transform_init (
		limat_vector_init (0.0f, -50.0f, 1000.0f),
		limat_quaternion_init (0.707f, 0.0f, 0.707f, 0.0f));
	m0 = limat_convert_transform_to_matrix (t0);
	t1 = limat_convert_matrix_to_transform (m0);
	if (!check_transform (t0, t1))
	{
		printf ("2: FAILED!\n");
		print_transform ("T", t0);
		print_matrix ("T->M", m0);
		print_transform ("M->T", t1);
	}
}

/**
 * The reference rotation matrix was created using Bullet maths library.
 *
 * The reference lookat matrices were created using gluLookAt.
 */
static void matrix_basics ()
{
	LIMatMatrix m0;
	LIMatMatrix m1 = {{ 0.877583, 0.000000, -0.479426, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.479426, 0.000000, 0.877583, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000 }};
	LIMatMatrix m2 = {{ 0.953184, -0.164941, 0.253446, 0.000000, 0.000000, 0.838140, 0.545455, 0.000000, -0.302391, -0.519919, 0.798901, 0.000000, -0.460111, 0.484196, -37.410606, 1.000000 }};
	LIMatMatrix m3 = {{ -0.103476, 0.023157, -0.994362, 0.000000, -0.819851, 0.564050, 0.098452, 0.000000, 0.563150, 0.825416, -0.039381, 0.000000, -1.585974, -2.499405, -100.460136, 1.000000 }};

	printf ("Testing matrix basics...\n");
	m0 = limat_matrix_rotation (0.5, 0.0, 1.0, 0.0);
	if (!check_matrix (m0, m1))
	{
		printf ("1: FAILED!\n");
		print_matrix ("Got", m0);
		print_matrix ("Expected", m1);
	}
	m0 = limat_matrix_lookat (10, 20, 30, 0.707, 0, 0.707, 0, 1, 0);
	if (!check_matrix (m0, m2))
	{
		printf ("2: FAILED!\n");
		print_matrix ("Got", m0);
		print_matrix ("Expected", m2);
	}
	m0 = limat_matrix_lookat (-100, 10, -1, 1, 0, 3, -2, 3, 4);
	if (!check_matrix (m0, m3))
	{
		printf ("3: FAILED!\n");
		print_matrix ("Got", m0);
		print_matrix ("Expected", m3);
	}
}

static void matrix_basis ()
{
	LIMatVector v0;
	LIMatVector v1;
	LIMatMatrix m0 = {{ 0.621610, 0.783327, 0.000000, 0.000000, -0.783327, 0.621610, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000 }};

	printf ("Testing matrix basis...\n");
	v0 = limat_matrix_get_basis (m0, 0);
	v1 = limat_vector_init (0.621610, -0.783327, 0.000000);
	if (!check_vector (v0, v1))
	{
		printf ("1: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	v0 = limat_matrix_get_basis (m0, 1);
	v1 = limat_vector_init (0.783327, 0.621610, 0.000000);
	if (!check_vector (v0, v1))
	{
		printf ("2: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	v0 = limat_matrix_get_basis (m0, 2);
	v1 = limat_vector_init (0.000000, 0.000000, 1.000000);
	if (!check_vector (v0, v1))
	{
		printf ("3: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
}

static void matrix_look ()
{
	LIMatMatrix m0;
	LIMatVector v0;
	LIMatVector v1;

	printf ("Testing matrix look...\n");
	m0 = limat_matrix_look (10, 20, 30, 0, 0, -1, 0, 1, 0);
	v0 = limat_vector_init (0.0f, 0.0f, 0.0f);
	v1 = limat_vector_init (-10.0f, -20.0f, -30.0f);
	v0 = limat_matrix_transform (m0, v0);
	if (!check_vector (v0, v1))
	{
		printf ("1: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	v0 = limat_vector_init (1.0f, 0.0f, 0.0f);
	v1 = limat_vector_init (-9.0f, -20.0f, -30.0f);
	v0 = limat_matrix_transform (m0, v0);
	if (!check_vector (v0, v1))
	{
		printf ("2: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	v0 = limat_vector_init (0.0f, 1.0f, 0.0f);
	v1 = limat_vector_init (-10.0f, -19.0f, -30.0f);
	v0 = limat_matrix_transform (m0, v0);
	if (!check_vector (v0, v1))
	{
		printf ("3: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	v0 = limat_vector_init (0.0f, 0.0f, 1.0f);
	v1 = limat_vector_init (-10.0f, -20.0f, -29.0f);
	v0 = limat_matrix_transform (m0, v0);
	if (!check_vector (v0, v1))
	{
		printf ("4: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
}

/**
 * The reference perpective matrix was created using gluPerspective.
 */
static void matrix_perspective ()
{
	LIMatMatrix m0;
	LIMatMatrix m1 = {{ 0.764657, 0.000000, 0.000000, 0.000000, 0.000000, 1.376382, 0.000000, 0.000000, 0.000000, 0.000000, -4.040000, -1.000000, 0.000000, 0.000000, -172.367996, 0.000000 }};

	printf ("Testing matrix perspective...\n");
	m0 = limat_matrix_perspective (72.0f / 180.0f * M_PI, 1.8, 34.2, 56.7);
	if (!check_matrix (m0, m1))
	{
		printf ("1: FAILED!\n");
		print_matrix ("Got", m0);
		print_matrix ("Expected", m1);
	}
}

/**
 * \brief Does a basic conformance test for quaternions.
 *
 * The reference results were calculated using the maths library of Bullet.
 * Vector by quaternion transformation was performed by first converting the
 * quaternion to a matrix and then multiplying the vector by the matrix.
 */
static void quaternion_basics ()
{
	LIMatQuaternion q0;
	LIMatQuaternion q1;
	LIMatVector v0;
	LIMatVector v1;

	printf ("Testing quaternion basics...\n");
	q0 = limat_quaternion_init (1.0, 2.0, 3.0, 4.0);
	q1 = limat_quaternion_init (-1.0, 0.5, 1.0, -2.0);
	q0 = limat_quaternion_multiply (q0, q1);
	q1 = limat_quaternion_init (-5.500000, -6.000000, 0.500000, -11.000000);
	if (!check_quaternion (q0, q1))
	{
		printf ("1: FAILED!\n");
		print_quaternion ("Got", q0);
		print_quaternion ("Expected", q1);
	}
	q0 = limat_quaternion_rotation (0.5f, limat_vector_init (0.0f, 1.0f, 0.0f));
	q1 = limat_quaternion_init (0.000000, 0.247404, 0.000000, 0.968912);
	if (!check_quaternion (q0, q1))
	{
		printf ("2: FAILED!\n");
		print_quaternion ("Got", q0);
		print_quaternion ("Expected", q1);
	}
	v0 = limat_quaternion_transform (q0, limat_vector_init (2.0f, 3.0f, 4.0f));
	v1 = limat_vector_init (3.672867, 3.000000, 2.551479);
	if (!check_vector (v0, v1))
	{
		printf ("3: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	q0 = limat_quaternion_rotation (-1.5f, limat_vector_init (1.0f, 0.0f, 0.0f));
	q1 = limat_quaternion_init (-0.681639, -0.000000, -0.000000, 0.731689);
	if (!check_quaternion (q0, q1))
	{
		printf ("4: FAILED!\n");
		print_quaternion ("Got", q0);
		print_quaternion ("Expected", q1);
	}
	v0 = limat_quaternion_transform (q0, limat_vector_init (-5.0f, 0.5f, 1.0f));
	v1 = limat_vector_init (-5.000000, 1.032864, -0.428010);
	if (!check_vector (v0, v1))
	{
		printf ("5: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	q0 = limat_quaternion_init (0.0f, 0.0f, 0.0f, 0.0f);
	q0 = limat_quaternion_normalize (q0);
	q1 = limat_quaternion_init (0.0f, 0.0f, 0.0f, 0.0f);
	if (isnan (q0.x) || isnan (q0.y) || isnan (q0.z) || isnan (q0.w))
	{
		printf ("6: FAILED!\n");
		print_quaternion ("Got", q0);
		print_quaternion ("Expected", q1);
	}
}

static void quaternion_multiply ()
{
	LIMatMatrix m0;
	LIMatMatrix m1;
	LIMatMatrix m2;
	LIMatQuaternion q0;
	LIMatQuaternion q1;
	LIMatQuaternion q2;
	LIMatQuaternion q3;
	LIMatVector v0;
	LIMatVector v1;

	printf ("Testing quaternion multiplication...\n");
	q0 = limat_quaternion_rotation (0.5f, limat_vector_init (0.0f, 1.0f, 0.0f));
	m0 = limat_matrix_rotation (0.5f, 0.0f, 1.0f, 0.0f);
	v0 = limat_quaternion_transform (q0, limat_vector_init (10.0f, 100.0f, -50.0f));
	v1 = limat_matrix_transform (m0, limat_vector_init (10.0f, 100.0f, -50.0f));
	if (!check_vector (v0, v1))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m0);
		print_quaternion ("M->Q", limat_convert_matrix_to_quaternion (m0));
		print_quaternion ("Q", q0);
		print_vector ("M*v", v0);
		print_vector ("Q*v", v1);
	}
	q1 = limat_quaternion_rotation (1.0f, limat_vector_init (1.0f, 0.0f, 0.0f));
	q2 = limat_quaternion_multiply (q0, q1);
	m1 = limat_matrix_rotation (1.0f, 1.0f, 0.0f, 0.0f);
	m2 = limat_matrix_multiply (m0, m1);
	q3 = limat_convert_matrix_to_quaternion (m2);
	if (!check_quaternion (q2, q3))
	{
		printf ("2: FAILED!\n");
		print_matrix ("M", m2);
		print_quaternion ("M->Q", q3);
		print_quaternion ("Q", q2);
	}
	q2 = limat_quaternion_multiply (q1, q0);
	m2 = limat_matrix_multiply (m1, m0);
	q3 = limat_convert_matrix_to_quaternion (m2);
	if (!check_quaternion (q2, q3))
	{
		printf ("3: FAILED!\n");
		print_matrix ("M", m2);
		print_quaternion ("M->Q", q3);
		print_quaternion ("Q", q2);
	}
}

static void quaternion_rotation ()
{
	LIMatMatrix m0;
	LIMatQuaternion q0;
	LIMatQuaternion q1;
	LIMatVector v0;
	LIMatVector v1;

	printf ("Testing quaternion rotation...\n");
	m0 = limat_matrix_rotation (0.5f, 0.0f, 1.0f, 0.0f);
	q0 = limat_quaternion_rotation (0.5f, limat_vector_init (0.0f, 1.0f, 0.0f));
	q1 = limat_convert_matrix_to_quaternion (m0);
	if (!check_quaternion (q0, q1))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m0);
		print_quaternion ("Q", q0);
		print_quaternion ("M->Q", q1);
	}
	v0 = limat_matrix_transform (m0, limat_vector_init (10.0f, -10.0f, 0.0f));
	v1 = limat_quaternion_transform (q0, limat_vector_init (10.0f, -10.0f, 0.0f));
	if (!check_vector (v0, v1))
	{
		printf ("2: FAILED!\n");
		print_matrix ("M", m0);
		print_vector ("Q*v", v0);
		print_vector ("M*v", v1);
	}
}

static void quaternion_look ()
{
	LIMatMatrix m0;
	LIMatQuaternion q0;
	LIMatQuaternion q1;
	LIMatVector v0;
	LIMatVector v1;

	printf ("Testing quaternion look...\n");
	q0 = limat_quaternion_look (limat_vector_init (0.707, 0.0, 0.707), limat_vector_init (0.0, 1.0, 0.0));
	m0 = limat_matrix_look (0.0, 0.0, 0.0, 0.707, 0.0, 0.707, 0.0, 1.0, 0.0);
	q1 = limat_convert_matrix_to_quaternion (m0);
	if (!check_quaternion (q0, q1))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m0);
		print_quaternion ("M->Q", q1);
		print_quaternion ("Q", q0);
	}
	v0 = limat_matrix_transform (m0, limat_vector_init (10.0f, -10.0f, 0.0f));
	v1 = limat_quaternion_transform (q0, limat_vector_init (10.0f, -10.0f, 0.0f));
	if (!check_vector (v0, v1))
	{
		printf ("2: FAILED!\n");
		print_vector ("M*v", v0);
		print_vector ("Q*v", v1);
	}
}

static void quaternion_basis ()
{
	LIMatMatrix m0;
	LIMatQuaternion q0;
	LIMatQuaternion q1;
	LIMatVector v0;
	LIMatVector v1;

	printf ("Testing quaternion basis...\n");
	q0 = limat_quaternion_init (0.000000, 0.000000, 0.434966, 0.900447);
	v0 = limat_quaternion_get_basis (q0, 0);
	v1 = limat_vector_init (0.621610, -0.783327, 0.000000);
	if (!check_vector (v0, v1))
	{
		printf ("1: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	v0 = limat_quaternion_get_basis (q0, 1);
	v1 = limat_vector_init (0.783327, 0.621610, 0.000000);
	if (!check_vector (v0, v1))
	{
		printf ("2: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
	v0 = limat_quaternion_get_basis (q0, 2);
	v1 = limat_vector_init (0.000000, 0.000000, 1.000000);
	if (!check_vector (v0, v1))
	{
		printf ("3: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}

	q0 = limat_quaternion_look (limat_vector_init (-0.707, 0.0, 0.707), limat_vector_init (0.0, 1.0, 0.0));
	m0 = limat_matrix_look (0.0, 0.0, 0.0, -0.707, 0.0, 0.707, 0.0, 1.0, 0.0);
	q1 = limat_convert_matrix_to_quaternion (m0);
	if (!check_quaternion (q0, q1))
	{
		printf ("4: FAILED!\n");
		print_matrix ("M", m0);
		print_quaternion ("M->Q", q1);
		print_quaternion ("Q", q0);
	}
	v0 = limat_matrix_get_basis (m0, 0);
	v1 = limat_quaternion_get_basis (q0, 0);
	if (!check_vector (v0, v1))
	{
		printf ("5: FAILED!\n");
		print_vector ("M.x", v0);
		print_vector ("Q.x", v1);
	}
	v0 = limat_matrix_get_basis (m0, 1);
	v1 = limat_quaternion_get_basis (q0, 1);
	if (!check_vector (v0, v1))
	{
		printf ("6: FAILED!\n");
		print_vector ("M.y", v0);
		print_vector ("Q.y", v1);
	}
	v0 = limat_matrix_get_basis (m0, 2);
	v1 = limat_quaternion_get_basis (q0, 2);
	if (!check_vector (v0, v1))
	{
		printf ("7: FAILED!\n");
		print_vector ("M.z", v0);
		print_vector ("Q.z", v1);
	}
}

static void transform_look ()
{
	LIMatMatrix m0;
	LIMatTransform t0;
	LIMatTransform t1;

	printf ("Testing transform look...\n");
	m0 = limat_matrix_look (100.0, 0.0, 0.0, 0.707, 0.0, 0.707, 0.0, 1.0, 0.0);
	t0 = limat_transform_look (
		limat_vector_init (100.0, 0.0, 0.0),
		limat_vector_init (0.707, 0.0, 0.707),
		limat_vector_init (0.0, 1.0, 0.0));
	t1 = limat_convert_matrix_to_transform (m0);
	if (!check_transform (t0, t1))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m0);
		print_transform ("M->T", t1);
		print_transform ("T", t0);
	}
}

static void transform_multiply ()
{
	LIMatMatrix m0;
	LIMatMatrix m1;
	LIMatMatrix m2;
	LIMatTransform t0;
	LIMatTransform t1;
	LIMatTransform t2;
	LIMatTransform t3;

	printf ("Testing transform multiplication...\n");
	t0 = limat_convert_quaternion_to_transform (limat_quaternion_rotation (0.5f, limat_vector_init (0.0f, 1.0f, 0.0f)));
	t1 = limat_convert_vector_to_transform (limat_vector_init (100.0f, 0.0f, 0.0f));
	t2 = limat_transform_multiply (t0, t1);
	m0 = limat_matrix_rotation (0.5f, 0.0f, 1.0f, 0.0f);
	m1 = limat_matrix_translation (100.0f, 0.0f, 0.0f);
	m2 = limat_matrix_multiply (m0, m1);
	t3 = limat_convert_matrix_to_transform (m2);
	if (!check_transform (t2, t3))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m2);
		print_transform ("M->T", t3);
		print_transform ("T", t2);
	}
	t2 = limat_transform_multiply (t1, t2);
	m2 = limat_matrix_multiply (m1, m2);
	t3 = limat_convert_matrix_to_transform (m2);
	if (!check_transform (t2, t3))
	{
		printf ("2: FAILED!\n");
		print_matrix ("M", m2);
		print_transform ("M->T", t3);
		print_transform ("T", t2);
	}
	t0 = limat_transform_init (limat_vector_init (1.0f, 2.0f, -3.0f), limat_quaternion_init (1.0f, 0.0f, 0.0f, 0.0f));
	t1 = limat_transform_init (limat_vector_init (10.0f, -5.0f, 1.0f), limat_quaternion_init (0.0f, 0.0f, 1.0f, 0.0f));
	t2 = limat_transform_multiply (t0, t1);
	m0 = limat_convert_transform_to_matrix (t0);
	m1 = limat_convert_transform_to_matrix (t1);
	m2 = limat_matrix_multiply (m0, m1);
	t3 = limat_convert_matrix_to_transform (m2);
	if (!check_transform (t2, t3))
	{
		printf ("3: FAILED!\n");
		print_matrix ("M", m2);
		print_transform ("M->T", t3);
		print_transform ("T", t2);
	}
	t2 = limat_transform_multiply (t1, t2);
	m2 = limat_matrix_multiply (m1, m2);
	t3 = limat_convert_matrix_to_transform (m2);
	if (!check_transform (t2, t3))
	{
		printf ("4: FAILED!\n");
		print_matrix ("M", m2);
		print_transform ("M->T", t3);
		print_transform ("T", t2);
	}
}

static void transform_invert ()
{
	LIMatMatrix m0;
	LIMatMatrix m1;
	LIMatTransform t0;
	LIMatTransform t1;
	LIMatTransform t2;

	printf ("Testing transform inverse...\n");
	t0 = limat_transform_init (limat_vector_init (10, -3, 9), limat_quaternion_init (0.5, -0.5, 0.5, -0.5));
	t1 = limat_transform_invert (t0);
	m0 = limat_convert_transform_to_matrix (t0);
	m1 = limat_matrix_invert (m0);
	t2 = limat_convert_matrix_to_transform (m1);
	if (!check_transform (t1, t2))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m1);
		print_transform ("M->T", t2);
		print_transform ("T", t1);
	}
	t0 = limat_transform_init (limat_vector_init (-1, 7, -2), limat_quaternion_init (-1, 0, 0, 0));
	t1 = limat_transform_invert (t0);
	m0 = limat_convert_transform_to_matrix (t0);
	m1 = limat_matrix_invert (m0);
	t2 = limat_convert_matrix_to_transform (m1);
	if (!check_transform (t1, t2))
	{
		printf ("2: FAILED!\n");
		print_matrix ("M", m1);
		print_transform ("M->T", t2);
		print_transform ("T", t1);
	}
}

static void transform_vector ()
{
	LIMatMatrix m0;
	LIMatTransform t0;
	LIMatVector v0;
	LIMatVector v1;
	LIMatVector v2;

	printf ("Testing vector transforming...\n");
	v0 = limat_vector_init (10.0f, 20.0f, 30.0f);
	v1 = limat_vector_normalize (limat_vector_init (1.0f, 0.5f, 0.25f));
	v2 = limat_vector_init (0.0f, 1.0f, 0.0f);
	m0 = limat_matrix_look (v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z);
	t0 = limat_transform_look (v0, v1, v2);
	v0 = limat_vector_init (-30.0f, 10.0f, -20.0f);
	v1 = limat_matrix_transform (m0, v0);
	v2 = limat_transform_transform (t0, v0);
	if (!check_vector (v1, v2))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m0);
		print_transform ("T", t0);
		print_vector ("M*v", v0);
		print_vector ("T*v", v1);
	}
	v0 = limat_vector_init (1000.0f, -20.0f, 1.0f);
	v1 = limat_vector_normalize (limat_vector_init (0.1f, 0.1f, 0.9f));
	v2 = limat_vector_init (1.0f, 0.0f, 0.0f);
	m0 = limat_matrix_look (v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z);
	t0 = limat_transform_look (v0, v1, v2);
	v0 = limat_vector_init (1.0f, 1.0f, 1.0f);
	v1 = limat_matrix_transform (m0, v0);
	v2 = limat_transform_transform (t0, v0);
	if (!check_vector (v1, v2))
	{
		printf ("2: FAILED!\n");
		print_matrix ("M", m0);
		print_transform ("T", t0);
		print_vector ("M*v", v0);
		print_vector ("T*v", v1);
	}
}

static void transform_multiply_2 ()
{
	LIMatMatrix m0;
	LIMatMatrix m1;
	LIMatMatrix m2;
	LIMatMatrix m3;
	LIMatTransform t0 = {{61.829472,20.202068,67.273712}, {-0.545886,0.190507,-0.467286,0.668849}};
	LIMatTransform t1 = {{0.006661,0.008117,0.009031}, {-0.707107,-0.000000,0.000000,0.707107}};
	LIMatTransform t2 = {{-0.006661,0.009031,-0.008117}, {0.707107,0.000000,-0.000000,0.707107}};
	LIMatTransform t3 = {{61.823761,20.202841,67.261116}, {0.086948,-0.195712,-0.465130,0.858948}};
	LIMatTransform t4;

	printf ("Testing transform multiplication 2...\n");
	m0 = limat_convert_transform_to_matrix (t0);
	t4 = limat_convert_matrix_to_transform (m0);
	if (!check_transform (t0, t4))
	{
		printf ("1: FAILED!\n");
		print_matrix ("M", m0);
		print_transform ("T", t0);
		print_transform ("Tm", t4);
	}
	m1 = limat_convert_transform_to_matrix (t1);
	t4 = limat_convert_matrix_to_transform (m1);
	if (!check_transform (t1, t4))
	{
		printf ("2: FAILED!\n");
		print_matrix ("M", m1);
		print_transform ("T", t1);
		print_transform ("Tm", t4);
	}
	m2 = limat_matrix_invert (m1);
	t4 = limat_convert_matrix_to_transform (m2);
	if (!check_transform (t2, t4))
	{
		printf ("3: FAILED!\n");
		print_matrix ("M", m2);
		print_transform ("T", t1);
		print_transform ("Tm", t4);
	}
	m3 = limat_matrix_multiply (m0, m2);
	t4 = limat_convert_matrix_to_transform (m3);
	if (!check_transform (t3, t4))
	{
		printf ("4: FAILED!\n");
		print_matrix ("M", m3);
		print_transform ("T", t1);
		print_transform ("Tm", t4);
	}
	t4 = limat_transform_multiply (t0, t2);
	if (!check_transform (t3, t4))
	{
		printf ("5: FAILED!\n");
		print_transform ("Got", t4);
		print_transform ("Expected", t3);
	}
}

static void vector_basics ()
{
	LIMatVector v0;
	LIMatVector v1;

	printf ("Testing vector basics...\n");
	v0 = limat_vector_init (0.0f, 0.0f, 0.0f);
	v0 = limat_vector_normalize (v0);
	v1 = limat_vector_init (0.0f, 0.0f, 0.0f);
	if (isnan (v0.x) || isnan (v0.y) || isnan (v0.z))
	{
		printf ("1: FAILED!\n");
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
}

/*****************************************************************************/

static void dualquat_basics ()
{
	LIMatDualquat dq0;
	LIMatDualquat dq1;
	LIMatVector v0;
	LIMatVector v1;
	LIMatVector v2;
	LIMatQuaternion q0;

	printf ("Testing dual quaternion basics...\n");

	/* Multiplication. */
	v0 = limat_vector_init (-10.0f, 20.0f, 30.0f);
	q0 = limat_quaternion_init (0.0f, 1.0f, 0.0f, 0.0f);
	dq0 = limat_dualquat_init (v0, q0);
	dq1 = limat_dualquat_multiply (
		limat_dualquat_init_translation (v0),
		limat_dualquat_init_rotation (q0));
	if (!check_quaternion (dq0.r, dq1.r) ||
	    !check_quaternion (dq0.d, dq1.d))
	{
		printf ("1: FAILED!\n");
		print_dualquat ("Got", dq0);
		print_dualquat ("Expected", dq1);
	}

	/* Transforming vectors. */
	v0 = limat_vector_init (10.0f, 20.0f, 30.0f);
	dq0 = limat_dualquat_init_translation (v0);
	v1 = limat_dualquat_transform (dq0, limat_vector_init (3.0f, 2.0f, 1.0f));
	v2 = limat_vector_init (13.0f, 22.0f, 31.0f);
	if (!check_vector (v1, v2))
	{
		printf ("2: FAILED!\n");
		print_dualquat ("DQ", dq0);
		print_vector ("Got", v0);
		print_vector ("Expected", v1);
	}
}

void limat_math_unittest ()
{
	convert_quaternion_matrix ();
	convert_transform_matrix ();
	matrix_basics ();
	matrix_basis ();
	matrix_look ();
	matrix_perspective ();
	quaternion_basics ();
	quaternion_rotation ();
	quaternion_multiply ();
	quaternion_look ();
	quaternion_basis ();
	transform_multiply ();
	transform_multiply_2 ();
	transform_invert ();
	transform_look ();
	transform_vector ();
	vector_basics ();
	dualquat_basics ();
}
