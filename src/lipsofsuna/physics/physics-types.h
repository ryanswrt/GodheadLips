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

#ifndef __PHYSICS_TYPES_H__
#define __PHYSICS_TYPES_H__

#include <lipsofsuna/math.h>

#define LIPHY_DEFAULT_SPEED 3.0f
#define LIPHY_DEFAULT_COLLISION_GROUP 0x0001
#define LIPHY_DEFAULT_COLLISION_MASK 0xFFFF
#define LIPHY_GROUP_STATICS 0x4000
#define LIPHY_GROUP_TILES 0x8000

enum _LIPhyControlMode
{
	LIPHY_CONTROL_MODE_NONE,
	LIPHY_CONTROL_MODE_CHARACTER,
	LIPHY_CONTROL_MODE_RIGID,
	LIPHY_CONTROL_MODE_STATIC,
	LIPHY_CONTROL_MODE_VEHICLE,
	LIPHY_CONTROL_MODE_MAX
};

typedef enum _LIPhyControlMode LIPhyControlMode;
typedef struct _LIPhyCollision LIPhyCollision;
typedef struct _LIPhyConstraint LIPhyConstraint;
typedef struct _LIPhyContact LIPhyContact;
typedef struct _LIPhyModel LIPhyModel;
typedef struct _LIPhyObject LIPhyObject;
typedef struct _LIPhyPhysics LIPhyPhysics;
typedef struct _LIPhySector LIPhySector;
typedef struct _LIPhyShape LIPhyShape;
typedef struct _LIPhyTerrain LIPhyTerrain;
typedef void (*LIPhyCallback)(LIPhyObject* self, float secs);
typedef void (*LIPhyContactCall)(LIPhyObject* self, LIPhyContact* contact);
typedef void (*LIPhyTransformCall)(LIPhyObject* object);

struct _LIPhyContact
{
	int terrain_tile[3];
	float impulse;
	LIMatVector point;
	LIMatVector normal;
	LIPhyObject* object0;
	LIPhyObject* object1;
	LIPhyTerrain* terrain;
};

struct _LIPhyCollision
{
	int terrain_tile[3];
	float fraction;
	LIMatVector normal;
	LIMatVector point;
	LIPhyObject* object;
	LIPhyTerrain* terrain;
};

#endif
