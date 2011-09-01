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
 * \addtogroup LIExtTilesRender TilesRender
 * @{
 */

#include "ext-module.h"
#include "ext-block.h"

LIExtBlock* liext_tiles_render_block_new (
	LIExtModule* module)
{
	LIExtBlock* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtBlock));
	if (self == NULL)
		return NULL;
	self->module = module;

	return self;
}

void liext_tiles_render_block_free (
	LIExtBlock* self)
{
	if (self->object != NULL)
		liren_object_free (self->object);
	if (self->model != NULL)
		liren_model_free (self->model);
	lisys_free (self);
}

void liext_tiles_render_block_clear (
	LIExtBlock* self)
{
	if (self->object != NULL)
	{
		liren_object_free (self->object);
		self->object = NULL;
	}
	if (self->model != NULL)
	{
		liren_model_free (self->model);
		self->model = NULL;
	}
}

/** @} */
/** @} */
