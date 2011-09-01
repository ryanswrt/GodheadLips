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
 * \addtogroup LICalMarshal Marshal
 * @{
 */

#include "cal-marshal.h"

void lical_marshal_DATA (void* call, void* data, va_list args)
{
	int (*func)(void*) = call;
	func(data);
}

void lical_marshal_DATA_FLT (void* call, void* data, va_list args)
{
	int (*func)(void*, float) = call;
	double arg0 = va_arg (args, double);
	func(data, arg0);
}

void lical_marshal_DATA_INT (void* call, void* data, va_list args)
{
	int (*func)(void*, int) = call;
	int arg0 = va_arg (args, int);
	func(data, arg0);
}

void lical_marshal_DATA_INT_INT (void* call, void* data, va_list args)
{
	int (*func)(void*, int, int) = call;
	int arg0 = va_arg (args, int);
	int arg1 = va_arg (args, int);
	func(data, arg0, arg1);
}

void lical_marshal_DATA_INT_PTR (void* call, void* data, va_list args)
{
	int (*func)(void*, int, void*) = call;
	int arg0 = va_arg (args, int);
	void* arg1 = va_arg (args, void*);
	func(data, arg0, arg1);
}

void lical_marshal_DATA_PTR (void* call, void* data, va_list args)
{
	int (*func)(void*, void*) = call;
	void* arg0 = va_arg (args, void*);
	func(data, arg0);
}

void lical_marshal_DATA_PTR_INT (void* call, void* data, va_list args)
{
	int (*func)(void*, void*, int) = call;
	void* arg0 = va_arg (args, void*);
	int arg1 = va_arg (args, int);
	func(data, arg0, arg1);
}

void lical_marshal_DATA_PTR_PTR (void* call, void* data, va_list args)
{
	int (*func)(void*, void*, void*) = call;
	void* arg0 = va_arg (args, void*);
	void* arg1 = va_arg (args, void*);
	func(data, arg0, arg1);
}

void lical_marshal_DATA_PTR_PTR_INT (void* call, void* data, va_list args)
{
	int (*func)(void*, void*, void*, int) = call;
	void* arg0 = va_arg (args, void*);
	void* arg1 = va_arg (args, void*);
	int arg2 = va_arg (args, int);
	func(data, arg0, arg1, arg2);
}

void lical_marshal_DATA_PTR_PTR_PTR (void* call, void* data, va_list args)
{
	int (*func)(void*, void*, void*, void*) = call;
	void* arg0 = va_arg (args, void*);
	void* arg1 = va_arg (args, void*);
	void* arg2 = va_arg (args, void*);
	func(data, arg0, arg1, arg2);
}

/** @} */
/** @} */
