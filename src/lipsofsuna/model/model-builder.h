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

#ifndef __MODEL_BUILDER_H__
#define __MODEL_BUILDER_H__

#include "lipsofsuna/algorithm.h"
#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"
#include "model.h"

typedef struct _LIMdlBuilderFaces LIMdlBuilderFaces;
struct _LIMdlBuilderFaces
{
	struct
	{
		int count;
		int capacity;
		LIMdlIndex* array;
	} indices;
};

typedef struct _LIMdlBuilderLod LIMdlBuilderLod;
struct _LIMdlBuilderLod
{
	struct
	{
		int count;
		int capacity;
		LIMdlBuilderFaces* array;
	} face_groups;
};

typedef struct _LIMdlBuilder LIMdlBuilder;
struct _LIMdlBuilder
{
	int material_capacity;
	int weightgroup_capacity;
	int vertex_capacity;
	int vertex_lookup_count;
	LIAlgMemdic* vertex_lookup;
	LIMdlModel* model;
	struct
	{
		int count;
		int capacity;
		LIMdlBuilderLod* array;
	} lod;
};

LIAPICALL (LIMdlBuilder*, limdl_builder_new, (
	LIMdlModel* model));

LIAPICALL (void, limdl_builder_free, (
	LIMdlBuilder* self));

LIAPICALL (int, limdl_builder_add_detail_levels, (
	LIMdlBuilder* self,
	int           count));

LIAPICALL (int, limdl_builder_calculate_lod, (
	LIMdlBuilder* self,
	int           levels,
	float         factor));

LIAPICALL (int, limdl_builder_find_vertex, (
	LIMdlBuilder*      self,
	const LIMdlVertex* vertex));

LIAPICALL (int, limdl_builder_finish, (
	LIMdlBuilder* self));

LIAPICALL (int, limdl_builder_insert_face, (
	LIMdlBuilder*      self,
	int                level,
	int                material,
	const LIMdlVertex* vertices,
	const int*         bone_mapping));

LIAPICALL (int, limdl_builder_insert_face_welded, (
	LIMdlBuilder*      self,
	int                level,
	int                material,
	const LIMdlVertex* vertices));

LIAPICALL (int, limdl_builder_insert_indices, (
	LIMdlBuilder*     self,
	int               level,
	int               material,
	const LIMdlIndex* indices,
	int               count,
	int               vertex_start_remap));

LIAPICALL (int, limdl_builder_insert_material, (
	LIMdlBuilder*        self,
	const LIMdlMaterial* material));

LIAPICALL (int, limdl_builder_insert_node, (
	LIMdlBuilder*    self,
	const LIMdlNode* node));

LIAPICALL (int, limdl_builder_insert_vertices, (
	LIMdlBuilder*      self,
	const LIMdlVertex* vertices,
	int                count,
	const int*         bone_mapping));

LIAPICALL (int, limdl_builder_insert_weightgroup, (
	LIMdlBuilder* self,
	const char*   name,
	const char*   bone));

#endif
