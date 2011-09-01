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
 * \addtogroup LIVox Voxel
 * @{
 * \addtogroup LIVoxUnittest Unittest
 * @{
 */

#include "voxel-manager.h"
#include "voxel-material.h"

void livox_unittest (
	LIVoxVoxel* self,
	int         type)
{
	int i;
	LIAlgSectors* sectors;
	LICalCallbacks* callbacks;
	LIVoxManager* manager;
	LIVoxMaterial* material;
	LIVoxVoxel voxel;

	callbacks = lical_callbacks_new ();
	sectors = lialg_sectors_new (128, 24);
	manager = livox_manager_new (callbacks, sectors);
	material = livox_material_new ();
	material->id = 1;
	material->type = LIVOX_MATERIAL_TYPE_SLOPED;
	livox_manager_insert_material (manager, material);

	/* Rebuild benchmarking. */
	printf ("Benchmarking terrain rebuilding.\n");
	livox_voxel_init (&voxel, 1);
	for (i = 0 ; i < 1000000 ; i++)
	{
		livox_manager_set_voxel (manager, 48, 48, 48, &voxel); 
		livox_manager_mark_updates (manager);
		livox_manager_update_marked (manager);
	}

	livox_manager_free (manager);
	lical_callbacks_free (callbacks);
	lialg_sectors_free (sectors);
}

/** @} */
/** @} */
