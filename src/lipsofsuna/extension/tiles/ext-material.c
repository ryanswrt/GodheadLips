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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTiles Tiles
 * @{
 */

#include "ext-module.h"

#define SCRIPT_POINTER_MODEL ((void*) -1)

static void Material_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIVoxMaterial* material;

	/* Allocate material. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_MATERIAL);
	material = livox_material_new ();
	if (material == NULL)
		return;

	/* Insert to the voxel manager. */
	/* We currently rely on materials not being deletable. Even if they are
	   garbage collected, the actual pointers stored to the manager remain
	   until the game ends. */
	material->id = module->voxels->materials->size + 1;
	if (!livox_manager_insert_material (module->voxels, material))
	{
		livox_material_free (material);
		return;
	}

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, material, LIEXT_SCRIPT_MATERIAL, NULL);
	if (data == NULL)
	{
		livox_manager_remove_material (module->voxels, material->id);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Material_get_cullface (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_bool (args, (self->material.flags & LIMDL_MATERIAL_FLAG_CULLFACE) != 0);
}
static void Material_set_cullface (LIScrArgs* args)
{
	int value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		if (value)
			self->material.flags |= LIMDL_MATERIAL_FLAG_CULLFACE;
		else
			self->material.flags &= ~LIMDL_MATERIAL_FLAG_CULLFACE;
	}
}

static void Material_get_diffuse (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, self->material.diffuse[0]);
	liscr_args_seti_float (args, self->material.diffuse[1]);
	liscr_args_seti_float (args, self->material.diffuse[2]);
	liscr_args_seti_float (args, self->material.diffuse[3]);
}
static void Material_set_diffuse (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_geti_float (args, 0, self->material.diffuse + 0);
	liscr_args_geti_float (args, 1, self->material.diffuse + 1);
	liscr_args_geti_float (args, 2, self->material.diffuse + 2);
	liscr_args_geti_float (args, 3, self->material.diffuse + 3);
}

static void Material_get_flags (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_int (args, self->flags);
}
static void Material_set_flags (LIScrArgs* args)
{
	int value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_int (args, 0, &value))
		self->flags = value;
}

static void Material_get_friction (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_float (args, self->friction);
}
static void Material_set_friction (LIScrArgs* args)
{
	float value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->friction = value;
}

static void Material_get_id (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_int (args, self->id);
}

static void Material_get_name (LIScrArgs* args)
{
	liscr_args_seti_string (args, ((LIVoxMaterial*) args->self)->name);
}
static void Material_set_name (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		livox_material_set_name (args->self, value);
}

static void Material_get_shader (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_string (args, self->material.shader);
}
static void Material_set_shader (LIScrArgs* args)
{
	const char* value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_string (args, 0, &value))
		limdl_material_set_shader (&self->material, value);
}

static void Material_get_shininess (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_float (args, self->material.shininess);
}
static void Material_set_shininess (LIScrArgs* args)
{
	float value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->material.shininess = value;
}

static void Material_get_specular (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, self->material.specular[0]);
	liscr_args_seti_float (args, self->material.specular[1]);
	liscr_args_seti_float (args, self->material.specular[2]);
	liscr_args_seti_float (args, self->material.specular[3]);
}
static void Material_set_specular (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_geti_float (args, 0, self->material.specular + 0);
	liscr_args_geti_float (args, 1, self->material.specular + 1);
	liscr_args_geti_float (args, 2, self->material.specular + 2);
	liscr_args_geti_float (args, 3, self->material.specular + 3);
}

static void Material_get_texture (LIScrArgs* args)
{
	int i;
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	for (i = 0 ; i < self->material.textures.count ; i++)
		liscr_args_seti_string (args, self->material.textures.array[i].string);
}
static void Material_set_texture (LIScrArgs* args)
{
	int i;
	const int texflags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	const char* value;
	LIVoxMaterial* self = args->self;

	limdl_material_clear_textures (&self->material);
	for (i = 0 ; 1 ; i++)
	{
		if (!liscr_args_geti_string (args, i, &value))
			break;
		limdl_material_append_texture (&self->material,
			LIMDL_TEXTURE_TYPE_IMAGE, texflags, value);
	}
}

static void Material_get_texture_scale (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_float (args, self->texture_scale);
}
static void Material_set_texture_scale (LIScrArgs* args)
{
	float value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->texture_scale = value;
}

static void Material_get_type (LIScrArgs* args)
{
	LIVoxMaterial* self;

	self = args->self;
	switch (self->type)
	{
		case LIVOX_MATERIAL_TYPE_LIQUID:
			liscr_args_seti_string (args, "liquid");
			break;
		case LIVOX_MATERIAL_TYPE_ROUNDED:
			liscr_args_seti_string (args, "rounded");
			break;
		case LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL:
			liscr_args_seti_string (args, "rounded fractal");
			break;
		case LIVOX_MATERIAL_TYPE_SLOPED:
			liscr_args_seti_string (args, "sloped");
			break;
		case LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL:
			liscr_args_seti_string (args, "sloped fractal");
			break;
		default:
			liscr_args_seti_string (args, "cube");
			break;
	}
}
static void Material_set_type (LIScrArgs* args)
{
	const char* value;
	LIVoxMaterial* self;

	self = args->self;
	if (liscr_args_geti_string (args, 0, &value))
	{
		if (!strcmp (value, "cube"))
			self->type = LIVOX_MATERIAL_TYPE_CUBE;
		else if (!strcmp (value, "liquid"))
			self->type = LIVOX_MATERIAL_TYPE_LIQUID;
		else if (!strcmp (value, "rounded"))
			self->type = LIVOX_MATERIAL_TYPE_ROUNDED;
		else if (!strcmp (value, "rounded fractal"))
			self->type = LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL;
		else if (!strcmp (value, "sloped"))
			self->type = LIVOX_MATERIAL_TYPE_SLOPED;
		else if (!strcmp (value, "sloped fractal"))
			self->type = LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL;
	}
}

/*****************************************************************************/

void liext_script_material (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_MATERIAL, "material_new", Material_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_cullface", Material_get_cullface);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_cullface", Material_set_cullface);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_diffuse", Material_get_diffuse);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_diffuse", Material_set_diffuse);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_flags", Material_get_flags);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_flags", Material_set_flags);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_friction", Material_get_friction);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_friction", Material_set_friction);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_id", Material_get_id);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_name", Material_get_name);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_name", Material_set_name);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_shader", Material_get_shader);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_shader", Material_set_shader);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_shininess", Material_get_shininess);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_shininess", Material_set_shininess);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_specular", Material_get_specular);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_specular", Material_set_specular);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_texture", Material_get_texture);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_texture", Material_set_texture);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_texture_scale", Material_get_texture_scale);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_texture_scale", Material_set_texture_scale);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_get_type", Material_get_type);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MATERIAL, "material_set_type", Material_set_type);
}

/** @} */
/** @} */
