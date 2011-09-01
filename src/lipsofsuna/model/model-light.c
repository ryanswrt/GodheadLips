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
 * \addtogroup LIMdlLight Light
 * @{
 */

#include "model-light.h"
#include "model-node.h"

/**
 * \brief Reads the light node from a stream.
 * \param self Light.
 * \param reader Stream reader.
 * \return Nonzero on success.
 */
int limdl_light_read (
	LIMdlNode*   self,
	LIArcReader* reader)
{
	uint32_t tmp;
	LIMdlLight* light = &self->light;

	/* Read flags. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->light.flags = tmp;

	/* Read geometry and color. */
	if (tmp & 0x80)
	{
		/* New version with ambient and specular. */
		if (!liarc_reader_get_float (reader, &light->projection.fov) ||
			!liarc_reader_get_float (reader, &light->projection.nearz) ||
			!liarc_reader_get_float (reader, &light->projection.farz) ||
			!liarc_reader_get_float (reader, light->ambient + 0) ||
			!liarc_reader_get_float (reader, light->ambient + 1) ||
			!liarc_reader_get_float (reader, light->ambient + 2) ||
			!liarc_reader_get_float (reader, light->ambient + 3) ||
			!liarc_reader_get_float (reader, light->diffuse + 0) ||
			!liarc_reader_get_float (reader, light->diffuse + 1) ||
			!liarc_reader_get_float (reader, light->diffuse + 2) ||
			!liarc_reader_get_float (reader, light->diffuse + 3) ||
			!liarc_reader_get_float (reader, light->specular + 0) ||
			!liarc_reader_get_float (reader, light->specular + 1) ||
			!liarc_reader_get_float (reader, light->specular + 2) ||
			!liarc_reader_get_float (reader, light->specular + 3) ||
			!liarc_reader_get_float (reader, light->equation + 0) ||
			!liarc_reader_get_float (reader, light->equation + 1) ||
			!liarc_reader_get_float (reader, light->equation + 2) ||
			!liarc_reader_get_float (reader, &light->spot.cutoff) ||
			!liarc_reader_get_float (reader, &light->spot.exponent))
			return 0;
	}
	else
	{
		/* Old version without ambient. */
		if (!liarc_reader_get_float (reader, &light->projection.fov) ||
			!liarc_reader_get_float (reader, &light->projection.nearz) ||
			!liarc_reader_get_float (reader, &light->projection.farz) ||
			!liarc_reader_get_float (reader, light->diffuse + 0) ||
			!liarc_reader_get_float (reader, light->diffuse + 1) ||
			!liarc_reader_get_float (reader, light->diffuse + 2) ||
			!liarc_reader_get_float (reader, light->equation + 0) ||
			!liarc_reader_get_float (reader, light->equation + 1) ||
			!liarc_reader_get_float (reader, light->equation + 2) ||
			!liarc_reader_get_float (reader, &light->spot.cutoff) ||
			!liarc_reader_get_float (reader, &light->spot.exponent))
			return 0;
		light->ambient[0] = 0.0f;
		light->ambient[1] = 0.0f;
		light->ambient[2] = 0.0f;
		light->ambient[3] = 0.0f;
		light->diffuse[3] = 1.0f;
	}

	return 1;
}

/**
 * \brief Writes the light node to a stream.
 * \param self Light.
 * \param writer Stream writer.
 * \return Nonzero on success.
 */
int limdl_light_write (
	const LIMdlNode* self,
	LIArcWriter*     writer)
{
	const LIMdlLight* light = &self->light;

	liarc_writer_append_uint32 (writer, light->flags | 0x80);
	liarc_writer_append_float (writer, light->projection.fov);
	liarc_writer_append_float (writer, light->projection.nearz);
	liarc_writer_append_float (writer, light->projection.farz);
	liarc_writer_append_float (writer, light->ambient[0]);
	liarc_writer_append_float (writer, light->ambient[1]);
	liarc_writer_append_float (writer, light->ambient[2]);
	liarc_writer_append_float (writer, light->ambient[3]);
	liarc_writer_append_float (writer, light->diffuse[0]);
	liarc_writer_append_float (writer, light->diffuse[1]);
	liarc_writer_append_float (writer, light->diffuse[2]);
	liarc_writer_append_float (writer, light->diffuse[3]);
	liarc_writer_append_float (writer, light->specular[0]);
	liarc_writer_append_float (writer, light->specular[1]);
	liarc_writer_append_float (writer, light->specular[2]);
	liarc_writer_append_float (writer, light->specular[3]);
	liarc_writer_append_float (writer, light->equation[0]);
	liarc_writer_append_float (writer, light->equation[1]);
	liarc_writer_append_float (writer, light->equation[2]);
	liarc_writer_append_float (writer, light->spot.cutoff);
	liarc_writer_append_float (writer, light->spot.exponent);

	return !writer->error;
}

/**
 * \brief Gets the modelview matrix of the light node.
 * \param self Light.
 * \param value Return location for the matrix.
 */
void limdl_light_get_modelview (
	const LIMdlNode* self,
	LIMatMatrix*     value)
{
	*value = limat_convert_transform_to_matrix (self->transform.global);
}

/**
 * \brief Gets the projection matrix of the light node.
 * \param self Light.
 * \param value Return location for the matrix.
 */
void limdl_light_get_projection (
	const LIMdlNode* self,
	LIMatMatrix*     value)
{
	*value = limat_matrix_perspective (
		self->light.projection.fov, 1.0f,
		self->light.projection.nearz,
		self->light.projection.farz);
}

/** @} */
/** @} */
