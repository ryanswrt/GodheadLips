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

#define MAX_TEXTURES    9


/**
    Script callback Los.heightmap_generate
      This function converts the image data to Voxels:
      <li>First image contains height information in color values</li>
      <li>Second image contains material information in color values</li>
      <li>Position is the lower inferior voxel of the mapped area</li>
      <li>Size is the volume of the mapped area. Base size must fit with image size</li>
      <li>Materials is an array of mat ID used to associate voxel types</li>
    
    \param LIScrArgs* args
*/
static void Heightmap_heightmap_generate (LIScrArgs* args)
{
	LIExtModule* module;
	LIVoxManager* voxels;
	LIVoxVoxel* tmp;
    const char* map_file;
    const char* tiles_file;
	LIMatVector posv;
	LIMatVector sizev;
    int materials[MAX_TEXTURES];
    void* map_data;
    void* tiles_data;
	int min[3];
	int max[3];
    int size[3];
    int x,y,z, y2, i, value;
    
    printf("Starting heightmap generation\n");
    
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_HEIGHTMAP);
	voxels = limai_program_find_component (module->program, "voxels");
	if (voxels == NULL)
		return;
        
	if (!liscr_args_geti_string (args, 0, &map_file) &&
	    liscr_args_gets_string (args, "map", &map_file))
        return;
	if (!liscr_args_geti_string (args, 1, &tiles_file))
	    liscr_args_gets_string (args, "tiles", &tiles_file);
	if (!liscr_args_geti_vector (args, 2, &posv))
	    liscr_args_gets_vector (args, "pos", &posv);
	if (!liscr_args_geti_vector (args, 3, &sizev))
		liscr_args_gets_vector (args, "size", &sizev);
	if (!liscr_args_geti_intv (args, 4, MAX_TEXTURES, materials))
		liscr_args_gets_intv (args, "materials", MAX_TEXTURES, materials);

    if (liext_heightmap_generate(module, map_file, &map_data) != 0)
    {
        return;
    }
    if (liext_heightmap_generate(module, tiles_file, &tiles_data) != 0)
    {
        liext_heightmap_cleanup(module, &map_data);
        return;
    }
    
	/* Calculate the size of the area. */
    size[0] = sizev.x;
    size[1] = sizev.y;
    size[2] = sizev.z;
	min[0] = posv.x;
	min[1] = posv.y;
	min[2] = posv.z;
	max[0] = (int)posv.x + size[0];
	max[1] = (int)posv.y + size[1];
	max[2] = (int)posv.z + size[2];
    
    printf("Coords: %d %d %d / %d %d %d / %d %d %d\n", min[0], min[1], min[2], max[0], max[1], max[2], size[0], size[1], size[2]);

	/* Batch copy terrain data. */
	/* Reading all tiles at once is faster than operating on
	   individual tiles since there are fewer sector lookups. */
	tmp = lisys_calloc (size[0] * size[1] * size[2], sizeof (LIVoxVoxel));
	if (tmp == NULL)
    {
        liext_heightmap_cleanup(module, &map_data);
        liext_heightmap_cleanup(module, &tiles_data);
		return;
    }
	livox_manager_copy_voxels (voxels, min[0], min[1], min[2],
		size[0], size[1], size[2], tmp);

	/* Apply heightmap to the copied tiles. */
    i = 0;
	for (z = min[2] ; z < max[2] ; z++)
	for (y = min[1] ; y < max[1] ; y++)
	for (x = min[0] ; x < max[0] ; x++)
	{
        //TODO: better resolution
		y2 = 255 - liext_heightmap_find(module, x, z, map_data);
        printf("gen y: %d\n", y2);
        if (y2 != -1 && y <= y2)
        {
            value = liext_heightmap_find(module, x, z, tiles_data);
            if (value < 0 || value >= MAX_TEXTURES)
            {
                value = 0;
            }
            livox_voxel_init (tmp + i, 15 /* materials[value] */);
            printf("voxel: %d %d\n", i, value);
        }
        i++;
	}

	/* Batch write the copied tiles. */
	livox_manager_paste_voxels (voxels, min[0], min[1], min[2],
		size[0], size[1], size[2], tmp);
	lisys_free (tmp);
    
    printf("Area has been successfully loaded from heightmap\n");
    
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
