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

#ifndef __MODEL_MATERIAL_H__
#define __MODEL_MATERIAL_H__

#include <string.h>
#include <lipsofsuna/archive.h>
#include <lipsofsuna/system.h>
#include "model-texture.h"

enum
{
	LIMDL_MATERIAL_FLAG_BILLBOARD    = 0x01,
	LIMDL_MATERIAL_FLAG_COLLISION    = 0x02,
	LIMDL_MATERIAL_FLAG_CULLFACE     = 0x04,
	LIMDL_MATERIAL_FLAG_TRANSPARENCY = 0x08,
};

typedef struct _LIMdlMaterial LIMdlMaterial;
struct _LIMdlMaterial
{
	int flags;
	char* shader;
	float emission;
	float shininess;
	float diffuse[4];
	float specular[4];
	float strand_start;
	float strand_end;
	float strand_shape;
	struct
	{
		int count;
		LIMdlTexture* array;
	} textures;
};

LIAPICALL (int, limdl_material_init, (
	LIMdlMaterial* self));

LIAPICALL (int, limdl_material_init_copy, (
	LIMdlMaterial*       self,
	const LIMdlMaterial* src));

LIAPICALL (void, limdl_material_free, (
	LIMdlMaterial* self));

LIAPICALL (int, limdl_material_append_texture, (
	LIMdlMaterial* self,
	int            type,
	int            flags,
	const char*    name));

LIAPICALL (void, limdl_material_clear_textures, (
	LIMdlMaterial* self));

LIAPICALL (int, limdl_material_compare, (
	const LIMdlMaterial* self,
	const LIMdlMaterial* material));

LIAPICALL (int, limdl_material_compare_shader_and_texture, (
	const LIMdlMaterial* self,
	const char*          shader,
	const char*          texture));

LIAPICALL (int, limdl_material_read, (
	LIMdlMaterial* self,
	LIArcReader*   reader));

LIAPICALL (int, limdl_material_realloc_textures, (
	LIMdlMaterial* self,
	int            count));

LIAPICALL (int, limdl_material_write, (
	LIMdlMaterial* self,
	LIArcWriter*   writer));

LIAPICALL (void, limdl_material_set_diffuse, (
	LIMdlMaterial* self,
	const float*   value));

LIAPICALL (int, limdl_material_set_shader, (
	LIMdlMaterial* self,
	const char*    value));

LIAPICALL (void, limdl_material_set_specular, (
	LIMdlMaterial* self,
	const float*   value));

LIAPICALL (int, limdl_material_set_texture, (
	LIMdlMaterial* self,
	int            unit,
	int            type,
	int            flags,
	const char*    name));

#endif
