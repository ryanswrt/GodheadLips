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

#ifndef __MODEL_H__
#define __MODEL_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "model-animation.h"
#include "model-bone.h"
#include "model-faces.h"
#include "model-hair.h"
#include "model-light.h"
#include "model-lod.h"
#include "model-material.h"
#include "model-particle.h"
#include "model-node.h"
#include "model-shape.h"
#include "model-shape-key.h"
#include "model-types.h"
#include "model-vertex.h"
#include "model-weight-group.h"

#define LIMDL_FORMAT_VERSION 0xFFFFFFF3

typedef int LIMdlModelFlags;

struct _LIMdlModel
{
	int flags;
	LIMatAabb bounds;
	struct { int count; LIMdlAnimation* array; } animations;
	struct { int count; LIMdlHairs* array; } hairs;
	struct { int count; LIMdlLod* array; } lod;
	struct { int count; LIMdlMaterial* array; } materials;
	struct { int count; LIMdlNode** array; } nodes;
	struct { int count; LIMdlParticleSystem* array; } particle_systems;
	struct { int count; LIMdlShape* array; } shapes;
	struct { int count; LIMdlShapeKey* array; } shape_keys;
	struct { int count; LIMdlVertex* array; } vertices;
	struct { int count; LIMdlWeightGroup* array; } weight_groups;
};

LIAPICALL (LIMdlModel*, limdl_model_new, ());

LIAPICALL (LIMdlModel*, limdl_model_new_copy, (
	const LIMdlModel* model));

LIAPICALL (LIMdlModel*, limdl_model_new_from_data, (
	LIArcReader* reader,
	int          mesh));

LIAPICALL (LIMdlModel*, limdl_model_new_from_file, (
	const char* path,
	int         mesh));

LIAPICALL (void, limdl_model_free, (
	LIMdlModel* self));

LIAPICALL (void, limdl_model_calculate_bounds, (
	LIMdlModel* self));

LIAPICALL (void, limdl_model_calculate_tangents, (
	LIMdlModel* self));

LIAPICALL (void, limdl_model_clear_vertices, (
	LIMdlModel* self));

LIAPICALL (LIMdlAnimation*, limdl_model_find_animation, (
	LIMdlModel* self,
	const char* name));

LIAPICALL (int, limdl_model_find_material, (
	const LIMdlModel*    self,
	const LIMdlMaterial* material));

LIAPICALL (LIMdlMaterial*, limdl_model_find_material_by_texture, (
	LIMdlModel* self,
	const char* shader,
	const char* texture));

LIAPICALL (LIMdlNode*, limdl_model_find_node, (
	const LIMdlModel* self,
	const char*       name));

LIAPICALL (LIMdlShapeKey*, limdl_model_find_shape_key, (
	LIMdlModel* self,
	const char* name));

LIAPICALL (int, limdl_model_find_vertex, (
	LIMdlModel*        self,
	const LIMdlVertex* vertex));

LIAPICALL (int, limdl_model_find_weightgroup, (
	LIMdlModel* self,
	const char* name,
	const char* bone));

LIAPICALL (int, limdl_model_merge, (
	LIMdlModel* self,
	LIMdlModel* model));

LIAPICALL (int, limdl_model_morph, (
	LIMdlModel* self,
	LIMdlModel* ref,
	const char* shape,
	float       value));

LIAPICALL (int, limdl_model_write, (
	const LIMdlModel* self,
	LIArcWriter*      writer));

LIAPICALL (int, limdl_model_write_file, (
	const LIMdlModel* self,
	const char*       path));

LIAPICALL (int, limdl_model_get_memory, (
	const LIMdlModel* self));

#endif
