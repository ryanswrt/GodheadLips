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

#include <lipsofsuna/network.h>
#include "ext-module.h"

#ifndef LI_DISABLE_SOUND
static int private_tick (
	LIExtModule* self,
	float        secs);
#endif

/*****************************************************************************/

LIMaiExtensionInfo liext_sound_info =
{
	LIMAI_EXTENSION_VERSION, "Sound",
	liext_sound_new,
	liext_sound_free
};

LIExtModule* liext_sound_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->music_looping = 0;
	self->music_volume = 1.0f;
	self->music_fading = 1.0f;
	self->listener_rotation = limat_quaternion_identity ();

#ifndef LI_DISABLE_SOUND
	/* Allocate objects. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		liext_sound_free (self);
		return NULL;
	}

	/* Initialize sound. */
	self->system = lisnd_system_new ();
	if (self->system != NULL)
		self->sound = lisnd_manager_new (self->system);
	else
		printf ("WARNING: cannot initialize sound\n");

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "tick", 1, private_tick, self, self->calls + 0))
	{
		liext_sound_free (self);
		return NULL;
	}
#endif

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_SOUND, self);
	liext_script_sound (program->script);

	return self;
}

void liext_sound_free (
	LIExtModule* self)
{
#ifndef LI_DISABLE_SOUND
	LIAlgU32dicIter iter;

	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Free objects. */
	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
			liext_object_free (iter.value);
		lialg_u32dic_free (self->objects);
	}

	/* Free music. */
	if (self->music != NULL)
		lisnd_source_free (self->music);
	if (self->music_fade != NULL)
		lisnd_source_free (self->music_fade);

	/* Disable sound. */
	if (self->sound != NULL)
		lisnd_manager_free (self->sound);
	if (self->sound != NULL)
		lisnd_system_free (self->system);
#endif

	lisys_free (self);
}

#ifndef LI_DISABLE_SOUND
/**
 * \brief Finds a sound sample by name.
 *
 * \param self Module.
 * \param name Name of the sample.
 * \return Sample owned by the module or NULL.
 */
LISndSample* liext_sound_find_sample (
	LIExtModule* self,
	const char*  name)
{
	int ret;
	char* path;
	LISndSample* sample;

	/* Check for existing. */
	sample = lisnd_manager_get_sample (self->sound, name);
	if (sample != NULL)
		return sample;

	/* Try to load FLAC. */
	path = lisys_path_format (self->program->paths->module_data,
		LISYS_PATH_SEPARATOR, "sounds",
		LISYS_PATH_SEPARATOR, name, ".flac", NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
	{
		ret = lisnd_manager_set_sample (self->sound, name, path);
		lisys_free (path);
		if (ret)
			return lisnd_manager_get_sample (self->sound, name);
	}
	else
		lisys_free (path);

	/* Try to load OGG. */
	path = lisys_path_format (self->program->paths->module_data,
		LISYS_PATH_SEPARATOR, "sounds",
		LISYS_PATH_SEPARATOR, name, ".ogg", NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
	{
		ret = lisnd_manager_set_sample (self->sound, name, path);
		lisys_free (path);
		if (ret)
			return lisnd_manager_get_sample (self->sound, name);
	}
	else
		lisys_free (path);

	return 0;
}

LISndSource* liext_sound_set_effect (
	LIExtModule* self,
	uint32_t     object,
	const char*  effect,
	int          flags)
{
	int create;
	LIEngObject* engobj;
	LIExtObject* extobj;
	LIMatTransform transform;
	LISndSample* sample;
	LISndSource* source;

	/* Find sample. */
	if (self->sound == NULL)
	{
		lisys_error_set (ENOTSUP, "no sound support");
		return NULL;
	}
	sample = liext_sound_find_sample (self, effect);
	if (sample == NULL)
		return NULL;

	/* Find engine object. */
	create = 0;
	engobj = lieng_engine_find_object (self->program->engine, object);
	if (engobj == NULL)
		return NULL;

	/* Find or create sound object. */
	extobj = lialg_u32dic_find (self->objects, object);
	if (extobj == NULL)
	{
		create = 1;
		extobj = liext_object_new ();
		if (extobj == NULL)
			return NULL;
		if (!lialg_u32dic_insert (self->objects, object, extobj))
		{
			liext_object_free (extobj);
			return NULL;
		}
	}

	/* Allocate new source. */
	source = lisnd_source_new_with_sample (self->system, sample, flags & LIEXT_SOUND_FLAG_NONPOSITIONAL);
	if (source == NULL)
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_object_free (extobj);
		}
		return NULL;
	}
	if (!lialg_list_prepend (&extobj->sounds, source))
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_object_free (extobj);
		}
		lisnd_source_free (source);
		return NULL;
	}

	/* Set properties. */
	lieng_object_get_transform (engobj, &transform);
	lisnd_source_set_position (source, &transform.position);
	if (flags & LIEXT_SOUND_FLAG_REPEAT)
		lisnd_source_set_looping (source, 1);
	lisnd_source_set_playing (source, 1);

	return source;
}

int liext_sound_set_music (
	LIExtModule* self,
	const char*  value)
{
	LISndSource* music;
	LISndSample* sample;

	/* Find sample. */
	if (self->sound == NULL)
		return 1;
	sample = liext_sound_find_sample (self, value);
	if (sample == NULL)
		return 0;

	/* Fade in a new music track. */
	music = lisnd_source_new (self->system, 1);
	if (music == NULL)
		return 0;
	lisnd_source_queue_sample (music, sample);
	lisnd_source_set_fading (music, 0.0f, 1.0f / self->music_fading);
	lisnd_source_set_volume (music, self->music_volume);
	lisnd_source_set_looping (music, self->music_looping);
	lisnd_source_set_playing (music, 1);

	/* Fade out the old music track. */
	if (self->music_fade != NULL)
		lisnd_source_free (self->music_fade);
	if (self->music != NULL)
		lisnd_source_set_fading (self->music, 1.0f, -1.0f / self->music_fading);
	self->music_fade = self->music;
	self->music = music;

	return 1;
}

void liext_sound_set_music_fading (
	LIExtModule* self,
	float        value)
{
	self->music_fading = value;
}

void liext_sound_set_music_looping (
	LIExtModule* self,
	int          value)
{
	if (self->music_looping != value)
	{
		self->music_looping = value;
		if (self->music != NULL)
			lisnd_source_set_looping (self->music, self->music_looping);
	}
}

void liext_sound_set_music_volume (
	LIExtModule* self,
	float        value)
{
	if (self->sound == NULL)
		return;
	if (value < 0.0f)
		value = 0.0f;
	if (value > 1.0f)
		value = 1.0f;
	self->music_volume = value;

	if (self->music != NULL)
		lisnd_source_set_volume (self->music, value);
	if (self->music_fade != NULL)
		lisnd_source_set_volume (self->music_fade, value);
}
#endif

/*****************************************************************************/

#ifndef LI_DISABLE_SOUND
LIExtObject* liext_object_new ()
{
	return lisys_calloc (1, sizeof (LIExtObject));
}

void liext_object_free (
	LIExtObject* self)
{
	LIAlgList* ptr;

	for (ptr = self->sounds ; ptr != NULL ; ptr = ptr->next)
		lisnd_source_free (ptr->data);
	lialg_list_free (self->sounds);
	lisys_free (self);
}

int liext_object_update (
	LIExtObject* self,
	LIEngObject* object,
	LIExtModule* module,
	float        secs)
{
	LIAlgList* ptr;
	LIAlgList* next;
	LIMatTransform transform;
	LIMatVector vector;
	LIPhyObject* phyobj;
	LIPhyPhysics* physics;
	LISndSource* source;

	/* Find the optional physics manager. */
	physics = limai_program_find_component (module->program, "physics");

	/* Update each sound source. */
	for (ptr = self->sounds ; ptr != NULL ; ptr = next)
	{
		next = ptr->next;
		source = ptr->data;
		if (source->stereo)
		{
			lisnd_source_set_position (source, &module->listener_position);
		}
		else if (object != NULL)
		{
			lieng_object_get_transform (object, &transform);
			lisnd_source_set_position (source, &transform.position);
		}
		if (physics != NULL && object != NULL)
		{
			phyobj = liphy_physics_find_object (physics, object->id);
			if (phyobj != NULL)
			{
				liphy_object_get_velocity (phyobj, &vector);
				lisnd_source_set_velocity (source, &vector);
			}
		}
		if (!lisnd_source_update (source, secs))
		{
			lisnd_source_free (source);
			lialg_list_remove (&self->sounds, ptr);
		}
	}

	return self->sounds != NULL;
}
#endif

/*****************************************************************************/

#ifndef LI_DISABLE_SOUND
static int private_tick (
	LIExtModule* self,
	float        secs)
{
	LIAlgU32dicIter iter;
	LIEngObject* engobj;
	LIExtObject* extobj;
	LIMatVector direction;
	LIMatVector velocity;
	LIMatVector up;

	/* Update listener position. */
	velocity = self->listener_velocity;
	direction = limat_quaternion_get_basis (self->listener_rotation, 2);
	up = limat_quaternion_get_basis (self->listener_rotation, 1);
	lisnd_system_set_listener (self->system, &self->listener_position, &velocity, &direction, &up);

	/* Update music. */
	if (self->music_fade != NULL)
	{
		if (!lisnd_source_update (self->music_fade, secs))
		{
			limai_program_event (self->program, "music-fade-ended", NULL);
			lisnd_source_free (self->music_fade);
			self->music_fade = NULL;
		}
	}
	if (self->music != NULL)
	{
		lisnd_source_set_position (self->music, &self->listener_position);
		if (!lisnd_source_update (self->music, secs))
		{
			limai_program_event (self->program, "music-ended", NULL);
			lisnd_source_free (self->music);
			self->music = NULL;
		}
	}

	/* Update sound effects. */
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		extobj = iter.value;
		engobj = lieng_engine_find_object (self->program->engine, iter.key);
		if (!liext_object_update (extobj, engobj, self, secs))
		{
			lialg_u32dic_remove (self->objects, iter.key);
			liext_object_free (extobj);
		}
	}

	return 1;
}
#endif

/** @} */
/** @} */
