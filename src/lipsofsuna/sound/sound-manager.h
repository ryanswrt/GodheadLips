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

#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/system.h>
#include "sound-sample.h"
#include "sound-system.h"

typedef struct _LISndManager LISndManager;
struct _LISndManager
{
	LISndSystem* system;
	LIAlgStrdic* samples;
};

LIAPICALL (LISndManager*, lisnd_manager_new, (
	LISndSystem* system));

LIAPICALL (void, lisnd_manager_free, (
	LISndManager* self));

LIAPICALL (void, lisnd_manager_clear, (
	LISndManager* self));

LIAPICALL (void, lisnd_manager_set_listener, (
	LISndManager*      self,
	const LIMatVector* pos,
	const LIMatVector* vel,
	const LIMatVector* dir,
	const LIMatVector* up));

LIAPICALL (LISndSample*, lisnd_manager_get_sample, (
	LISndManager* self,
	const char*   name));

LIAPICALL (int, lisnd_manager_set_sample, (
	LISndManager* self,
	const char*   name,
	const char*   path));

#endif
