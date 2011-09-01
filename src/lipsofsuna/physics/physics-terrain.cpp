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
 * \addtogroup LIPhy Physics
 * @{
 * \addtogroup LIPhyTerrain Terrain
 * @{
 */

#include <lipsofsuna/system.h>
#include "physics-model.h"
#include "physics-terrain.h"
#include "physics-private.h"

/**
 * \brief Creates a terrain physics manager.
 * \param physics Physics engine.
 * \param voxels Voxel manager.
 * \param collision_group Collision group.
 * \param collision_mask Collision mask.
 * \return New terrain physics manager.
 */
LIPhyTerrain* liphy_terrain_new (
	LIPhyPhysics* physics,
	LIVoxManager* voxels,
	int           collision_group,
	int           collision_mask)
{
	btQuaternion r(0.0f, 0.0f, 0.0f, 1.0f);
	btVector3 p(0.0f, 0.0f, 0.0f);
	LIPhyTerrain* self;

	/* Allocate self. */
	self = (LIPhyTerrain*) lisys_calloc (1, sizeof (LIPhyTerrain));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->voxels = voxels;
	self->collision_group = collision_group;
	self->collision_mask = collision_mask;

	/* Prepare collision object to tile index lookup. */
	self->pointer = (LIPhyPointer*) lisys_calloc (1, sizeof (LIPhyPointer));
	if (self->pointer == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->pointer->object = 0;
	self->pointer->pointer = self;

	/* Create the collision shape. */
	self->shape = new LIPhyTerrainShape (self);

	/* Create the collision object. */
	self->object = new btCollisionObject ();
	self->object->setCollisionFlags (btCollisionObject::CF_STATIC_OBJECT);
	self->object->setActivationState (DISABLE_DEACTIVATION);
	self->object->setCollisionShape (self->shape);
	self->object->setWorldTransform (btTransform (r, p));
	self->object->setUserPointer (self->pointer);

	return self;
}

/**
 * \brief Frees the terrain block.
 * \param self Terrain.
 */
void liphy_terrain_free (
	LIPhyTerrain* self)
{
	liphy_terrain_set_realized (self, 0);
	delete self->object;
	delete self->shape;
	lisys_free (self->pointer);
	lisys_free (self);
}

int liphy_terrain_cast_ray (
	const LIPhyTerrain* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	LIPhyCollision*     result)
{
	LIMatVector p0;
	LIMatVector p1;
	LIMatVector point;
	LIMatVector tile;
	LIMatVector center;

	p0 = limat_vector_multiply (*start, 1.0f / self->voxels->tile_width);
	p1 = limat_vector_multiply (*end, 1.0f / self->voxels->tile_width);
	if (!livox_manager_intersect_ray (self->voxels, &p0, &p1, &point, &tile))
		return 0;
	result->fraction =
		limat_vector_get_length (limat_vector_subtract (point, p0)) /
		(limat_vector_get_length (limat_vector_subtract (p1, p0)) + 0.00001f);
	center = limat_vector_add (tile, limat_vector_init (0.5, 0.5f, 0.5f));
	result->normal = limat_vector_subtract (point, center);
	result->normal = limat_vector_normalize (result->normal);
	result->point = limat_vector_multiply (point, self->voxels->tile_width);
	result->object = NULL;
	result->terrain = (LIPhyTerrain*) self;
	result->terrain_tile[0] = (int) tile.x;
	result->terrain_tile[1] = (int) tile.y;
	result->terrain_tile[2] = (int) tile.z;

	return 1;
}

int liphy_terrain_cast_shape (
	const LIPhyTerrain* self,
	const LIMatTransform* start,
	const LIMatTransform* end,
	const LIPhyShape*     shape,
	LIPhyCollision*       result)
{
	/* TODO */
	return 0;
}

int liphy_terrain_cast_sphere (
	const LIPhyTerrain* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	float               radius,
	LIPhyCollision*     result)
{
	/* TODO */
	return 0;
}

/**
 * \brief Realizes or unrealizes the terrain.
 * \param self Terrain.
 * \param value Nonzero to realize, zero to unrealize.
 */
void liphy_terrain_set_realized (
	LIPhyTerrain* self,
	int           value)
{
	if (self->realized == value)
		return;
	self->realized = value;
	if (value)
		self->physics->dynamics->addCollisionObject (self->object, self->collision_group, self->collision_mask);
	else
		self->physics->dynamics->removeCollisionObject (self->object);
}

/** @} */
/** @} */
