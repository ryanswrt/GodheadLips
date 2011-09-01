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

#ifndef __RENDER32_MODEL_H__
#define __RENDER32_MODEL_H__

#include "lipsofsuna/image.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "render.h"
#include "render-lod.h"
#include "render-material.h"
#include "render-types.h"

LIAPICALL (LIRenModel32*, liren_model32_new, (
	LIRenRender32* render,
	LIMdlModel*    model,
	int            id));

LIAPICALL (void, liren_model32_free, (
	LIRenModel32* self));

LIAPICALL (int, liren_model32_deform, (
	LIRenModel32*    self,
	const char*      shader,
	const LIMdlPose* pose));

LIAPICALL (void, liren_model32_reload, (
	LIRenModel32* self,
	int           pass));

LIAPICALL (void, liren_model32_replace_image, (
	LIRenModel32* self,
	LIRenImage32* image));

LIAPICALL (void, liren_model32_get_bounds, (
	LIRenModel32* self,
	LIMatAabb*    aabb));

LIAPICALL (LIRenLod32*, liren_model32_get_distance_lod, (
	LIRenModel32*      self,
	const LIMatVector* position,
	const LIMatVector* camera));

LIAPICALL (int, liren_model32_set_model, (
	LIRenModel32* self,
	LIMdlModel*   model));

#endif
