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

#ifndef __RENDER21_MODEL_H__
#define __RENDER21_MODEL_H__

#include "lipsofsuna/image.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "render.h"
#include "render-types.h"

LIAPICALL (LIRenModel21*, liren_model21_new, (
	LIRenRender21* render,
	LIMdlModel*    model,
	int            id));

LIAPICALL (void, liren_model21_free, (
	LIRenModel21* self));

LIAPICALL (int, liren_model21_deform, (
	LIRenModel21*    self,
	const char*      shader,
	const LIMdlPose* pose));

LIAPICALL (void, liren_model21_reload, (
	LIRenModel21* self,
	int           pass));

LIAPICALL (void, liren_model21_replace_image, (
	LIRenModel21* self,
	LIRenImage21* image));

LIAPICALL (void, liren_model21_get_bounds, (
	LIRenModel21* self,
	LIMatAabb*    aabb));

LIAPICALL (int, liren_model21_set_model, (
	LIRenModel21* self,
	LIMdlModel*   model));

#endif
