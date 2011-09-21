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
#include <lipsofsuna/system.h>


/*
    Put defines, macros and internal functions definitions here
*/

/*****************************************************************************/

LIMaiExtensionInfo liext_heightmap_info =
{
	LIMAI_EXTENSION_VERSION, "Heightmap",
	liext_heightmap_new,
	liext_heightmap_free
};

LIExtModule* liext_heightmap_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

    /* Allocate paths */
    //self->paths = lipth_paths_new (NULL, LIEXT_SCRIPT_HEIGHTMAP);
    
	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_HEIGHTMAP, self);
	liext_script_heightmap (program->script);
	printf("heightmap initialized");
	return self;
}

void liext_heightmap_free (
	LIExtModule* self)
{
    //lipth_paths_free(self->paths);
	lisys_free (self);
}

int liext_heightmap_generate (
	LIExtModule* self,
	const char* file,
    void**     data)
{
    SDL_Surface* surface;
    char        location[100];

    if (!data) return -1;
    
    //location = lipth_paths_get_graphics (self->paths, file);
    strcpy(location, "./data/godhead/graphics/");
    strcat(location, file);
    printf("Location of graphics file: %s\n", location);
    
    surface = SDL_LoadBMP(location);
    if (surface)
    {
        if (surface->format->BitsPerPixel != 24)
        {
            SDL_FreeSurface (surface);
            printf("Failed, not 24 bpp\n");
            return -1;
        }
    }
    else
    {	
		printf("Failed, no surface\n");
        return -1;
    }
    
    /* TODO: stretch image (interpolation) */
    *data = surface;
    printf("Image loaded\n");
    return 0;
}

int liext_heightmap_find (
	LIExtModule* self,
    int        x,
    int        y,
    void*      data)
{
    SDL_Surface* surface;
    unsigned char* pixels;

    surface = (SDL_Surface*) data;
    pixels = (unsigned char*) (surface->pixels);
    
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h)
    {
        //printf("Bad coordinate given, ignoring");
        return -1;
    }
    
    /* We only get the R component for now */
    return pixels[ 3 * ((y * surface->w) + x) ];
}

void liext_heightmap_cleanup(
    LIExtModule* self,
    void**     data)
{
    SDL_FreeSurface(*data);
    *data = 0;
}

/*****************************************************************************/

/*
    Put internal functions here
*/


/** @} */
/** @} */
