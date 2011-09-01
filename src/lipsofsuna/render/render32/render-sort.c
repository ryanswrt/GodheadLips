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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenSort32 Sort32
 * @{
 */

#include "render-context.h"
#include "render-private.h"
#include "render-sort.h"

static int private_resize_faces (
	LIRenSort32* self,
	int        count);

/*****************************************************************************/

LIRenSort32* liren_sort32_new (
	LIRenRender32* render)
{
	LIRenSort32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenSort32));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Allocate depth sort buckets for transparent faces. */
	self->buckets.count = 4096;
	self->buckets.array = lisys_calloc (self->buckets.count, sizeof (LIRenSortface32*));
	if (self->buckets.array == NULL)
	{
		liren_sort32_free (self);
		return NULL;
	}

	/* Allocate buffer for transparent faces. */
	self->faces.capacity = 1024;
	self->faces.array = lisys_calloc (self->faces.capacity, sizeof (LIRenSortface32));
	if (self->faces.array == NULL)
	{
		liren_sort32_free (self);
		return NULL;
	}

	return self;
}

void liren_sort32_free (
	LIRenSort32* self)
{
	lisys_free (self->buckets.array);
	lisys_free (self->faces.array);
	lisys_free (self);
}

int liren_sort32_add_faces (
	LIRenSort32*     self,
	LIMatAabb*       bounds,
	LIMatMatrix*     matrix,
	int              index,
	int              count,
	LIRenMesh32*     mesh,
	LIRenMaterial32* material,
	LIMdlPose*       pose,
	LIMatVector*     center)
{
	int n;
	int bucket;
	float dist;
	LIMatMatrix mat;
	LIMatVector center_;
	LIMatVector diff;
	LIMatVector eye;

	/* Calculate camera position. */
	mat = self->render->context->matrix.modelviewinverse;
	eye = limat_matrix_transform (mat, limat_vector_init (0.0f, 0.0f, 0.0f));

	/* Resize the buffer if necessary. */
	if (!private_resize_faces (self, self->faces.count + 1))
		return 0;

	/* Calculate the center of the group. */
	center_ = limat_matrix_transform (*matrix, *center);

	/* Calculate bucket index based on distance to camera. */
	/* TODO: Would be better to use far plane distance here? */
	/* TODO: Non-linear mapping might work better for details closer to the camera? */
	diff = limat_vector_subtract (center_, eye);
	dist = limat_vector_get_length (diff);
	bucket = dist / 50.0f * (self->buckets.count - 1);
	bucket = LIMAT_CLAMP (bucket, 0, self->buckets.count - 1);

	/* Add the whole group to the depth bucket. */
	n = self->faces.count;
	self->faces.array[n].next = self->buckets.array[bucket];
	self->faces.array[n].type = LIREN_SORT_TYPE_GROUP;
	self->faces.array[n].group.bounds = *bounds;
	self->faces.array[n].group.matrix = *matrix;
	self->faces.array[n].group.index = index;
	self->faces.array[n].group.count = count;
	self->faces.array[n].group.pose = pose;
	self->faces.array[n].group.mesh = mesh;
	self->faces.array[n].group.material = material;
	self->buckets.array[bucket] = self->faces.array + n;
	self->faces.count++;

	return 1;
}

int liren_sort32_add_object (
	LIRenSort32*   self,
	LIRenObject32* object)
{
	int i;
	int ret = 1;
	LIMatAabb bounds;
	LIRenLod32* lod;
	LIRenMaterial32* material;

	/* Frustum culling. */
	liren_object32_get_bounds (object, &bounds);
	if (limat_frustum_cull_aabb (&self->frustum, &bounds))
		return 1;

	/* Add each face group. */
	for (i = 0 ; i < object->model->materials.count ; i++)
	{
		/* Don't add groups with no valid shader. */
		/* Don't sort groups that don't need sorting. */
		material = object->model->materials.array[i];
		if (material->shader == NULL || !material->shader->sort)
			continue;

		/* Select the level of detail. */
		lod = liren_model32_get_distance_lod (object->model,
			&object->transform.position, &self->render->context->camera_position);

		/* Append the group to the sorting buckets. */
		ret &= liren_sort32_add_faces (self, &bounds, &object->orientation.matrix,
			lod->groups.array[i].start, lod->groups.array[i].count,
			&lod->mesh, material, object->pose, &lod->groups.array[i].center);
	}

	/* Add particle systems of the object. */
	liren_particles32_sort (
		&object->model->particles,
		object->particle.time,
		object->particle.loop,
		&object->transform, self);

	return ret;
}

int liren_sort32_add_particle (
	LIRenSort32*       self,
	const LIMatVector* position,
	float              size,
	const float*       diffuse,
	LIRenImage32*      image,
	LIRenShader32*     shader)
{
	int num;
	int bucket;
	float dist;
	LIMatMatrix mat;
	LIMatVector diff;
	LIMatVector eye;

	/* Don't add particles with no valid shader. */
	if (shader == NULL)
		return 1;

	/* Resize the buffer if necessary. */
	if (!private_resize_faces (self, self->faces.count + 1))
		return 0;

	/* Calculate camera position. */
	mat = self->render->context->matrix.modelviewinverse;
	eye = limat_matrix_transform (mat, limat_vector_init (0.0f, 0.0f, 0.0f));

	/* Append the particle to the buffer. */
	num = self->faces.count;
	self->faces.array[num].type = LIREN_SORT_TYPE_PARTICLE;
	self->faces.array[num].particle.position = *position;
	self->faces.array[num].particle.size = size;
	self->faces.array[num].particle.image = image;
	self->faces.array[num].particle.shader = shader;
	memcpy (self->faces.array[num].particle.diffuse, diffuse, 4 * sizeof (float));

	/* Calculate bucket index based on distance to camera. */
	/* TODO: Would be better to use far plane distance here? */
	/* TODO: Non-linear mapping might work better for details closer to the camera? */
	diff = limat_vector_subtract (*position, eye);
	dist = limat_vector_get_length (diff);
	bucket = dist / 50.0f * (self->buckets.count - 1);
	bucket = LIMAT_CLAMP (bucket, 0, self->buckets.count - 1);

	/* Insert to the depth bucket. */
	self->faces.array[num].next = self->buckets.array[bucket];
	self->buckets.array[bucket] = self->faces.array + num;
	self->faces.count = ++num;

	return 1;
}

void liren_sort32_clear (
	LIRenSort32*       self,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection)
{
	self->faces.count = 0;
	memset (self->buckets.array, 0, self->buckets.count * sizeof (LIRenSortface32*));
	self->modelview = *modelview;
	self->projection = *projection;
	limat_frustum_init (&self->frustum, modelview, projection);
}

/*****************************************************************************/

static int private_resize_faces (
	LIRenSort32* self,
	int        count)
{
	int i;
	int num;
	LIRenSortface32* ptr;
	LIRenSortface32* tmp;

	if (self->faces.capacity > count)
		return 1;

	/* Reallocate the face array. */
	num = self->faces.capacity << 1;
	while (num < count)
		num <<= 1;
	tmp = lisys_realloc (self->faces.array, num * sizeof (LIRenSortface32));
	if (tmp == NULL)
		return 0;

	/* If the address of the face buffer changed, we need to adjust
	   the pointers in the buckets to match the new address. */
	if (tmp != self->faces.array)
	{
		for (i = 0 ; i < self->buckets.count ; i++)
		{
			if (self->buckets.array[i] != NULL)
			{
				self->buckets.array[i] = self->buckets.array[i] - self->faces.array + tmp;
				for (ptr = self->buckets.array[i] ; ptr != NULL ; ptr = ptr->next)
				{
					if (ptr->next)
						ptr->next = ptr->next - self->faces.array + tmp;
				}
			}
		}
	}

	/* Use the new face array. */
	self->faces.array = tmp;
	self->faces.capacity = num;

	return 1;
}

/** @} */
/** @} */
