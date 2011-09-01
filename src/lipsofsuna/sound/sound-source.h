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

#ifndef __SOUND_SOURCE_H__
#define __SOUND_SOURCE_H__

#include "sound-sample.h"
#include "sound-system.h"
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>

typedef struct _LISndSource LISndSource;
struct _LISndSource
{
	int blocked_playing;
	int stereo;
	float volume;
	float fade_factor;
	float fade_value;
	ALuint source;
	ALint queued;
	LISndSample* blocked_sample;
};

LIAPICALL (LISndSource*, lisnd_source_new, (
	LISndSystem* system,
	int          stereo));

LIAPICALL (LISndSource*, lisnd_source_new_with_sample, (
	LISndSystem* system,
	LISndSample* sample,
	int          stereo));

LIAPICALL (void, lisnd_source_free, (
	LISndSource* self));

LIAPICALL (void, lisnd_source_queue_sample, (
	LISndSource* self,
	LISndSample* sample));

LIAPICALL (int, lisnd_source_update, (
	LISndSource* self,
	float        secs));

LIAPICALL (void, lisnd_source_set_fading, (
	LISndSource* self,
	float        start,
	float        speed));

LIAPICALL (void, lisnd_source_set_looping, (
	LISndSource* self,
	int          looping));

LIAPICALL (void, lisnd_source_set_pitch, (
	LISndSource* self,
	float        value));

LIAPICALL (int, lisnd_source_get_playing, (
	LISndSource* self));

LIAPICALL (void, lisnd_source_set_playing, (
	LISndSource* self,
	int          playing));

LIAPICALL (float, lisnd_source_get_offset, (
	LISndSource* self));

LIAPICALL (void, lisnd_source_set_offset, (
	LISndSource* self,
	float        secs));

LIAPICALL (void, lisnd_source_set_position, (
	LISndSource*       self,
	const LIMatVector* value));

LIAPICALL (void, lisnd_source_set_velocity, (
	LISndSource*       self,
	const LIMatVector* value));

LIAPICALL (void, lisnd_source_set_volume, (
	LISndSource* self,
	float        value));

#endif
