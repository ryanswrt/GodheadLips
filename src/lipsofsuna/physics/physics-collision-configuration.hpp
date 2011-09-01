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

#ifndef __PHYSICS_COLLISION_CONFIGURATION_HPP__
#define __PHYSICS_COLLISION_CONFIGURATION_HPP__

#include "physics-private.h"
#include "physics-terrain-collision-algorithm.hpp"
#include "physics-terrain.hpp"
#include <btBulletCollisionCommon.h>

class LIPhyCollisionConfiguration : public btDefaultCollisionConfiguration
{
public:
	LIPhyCollisionConfiguration () : btDefaultCollisionConfiguration ()
	{
		void* mem = btAlignedAlloc (sizeof (LIPhyTerrainCollisionAlgorithm::CreateFunc), 16);
		this->terrain_create_func = new(mem) LIPhyTerrainCollisionAlgorithm::CreateFunc (this->m_simplexSolver, this->m_pdSolver);
	}
	virtual ~LIPhyCollisionConfiguration ()
	{
		this->terrain_create_func->~CreateFunc ();
		btAlignedFree (this->terrain_create_func);
	}
	btCollisionAlgorithmCreateFunc* getCollisionAlgorithmCreateFunc (int proxyType0, int proxyType1)
	{
		if ((proxyType0 == CONVEX_HULL_SHAPE_PROXYTYPE) && (proxyType1 == CUSTOM_CONVEX_SHAPE_TYPE))
			return this->terrain_create_func;
		if ((proxyType0 == CUSTOM_CONVEX_SHAPE_TYPE) && (proxyType1 == CONVEX_HULL_SHAPE_PROXYTYPE))
			return this->terrain_create_func;
		return btDefaultCollisionConfiguration::getCollisionAlgorithmCreateFunc (proxyType0, proxyType1);
	}
	LIPhyTerrainCollisionAlgorithm::CreateFunc* terrain_create_func;
};

#endif
