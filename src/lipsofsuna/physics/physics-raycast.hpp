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

#ifndef __PHYSICS_RAYCAST_HPP__
#define __PHYSICS_RAYCAST_HPP__

class LIPhyRaycastWorld : public btCollisionWorld::ClosestRayResultCallback
{
public:
	LIPhyRaycastWorld (LIPhyObject** ignore_array, int ignore_count, const btVector3& src, const btVector3& dst) :
		btCollisionWorld::ClosestRayResultCallback (src, dst)
	{
		this->ignore_count = ignore_count;
		this->ignore_array = ignore_array;
		this->object = NULL;
		this->terrain = NULL;
		this->terrain_tile[0] = 0;
		this->terrain_tile[1] = 0;
		this->terrain_tile[2] = 0;
	}
	virtual btScalar addSingleResult (btCollisionWorld::LocalRayResult& result, bool world)
	{
		int i;
		LIPhyObject* object;
		LIPhyPointer* pointer = (LIPhyPointer*) result.m_collisionObject->getUserPointer ();
		if (pointer->object)
		{
			object = (LIPhyObject*) pointer->pointer;
			for (i = 0 ; i < this->ignore_count ; i++)
			{
				if (object == this->ignore_array[i])
					return 1.0;
			}
			this->object = object;
			this->terrain = NULL;
			this->terrain_tile[0] = 0;
			this->terrain_tile[1] = 0;
			this->terrain_tile[2] = 0;
		}
		else
		{
			this->object = NULL;
			this->terrain = (LIPhyTerrain*) pointer->pointer;
			this->terrain_tile[0] = pointer->tile[0];
			this->terrain_tile[1] = pointer->tile[1];
			this->terrain_tile[2] = pointer->tile[2];
		}
		return btCollisionWorld::ClosestRayResultCallback::addSingleResult (result, world);
	}
public:
	int ignore_count;
	LIPhyObject** ignore_array;
	LIPhyObject* object;
	LIPhyTerrain* terrain;
	int terrain_tile[3];
};

#endif
