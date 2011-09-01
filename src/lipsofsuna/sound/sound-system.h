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

#ifndef __SOUND_SYSTEM_H__
#define __SOUND_SYSTEM_H__

#include <AL/al.h>
#include <AL/alc.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>

typedef struct _LISndSystem LISndSystem;
struct _LISndSystem
{
	ALCdevice* device;
	ALCcontext* context;
};

LIAPICALL (LISndSystem*, lisnd_system_new, ());

LIAPICALL (void, lisnd_system_free, (LISndSystem* self));

LIAPICALL (void, lisnd_system_set_listener, (
	LISndSystem*       self,
	const LIMatVector* pos,
	const LIMatVector* vel,
	const LIMatVector* dir,
	const LIMatVector* up));

#endif
