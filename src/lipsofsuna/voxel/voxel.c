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
 * \addtogroup LIVoxVoxel Voxel
 * @{
 */

#include "voxel.h"

/**
 * \brief Initializes a voxel of the given type.
 * \param self Voxel.
 * \param type Terrain type.
 */
void livox_voxel_init (
	LIVoxVoxel* self,
	int         type)
{
	self->type = type;
}

/**
 * \brief Reads the voxel from a stream.
 * \param self Voxel.
 * \param reader Stream reader.
 * \return Nonzero on success.
 */
int livox_voxel_read (
	LIVoxVoxel*  self,
	LIArcReader* reader)
{
	uint8_t terrain;

	if (!liarc_reader_get_uint8 (reader, &terrain))
		return 0;
	self->type = terrain;

	return 1;
}

/**
 * \brief Writes the voxel to a stream.
 * \param self Voxel.
 * \param writer Stream writer.
 * \return Nonzero on success.
 */
int livox_voxel_write (
	LIVoxVoxel*  self,
	LIArcWriter* writer)
{
	return liarc_writer_append_uint8 (writer, self->type);
}

/**
 * \brief Gets the terrain type of the voxel.
 * \param self Voxel.
 * \return Terrain type.
 */
int livox_voxel_get_type (
	const LIVoxVoxel* self)
{
	return self->type;
}

/** @} */
/** @} */
