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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysSystem System
 * @{
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_WINDOWS_H
#define _WIN32_IE 0x0400
#include <windows.h>
#include <shlobj.h>
#endif
#include "system.h"
#include "system-error.h"
#include "system-path.h"

void lisys_assert_fail (
	const char* asrt,
	const char* file,
	int         line,
	const char* func)
{
	fprintf (stderr, "%s:%d: %s: Assertion `%s' failed.\n", file, line, func, asrt);
	abort ();
}

/**
 * \brief Gets the current time.
 *
 * \param t Return location for the time or NULL.
 * \return Time.
 */
time_t lisys_time (
	time_t* t)
{
	return time (t);
}

/** @} */
/** @} */
