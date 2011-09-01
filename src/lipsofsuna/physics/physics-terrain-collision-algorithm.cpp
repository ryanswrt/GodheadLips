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

#include "physics-private.h"
#include "physics-terrain.h"
#include "physics-terrain.hpp"
#include "physics-terrain-collision-algorithm.hpp"

float LIPhyTerrainCollisionAlgorithm::tile_size = -1.0f;
btConvexHullShape* LIPhyTerrainCollisionAlgorithm::slopes_above[16];
btConvexHullShape* LIPhyTerrainCollisionAlgorithm::slopes_below[16];

LIPhyTerrainCollisionAlgorithm::LIPhyTerrainCollisionAlgorithm (btPersistentManifold* mf, const btCollisionAlgorithmConstructionInfo& ci, btCollisionObject* body0, btCollisionObject* body1, btSimplexSolverInterface* simplexSolver, btConvexPenetrationDepthSolver* pdSolver, int numPerturbationIterations, int minimumPointsPerturbationThreshold) :
	btConvexConvexAlgorithm (mf, ci, body0, body1, simplexSolver, pdSolver, numPerturbationIterations, minimumPointsPerturbationThreshold)
{
}

void LIPhyTerrainCollisionAlgorithm::processCollision (btCollisionObject* body0, btCollisionObject* body1, const btDispatcherInfo& dispatchInfo, btManifoldResult* resultOut)
{
	int i;
	int x;
	int y;
	int z;
	int min[3];
	int max[3];
	int size[3];
	float liquid;
	float tile_size;
	btCollisionObject* object_convex;
	btCollisionObject* object_terrain;
	btConvexHullShape* shape_convex;
	btVector3 aabb_min;
	btVector3 aabb_max;
	btTransform transform;
	LIPhyObject* object_info;
	LIPhyPointer* pointer_convex;
	LIPhyPointer* pointer_terrain;
	LIPhyTerrainShape* shape_terrain;
	LIVoxMaterial* material;
	LIVoxVoxel* tile;
	LIVoxVoxel* tiles;

	/* Filter collisions. */
	btBroadphaseProxy* proxy0 = body0->getBroadphaseHandle ();
	btBroadphaseProxy* proxy1 = body1->getBroadphaseHandle ();
	if (!(proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) ||
	    !(proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask))
		return;

	/* Identify the bodies. */
	if (body0->getCollisionShape ()->getShapeType () == CUSTOM_CONVEX_SHAPE_TYPE)
	{
		object_terrain = body0;
		object_convex = body1;
	}
	else
	{
		object_terrain = body1;
		object_convex = body0;
	}
	lisys_assert (object_convex->getCollisionShape ()->getShapeType () == CONVEX_HULL_SHAPE_PROXYTYPE);
	lisys_assert (object_terrain->getCollisionShape ()->getShapeType () == CUSTOM_CONVEX_SHAPE_TYPE);
	shape_convex = (btConvexHullShape*) object_convex->getCollisionShape ();
	shape_terrain = (LIPhyTerrainShape*) object_terrain->getCollisionShape ();
	pointer_convex = (LIPhyPointer*) object_convex->getUserPointer ();
	pointer_terrain = (LIPhyPointer*) object_terrain->getUserPointer ();
	object_info = (LIPhyObject*) pointer_convex->pointer;

	/* Get the range of intersecting tiles. */
	tile_size = shape_terrain->terrain->voxels->tile_width;
	transform = object_convex->getWorldTransform ();
	shape_convex->getAabb (transform, aabb_min, aabb_max);
	aabb_min /= tile_size;
	aabb_max /= tile_size;
	for (i = 0 ; i < 3 ; i++)
	{
		min[i] = (int) aabb_min[i];
		max[i] = (int) aabb_max[i];
		size[i] = max[i] - min[i] + 1;
	}
	aabb_min = btVector3 (min[0] + 0.5f, min[1] + 0.5f, min[2] + 0.5f) * tile_size;

	/* Get the contents of the range of tiles. */
	tiles = (LIVoxVoxel*) lisys_calloc (size[0] * size[1] * size[2], sizeof (LIVoxVoxel));
	if (tiles == NULL)
		return;
	livox_manager_copy_voxels (shape_terrain->terrain->voxels,
		min[0], min[1], min[2], size[0], size[1], size[2], tiles);

	/* Ensure correct collision shape sizes. */
	if (tile_size != LIPhyTerrainCollisionAlgorithm::tile_size)
		rebuild_shape_cache (tile_size);

	/* Modify the terrain object. */
	btBoxShape box_shape (btVector3 (0.5f * tile_size, 0.5f * tile_size, 0.5f * tile_size));

	/* Process collisions with each tile. */
	liquid = 0.0f;
	transform.setIdentity ();
	for (z = 0 ; z < size[2] ; z++)
	for (y = 0 ; y < size[1] ; y++)
	for (x = 0 ; x < size[0] ; x++)
	{
		tile = tiles + x + (y + z * size[1]) * size[0];
		if (!tile->type)
			continue;
		material = livox_manager_find_material (shape_terrain->terrain->voxels, tile->type);
		if (material == NULL)
			continue;
		if (material->type == LIVOX_MATERIAL_TYPE_LIQUID)
		{
			liquid += 1.0f;
			continue;
		}
		transform.setOrigin (aabb_min + btVector3 (x, y, z) * tile_size);
		if (material->type == LIVOX_MATERIAL_TYPE_SLOPED ||
		    material->type == LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL)
		{
			if (tile->hint & LIVOX_HINT_SLOPE_FACEUP)
				object_terrain->setCollisionShape (slopes_above[tile->hint & LIVOX_HINT_SLOPE_CORNERALL]);
			else if (tile->hint & LIVOX_HINT_SLOPE_FACEDOWN)
				object_terrain->setCollisionShape (slopes_below[tile->hint & LIVOX_HINT_SLOPE_CORNERALL]);
			else
				object_terrain->setCollisionShape (&box_shape);
		}
		else
			object_terrain->setCollisionShape (&box_shape);
		object_terrain->setWorldTransform (transform);
		object_terrain->setInterpolationWorldTransform (transform);
		pointer_terrain->tile[0] = min[0] + x;
		pointer_terrain->tile[1] = min[1] + y;
		pointer_terrain->tile[2] = min[2] + z;
		btConvexConvexAlgorithm::processCollision (object_terrain, object_convex, dispatchInfo, resultOut);
	}

	/* Calculate liquid percentage. */
	/* The collision shape often marginally intersects with irrelevant tiles.
	   We compensate for that by amplifying the submerging percentage. */
	liquid /= size[0] * size[1] * size[2];
	liquid = LIMAT_MIN (1.0f, 1.3f * liquid);
	object_info->submerged = liquid;

	/* Apply liquid gravity. */
	btVector3 gravity_normal = btVector3 (object_info->config.gravity.x,
		object_info->config.gravity.y, object_info->config.gravity.z);
	btVector3 gravity_liquid = btVector3 (object_info->config.gravity_liquid.x,
		object_info->config.gravity_liquid.y, object_info->config.gravity_liquid.z);
	object_info->control->set_gravity (liquid * gravity_liquid + (1.0f - liquid) * gravity_normal);

	/* Apply liquid friction. */
	btVector3 velocity;
	object_info->control->get_velocity (&velocity);
	velocity *= 1.0f - (object_info->config.friction_liquid / 60);
	object_info->control->set_velocity (velocity);
	object_info->control->get_angular (&velocity);
	velocity *= 1.0f - (object_info->config.friction_liquid / 60);
	object_info->control->set_angular (velocity);

	/* Restore the terrain object. */
	transform.setIdentity ();
	object_terrain->setCollisionShape (shape_terrain);
	object_terrain->setWorldTransform (transform);
	object_terrain->setInterpolationWorldTransform (transform);
	lisys_free (tiles);
}

void LIPhyTerrainCollisionAlgorithm::rebuild_shape_cache (float size)
{
	int i;
	int j;
	btVector3 slope_verts[8];
	btVector3 cube_verts[8] =
	{
		btVector3(-0.5f * size, -0.5f * size, -0.5f * size),
		btVector3( 0.5f * size, -0.5f * size, -0.5f * size),
		btVector3(-0.5f * size, -0.5f * size,  0.5f * size),
		btVector3( 0.5f * size, -0.5f * size,  0.5f * size),
		btVector3(-0.5f * size,  0.5f * size, -0.5f * size),
		btVector3( 0.5f * size,  0.5f * size, -0.5f * size),
		btVector3(-0.5f * size,  0.5f * size,  0.5f * size),
		btVector3( 0.5f * size,  0.5f * size,  0.5f * size)
	};

	/* Free the old shapes. */
	/* FIXME: These are leaked once at exit regardless. */
	if (tile_size != -1.0f)
	{
		for (i = 0 ; i < 16 ; i++)
			delete slopes_above[i];
		for (i = 0 ; i < 16 ; i++)
			delete slopes_below[i];
	}

	/* Slopes above. */
	for (i = 0 ; i < 16 ; i++)
	{
		j = 0;
		slope_verts[j++] = cube_verts[0];
		slope_verts[j++] = cube_verts[1];
		slope_verts[j++] = cube_verts[2];
		slope_verts[j++] = cube_verts[3];
		if (!(i & LIVOX_HINT_SLOPE_CORNER00)) slope_verts[j++] = cube_verts[4];
		if (!(i & LIVOX_HINT_SLOPE_CORNER10)) slope_verts[j++] = cube_verts[5];
		if (!(i & LIVOX_HINT_SLOPE_CORNER01)) slope_verts[j++] = cube_verts[6];
		if (!(i & LIVOX_HINT_SLOPE_CORNER11)) slope_verts[j++] = cube_verts[7];
		slopes_above[i] = new btConvexHullShape ((btScalar*) slope_verts, j, sizeof (btVector3));
	}

	/* Slopes below. */
	for (i = 0 ; i < 16 ; i++)
	{
		j = 0;
		slope_verts[j++] = cube_verts[4];
		slope_verts[j++] = cube_verts[5];
		slope_verts[j++] = cube_verts[6];
		slope_verts[j++] = cube_verts[7];
		if (!(i & LIVOX_HINT_SLOPE_CORNER00)) slope_verts[j++] = cube_verts[0];
		if (!(i & LIVOX_HINT_SLOPE_CORNER10)) slope_verts[j++] = cube_verts[1];
		if (!(i & LIVOX_HINT_SLOPE_CORNER01)) slope_verts[j++] = cube_verts[2];
		if (!(i & LIVOX_HINT_SLOPE_CORNER11)) slope_verts[j++] = cube_verts[3];
		slopes_below[i] = new btConvexHullShape ((btScalar*) slope_verts, j, sizeof (btVector3));
	}

	tile_size = size;
}
