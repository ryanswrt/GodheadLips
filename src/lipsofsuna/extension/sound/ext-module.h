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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#ifndef LI_DISABLE_SOUND
#include "lipsofsuna/sound.h"
#endif
#include "lipsofsuna/extension.h"

#define LIEXT_SCRIPT_SOUND "Sound"

enum
{
	LIEXT_SOUND_FLAG_NONPOSITIONAL = 0x1,
	LIEXT_SOUND_FLAG_REPEAT = 0x2
};

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	int music_looping;
	float music_fading;
	float music_volume;
	LIMaiProgram* program;
	LIMatVector listener_position;
	LIMatVector listener_velocity;
	LIMatQuaternion listener_rotation;
#ifndef LI_DISABLE_SOUND
	LIAlgU32dic* objects;
	LICalHandle calls[1];
	LISndSystem* system;
	LISndManager* sound;
	LISndSource* music;
	LISndSource* music_fade;
#endif
};

LIExtModule* liext_sound_new (
	LIMaiProgram* program);

void liext_sound_free (
	LIExtModule* self);

#ifndef LI_DISABLE_SOUND
LISndSample* liext_sound_find_sample (
	LIExtModule* self,
	const char*  name);

LISndSource* liext_sound_set_effect (
	LIExtModule* self,
	uint32_t     object,
	const char*  effect,
	int          flags);

int liext_sound_set_music (
	LIExtModule* self,
	const char*  value);

void liext_sound_set_music_fading (
	LIExtModule* self,
	float        value);

void liext_sound_set_music_looping (
	LIExtModule* self,
	int          value);

void liext_sound_set_music_volume (
	LIExtModule* self,
	float        value);
#endif

/*****************************************************************************/

#ifndef LI_DISABLE_SOUND
typedef struct _LIExtObject LIExtObject;
struct _LIExtObject
{
	LIAlgList* sounds;
};

LIExtObject* liext_object_new ();

void liext_object_free (
	LIExtObject* self);
#endif

/*****************************************************************************/

void liext_script_sound (
	LIScrScript* self);

#endif
