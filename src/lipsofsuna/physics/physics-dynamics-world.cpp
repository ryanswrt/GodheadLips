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

#include "physics-dynamics-world.hpp"
#include "physics-private.h"
#include "physics-terrain.h"

LIPhyDynamicsWorld::LIPhyDynamicsWorld (btDispatcher* dispatcher, btBroadphaseInterface* pairCache, btConstraintSolver* constraintSolver, btCollisionConfiguration* collisionConfiguration) :
	btDiscreteDynamicsWorld (dispatcher, pairCache, constraintSolver, collisionConfiguration)
{
	this->setGravity (btVector3 (0.0f, -10.0f, 0.0f));
}

void LIPhyDynamicsWorld::addCollisionObject (btCollisionObject* collisionObject, short int collisionFilterGroup, short int collisionFilterMask)
{
	LIPhyPointer* pointer = (LIPhyPointer*) collisionObject->getUserPointer ();
	if (pointer != NULL && !pointer->object)
		this->terrain = (LIPhyTerrain*) pointer->pointer;
	btDiscreteDynamicsWorld::addCollisionObject (collisionObject, collisionFilterGroup, collisionFilterMask);
}

void LIPhyDynamicsWorld::rayTest (const btVector3& rayFromWorld, const btVector3& rayToWorld, RayResultCallback& resultCallback) const
{
	LIMatVector start;
	LIMatVector end;
	LIPhyCollision result;

	/* Test against stock objects. */
	btDiscreteDynamicsWorld::rayTest (rayFromWorld, rayToWorld, resultCallback);

	/* Test against terrain. */
	if (this->terrain != NULL)
	{
		start = limat_vector_init (rayFromWorld[0], rayFromWorld[1], rayFromWorld[2]);
		end = limat_vector_init (rayToWorld[0], rayToWorld[1], rayToWorld[2]);
		if (liphy_terrain_cast_ray (this->terrain, &start, &end, &result))
		{
			if (result.fraction < resultCallback.m_closestHitFraction)
			{
				btVector3 normal (result.normal.x, result.normal.y, result.normal.z);
				LIPhyTerrain* terrain = (LIPhyTerrain*) this->terrain;
				LIPhyPointer* pointer = (LIPhyPointer*) terrain->object->getUserPointer ();
				pointer->tile[0] = result.terrain_tile[0];
				pointer->tile[1] = result.terrain_tile[1];
				pointer->tile[2] = result.terrain_tile[2];
				LocalRayResult rayres (terrain->object, NULL, normal, result.fraction);
				resultCallback.addSingleResult (rayres, true);
			}
		}
	}
}

void LIPhyDynamicsWorld::removeCollisionObject (btCollisionObject* collisionObject)
{
	LIPhyPointer* pointer = (LIPhyPointer*) collisionObject->getUserPointer ();
	if (pointer != NULL && !pointer->object)
		this->terrain = NULL;
	btDiscreteDynamicsWorld::removeCollisionObject (collisionObject);
}
