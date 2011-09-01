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

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenMaterial32 Material32
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.h"
#include "render-material.h"
#include "render-private.h"

/**
 * \brief Creates a new material.
 * \return Material or NULL.
 */
LIRenMaterial32* liren_material32_new ()
{
	LIRenMaterial32* self;

	self = lisys_calloc (1, sizeof (LIRenMaterial32));
	if (self == NULL)
		return NULL;
	self->shininess = 64;

	return self;
}

/**
 * \brief Creates a new material from a model material.
 * \param render Renderer.
 * \param material Model material.
 * \return Material or NULL.
 */
LIRenMaterial32* liren_material32_new_from_model (
	LIRenRender32*       render,
	const LIMdlMaterial* material)
{
	int j;
	LIMdlTexture* texture;
	LIRenImage32* image;
	LIRenMaterial32* self;

	self = lisys_calloc (1, sizeof (LIRenMaterial32));
	if (self == NULL)
		return NULL;
	if (material->flags & LIMDL_MATERIAL_FLAG_BILLBOARD)
		self->flags |= LIREN_MATERIAL_FLAG_BILLBOARD;
	if (material->flags & LIMDL_MATERIAL_FLAG_CULLFACE)
		self->flags |= LIREN_MATERIAL_FLAG_CULLFACE;
	if (material->flags & LIMDL_MATERIAL_FLAG_TRANSPARENCY)
		self->flags |= LIREN_MATERIAL_FLAG_TRANSPARENCY;
	self->parameters[0] = material->emission;
	self->shininess = LIMAT_CLAMP (material->shininess, 1.0f, 127.0f);
	self->diffuse[0] = material->diffuse[0];
	self->diffuse[1] = material->diffuse[1];
	self->diffuse[2] = material->diffuse[2];
	self->diffuse[3] = material->diffuse[3];
	self->specular[0] = material->specular[0];
	self->specular[1] = material->specular[1];
	self->specular[2] = material->specular[2];
	self->specular[3] = material->specular[3];
	self->strand_start = material->strand_start;
	self->strand_end = material->strand_end;
	self->strand_shape = material->strand_shape;

	/* Find the shader. */
	self->shader = liren_render32_find_shader (render, material->shader);

	/* Set textures. */
	if (!liren_material32_set_texture_count (self, material->textures.count))
	{
		liren_material32_free (self);
		return 0;
	}
	for (j = 0 ; j < material->textures.count ; j++)
	{
		texture = material->textures.array + j;

		/* Find or load. */
		if (texture->type == LIMDL_TEXTURE_TYPE_IMAGE)
		{
			image = liren_render32_find_image (render, texture->string);
			if (image == NULL)
				image = liren_render32_load_image (render, texture->string);
		}
		else
			image = NULL;

		/* Set texture. */
		liren_material32_set_texture (self, j, texture, image);
	}

	return self;
}

/**
 * \brief Frees the material.
 * \param self Material.
 */
void liren_material32_free (
	LIRenMaterial32* self)
{
	lisys_free (self->textures.array);
	lisys_free (self);
}

/**
 * \brief Sets the diffuse color of the material.
 * \param self Material.
 * \param value Color in RGBA.
 */
void liren_material32_set_diffuse (
	LIRenMaterial32* self,
	const float*   value)
{
	memcpy (self->diffuse, value, 4 * sizeof (float));
}

/**
 * \brief Sets the rendering flags of the material.
 * \param self Material.
 * \param flags Flags.
 */
void liren_material32_set_flags (
	LIRenMaterial32* self,
	int            flags)
{
	self->flags = flags;
}

/**
 * \brief Sets the custom shader parameters of the material.
 * \param self Material.
 * \param value Array of 4 floats.
 */
void liren_material32_set_params (
	LIRenMaterial32* self,
	const float*     value)
{
	memcpy (self->parameters, value, 4 * sizeof (float));
}

/**
 * \brief Assigns a shader to the material.
 *
 * A weak reference to the shader is stored to the material. Thus,
 * the shader must exist for the lifetime of the material or invalid
 * memory access will occur.
 *
 * The shader must be assigned before any textures are assigned and
 * cannot be changed after the first call.
 *
 * \param self Material.
 * \param value Shader.
 */
int liren_material32_set_shader (
	LIRenMaterial32* self,
	LIRenShader32*   value)
{
	self->shader = value;

	return 1;
}

/**
 * \brief Sets the specular color of the material.
 * \param self Material.
 * \param value Color in RGBA.
 */
void liren_material32_set_specular (
	LIRenMaterial32* self,
	const float*   value)
{
	memcpy (self->specular, value, 4 * sizeof (float));
}

/**
 * \brief Assigns a texture to the material.
 *
 * \param self Material.
 * \param index Texture index.
 * \param texture Texture data.
 * \param image Image.
 */
void liren_material32_set_texture (
	LIRenMaterial32* self,
	int            index,
	LIMdlTexture*  texture,
	LIRenImage32*  image)
{
	if (index < 0 || index >= self->textures.count)
		return;
	self->textures.array[index] = image;
}

int liren_material32_set_texture_count (
	LIRenMaterial32* self,
	int            value)
{
	int i;
	LIRenImage32** tmp;

	if (value)
	{
		if (value > self->textures.count)
		{
			tmp = lisys_realloc (self->textures.array, value * sizeof (LIRenImage32*));
			if (tmp == NULL)
				return 0;
			for (i = self->textures.count ; i < value ; i++)
				tmp[i] = NULL;
			self->textures.array = tmp;
			self->textures.count = value;
		}
		else
		{
			tmp = lisys_realloc (self->textures.array, value * sizeof (LIRenImage32*));
			if (tmp != NULL)
				self->textures.array = tmp;
			self->textures.count = value;
		}
	}
	else
	{
		lisys_free (self->textures.array);
		self->textures.array = NULL;
		self->textures.count = 0;
	}

	return 1;
}

/** @} */
/** @} */
