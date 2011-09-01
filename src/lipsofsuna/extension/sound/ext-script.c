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
 * \addtogroup LIExtSound Sound
 * @{
 */

#include "ext-module.h"

static void Sound_effect (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	int tmp;
	int flags = 0;
	float pitch;
	float volume;
	const char* effect;
	LIEngObject* object;
	LIExtModule* module;
	LIMatVector velocity;
	LIScrData* data;
	LISndSource* source;

	if (liscr_args_gets_string (args, "effect", &effect) &&
	    liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data))
	{
		if (liscr_args_gets_bool (args, "positional", &tmp) && !tmp)
			flags |= LIEXT_SOUND_FLAG_NONPOSITIONAL;
		if (liscr_args_gets_bool (args, "repeating", &tmp) && tmp)
			flags |= LIEXT_SOUND_FLAG_REPEAT;
		object = liscr_data_get_data (data);
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
		source = liext_sound_set_effect (module, object->id, effect, flags);
		if (source != NULL)
		{
			if (liscr_args_gets_float (args, "pitch", &pitch))
				lisnd_source_set_pitch (source, pitch);
			if (liscr_args_gets_float (args, "volume", &volume))
				lisnd_source_set_volume (source, volume);
			if (liscr_args_gets_vector (args, "velocity", &velocity))
				lisnd_source_set_velocity (source, &velocity);
		}
	}
#endif
}

static void Sound_get_listener_position (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
	liscr_args_seti_vector (args, &module->listener_position);
}
static void Sound_set_listener_position (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector value;

	if (liscr_args_geti_vector (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
		module->listener_position = value;
	}
}

static void Sound_get_listener_rotation (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
	liscr_args_seti_quaternion (args, &module->listener_rotation);
}
static void Sound_set_listener_rotation (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatQuaternion value;

	if (liscr_args_geti_quaternion (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
		module->listener_rotation = value;
	}
}

static void Sound_get_listener_velocity (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
	liscr_args_seti_vector (args, &module->listener_velocity);
}
static void Sound_set_listener_velocity (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector value;

	if (liscr_args_geti_vector (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
		module->listener_velocity = value;
	}
}

static void Sound_set_music (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	const char* value;
	LIExtModule* module;

	if (liscr_args_geti_string (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
		liext_sound_set_music (module, value);
	}
#endif
}

static void Sound_set_music_fading (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		value = LIMAT_MAX (0.0f, value);
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
		liext_sound_set_music_fading (module, value);
	}
#endif
}

static void Sound_get_music_offset (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
	if (module->music != NULL)
		liscr_args_seti_float (args, lisnd_source_get_offset (module->music));
	else
		liscr_args_seti_float (args, 0.0f);
#else
	liscr_args_seti_float (args, 0.0f);
#endif
}

static void Sound_set_music_offset (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
		if (module->music != NULL)
			lisnd_source_set_offset (module->music, value);
	}
#endif
}

static void Sound_set_music_volume (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND);
		liext_sound_set_music_volume (module, value);
	}
#endif
}

/*****************************************************************************/

void liext_script_sound (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_effect", Sound_effect);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_get_listener_position", Sound_get_listener_position);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_set_listener_position", Sound_set_listener_position);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_get_listener_rotation", Sound_get_listener_rotation);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_set_listener_rotation", Sound_set_listener_rotation);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_get_listener_velocity", Sound_get_listener_velocity);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_set_listener_velocity", Sound_set_listener_velocity);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_set_music", Sound_set_music);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_set_music_fading", Sound_set_music_fading);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_get_music_offset", Sound_get_music_offset);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_set_music_offset", Sound_set_music_offset);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND, "sound_set_music_volume", Sound_set_music_volume);
}

/** @} */
/** @} */
