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
 * \addtogroup LICal Callback
 * @{
 * \addtogroup LICalHandle Handle
 * @{
 */

#include "cal-callbacks.h"
#include "cal-handle.h"

typedef struct _LICalCalltype LICalCalltype;
struct _LICalCalltype
{
	LICalCallfunc* funcs;
};

static void private_free_type (
	LICalCalltype* self);

/*****************************************************************************/

/**
 * \brief Releases an event handler callback.
 * \param self Handle.
 */
void lical_handle_release (
	LICalHandle* self)
{
	LICalCalltype* typ;
	LICalCallfunc* func;

	if (self->func == NULL)
		return;
	typ = lialg_strdic_find (self->calls->types, self->type);
	if (typ == NULL)
		return;
	for (func = typ->funcs ; func != NULL ; func = func->next)
	{
		if (func == self->func)
		{
			/* Remove from type. */
			if (func->prev == NULL)
				typ->funcs = func->next;
			else
				func->prev->next = func->next;
			if (func->next != NULL)
				func->next->prev = func->prev;

			/* Remove empty types. */
			if (typ->funcs == NULL)
			{
				lialg_strdic_remove (self->calls->types, self->type);
				private_free_type (typ);
			}

			/* Queue for removal. */
			func->prev = self->calls->removed;
			func->removed = 1;
			self->calls->removed = func;
			break;
		}
	}
	memset (self, 0, sizeof (LICalHandle));
}

/**
 * \brief Releases event handler callbacks.
 * \param self Array of handles.
 * \param count Number of handles.
 */
void lical_handle_releasev (
	LICalHandle* self,
	int          count)
{
	int i;

	for (i = 0 ; i < count ; i++)
		lical_handle_release (self + i);
}

/*****************************************************************************/

static void private_free_type (
	LICalCalltype* self)
{
	LICalCallfunc* func;
	LICalCallfunc* func_next;

	for (func = self->funcs ; func != NULL ; func = func_next)
	{
		func_next = func->next;
		lisys_free (func);
	}
	lisys_free (self);
}

/** @} */
/** @} */
