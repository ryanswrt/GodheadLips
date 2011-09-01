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
 * \addtogroup LIExtModelEditing ModelEditing
 * @{
 */

#include "ext-module.h"

LIMaiExtensionInfo liext_model_editing_info =
{
	LIMAI_EXTENSION_VERSION, "ModelEditing",
	liext_model_editing_new,
	liext_model_editing_free
};

LIExtModule* liext_model_editing_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Extend classes. */
	liext_script_model_editing (program->script);

	return self;
}

void liext_model_editing_free (
	LIExtModule* self)
{
	lisys_free (self);
}

/** @} */
/** @} */
