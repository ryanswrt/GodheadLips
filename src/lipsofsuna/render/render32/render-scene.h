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

#ifndef __RENDER32_SCENE_H__
#define __RENDER32_SCENE_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/system.h>
#include "render-framebuffer.h"
#include "render-light.h"
#include "render-object.h"
#include "render-types.h"

LIAPICALL (LIRenScene32*, liren_scene32_new, (
	void*          scene,
	LIRenRender32* render));

LIAPICALL (void, liren_scene32_free, (
	LIRenScene32* self));

LIAPICALL (void, liren_scene32_insert_light, (
	LIRenScene32* self,
	LIRenLight32* light));

LIAPICALL (void, liren_scene32_remove_light, (
	LIRenScene32* self,
	LIRenLight32* light));

LIAPICALL (void, liren_scene32_remove_model, (
	LIRenScene32* self,
	LIRenModel32* model));

LIAPICALL (void, liren_scene32_render, (
	LIRenScene32*       self,
	LIRenFramebuffer32* framebuffer,
	const GLint*        viewport,
	LIMatMatrix*        modelview,
	LIMatMatrix*        projection,
	LIMatFrustum*       frustum,
	LIRenPassRender*    render_passes,
	int                 render_passes_num,
	LIRenPassPostproc*  postproc_passes,
	int                 postproc_passes_num));

LIAPICALL (void, liren_scene32_update, (
	LIRenScene32* self,
	float         secs));

#endif
