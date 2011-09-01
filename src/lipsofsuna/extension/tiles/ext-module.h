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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include "lipsofsuna/voxel.h"
#include "lipsofsuna/extension.h"

typedef struct _LIExtBlock LIExtBlock;
typedef struct _LIExtModule LIExtModule;

#define LIEXT_SCRIPT_MATERIAL "Material"
#define LIEXT_SCRIPT_VOXEL "Voxel"

struct _LIExtModule
{
	float timer;
	LICalHandle calls[1];
	LIMaiProgram* program;
	LIVoxManager* voxels;
};

LIExtModule* liext_tiles_new (
	LIMaiProgram* program);

void liext_tiles_free (
	LIExtModule* self);

int liext_tiles_build_block (
	LIExtModule*    self,
	LIVoxBlockAddr* addr);

LIVoxManager* liext_tiles_get_voxels (
	LIExtModule* self);

/*****************************************************************************/

void liext_script_material (
	LIScrScript* self);

void liext_script_tile (
	LIScrScript* self);

void liext_script_voxel (
	LIScrScript* self);

#endif
