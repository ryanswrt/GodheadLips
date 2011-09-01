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

#ifndef __RENDER32_MATERIAL_H__
#define __RENDER32_MATERIAL_H__

#include "lipsofsuna/image.h"
#include "lipsofsuna/model.h"
#include "render-image.h"
#include "render-shader.h"
#include "render-types.h"

LIAPICALL (LIRenMaterial32*, liren_material32_new, ());

LIAPICALL (LIRenMaterial32*, liren_material32_new_from_model, (
	LIRenRender32*       render,
	const LIMdlMaterial* material));

LIAPICALL (void, liren_material32_free, (
	LIRenMaterial32* self));

LIAPICALL (void, liren_material32_set_diffuse, (
	LIRenMaterial32* self,
	const float*     value));

LIAPICALL (void, liren_material32_set_flags, (
	LIRenMaterial32* self,
	int              flags));

LIAPICALL (void, liren_material32_set_params, (
	LIRenMaterial32* self,
	const float*     value));

LIAPICALL (int, liren_material32_set_shader, (
	LIRenMaterial32* self,
	LIRenShader32*   value));

LIAPICALL (void, liren_material32_set_specular, (
	LIRenMaterial32* self,
	const float*     value));

LIAPICALL (void, liren_material32_set_texture, (
	LIRenMaterial32* self,
	int              index,
	LIMdlTexture*    texture,
	LIRenImage32*    image));

LIAPICALL (int, liren_material32_set_texture_count, (
	LIRenMaterial32* self,
	int              value));

#endif
