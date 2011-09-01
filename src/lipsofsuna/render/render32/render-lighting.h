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

#ifndef __RENDER32_LIGHTING_H__
#define __RENDER32_LIGHTING_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/video.h>
#include "render.h"
#include "render-light.h"
#include "render-types.h"

struct _LIRenLighting32
{
	LIAlgPtrdic* lights;
	LIRenRender32* render;
};

LIAPICALL (LIRenLighting32*, liren_lighting32_new, (
	LIRenRender32* render));

LIAPICALL (void, liren_lighting32_free, (
	LIRenLighting32* self));

LIAPICALL (int, liren_lighting32_insert_light, (
	LIRenLighting32* self,
	LIRenLight32*    light));

LIAPICALL (void, liren_lighting32_remove_light, (
	LIRenLighting32* self,
	LIRenLight32*    light));

LIAPICALL (void, liren_lighting32_update, (
	LIRenLighting32* self));

LIAPICALL (void, liren_lighting32_upload, (
	LIRenLighting32* self,
	LIRenContext32*  context,
	LIMatVector*     center));

#endif
