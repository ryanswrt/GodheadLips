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
 * \addtogroup LIAi Ai
 * @{
 * \addtogroup LIAiPath Path
 * @{
 */

#include <lipsofsuna/system.h>
#include "ai-path.h"

/**
 * \brief Creates a new path.
 *
 * \return New path or NULL.
 */
LIAiPath*
liai_path_new ()
{
	LIAiPath* self;

	self = lisys_calloc (1, sizeof (LIAiPath));
	return self;
}

/**
 * \brief Frees the path.
 *
 * \param self Path.
 */
void
liai_path_free (LIAiPath* self)
{
	lisys_free (self->points.points);
	lisys_free (self);
}

/**
 * \brief Appends a node to the path.
 *
 * \param self Path.
 * \param point Position vector.
 * \return Nonzero on success.
 */
int
liai_path_append_point (LIAiPath*          self,
                        const LIMatVector* point)
{
	LIMatVector* tmp;

	/* Allocate space. */
	tmp = lisys_realloc (self->points.points, (self->points.count + 1) * sizeof (LIMatVector));
	if (tmp == NULL)
		return 0;
	self->points.points = tmp;
	tmp += self->points.count++;

	/* Set value. */
	*tmp = *point;
	return 1;
}

/**
 * \brief Gets a node from the path.
 *
 * \param self Path.
 * \param index Node index.
 * \param value Return location for the position vector.
 */
void
liai_path_get_point (const LIAiPath* self,
                     int             index,
                     LIMatVector*    value)
{
	lisys_assert (index >= 0);
	lisys_assert (index < self->points.count);

	*value = self->points.points[index];
}

/**
 * \brief Gets the number of nodes in the path.
 *
 * \param self Path.
 * \return Number of nodes.
 */
int
liai_path_get_length (const LIAiPath* self)
{
	return self->points.count;
}

/**
 * \brief Gets the saved position in the path.
 *
 * \param self Path.
 * \return Position index.
 */
int
liai_path_get_position (const LIAiPath* self)
{
	return self->position;
}

/**
 * \brief Stores a position number to the path.
 *
 * \param self Path.
 * \param index Position index.
 */
void
liai_path_set_position (LIAiPath* self,
                        int       index)
{
	lisys_assert (index >= 0);
	lisys_assert (index <= self->points.count);

	self->position = index;
}

/** @} */
/** @} */
