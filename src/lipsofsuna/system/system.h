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

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#include "system-compiler.h"

#ifdef _WIN32
#define LISYS_EXTENSION_DLL "dll"
#define LISYS_EXTENSION_EXE ".exe"
#else
#define LISYS_EXTENSION_DLL "so"
#define LISYS_EXTENSION_EXE ""
#endif

#ifdef NDEBUG
#define lisys_assert(exp) ((void) 0)
#else
#define lisys_assert(exp) ((exp)? (void) 0 : lisys_assert_fail (#exp, __FILE__, __LINE__, __func__))
#endif

LIAPICALL (void, lisys_assert_fail, (
	const char* asrt,
	const char* file,
	int         line,
	const char* func));

LIAPICALL (time_t, lisys_time, (
	time_t* t));

#endif
