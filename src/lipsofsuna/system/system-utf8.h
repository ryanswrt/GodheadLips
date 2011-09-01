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

#ifndef __SYSTEM_UTF8_H__
#define __SYSTEM_UTF8_H__

#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include "system-compiler.h"

LIAPICALL (int, lisys_utf8_get_char, (
	const char* self,
	wchar_t*    result));

LIAPICALL (int, lisys_utf8_get_length, (
	const char* self));

LIAPICALL (char*, lisys_utf8_get_next, (
	const char* self));

LIAPICALL (int, lisys_utf8_get_valid, (
	const char* self));

LIAPICALL (wchar_t*, lisys_utf8_to_wchar, (
	const char* self));

LIAPICALL (char*, lisys_wchar_to_utf8, (
	wchar_t self));

#endif
