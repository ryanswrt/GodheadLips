/* 
 * Godhead
 * Copyright 2011 Godhead development team
 * Based on Lips of Suna
 *
 * Lips of Suna
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
 * \addtogroup LIExtHeightmap Heightmap
 * @{
 */
#include "SDL.h"
#include "SDL_main.h"
#include "ext-module.h"

#define MAX_TEXTURES    256


/**
    Script callback Los.heightmap_generate
    \param LIScrArgs* args
    
    str file, vector pos, vec size, array materials
*/
static void Heightmap_heightmap_generate (LIScrArgs* args)
{
	LIExtModule* module;
    const char* map_file;
    const char* tiles_file;
	LIMatVector posv;
	LIMatVector size;
    int materials[MAX_TEXTURES];
    int* map_data;
    int* tiles_data;
	int min[3];
	int max[3];
	int sz[3];
    int pos[3];
    int i;
    int value;
	LIVoxVoxel* tmp;
	LIVoxManager* voxels;
    
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_HEIGHTMAP);
	voxels = limai_program_find_component (module->program, "voxels");
	if (voxels == NULL)
		return;
        
	if (!liscr_args_geti_string (args, 0, &map_file) &&
	    liscr_args_gets_string (args, "map", &map_file))
        return;
	if (!liscr_args_geti_string (args, 1, &tiles_file))
	    liscr_args_gets_string (args, "tiles", &tiles_file);
	if (!liscr_args_geti_vector (args, 2, &pos))
	    liscr_args_gets_vector (args, "pos", &pos);
	if (!liscr_args_geti_vector (args, 3, &size))
		liscr_args_gets_vector (args, "size", &size);
	if (!liscr_args_geti_intv (args, 4, MAX_TEXTURES, materials))
		liscr_args_gets_intv (args, "materials", MAX_TEXTURES, materials);

    if (liext_heightmap_generate(module, map_file, size.x, size.y, size.z, &map_data) != 0)
    {
        return;
    }
    if (liext_heightmap_generate(module, tiles_file, size.x, size.y, size.z, &tiles_data) != 0)
    {
        liext_heightmap_cleanup(module, &map_data);
        return;
    }
    
	/* Calculate the size of the area. */
	min[0] = posv.x;
	min[1] = posv.y;
	min[2] = posv.z;
	max[0] = posv.x + size.x - 1;
	max[1] = posv.y + size.y - 1;
	max[2] = posv.z + size.z - 1;
	sz[0] = size.x;
	sz[1] = size.y;
	sz[2] = size.z;

	/* Batch copy terrain data. */
	/* Reading all tiles at once is faster than operating on
	   individual tiles since there are fewer sector lookups. */
	tmp = lisys_calloc (sz[0] * sz[1] * sz[2], sizeof (LIVoxVoxel));
	if (tmp == NULL)
    {
        liext_heightmap_cleanup(module, &map_data);
        liext_heightmap_cleanup(module, &tiles_data);
		return;
    }
	livox_manager_copy_voxels (voxels, min[0], min[1], min[2],
		sz[0], sz[1], sz[2], tmp);

	/* Apply heightmap to the copied tiles. */
	for (pos[1] = min[1] ; pos[1] < max[1] ; pos[1]++)
	for (pos[0] = min[0] ; pos[0] < max[0] ; pos[0]++)
	{
		pos[2] = liext_heightmap_find(module, pos[0], pos[1], map_data);
		value = liext_heightmap_find(module, pos[0], pos[1], tiles_data);
        i = (pos[2] * size.x * size.y) + (pos[1] * size.x) + pos[0];
        livox_voxel_init (tmp + i, value);
	}

	/* Batch write the copied tiles. */
	livox_manager_paste_voxels (voxels, min[0], min[1], min[2],
		sz[0], sz[1], sz[2], tmp);
	lisys_free (tmp);
    
    liext_heightmap_cleanup(module, &map_data);
    liext_heightmap_cleanup(module, &tiles_data);
}

/*****************************************************************************/

void liext_script_heightmap (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_HEIGHTMAP, "heightmap_generate", Heightmap_heightmap_generate);
}

/** @} */
/** @} */
