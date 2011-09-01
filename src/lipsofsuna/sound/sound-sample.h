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

#ifndef __SOUND_SAMPLE_H__
#define __SOUND_SAMPLE_H__

#include "lipsofsuna/system.h"
#include "sound-system.h"

typedef struct _LISndSample LISndSample;
struct _LISndSample
{
	int loaded;
	int stereo;
	char* file;
	ALuint buffers[2];
	LISysAsyncCall* worker;
};

LIAPICALL (LISndSample*, lisnd_sample_new, (
	LISndSystem* system,
	const char*  file));

LIAPICALL (void, lisnd_sample_free, (
	LISndSample* self));

#endif
