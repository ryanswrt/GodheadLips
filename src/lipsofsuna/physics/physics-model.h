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

#ifndef __PHYSICS_MODEL_H__
#define __PHYSICS_MODEL_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "physics-types.h"

LIAPICALL (LIPhyModel*, liphy_model_new, (
	LIPhyPhysics* physics,
	LIMdlModel*   model,
	int           id));

LIAPICALL (void, liphy_model_free, (
	LIPhyModel* self));

LIAPICALL (int, liphy_model_build, (
	LIPhyModel* self,
	int         terrain));

LIAPICALL (LIPhyShape*, liphy_model_find_shape, (
	LIPhyModel* self,
	const char* name));

LIAPICALL (int, liphy_model_set_model, (
	LIPhyModel* self,
	LIMdlModel* model));

#endif
