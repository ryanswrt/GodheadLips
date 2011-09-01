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

/**
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlBuilder Builder
 * @{
 */

#include "model-builder.h"

static int private_faces_init (
	LIMdlBuilderFaces* self,
	LIMdlFaces*        src,
	LIMdlLod*          srclod);

static int private_faces_copy (
	LIMdlBuilderFaces* self,
	LIMdlBuilderFaces* src);

static void private_faces_clear (
	LIMdlBuilderFaces* self);

static int private_lod_init (
	LIMdlBuilderLod* self,
	LIMdlLod*        src);

static int private_lod_copy (
	LIMdlBuilderLod* self,
	LIMdlBuilderLod* src);

static void private_lod_clear (
	LIMdlBuilderLod* self);

static int private_realloc_array (
	void*  array,
	int*   capacity,
	int    required,
	size_t size);

/*****************************************************************************/

/**
 * \brief Creates a model builder.
 * \param model Model or NULL.
 * \return Model builder or NULL.
 */
LIMdlBuilder* limdl_builder_new (
	LIMdlModel* model)
{
	int j;
	LIMdlBuilder* self;
	LIMdlBuilderLod* dstlod;
	LIMdlLod* srclod;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlBuilder));
	if (self == NULL)
		return NULL;

	/* Use or allocate model. */
	if (model == NULL)
	{
		self->model = limdl_model_new ();
		if (self->model == NULL)
		{
			limdl_builder_free (self);
			return NULL;
		}
	}
	else
		self->model = model;

	/* Allocate levels of detail. */
	lisys_assert (self->model->lod.count);
	self->lod.array = lisys_calloc (self->model->lod.count, sizeof (LIMdlBuilderLod));
	if (self->lod.array == NULL)
	{
		limdl_builder_free (self);
		return NULL;
	}
	self->lod.count = self->model->lod.count;

	/* Initialize levels of detail. */
	for (j = 0 ; j < self->model->lod.count ; j++)
	{
		srclod = self->model->lod.array + j;
		dstlod = self->lod.array + j;
		lisys_assert (srclod->face_groups.count == self->model->materials.count);
		if (!private_lod_init (dstlod, srclod))
		{
			limdl_builder_free (self);
			return NULL;
		}
	}

	/* Initialize builder state. */
	self->material_capacity = self->model->materials.count;
	self->weightgroup_capacity = self->model->weight_groups.count;
	self->vertex_capacity = self->model->vertices.count;

	return self;
}

/**
 * \brief Frees the model builder.
 * \param self Model builder.
 */
void limdl_builder_free (
	LIMdlBuilder* self)
{
	int j;

	if (self->vertex_lookup)
		lialg_memdic_free (self->vertex_lookup);
	for (j = 0 ; j < self->lod.count ; j++)
		private_lod_clear (self->lod.array + j);
	lisys_free (self->lod.array);
	lisys_free (self);
}

/**
 * \brief Adds detail levels.
 * \param self Model builder.
 * \param count Number of levels to add.
 * \return Nonzero on success.
 */
int limdl_builder_add_detail_levels (
	LIMdlBuilder* self,
	int           count)
{
	int i;
	int orig;
	LIMdlBuilderLod* lod;

	orig = self->lod.count - 1;
	lod = lisys_realloc (self->lod.array, (self->lod.count + count) * sizeof (LIMdlBuilderLod));
	if (lod == NULL)
		return 0;
	self->lod.array = lod;

	for (i = 0 ; i < count ; i++)
	{
		lod = self->lod.array + self->lod.count;
		if (!private_lod_copy (lod, self->lod.array + orig))
			return 0;
		lisys_assert (lod->face_groups.count == self->model->materials.count);
		self->lod.count++;
	}

	return 1;
}

/**
 * \brief Finds the index of a matching vertex.
 * \param self Model builder.
 * \param vertex Matched vertex.
 * \return Vertex index or -1.
 */
int limdl_builder_find_vertex (
	LIMdlBuilder*      self,
	const LIMdlVertex* vertex)
{
	int i;
	void* ptr;
	LIMdlVertex tmp;

	/* Create the vertex lookup table. */
	if (self->vertex_lookup == NULL)
	{
		self->vertex_lookup = lialg_memdic_new ();
		if (self->vertex_lookup == NULL)
			return -1;
	}

	/* Update the vertex lookup table. */
	for (i = self->vertex_lookup_count ; i < self->model->vertices.count ; i++)
	{
		ptr = (void*)(intptr_t)(i + 1);
		tmp = self->model->vertices.array[i];
		limdl_vertex_round (&tmp);
		lialg_memdic_insert (self->vertex_lookup, &tmp, sizeof (LIMdlVertex), ptr);
	}
	self->vertex_lookup_count = self->model->vertices.count;

	/* Find the vertex. */
	tmp = *vertex;
	limdl_vertex_round (&tmp);
	ptr = lialg_memdic_find (self->vertex_lookup, &tmp, sizeof (LIMdlVertex));
	if (ptr == NULL)
		return -1;

	return ((int)(intptr_t) ptr) - 1;
}

/**
 * \brief Finishes building the model.
 * \param self Model builder.
 * \return Nonzero on success.
 */
int limdl_builder_finish (
	LIMdlBuilder* self)
{
	int i;
	int j;
	int num_indices;
	int pos_indices;
	LIMdlBuilderFaces* srcfaces;
	LIMdlFaces* dstfaces;
	LIMdlFaces* new_faces;
	LIMdlIndex* new_indices;
	LIMdlLod* new_lod;
	LIMdlLod* dstlod;
	LIMdlBuilderLod* srclod;

	/* Create new LOD data. */
	new_lod = lisys_calloc (self->lod.count, sizeof (LIMdlLod));
	if (new_lod == NULL)
		return 0;
	for (j = 0 ; j < self->lod.count ; j++)
	{
		srclod = self->lod.array + j;
		dstlod = new_lod + j;

		/* Recreate face groups. */
		num_indices = 0;
		if (srclod->face_groups.count)
		{
			new_faces = lisys_calloc (srclod->face_groups.count, sizeof (LIMdlFaces));
			if (new_faces == NULL)
			{
				lisys_free (new_lod);
				return 0;
			}
			for (i = 0 ; i < srclod->face_groups.count ; i++)
			{
				srcfaces = srclod->face_groups.array + i;
				dstfaces = new_faces + i;
				dstfaces->start = num_indices;
				dstfaces->count = srcfaces->indices.count;
				num_indices += srcfaces->indices.count;
			}
			dstlod->face_groups.array = new_faces;
			dstlod->face_groups.count = srclod->face_groups.count;
		}
		lisys_assert (dstlod->face_groups.count == srclod->face_groups.count);
		lisys_assert (dstlod->face_groups.count == self->model->materials.count);

		/* Merge indices of face groups. */
		if (num_indices)
		{
			new_indices = lisys_calloc (num_indices, sizeof (LIMdlIndex));
			if (new_indices == NULL)
			{
				lisys_free (new_lod);
				return 0;
			}
			pos_indices = 0;
			for (i = 0 ; i < srclod->face_groups.count ; i++)
			{
				srcfaces = srclod->face_groups.array + i;
				memcpy (new_indices + pos_indices, srcfaces->indices.array, srcfaces->indices.count * sizeof (LIMdlIndex));
				pos_indices += srcfaces->indices.count;
			}
			lisys_assert (pos_indices == num_indices);
			dstlod->indices.array = new_indices;
			dstlod->indices.count = num_indices;
		}
	}

	/* Use the new LOD data. */
	for (i = 0 ; i < self->model->lod.count ; i++)
		limdl_lod_free (self->model->lod.array + i);
	lisys_free (self->model->lod.array);
	self->model->lod.array = new_lod;
	self->model->lod.count = self->lod.count;

	/* Calculate the bounding box. */
	limdl_model_calculate_bounds (self->model);

	/* Calculate vertex tangents. */
	limdl_model_calculate_tangents (self->model);

	return 1;
}

/**
 * \brief Inserts a triangle to the model.
 *
 * Inserts new vertices, vertex weights, and indices to the model.
 *
 * \param self Model builder.
 * \param level Level of detail.
 * \param material Material index.
 * \param vertices Array of three vertices.
 * \param bone_mapping Bone index mapping array or NULL.
 * \return Nonzero on success.
 */
int limdl_builder_insert_face (
	LIMdlBuilder*      self,
	int                level,
	int                material,
	const LIMdlVertex* vertices,
	const int*         bone_mapping)
{
	LIMdlIndex indices[3];

	/* Insert vertices. */
	if (!limdl_builder_insert_vertices (self, vertices, 3, bone_mapping))
		return 0;

	/* Insert indices. */
	indices[0] = self->model->vertices.count - 3;
	indices[1] = self->model->vertices.count - 2;
	indices[2] = self->model->vertices.count - 1;
	if (!limdl_builder_insert_indices (self, level, material, indices, 3, 0))
	{
		self->model->vertices.count -= 3;
		return 0;
	}

	return 1;
}

/**
 * \brief Inserts a triangle to the model while trying to weld vertices.
 *
 * Inserts vertices and indices to the model, merging the new vertex with
 * existing vertices, if possible.
 *
 * \param self Model builder.
 * \param level Level of detail.
 * \param material Material index.
 * \param vertices Array of three vertices.
 * \return Nonzero on success.
 */
int limdl_builder_insert_face_welded (
	LIMdlBuilder*      self,
	int                level,
	int                material,
	const LIMdlVertex* vertices)
{
	int i;
	int index;
	LIMdlIndex indices[3];

	/* Insert vertices. */
	for (i = 0 ; i < 3 ; i++)
	{
		index = limdl_builder_find_vertex (self, vertices + i);
		if (index == -1)
		{
			if (!limdl_builder_insert_vertices (self, vertices + i, 1, NULL))
				return 0;
			indices[i] = self->model->vertices.count - 1;
		}
		else
			indices[i] = index;
	}

	/* Insert indices. */
	if (!limdl_builder_insert_indices (self, level, material, indices, 3, 0))
	{
		self->model->vertices.count -= 3;
		return 0;
	}

	return 1;
}

/**
 * \brief Inserts indices to the model.
 * \param self Model builder.
 * \param level Detail level.
 * \param material Material index.
 * \param indices Array of indices.
 * \param count Number of indices.
 * \param vertex_start_remap Offset of the first vertex for remap purposes.
 * \return Nonzero on success.
 */
int limdl_builder_insert_indices (
	LIMdlBuilder*     self,
	int               level,
	int               material,
	const LIMdlIndex* indices,
	int               count,
	int               vertex_start_remap)
{
	int i;
	LIMdlBuilderLod* lod;
	LIMdlBuilderFaces* group;

	lisys_assert (material >= 0);
	lisys_assert (material < self->model->materials.count);
	lisys_assert (level >= 0);
	lisys_assert (level < self->lod.count);

	lod = self->lod.array + level;
	group = lod->face_groups.array + material;

	/* Allocate space for indices. */
	if (!private_realloc_array (&group->indices.array, &group->indices.capacity,
	    group->indices.count + count, sizeof (LIMdlIndex)))
		return 0;

	/* Insert indices. */
	if (vertex_start_remap)
	{
		for (i = 0 ; i < count ; i++)
			group->indices.array[group->indices.count++] = indices[i] + vertex_start_remap;
	}
	else
	{
		memcpy (group->indices.array + group->indices.count, indices, count * sizeof (LIMdlIndex));
		group->indices.count += count;
	}

	return 1;
}

/**
 * \brief Inserts a material to the model.
 * \param self Model builder.
 * \param material Material.
 * \return Nonzero on success.
 */
int limdl_builder_insert_material (
	LIMdlBuilder*        self,
	const LIMdlMaterial* material)
{
	int i;
	LIMdlBuilderLod* lod;
	LIMdlMaterial* tmp;

	/* Allocate space for materials. */
	if (!private_realloc_array (&self->model->materials.array,
	    &self->material_capacity, self->model->materials.count + 1, sizeof (LIMdlMaterial)))
		return 0;

	/* Allocate space for face groups. */
	for (i = 0 ; i < self->lod.count ; i++)
	{
		lod = self->lod.array + i;
		lisys_assert (self->model->materials.count == lod->face_groups.count);
		if (!private_realloc_array (&lod->face_groups.array,
		    &lod->face_groups.capacity, lod->face_groups.count + 1, sizeof (LIMdlBuilderFaces)))
			return 0;
		memset (lod->face_groups.array + lod->face_groups.count, 0, sizeof (LIMdlBuilderFaces));
		lod->face_groups.count++;
	}

	/* Copy material. */
	tmp = self->model->materials.array + self->model->materials.count;
	if (!limdl_material_init_copy (tmp, material))
		return 0;
	self->model->materials.count++;

	return 1;
}

/**
 * \brief Inserts a node to the model.
 * \param self Model builder.
 * \param node Node.
 * \return Nonzero on success.
 */
int limdl_builder_insert_node (
	LIMdlBuilder*    self,
	const LIMdlNode* node)
{
	LIMdlNode** tmp;

	tmp = realloc (self->model->nodes.array, (self->model->nodes.count + 1) * sizeof (LIMdlNode*));
	if (tmp == NULL)
		return 0;
	self->model->nodes.array = tmp;
	tmp += self->model->nodes.count;

	*tmp = limdl_node_copy (node);
	if (*tmp == NULL)
		return 0;
	self->model->nodes.count++;

	return 1;
}

/**
 * \brief Inserts vertices to the model.
 * \param self Model builder.
 * \param vertices Array of vertex.
 * \param count Number of vertices.
 * \param bone_mapping Bone index mapping array or NULL.
 * \return Nonzero on success.
 */
int limdl_builder_insert_vertices (
	LIMdlBuilder*      self,
	const LIMdlVertex* vertices,
	int                count,
	const int*         bone_mapping)
{
	int i;
	int j;
	int offset;
	LIMdlVertex* vertex;

	/* Allocate space for vertices. */
	if (!private_realloc_array (&self->model->vertices.array,
	    &self->vertex_capacity, self->model->vertices.count + count, sizeof (LIMdlVertex)))
		return 0;

	/* Append vertices. */
	offset = self->model->vertices.count;
	memcpy (self->model->vertices.array + offset, vertices, count * sizeof (LIMdlVertex));
	self->model->vertices.count += count;

	/* Map bone indices. */
	if (bone_mapping != NULL)
	{
		for (j = offset ; j < self->model->vertices.count ; j++)
		{
			vertex = self->model->vertices.array + j;
			for (i = 0 ; i < LIMDL_VERTEX_WEIGHTS_MAX ; i++)
			{
				if (vertex->bones[i])
					vertex->bones[i] = bone_mapping[vertex->bones[i] - 1] + 1;
			}
		}
	}

	return 1;
}

/**
 * \brief Inserts a weight group to the model.
 * \param self Model builder.
 * \param name Group name.
 * \param bone Bone name.
 * \return Nonzero on success.
 */
int limdl_builder_insert_weightgroup (
	LIMdlBuilder* self,
	const char*   name,
	const char*   bone)
{
	LIMdlWeightGroup* tmp;

	/* Allocate space for weight groups. */
	if (!private_realloc_array (&self->model->weight_groups.array,
	    &self->weightgroup_capacity, self->model->weight_groups.count + 1, sizeof (LIMdlWeightGroup)))
		return 0;

	/* Copy weight group. */
	tmp = self->model->weight_groups.array + self->model->weight_groups.count;
	tmp->name = lisys_string_dup (name);
	tmp->bone = lisys_string_dup (bone);
	if (tmp->name == NULL || tmp->bone == NULL)
	{
		lisys_free (tmp->name);
		lisys_free (tmp->bone);
		return 0;
	}
	self->model->weight_groups.count++;

	/* Map node. */
	tmp->node = limdl_model_find_node (self->model, tmp->bone);

	return 1;
}

/*****************************************************************************/

static int private_faces_init (
	LIMdlBuilderFaces* self,
	LIMdlFaces*        src,
	LIMdlLod*          srclod)
{
	memset (self, 0, sizeof (LIMdlBuilderFaces));

	if (src->count)
	{
		self->indices.array = lisys_calloc (src->count, sizeof (LIMdlIndex));
		if (self->indices.array == NULL)
			return 0;
		memcpy (self->indices.array, srclod->indices.array + src->start, src->count * sizeof (LIMdlIndex));
		self->indices.count = src->count;
		self->indices.capacity = src->count;
	}

	return 1;
}

static int private_faces_copy (
	LIMdlBuilderFaces* self,
	LIMdlBuilderFaces* src)
{
	memset (self, 0, sizeof (LIMdlBuilderFaces));

	if (src->indices.count)
	{
		self->indices.array = lisys_calloc (src->indices.count, sizeof (LIMdlIndex));
		if (self->indices.array == NULL)
			return 0;
		memcpy (self->indices.array, src->indices.array, src->indices.count * sizeof (LIMdlIndex));
		self->indices.count = src->indices.count;
		self->indices.capacity = src->indices.count;
	}

	return 1;
}

static void private_faces_clear (
	LIMdlBuilderFaces* self)
{
	lisys_free (self->indices.array);
}

static int private_lod_init (
	LIMdlBuilderLod* self,
	LIMdlLod*        src)
{
	int i;

	memset (self, 0, sizeof (LIMdlBuilderLod));

	if (src->face_groups.count)
	{
		self->face_groups.capacity = src->face_groups.count;
		self->face_groups.array = lisys_calloc (src->face_groups.count, sizeof (LIMdlBuilderFaces));
		if (self->face_groups.array == NULL)
		{
			private_lod_clear (self);
			return 0;
		}
		for (i = 0 ; i < src->face_groups.count ; i++)
		{
			if (!private_faces_init (self->face_groups.array + i, src->face_groups.array + i, src))
			{
				private_lod_clear (self);
				return 0;
			}
			self->face_groups.count++;
		}
	}

	return 1;
}

static int private_lod_copy (
	LIMdlBuilderLod* self,
	LIMdlBuilderLod* src)
{
	int i;

	memset (self, 0, sizeof (LIMdlBuilderLod));

	/* Allocate face groups. */
	if (!src->face_groups.count)
		return 1;
	self->face_groups.array = lisys_calloc (src->face_groups.count, sizeof (LIMdlBuilderFaces));
	if (self->face_groups.array == NULL)
		return 0;
	self->face_groups.capacity = src->face_groups.count;

	/* Copy face group data. */
	for (i = 0 ; i < src->face_groups.count ; i++)
	{
		if (!private_faces_copy (self->face_groups.array + i, src->face_groups.array + i))
		{
			private_lod_clear (self);
			return 0;
		}
		self->face_groups.count++;
	}

	lisys_assert (self->face_groups.count == src->face_groups.count);

	return 1;
}

static void private_lod_clear (
	LIMdlBuilderLod* self)
{
	int i;

	for (i = 0 ; i < self->face_groups.count ; i++)
		private_faces_clear (self->face_groups.array + i);
	lisys_free (self->face_groups.array);
}

static int private_realloc_array (
	void*  array,
	int*   capacity,
	int    required,
	size_t size)
{
	int i;
	void* tmp;

	if (*capacity < required)
	{
		i = (*capacity > 64)? *capacity : 64;
		while (i < required)
			i <<= 1;
		tmp = lisys_realloc (*((void**) array), i * size);
		if (tmp == NULL)
			return 0;
		*((void**) array) = tmp;
		*capacity = i;
	}

	return 1;
}

/** @} */
/** @} */
