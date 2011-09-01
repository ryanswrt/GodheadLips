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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlMaterial Material
 * @{
 */

#include "lipsofsuna/system.h"
#include "model-material.h"

/**
 * \brief Initializes a default material.
 *
 * \param self Material.
 * \return Nonzero on success.
 */
int
limdl_material_init (LIMdlMaterial* self)
{
	self->shader = lisys_string_dup ("default");
	if (self->shader == NULL)
		return 0;
	self->flags = 0;
	self->emission = 0.0f;
	self->shininess = 1.0f;
	self->diffuse[0] = 1.0f;
	self->diffuse[1] = 1.0f;
	self->diffuse[2] = 1.0f;
	self->diffuse[3] = 1.0f;
	self->specular[0] = 1.0f;
	self->specular[1] = 1.0f;
	self->specular[2] = 1.0f;
	self->specular[3] = 1.0f;
	self->strand_start = 0.1f;
	self->strand_end = 0.0f;
	self->strand_shape = 0.5f;
	self->textures.count = 0;
	self->textures.array = NULL;

	return 1;
}

/**
 * \brief Initializes a copy of a material.
 *
 * \param self Copy destination.
 * \param src Copy source.
 * \return Nonzero on success.
 */
int
limdl_material_init_copy (LIMdlMaterial*       self,
                          const LIMdlMaterial* src)
{
	int i;

	*self = *src;
	if (src->shader != NULL)
	{
		self->shader = lisys_string_dup (src->shader);
		if (self->shader == NULL)
			return 0;
	}
	if (src->textures.count)
	{
		self->textures.array = lisys_calloc (src->textures.count, sizeof (LIMdlTexture));
		if (self->textures.array == NULL)
		{
			lisys_free (self->shader);
			return 0;
		}
		memcpy (self->textures.array, src->textures.array, src->textures.count * sizeof (LIMdlTexture));
		for (i = 0 ; i < src->textures.count ; i++)
		{
			self->textures.array[i].string = lisys_string_dup (src->textures.array[i].string);
			if (self->textures.array[i].string == NULL)
			{
				for (i-- ; i >= 0 ; i--)
					lisys_free (self->textures.array[i].string);
				lisys_free (self->textures.array);
				lisys_free (self->shader);
				return 0;
			}
		}
	}

	return 1;
}

/**
 * \brief Frees the material.
 *
 * \param self Material.
 */
void
limdl_material_free (LIMdlMaterial* self)
{
	limdl_material_clear_textures (self);
	lisys_free (self->shader);
}

int
limdl_material_append_texture (LIMdlMaterial* self,
                               int            type,
                               int            flags,
                               const char*    name)
{
	if (!limdl_material_realloc_textures (self, self->textures.count + 1))
		return 0;
	if (!limdl_material_set_texture (self, self->textures.count - 1, type, flags, name))
	{
		self->textures.count--;
		return 0;
	}

	return 1;
}

/**
 * \brief Removes all textures from the material.
 *
 * \param self Material.
 */
void
limdl_material_clear_textures (LIMdlMaterial* self)
{
	int i;

	for (i = 0 ; i < self->textures.count ; i++)
		lisys_free (self->textures.array[i].string);
	lisys_free (self->textures.array);
	self->textures.array = NULL;
	self->textures.count = 0;
}

/**
 * \brief Compares two materials.
 *
 * \param self Material.
 * \param material Material.
 * \return Nonzero if the materials are identical.
 */
int
limdl_material_compare (const LIMdlMaterial* self,
                        const LIMdlMaterial* material)
{
	int i;

	if (self->flags != material->flags ||
	    self->emission != material->emission ||
	    self->shininess != material->shininess ||
	    self->diffuse[0] != material->diffuse[0] ||
	    self->diffuse[1] != material->diffuse[1] ||
	    self->diffuse[2] != material->diffuse[2] ||
	    self->diffuse[3] != material->diffuse[3] ||
	    self->specular[0] != material->specular[0] ||
	    self->specular[1] != material->specular[1] ||
	    self->specular[2] != material->specular[2] ||
	    self->specular[3] != material->specular[3] ||
	    self->textures.count != material->textures.count)
		return 0;
	if (strcmp (self->shader, material->shader))
		return 0;
	for (i = 0 ; i < self->textures.count ; i++)
	{
		if (!limdl_texture_compare (self->textures.array + i, material->textures.array + i))
			return 0;
	}

	return 1;
}

int limdl_material_compare_shader_and_texture (
	const LIMdlMaterial* self,
	const char*          shader,
	const char*          texture)
{
	if (shader != NULL)
	{
		/* The name of the shader must match. */
		if (self->shader != NULL && strcmp (shader, self->shader))
			return 0;
	}
	if (texture != NULL)
	{
		/* The name of the first texture must match. */
		if (!self->textures.count || self->textures.array[0].string == NULL)
			return 0;
		if (strcmp (texture, self->textures.array[0].string))
			return 0;
	}

	return 1;
}

/**
 * \brief Deserializes the material from a stream.
 *
 * The contents of the material are replaced with data read from the stream.
 * If the read fails, the function returns without modifying the material.
 *
 * \param self Material.
 * \param reader Stream reader.
 * \return Nonzero on success.
 */
int
limdl_material_read (LIMdlMaterial* self,
                     LIArcReader*   reader)
{
	int i;
	uint32_t tmp[4];
	LIMdlMaterial tmpmat;

	/* Initialize temporaries. */
	memset (&tmpmat, 0, sizeof (LIMdlMaterial));

	/* Read header to temporary. */
	if (!liarc_reader_get_uint32 (reader, tmp + 0) ||
		!liarc_reader_get_float (reader, &tmpmat.emission) ||
		!liarc_reader_get_float (reader, &tmpmat.shininess) ||
		!liarc_reader_get_float (reader, tmpmat.diffuse + 0) ||
		!liarc_reader_get_float (reader, tmpmat.diffuse + 1) ||
		!liarc_reader_get_float (reader, tmpmat.diffuse + 2) ||
		!liarc_reader_get_float (reader, tmpmat.diffuse + 3) ||
		!liarc_reader_get_float (reader, tmpmat.specular + 0) ||
		!liarc_reader_get_float (reader, tmpmat.specular + 1) ||
		!liarc_reader_get_float (reader, tmpmat.specular + 2) ||
		!liarc_reader_get_float (reader, tmpmat.specular + 3) ||
		!liarc_reader_get_float (reader, &tmpmat.strand_start) ||
		!liarc_reader_get_float (reader, &tmpmat.strand_end) ||
		!liarc_reader_get_float (reader, &tmpmat.strand_shape) ||
		!liarc_reader_get_uint32 (reader, tmp + 1))
		return 0;
	tmpmat.flags = tmp[0];
	tmpmat.textures.count = tmp[1];

	/* Read shader to temporary. */
	if (!liarc_reader_get_text (reader, "", &tmpmat.shader))
		return 0;

	/* Read textures to temporary. */
	if (tmpmat.textures.count > 0)
	{
		tmpmat.textures.array = lisys_calloc (tmpmat.textures.count, sizeof (LIMdlTexture));
		if (tmpmat.textures.array == NULL)
		{
			tmpmat.textures.count = 0;
			limdl_material_free (&tmpmat);
			return 0;
		}
		for (i = 0 ; i < tmpmat.textures.count ; i++)
		{
			if (!liarc_reader_get_uint32 (reader, tmp + 0) ||
			    !liarc_reader_get_uint32 (reader, tmp + 1) ||
			    !liarc_reader_get_uint32 (reader, tmp + 2) ||
			    !liarc_reader_get_uint32 (reader, tmp + 3) ||
			    !liarc_reader_get_text (reader, "", &tmpmat.textures.array[i].string))
			{
				limdl_material_free (&tmpmat);
				return 0;
			}
			tmpmat.textures.array[i].type = tmp[0];
			tmpmat.textures.array[i].flags = tmp[1];
			tmpmat.textures.array[i].width = tmp[2];
			tmpmat.textures.array[i].height = tmp[3];
		}
	}

	/* Succeeded so free old and copy over. */
	limdl_material_free (self);
	*self = tmpmat;

	return 1;
}

/**
 * \brief Allocates or reallocates the textures of the material.
 *
 * \param self Material.
 * \param count Texture count.
 * \return Nonzero on success.
 */
int
limdl_material_realloc_textures (LIMdlMaterial* self,
                                 int            count)
{
	int i;
	LIMdlTexture* tmp;

	if (count == self->textures.count)
		return 1;
	if (!count)
	{
		limdl_material_clear_textures (self);
		return 1;
	}
	else if (count < self->textures.count)
	{
		for (i = count ; i < self->textures.count ; i++)
			lisys_free (self->textures.array[i].string);
		tmp = lisys_realloc (self->textures.array, count * sizeof (LIMdlTexture));
		if (tmp != NULL)
			self->textures.array = tmp;
		self->textures.count = count;
	}
	else
	{
		tmp = lisys_realloc (self->textures.array, count * sizeof (LIMdlTexture));
		if (tmp == NULL)
			return 0;
		self->textures.array = tmp;
		for (i = self->textures.count ; i < count ; i++)
		{
			self->textures.array[i].type = LIMDL_TEXTURE_TYPE_EMPTY;
			self->textures.array[i].flags = LIMDL_TEXTURE_FLAG_REPEAT;
			self->textures.array[i].width = 0;
			self->textures.array[i].height = 0;
			self->textures.array[i].string = NULL;
		}
		self->textures.count = count;
	}

	return 1;
}

/**
 * \brief Serializes the material to a stream.
 *
 * \param self Material.
 * \param writer Stream writer.
 * \return Nonzero on success.
 */
int
limdl_material_write (LIMdlMaterial* self,
                      LIArcWriter*   writer)
{
	int i;
	LIMdlTexture* texture;

	if (!liarc_writer_append_uint32 (writer, self->flags) ||
	    !liarc_writer_append_float (writer, self->emission) ||
	    !liarc_writer_append_float (writer, self->shininess) ||
	    !liarc_writer_append_float (writer, self->diffuse[0]) ||
	    !liarc_writer_append_float (writer, self->diffuse[1]) ||
	    !liarc_writer_append_float (writer, self->diffuse[2]) ||
	    !liarc_writer_append_float (writer, self->diffuse[3]) ||
	    !liarc_writer_append_float (writer, self->specular[0]) ||
	    !liarc_writer_append_float (writer, self->specular[1]) ||
	    !liarc_writer_append_float (writer, self->specular[2]) ||
	    !liarc_writer_append_float (writer, self->specular[3]) ||
	    !liarc_writer_append_float (writer, self->strand_start) ||
	    !liarc_writer_append_float (writer, self->strand_end) ||
	    !liarc_writer_append_float (writer, self->strand_shape) ||
	    !liarc_writer_append_uint32 (writer, self->textures.count) ||
	    !liarc_writer_append_string (writer, self->shader) ||
	    !liarc_writer_append_nul (writer))
		return 0;
	for (i = 0 ; i < self->textures.count ; i++)
	{
		texture = self->textures.array + i;
		if (!liarc_writer_append_uint32 (writer, texture->type) ||
		    !liarc_writer_append_uint32 (writer, texture->flags) ||
		    !liarc_writer_append_uint32 (writer, texture->width) ||
		    !liarc_writer_append_uint32 (writer, texture->height) ||
		    !liarc_writer_append_string (writer, texture->string) ||
		    !liarc_writer_append_nul (writer))
			return 0;
	}

	return 1;
}

/**
 * \brief Sets the diffuse color of the material.
 * \param self Material.
 * \param value Array of four floats.
 */
void limdl_material_set_diffuse (
	LIMdlMaterial* self,
	const float*   value)
{
	memcpy (self->diffuse, value, 4 * sizeof (float));
}

/**
 * \brief Sets the shader of the material.
 * \param self Material.
 * \param value Shader name.
 * \return Nonzero on success.
 */
int limdl_material_set_shader (
	LIMdlMaterial* self,
	const char*    value)
{
	char* tmp;

	tmp = lisys_string_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->shader);
	self->shader = tmp;

	return 1;
}

/**
 * \brief Sets the specular color of the material.
 * \param self Material.
 * \param value Array of four floats.
 */
void limdl_material_set_specular (
	LIMdlMaterial* self,
	const float*   value)
{
	memcpy (self->specular, value, 4 * sizeof (float));
}

/**
 * \brief Sets texture unit information.
 * \param self Material.
 * \param unit Texture unit.
 * \param type Texture type.
 * \param flags Texture parameters.
 * \param name Image name.
 * \return Nonzero on success.
 */
int
limdl_material_set_texture (LIMdlMaterial* self,
                            int            unit,
                            int            type,
                            int            flags,
                            const char*    name)
{
	char* dup;

	lisys_assert (unit >= 0);
	lisys_assert (unit < self->textures.count);

	dup = lisys_string_dup (name);
	if (dup == NULL)
		return 0;
	lisys_free (self->textures.array[unit].string);
	self->textures.array[unit].type = type;
	self->textures.array[unit].flags = flags;
	self->textures.array[unit].string = dup;

	return 1;
}

/** @} */
/** @} */
