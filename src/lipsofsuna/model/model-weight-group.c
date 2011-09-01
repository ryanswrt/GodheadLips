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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlWeightGroup WeightGroup
 * @{
 */

#include "model-weight-group.h"

int limdl_weight_group_init_copy (
	LIMdlWeightGroup*       self,
	const LIMdlWeightGroup* group)
{
	if (group->name != NULL)
	{
		self->name = lisys_string_dup (group->name);
		if (self->name == NULL)
			return 0;
	}
	if (group->bone != NULL)
	{
		self->bone = lisys_string_dup (group->bone);
		if (self->bone == NULL)
		{
			lisys_free (self->bone);
			return 0;
		}
	}

	return 1;
}

/** @} */
/** @} */
