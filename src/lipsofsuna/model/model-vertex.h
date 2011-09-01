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

#ifndef __MODEL_VERTEX_H__
#define __MODEL_VERTEX_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"

#define LIMDL_VERTEX_WEIGHTS_MAX 4
#define LIMDL_VERTEX_WEIGHT_UINT16
#define LIMDL_VERTEX_WEIGHT_MAX 65535.0f

typedef uint16_t LIMdlVertexWeight;
typedef uint8_t LIMdlVertexColor;
typedef uint8_t LIMdlVertexBone;

typedef struct _LIMdlVertex LIMdlVertex;
struct _LIMdlVertex
{
	float texcoord[2];
	LIMatVector normal;
	LIMatVector coord;
	LIMatVector tangent;
	LIMdlVertexColor color[4];
	LIMdlVertexWeight weights[LIMDL_VERTEX_WEIGHTS_MAX];
	LIMdlVertexBone bones[LIMDL_VERTEX_WEIGHTS_MAX];
};

static inline void limdl_vertex_init (
	LIMdlVertex*       self,
	const LIMatVector* coord,
	const LIMatVector* normal,
	float              u,
	float              v)
{
	memset (self, 0, sizeof (LIMdlVertex));
	self->texcoord[0] = u;
	self->texcoord[1] = v;
	self->normal = *normal;
	self->coord = *coord;
	self->color[0] = 255;
	self->color[1] = 255;
	self->color[2] = 255;
	self->color[3] = 255;
#ifdef LIMDL_VERTEX_WEIGHT_FLOAT
	self->weights[0] = 1.0f;
#else
	self->weights[0] = (int) LIMDL_VERTEX_WEIGHT_MAX;
#endif
}

static inline void limdl_vertex_init_copy (
	LIMdlVertex*       self,
	const LIMdlVertex* vertex)
{
	memcpy (self, vertex, sizeof (LIMdlVertex));
}

static inline int limdl_vertex_compare (
	const LIMdlVertex* self,
	const LIMdlVertex* vertex)
{
	return memcmp (self, vertex, sizeof (LIMdlVertex));
}

static inline void limdl_vertex_round (
	LIMdlVertex* self)
{
#define ROUND(a, b) a = (int)(a / b + 0.5f) * b
	ROUND (self->texcoord[0], 0.01f);
	ROUND (self->texcoord[1], 0.01f);
	ROUND (self->normal.x, 0.1f);
	ROUND (self->normal.y, 0.1f);
	ROUND (self->normal.z, 0.1f);
	ROUND (self->coord.x, 0.1f);
	ROUND (self->coord.y, 0.1f);
	ROUND (self->coord.z, 0.1f);
	ROUND (self->tangent.x, 0.1f);
	ROUND (self->tangent.y, 0.1f);
	ROUND (self->tangent.z, 0.1f);
	ROUND (self->color[0], 0.1f);
	ROUND (self->color[1], 0.1f);
	ROUND (self->color[2], 0.1f);
	ROUND (self->color[3], 0.1f);
#undef ROUND
}

#endif
