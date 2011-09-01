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
 * \addtogroup LIVox Voxel
 * @{
 * \addtogroup LIVoxMaterial Material
 * @{
 */

#include "lipsofsuna/system.h"
#include "voxel-material.h"

/**
 * \brief Creates a new material.
 * \return New material or NULL.
 */
LIVoxMaterial* livox_material_new ()
{
	int flags;
	LIVoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxMaterial));
	if (self == NULL)
		return NULL;
	self->texture_scale = 1.0f;
	self->type = LIVOX_MATERIAL_TYPE_CUBE;

	/* Allocate name. */
	self->name = lisys_string_dup ("");
	if (self->name == NULL)
	{
		livox_material_free (self);
		return NULL;
	}

	/* Allocate materials. */
	flags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	if (!limdl_material_init (&self->material) ||
	    !limdl_material_append_texture (&self->material, LIMDL_TEXTURE_TYPE_IMAGE, flags, "stone-000"))
	{
		livox_material_free (self);
		return NULL;
	}
	self->material.flags |= LIMDL_MATERIAL_FLAG_CULLFACE;

	return self;
}

/**
 * \brief Creates a copy of a material.
 * \param src Material to copy.
 * \return Soft copy of the material or NULL.
 */
LIVoxMaterial* livox_material_new_copy (
	const LIVoxMaterial* src)
{
	LIVoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxMaterial));
	if (self == NULL)
		return NULL;

	/* Copy values. */
	self->id = src->id;
	self->flags = src->flags;
	self->friction = src->friction;
	self->texture_scale = src->texture_scale;

	/* Copy name. */
	self->name = lisys_string_dup (src->name);
	if (self->name == NULL)
	{
		livox_material_free (self);
		return NULL;
	}

	/* Copy materials. */
	if (!limdl_material_init_copy (&self->material, &src->material))
	{
		livox_material_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the material.
 * \param self Material.
 */
void livox_material_free (
	LIVoxMaterial* self)
{
	limdl_material_free (&self->material);
	lisys_free (self->name);
	lisys_free (self);
}

int livox_material_set_name (
	LIVoxMaterial* self,
	const char*    value)
{
	char* tmp;

	tmp = lisys_string_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->name);
	self->name = tmp;

	return 1;
}

/** @} */
/** @} */
