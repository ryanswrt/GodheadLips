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
 * \addtogroup LIVoxBlock Block
 * @{
 */

#include <lipsofsuna/system.h>
#include "voxel.h"
#include "voxel-block.h"
#include "voxel-manager.h"
#include "voxel-private.h"

/**
 * \brief Returns nonzero if the block is dirty.
 *
 * \param self Block.
 * \return Nonzero if dirty.
 */
int
livox_block_get_dirty (const LIVoxBlock* self)
{
	return self->dirty;
}

/**
 * \brief Sets or clears the dirty flag of the block.
 *
 * \param self Block.
 * \param value Zero to clear, nonzero to set.
 */
void
livox_block_set_dirty (LIVoxBlock* self,
                       int         value)
{
	self->dirty = value;
}

/**
 * \brief Gets the modification stamp of the block.
 *
 * \param self Block.
 * \return Modification stamp.
 */
int
livox_block_get_stamp (const LIVoxBlock* self)
{
	return self->stamp;
}

/** @} */
/** @} */
