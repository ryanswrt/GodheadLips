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

#ifndef __PHYSICS_DYNAMICS_WORLD_HPP__
#define __PHYSICS_DYNAMICS_WORLD_HPP__

#ifndef assert
#define assert(a)
#endif
#include <btBulletDynamicsCommon.h>

class LIPhyDynamicsWorld : public btDiscreteDynamicsWorld
{
public:
	LIPhyDynamicsWorld (btDispatcher* dispatcher, btBroadphaseInterface* pairCache, btConstraintSolver* constraintSolver, btCollisionConfiguration* collisionConfiguration);
	virtual void addCollisionObject (btCollisionObject* collisionObject, short int collisionFilterGroup = btBroadphaseProxy::DefaultFilter, short int collisionFilterMask = btBroadphaseProxy::AllFilter);
	virtual void rayTest (const btVector3& rayFromWorld, const btVector3& rayToWorld, RayResultCallback& resultCallback) const;
	virtual void removeCollisionObject (btCollisionObject* collisionObject);
public:
	struct _LIPhyTerrain* terrain;
};

#endif
