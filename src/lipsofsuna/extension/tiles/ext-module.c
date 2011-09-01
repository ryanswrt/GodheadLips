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

#define REBUILD_TIMER 0.2

static int private_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_tiles_info =
{
	LIMAI_EXTENSION_VERSION, "tiles",
	liext_tiles_new,
	liext_tiles_free
};

LIExtModule* liext_tiles_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Create voxel manager. */
	self->voxels = livox_manager_new (program->callbacks, program->sectors);
	if (self->voxels == NULL)
	{
		liext_tiles_free (self);
		return NULL;
	}

	/* Register component. */
	if (!limai_program_insert_component (program, "voxels", self->voxels))
	{
		liext_tiles_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "tick", 0, private_tick, self, self->calls + 0))
	{
		liext_tiles_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_MATERIAL, self);
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_VOXEL, self);
	liext_script_material (program->script);
	liext_script_voxel (program->script);

	return self;
}

void liext_tiles_free (
	LIExtModule* self)
{
	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Unregister component. */
	if (self->voxels != NULL)
		limai_program_remove_component (self->program, "voxels");

	/* Free resources. */
	if (self->voxels != NULL)
		livox_manager_free (self->voxels);

	lisys_free (self);
}

/**
 * \brief Gets the voxel manager of the module.
 *
 * This function is used by other modules, such as the NPC module, to interact
 * with the voxel terrain.
 *
 * \warning Accessing the terrain from a different thread isn't safe.
 *
 * \param self Module.
 * \return Voxel manager.
 */
LIVoxManager* liext_tiles_get_voxels (
	LIExtModule* self)
{
	return self->voxels;
}

/*****************************************************************************/

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	self->timer += secs;
	if (self->timer >= REBUILD_TIMER)
	{
		livox_manager_mark_updates (self->voxels);
		livox_manager_update_marked (self->voxels);
		self->timer = 0;
	}

	return 1;
}

/** @} */
/** @} */
