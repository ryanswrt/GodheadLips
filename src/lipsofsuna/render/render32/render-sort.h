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

#ifndef __RENDER32_SORT_H__
#define __RENDER32_SORT_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-buffer.h"
#include "render-material.h"
#include "render-types.h"
#include "render-mesh.h"

enum
{
	LIREN_SORT_TYPE_FACE,
	LIREN_SORT_TYPE_GROUP,
	LIREN_SORT_TYPE_PARTICLE
};

struct _LIRenSortgroup32
{
	int index;
	int count;
	int transparent;
	LIMatAabb bounds;
	LIMatMatrix matrix;
	LIRenMesh32* mesh;
	LIRenMaterial32* material;
};

struct _LIRenSortface32
{
	int type;
	LIRenSortface32* next;
	union
	{
		struct
		{
			int index;
			LIMatAabb bounds;
			LIMatMatrix matrix;
			LIRenMesh32* mesh;
			LIRenMaterial32* material;
		} face;
		struct
		{
			int index;
			int count;
			LIMatAabb bounds;
			LIMatMatrix matrix;
			LIMdlPose* pose;
			LIRenMesh32* mesh;
			LIRenMaterial32* material;
		} group;
		struct
		{
			float size;
			float diffuse[4];
			LIMatVector position;
			LIRenImage32* image;
			LIRenShader32* shader;
		} particle;
	};
};

struct _LIRenSort32
{
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenRender32* render;
	struct
	{
		int count;
		LIRenSortface32** array;
	} buckets;
	struct
	{
		int count;
		int capacity;
		LIRenSortface32* array;
	} faces;
};

LIAPICALL (LIRenSort32*, liren_sort32_new, (
	LIRenRender32* render));

LIAPICALL (void, liren_sort32_free, (
	LIRenSort32* self));

LIAPICALL (int, liren_sort32_add_group, (
	LIRenSort32*     self,
	LIMatAabb*       bounds,
	LIMatMatrix*     matrix,
	int              index,
	int              count,
	LIRenMesh32*     mesh,
	LIRenMaterial32* material,
	LIMatVector*     center));

LIAPICALL (int, liren_sort32_add_faces, (
	LIRenSort32*     self,
	LIMatAabb*       bounds,
	LIMatMatrix*     matrix,
	int              index,
	int              count,
	LIRenMesh32*     mesh,
	LIRenMaterial32* material,
	LIMdlPose*       pose,
	LIMatVector*     center));

LIAPICALL (int, liren_sort32_add_object, (
	LIRenSort32*   self,
	LIRenObject32* object));

LIAPICALL (int, liren_sort32_add_particle, (
	LIRenSort32*       self,
	const LIMatVector* position,
	float              size,
	const float*       diffuse,
	LIRenImage32*      image,
	LIRenShader32*     shader));

LIAPICALL (void, liren_sort32_clear, (
	LIRenSort32*       self,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection));

#endif
