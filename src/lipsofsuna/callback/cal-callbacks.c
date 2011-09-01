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
 * \addtogroup LICalCallbacks Callbacks
 * @{
 */

#include "cal-callbacks.h"

typedef struct _LICalCalltype LICalCalltype;
struct _LICalCalltype
{
	LICalCallfunc* funcs;
};

static void private_cleanup (
	LICalCallbacks* self);

static void private_free_type (
	LICalCalltype* self);

/*****************************************************************************/

/**
 * \brief Creates a new callback manager.
 * \return Callback manager or NULL.
 */
LICalCallbacks* lical_callbacks_new ()
{
	LICalCallbacks* self;

	self = lisys_calloc (1, sizeof (LICalCallbacks));
	if (self == NULL)
		return NULL;
	self->types = lialg_strdic_new ();
	if (self->types == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the callback manager.
 * \param self Callback manager.
 */
void lical_callbacks_free (
	LICalCallbacks* self)
{
	LIAlgStrdicIter iter;

	private_cleanup (self);
	LIALG_STRDIC_FOREACH (iter, self->types)
		private_free_type (iter.value);
	lialg_strdic_free (self->types);
	lisys_free (self);
}

/**
 * \brief Calls callbacks.
 * \param self Callback manager.
 * \param type Callback type to call.
 * \param marshal Parameter marshaler.
 * \param ... Parameters passed to the marshaler.
 */
void lical_callbacks_call (
	LICalCallbacks* self,
	const char*     type,
	LICalMarshal    marshal,
	                ...)
{
	va_list args;
	LICalCalltype* typ;
	LICalCallfunc* func;
	LICalCallfunc* func_next;

	typ = lialg_strdic_find (self->types, type);
	if (typ == NULL)
		return;
	for (func = typ->funcs ; func != NULL ; func = func_next)
	{
		func_next = func->next;
		if (func->removed)
			continue;
		va_start (args, marshal);
		marshal (func->call, func->data, args);
		va_end (args);
	}
}

/**
 * \brief Creates a new callback listener.
 * \param self Callback manager.
 * \param type Type of callbacks to listen to.
 * \param priority Handling priority, smaller is earlier.
 * \param call Callback function.
 * \param data Userdata to be passed to the callback function.
 * \param result Return location for a callback handle, or NULL.
 */
int lical_callbacks_insert (
	LICalCallbacks* self,
	const char*     type,
	int             priority,
	void*           call,
	void*           data,
	LICalHandle*    result)
{
	LICalCalltype* typ;
	LICalCallfunc* ptr;
	LICalCallfunc* func;

	/* Clear handle. */
	if (result != NULL)
		memset (result, 0, sizeof (LICalHandle));

	/* Find or create type. */
	typ = lialg_strdic_find (self->types, type);
	if (typ == NULL)
	{
		typ = lisys_calloc (1, sizeof (LICalCalltype));
		if (typ == NULL)
			return 0;
		if (!lialg_strdic_insert (self->types, type, typ))
		{
			lisys_free (typ);
			return 0;
		}
	}

	/* Allocate function. */
	func = lisys_calloc (1, sizeof (LICalCallfunc));
	if (func == NULL)
		return 0;
	func->prio = priority;
	func->call = call;
	func->data = data;

	/* Set handle. */
	if (result != NULL)
	{
		result->calls = self;
		result->func = func;
		strncpy (result->type, type, sizeof (result->type) - 1);
	}

	/* Insertion sort by priority. */
	if (typ->funcs == NULL)
	{
		typ->funcs = func;
		return 1;
	}
	if (priority < typ->funcs->prio)
	{
		func->next = typ->funcs;
		typ->funcs->prev = func;
		typ->funcs = func;
		return 1;
	}
	for (ptr = typ->funcs ; ptr->next != NULL ; ptr = ptr->next)
	{
		if (priority < ptr->next->prio)
			break;
	}
	func->prev = ptr;
	func->next = ptr->next;
	if (ptr->next != NULL)
		ptr->next->prev = func;
	ptr->next = func;

	return 1;
}

/**
 * \brief Updates callbacks.
 *
 * Removes callbacks without references.
 *
 * \param self Callbacks.
 */
void lical_callbacks_update (
	LICalCallbacks* self)
{
	private_cleanup (self);
}

/*****************************************************************************/

static void private_cleanup (
	LICalCallbacks* self)
{
	LICalCallfunc* func;
	LICalCallfunc* prev;

	for (func = self->removed ; func != NULL ; func = prev)
	{
		prev = func->prev;
		lisys_free (func);
	}
	self->removed = NULL;
}

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
