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
 * \addtogroup LIExtTilesPhysics TilesPhysics
 * @{
 */

#include "ext-module.h"

/*****************************************************************************/

LIMaiExtensionInfo liext_tiles_physics_info =
{
	LIMAI_EXTENSION_VERSION, "tiles-physics",
	liext_tiles_physics_new,
	liext_tiles_physics_free
};

LIExtModule* liext_tiles_physics_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Make sure the physics and tiles extensions are loaded. */
	if (!limai_program_insert_extension (program, "physics") ||
	    !limai_program_insert_extension (program, "tiles"))
	{
		liext_tiles_physics_free (self);
		return NULL;
	}

	/* Find the physics and voxel managers. */
	self->physics = limai_program_find_component (program, "physics");
	self->voxels = limai_program_find_component (program, "voxels");
	if (self->physics == NULL || self->voxels == NULL)
	{
		liext_tiles_physics_free (self);
		return NULL;
	}

	/* Allocate the physics terrain manager. */
	self->terrain = liphy_terrain_new (self->physics, self->voxels, LIPHY_GROUP_TILES, LIPHY_DEFAULT_COLLISION_MASK);
	if (self->terrain == NULL)
	{
		liext_tiles_physics_free (self);
		return NULL;
	}
	liphy_terrain_set_realized (self->terrain, 1);

	return self;
}

void liext_tiles_physics_free (
	LIExtModule* self)
{
	if (self->terrain != NULL)
		liphy_terrain_free (self->terrain);
	lisys_free (self);
}

/** @} */
/** @} */
