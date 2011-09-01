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

#ifndef __PHYSICS_CONSTRAINT_H__
#define __PHYSICS_CONSTRAINT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "physics.h"
#include "physics-types.h"

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LIPhyConstraint*, liphy_constraint_new_hinge, (
	LIPhyPhysics*      physics,
	LIPhyObject*       object,
	const LIMatVector* point,
	const LIMatVector* axis,
	int                limit,
	float              limit_min,
	float              limit_max));

LIAPICALL (void, liphy_constraint_free, (
	LIPhyConstraint* self));

#ifdef __cplusplus
}
#endif

#endif
