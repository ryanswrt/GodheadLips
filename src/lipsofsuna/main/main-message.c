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
 * \addtogroup LIMai Main
 * @{
 * \addtogroup LIMaiExtension Extension
 * @{
 */

#include "main-message.h"

LIMaiMessage* limai_message_new (
	int         type,
	const char* name,
	const void* data)
{
	LIMaiMessage* self;

	self = lisys_calloc (1, sizeof (LIMaiMessage));
	if (self == NULL)
		return 0;
	self->type = type;
	self->name = lisys_string_dup (name);
	if (self->name == NULL)
	{
		lisys_free (self);
		return 0;
	}
	switch (type)
	{
		case LIMAI_MESSAGE_TYPE_EMPTY:
			break;
		case LIMAI_MESSAGE_TYPE_MODEL:
			self->model = limdl_model_new_copy (data);
			if (self->model == NULL)
			{
				lisys_free (self->name);
				lisys_free (self);
				return 0;
			}
			break;
		case LIMAI_MESSAGE_TYPE_STRING:
			self->string = lisys_string_dup (data);
			if (self->string == NULL)
			{
				lisys_free (self->name);
				lisys_free (self);
				return 0;
			}
			break;
		default:
			lisys_free (self->name);
			lisys_free (self);
			lisys_assert (0);
			return 0;
	}

	return self;
}

void limai_message_free (
	LIMaiMessage* self)
{
	switch (self->type)
	{
		case LIMAI_MESSAGE_TYPE_EMPTY:
			break;
		case LIMAI_MESSAGE_TYPE_MODEL:
			if (self->model != NULL)
				limdl_model_free (self->model);
			break;
		case LIMAI_MESSAGE_TYPE_STRING:
			lisys_free (self->string);
			break;
		default:
			lisys_assert (0);
			break;
	}
	lisys_free (self->name);
	lisys_free (self);
}

/** @} */
/** @} */
