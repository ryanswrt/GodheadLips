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

#ifndef __SYSTEM_STRING_H__
#define __SYSTEM_STRING_H__

#include "system-compiler.h"

LIAPICALL (char*, lisys_string_dup, (
	const char* self));

LIAPICALL (char*, lisys_string_dupn, (
	const char* self,
	int         count));

LIAPICALL (char*, lisys_string_concat, (
	const char* self,
	const char* string));

LIAPICALL (char*, lisys_string_format, (
	const char* format,
	            ...)) LISYS_ATTR_FORMAT(1, 2);

LIAPICALL (int, lisys_string_split, (
	const char* self,
	char        separator,
	char***     result,
	int*        resultn));

LIAPICALL (char*, lisys_string_convert_sys_to_utf8, (
	const char* str));

#endif
