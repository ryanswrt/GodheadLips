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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtWatchdog Watchdog
 * @{
 */

#include "ext-module.h"

static void Watchdog_start (LIScrArgs* args)
{
	float timer = 5.0f;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_WATCHDOG);
	liscr_args_geti_float (args, 0, &timer);
	liext_watchdog_start (module, timer);
}

static void Watchdog_stop (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_WATCHDOG);
	liext_watchdog_stop (module);
}

/*****************************************************************************/

void liext_script_watchdog (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WATCHDOG, "watchdog_start", Watchdog_start);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WATCHDOG, "watchdog_stop", Watchdog_stop);
}

/** @} */
/** @} */
