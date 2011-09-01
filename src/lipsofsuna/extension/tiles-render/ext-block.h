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

#ifndef __EXT_BLOCK_H__
#define __EXT_BLOCK_H__

#include "ext-module.h"

struct _LIExtBlock
{
	LIExtModule* module;
	LIRenModel* model;
	LIRenObject* object;
};

LIExtBlock* liext_tiles_render_block_new (
	LIExtModule* module);

void liext_tiles_render_block_free (
	LIExtBlock* self);

void liext_tiles_render_block_clear (
	LIExtBlock* self);

#endif
