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
 */

#ifndef __MATH_VERTEX_H__
#define __MATH_VERTEX_H__

#include <stddef.h>
#include "math-vector.h"

/*
 * \addtogroup LIMatVtxops Vtxops
 * @{
 */

typedef struct _LIMatVtxops LIMatVtxops;
struct _LIMatVtxops
{
	size_t size;
	void (*interpolate)(const void* self, const void* other, float frac, void* result);
	void (*getcoord)(const void* self, LIMatVector* result);
	void (*setcoord)(void* self, const LIMatVector* value);
};

/** @} */

/*****************************************************************************/

/*
 * \addtogroup LIMatVertexV3 VertexV3
 * @{
 */

typedef struct _LIMatVertexV3 LIMatVertexV3;
struct _LIMatVertexV3
{
	LIMatVector coord;
};

static inline void
__limat_vtxops_v3_getcoord (const void*  self,
                            LIMatVector* result)
{
	const LIMatVertexV3* data = (LIMatVertexV3*) self;

	*result = data->coord;
}

static inline void
__limat_vtxops_v3_setcoord (void*              self,
                            const LIMatVector* value)
{
	LIMatVertexV3* data = (LIMatVertexV3*) self;

	data->coord = *value;
}

static inline void
__limat_vtxops_v3_interpolate (const void* self,
                               const void* other,
                               float       blend,
                               void*       result)
{
}

static const LIMatVtxops limat_vtxops_v3 =
{
	sizeof (LIMatVertexV3),
	__limat_vtxops_v3_interpolate,
	__limat_vtxops_v3_getcoord,
	__limat_vtxops_v3_setcoord
};

/** @} */

/*****************************************************************************/

/*
 * \addtogroup LIMatVertexT2V3N3 VertexT2V3N3
 * @{
 */

typedef struct _LIMatVertexT2N3V3 LIMatVertexT2N3V3;
struct _LIMatVertexT2N3V3
{
	float texcoord[2];
	LIMatVector normal;
	LIMatVector coord;
};

static inline void
__limat_vtxops_t2n3v3_getcoord (const void*  self,
                                LIMatVector* result)
{
	const LIMatVertexT2N3V3* data = (LIMatVertexT2N3V3*) self;

	*result = data->coord;
}

static inline void
__limat_vtxops_t2n3v3_setcoord (void*              self,
                                const LIMatVector* value)
{
	LIMatVertexT2N3V3* data = (LIMatVertexT2N3V3*) self;

	data->coord = *value;
}

static inline void
__limat_vtxops_t2n3v3_interpolate (const void* self,
                                   const void* other,
                                   float       blend,
                                   void*       result)
{
	const LIMatVertexT2N3V3* data0 = (LIMatVertexT2N3V3*) self;
	const LIMatVertexT2N3V3* data1 = (LIMatVertexT2N3V3*) other;
	LIMatVertexT2N3V3* data2 = (LIMatVertexT2N3V3*) result;

	data2->normal = limat_vector_normalize (limat_vector_add (
		limat_vector_multiply (data0->normal, blend),
		limat_vector_multiply (data1->normal, 1.0f - blend)));
	data2->texcoord[0] = data0->texcoord[0] * blend + data1->texcoord[0] * (1.0f - blend);
	data2->texcoord[1] = data0->texcoord[1] * blend + data1->texcoord[1] * (1.0f - blend);
}

static const LIMatVtxops limat_vtxops_t2n3v3 =
{
	sizeof (LIMatVertexT2N3V3),
	__limat_vtxops_t2n3v3_interpolate,
	__limat_vtxops_t2n3v3_getcoord,
	__limat_vtxops_t2n3v3_setcoord
};

/*****************************************************************************/

/*
 * \addtogroup LIMatVertexT6V3N3 VertexT6V3N3
 * @{
 */

typedef struct _LIMatVertexT6N3V3 LIMatVertexT6N3V3;
struct _LIMatVertexT6N3V3
{
	float texcoord[6];
	LIMatVector normal;
	LIMatVector coord;
};

static inline void
__limat_vtxops_t6n3v3_getcoord (const void*  self,
                                LIMatVector* result)
{
	const LIMatVertexT6N3V3* data = (LIMatVertexT6N3V3*) self;

	*result = data->coord;
}

static inline void
__limat_vtxops_t6n3v3_setcoord (void*              self,
                                const LIMatVector* value)
{
	LIMatVertexT6N3V3* data = (LIMatVertexT6N3V3*) self;

	data->coord = *value;
}

static inline void
__limat_vtxops_t6n3v3_interpolate (const void* self,
                                   const void* other,
                                   float       blend,
                                   void*       result)
{
	const LIMatVertexT6N3V3* data0 = (LIMatVertexT6N3V3*) self;
	const LIMatVertexT6N3V3* data1 = (LIMatVertexT6N3V3*) other;
	LIMatVertexT6N3V3* data2 = (LIMatVertexT6N3V3*) result;

	data2->normal = limat_vector_normalize (limat_vector_add (
		limat_vector_multiply (data0->normal, blend),
		limat_vector_multiply (data1->normal, 1.0f - blend)));
	data2->texcoord[0] = data0->texcoord[0] * blend + data1->texcoord[0] * (1.0f - blend);
	data2->texcoord[1] = data0->texcoord[1] * blend + data1->texcoord[1] * (1.0f - blend);
	data2->texcoord[2] = data0->texcoord[2] * blend + data1->texcoord[2] * (1.0f - blend);
	data2->texcoord[3] = data0->texcoord[3] * blend + data1->texcoord[3] * (1.0f - blend);
	data2->texcoord[4] = data0->texcoord[4] * blend + data1->texcoord[4] * (1.0f - blend);
	data2->texcoord[5] = data0->texcoord[5] * blend + data1->texcoord[5] * (1.0f - blend);
}

static const LIMatVtxops limat_vtxops_t6n3v3 =
{
	sizeof (LIMatVertexT6N3V3),
	__limat_vtxops_t6n3v3_interpolate,
	__limat_vtxops_t6n3v3_getcoord,
	__limat_vtxops_t6n3v3_setcoord
};

/** @} */

#endif

/** @} */
