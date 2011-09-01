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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtAi Ai
 * @{
 */

#include "ext-module.h"

static void private_block_free (
	LIExtModule*      self,
	LIVoxUpdateEvent* event);

static void private_block_load (
	LIExtModule*      self,
	LIVoxUpdateEvent* event);

/*****************************************************************************/

LIMaiExtensionInfo liext_ai_info =
{
	LIMAI_EXTENSION_VERSION, "Ai",
	liext_ai_new,
	liext_ai_free
};

LIExtModule* liext_ai_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Make sure the tiles extension is loaded. */
	if (!limai_program_insert_extension (program, "tiles"))
	{
		liext_ai_free (self);
		return NULL;
	}

	/* Allocate the AI manager. */
	self->voxels = limai_program_find_component (program, "voxels");
	self->ai = liai_manager_new (program->callbacks, program->sectors, self->voxels);

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_AI, self);
	liext_script_ai (program->script);

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "block-free", 1, private_block_free, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, "block-load", 1, private_block_load, self, self->calls + 1))
	{
		liext_ai_free (self);
		return NULL;
	}

	return self;
}

void liext_ai_free (
	LIExtModule* self)
{
	/* Free callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	if (self->ai != NULL)
		liai_manager_free (self->ai);
	lisys_free (self);
}

/*****************************************************************************/

static void private_block_free (
	LIExtModule*      self,
	LIVoxUpdateEvent* event)
{
}

static void private_block_load (
	LIExtModule*      self,
	LIVoxUpdateEvent* event)
{
	int blockw;
	int sectorw;

	/* Update waypoints. */
	blockw = self->voxels->tiles_per_line / self->voxels->blocks_per_line;
	sectorw = self->voxels->tiles_per_line;
	liai_manager_update_block (self->ai,
		sectorw * event->sector[0] + blockw * event->block[0],
		sectorw * event->sector[1] + blockw * event->block[1],
		sectorw * event->sector[2] + blockw * event->block[2],
		blockw, blockw, blockw);
}

/** @} */
/** @} */
