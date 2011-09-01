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

/**
 * \addtogroup LIMai Main
 * @{
 * \addtogroup LIMaiProgram Program
 * @{
 */

#include "lipsofsuna/voxel.h"
#include "main-extension.h"
#include "main-program.h"

static int private_init (
	LIMaiProgram* self,
	const char*   path,
	const char*   name,
	const char*   args);

static int private_object_motion (
	LIMaiProgram* self,
	LIEngObject*  object);

static int private_sector_free (
	LIMaiProgram* self,
	int           sector);

static int private_sector_load (
	LIMaiProgram* self,
	int           sector);

static int private_tick (
	LIMaiProgram* self,
	float         secs);

/****************************************************************************/

/**
 * \brief Creates a new program instance.
 *
 * \param path Root data directory.
 * \param name Module name.
 * \param args Module arguments or NULL.
 * \return New program or NULL.
 */
LIMaiProgram* limai_program_new (
	const char* path,
	const char* name,
	const char* args)
{
	LIMaiProgram* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMaiProgram));
	if (self == NULL)
		return NULL;
	self->args = lisys_string_dup ((args != NULL)? args : "");
	if (self->args == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize subsystems. */
	if (!private_init (self, path, name, args))
	{
		limai_program_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the program.
 *
 * \param self Program.
 */
void limai_program_free (
	LIMaiProgram* self)
{
	int i;
	LIMaiExtension* extension;
	LIMaiExtension* extension_next;
	LIMaiMessage* message;
	LIMaiMessage* message_next;

	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, "program-shutdown", lical_marshal_DATA);

	/* Clear all sector data. */
	if (self->sectors != NULL)
		lialg_sectors_clear (self->sectors);

	/* Free script. */
	if (self->script != NULL)
		liscr_script_free (self->script);

	/* Free extensions. */
	if (self->extensions != NULL)
	{
		for (extension = self->extensions ; extension != NULL ; extension = extension_next)
		{
			extension_next = extension->next;
			((void (*)(void*)) extension->info->free) (extension->object);
			if (extension->module != NULL)
				lisys_module_free (extension->module);
			lisys_free (extension);
		}
	}

	/* Free components. */
	if (self->components != NULL)
	{
		lisys_assert (self->components->size == 0);
		lialg_strdic_free (self->components);
	}

	/* Free engine. */
	if (self->engine != NULL)
		lieng_engine_free (self->engine);

	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, "program-free", lical_marshal_DATA);

	/* Free callbacks. */
	if (self->callbacks != NULL)
	{
		lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
		lical_callbacks_free (self->callbacks);
	}

	if (self->sectors != NULL)
		lialg_sectors_free (self->sectors);
	if (self->paths != NULL)
		lipth_paths_free (self->paths);

	/* Free messaging. */
	if (self->message_mutex != NULL)
		lisys_mutex_free (self->message_mutex);
	for (i = 0 ; i < LIMAI_MESSAGE_QUEUE_MAX ; i++)
	{
		for (message = self->messages[i] ; message != NULL ; message = message_next)
		{
			message_next = message->next;
			limai_message_free (message);
		}
	}

	lisys_free (self->launch_args);
	lisys_free (self->launch_name);
	lisys_free (self->args);
	lisys_free (self);
}

/**
 * \brief Emits an event.
 * \param self Program.
 * \param type Event type.
 * \param ... List of name,type,value triplets terminated by NULL.
 */
void limai_program_event (
	LIMaiProgram* self,
	const char*   type,
	              ...)
{
	va_list args;

	va_start (args, type);
	limai_program_eventva (self, type, args);
	va_end (args);
}

/**
 * \brief Emits an event.
 * \param self Program.
 * \param type Event type.
 * \param args Variable argument list.
 */
void limai_program_eventva (
	LIMaiProgram* self,
	const char*   type,
	va_list       args)
{
	int pint;
	void* pptr;
	char* pstr;
	float pfloat;
	const char* type_;
	const char* name;
	LIScrData* data;
	lua_State* lua = liscr_script_get_lua (self->script);

	/* Get the event queue. */
	lua_getglobal (lua, "__events");
	if (lua_type (lua, -1) != LUA_TTABLE)
	{
		lua_pop (lua, 1);
		lua_newtable (lua);
		lua_pushvalue (lua, -1);
		lua_setglobal (lua, "__events");
	}
	lua_pushnumber (lua, lua_objlen (lua, -1) + 1);

	/* Create the event. */
	lua_newtable (lua);
	lua_pushstring (lua, type);
	lua_setfield (lua, -2, "type");
	while (1)
	{
		/* Get name. */
		name = va_arg (args, char*);
		if (name == NULL)
			break;

		/* Duplicated from LIScrData due to the behavior of varargs
		   being undefined when passed to a function and then reused. */
		type_ = va_arg (args, char*);
		if (type_ == LISCR_TYPE_BOOLEAN)
		{
			pint = va_arg (args, int);
			lua_pushboolean (lua, pint);
		}
		else if (type_ == LISCR_TYPE_FLOAT)
		{
			pfloat = va_arg (args, double);
			lua_pushnumber (lua, pfloat);
		}
		else if (type_ == LISCR_TYPE_INT)
		{
			pint = va_arg (args, int);
			lua_pushnumber (lua, pint);
		}
		else if (type_ == LISCR_TYPE_STRING)
		{
			pstr = va_arg (args, char*);
			lua_pushstring (lua, pstr);
		}
		else if (!strcmp (type_, LISCR_SCRIPT_PACKET))
		{
			pptr = va_arg (args, void*);
			data = liscr_data_new (self->script, lua, pptr, LISCR_SCRIPT_PACKET, liarc_packet_free);
		}
		else if (!strcmp (type_, LISCR_SCRIPT_VECTOR))
		{
			data = liscr_data_new_alloc (self->script, lua, sizeof (LIMatVector), LISCR_SCRIPT_VECTOR);
			if (data == NULL)
				break;
			pptr = va_arg (args, void*);
			*((LIMatVector*) liscr_data_get_data (data)) = *((LIMatVector*) pptr);
		}
		else
		{
			pptr = va_arg (args, void*);
			if (pptr == NULL)
				break;
			liscr_pushdata (lua, pptr);
		}

		/* Set field. */
		lua_setfield (lua, -2, name);
	}

	/* Push to event queue. */
	lua_settable (lua, -3);
	lua_pop (lua, 1);
}

/**
 * \brief Executes a script file.
 * \param self Program.
 * \param file Filename.
 * \return Nonzero on success.
 */
int limai_program_execute_script (
	LIMaiProgram* self,
	const char*   file)
{
	int ret;
	char* path;
	char* path_mod;
	char* path_core;

	/* Get paths. */
	path = lipth_paths_get_script (self->paths, file);
	path_mod = lisys_path_concat (self->paths->module_data, "scripts", NULL);
	path_core = lisys_path_concat (self->paths->global_data, NULL);
	if (path == NULL || path_mod == NULL || path_core == NULL)
	{
		lisys_free (path);
		lisys_free (path_mod);
		lisys_free (path_core);
		return 0;
	}

	/* Load and execute the script. */
	ret = liscr_script_load_file (self->script, path, path_mod, path_core);
	lisys_free (path);
	lisys_free (path_mod);
	lisys_free (path_core);
	if (!ret)
		return 0;

	return 1;
}

/**
 * \brief Executes a script string.
 * \param self Program.
 * \param code Code string.
 * \return Nonzero on success.
 */
int limai_program_execute_string (
	LIMaiProgram* self,
	const char*   code)
{
	int ret;
	char* path_mod;
	char* path_core;

	/* Get paths. */
	path_mod = lisys_path_concat (self->paths->module_data, "scripts", NULL);
	path_core = lisys_path_concat (self->paths->global_data, NULL);
	if (path_mod == NULL || path_core == NULL)
	{
		lisys_free (path_mod);
		lisys_free (path_core);
		return 0;
	}

	/* Execute the script. */
	ret = liscr_script_load_string (self->script, code, path_mod, path_core);
	lisys_free (path_mod);
	lisys_free (path_core);
	if (!ret)
		return 0;

	return 1;
}

/**
 * \brief Finds a component by name.
 *
 * \param self Program.
 * \param name Component name.
 * \return Component or NULL.
 */
void* limai_program_find_component (
	LIMaiProgram* self,
	const char*   name)
{
	return lialg_strdic_find (self->components, name);
}

/**
 * \brief Finds an extension by name.
 *
 * \param self Program.
 * \param name Extension name.
 * \return Extension or NULL.
 */
LIMaiExtension* limai_program_find_extension (
	LIMaiProgram* self,
	const char*   name)
{
	LIMaiExtension* extension;

	for (extension = self->extensions ; extension != NULL ; extension = extension->next)
	{
		if (!strcmp (extension->name, name))
			return extension;
	}

	return NULL;
}

/**
 * \brief Loads an extension.
 * \param self Program.
 * \param name Extensions name.
 * \return Nonzero on success.
 */
int limai_program_insert_extension (
	LIMaiProgram* self,
	const char*   name)
{
	char* ptr;
	char* path;
	char* ident;
	LISysModule* module = NULL;
	LIMaiExtension* extension;
	LIMaiExtensionInfo* info;

	/* Check if already loaded. */
	extension = limai_program_find_extension (self, name);
	if (extension != NULL)
		return 1;

	/* Determine extension info struct name. We restrict the
	 * name of the extension to [a-z_] for security reasons. */
	ident = lisys_string_format ("liext_%s_info", name);
	if (ident == NULL)
		return 0;
	for (ptr = ident ; *ptr != '\0' ; ptr++)
	{
		if (*ptr < 'a' || *ptr > 'z')
			*ptr = '_';
	}

	/* Open an external library if the extension isn't built-in. */
	info = limai_extension_get_builtin (name);
	if (info == NULL)
	{
		path = lisys_path_format (self->paths->global_exts, LISYS_PATH_SEPARATOR,
			"lib", name, ".", LISYS_EXTENSION_DLL, NULL);
		if (path == NULL)
		{
			lisys_free (ident);
			return 0;
		}
		module = lisys_module_new (path, 0);
		lisys_free (path);
		if (module == NULL)
		{
			path = lisys_path_format (self->paths->global_exts, LISYS_PATH_SEPARATOR,
				name, ".", LISYS_EXTENSION_DLL, NULL);
			if (path == NULL)
			{
				lisys_free (ident);
				return 0;
			}
			module = lisys_module_new (path, 0);
			lisys_free (path);
			if (module == NULL)
			{
				lisys_free (ident);
				goto error;
			}
		}
		info = lisys_module_symbol (module, ident);
	}
	lisys_free (ident);

	/* Check for valid module info. */
	if (info == NULL)
	{
		lisys_error_set (EINVAL, "no module info");
		goto error;
	}
	if (info->version != LIMAI_EXTENSION_VERSION)
	{
		lisys_error_set (EINVAL, "invalid module version");
		goto error;
	}
	if (info->name == NULL || info->init == NULL || info->free == NULL)
	{
		lisys_error_set (EINVAL, "invalid module format");
		goto error;
	}

	/* Allocate extension. */
	extension = lisys_calloc (1, sizeof (LIMaiExtension));
	if (extension == NULL)
		goto error;
	strncpy (extension->name, name, sizeof (extension->name) - 1);
	extension->info = info;
	extension->module = module;

	/* Call module initializer. */
	extension->object = ((void* (*)(LIMaiProgram*)) info->init)(self);
	if (extension->object == NULL)
	{
		lisys_free (extension);
		goto error;
	}

	/* Insert to extension list. */
	/* We prepend to the beginning of the list so that the extensions will be
	   freed in the reverse order. This allows extensions to be cleaned up
	   correctly when they have non-circular dependencies. */
	extension->next = self->extensions;
	self->extensions = extension;

	return 1;

error:
	lisys_error_append ("cannot initialize module `%s'", name);
	if (module != NULL)
		lisys_module_free (module);
	return 0;
}

/**
 * \brief Registers a component.
 *
 * \param self Program.
 * \param name Component name.
 * \param value Component.
 * \return Nonzero on success.
 */
int limai_program_insert_component (
	LIMaiProgram* self,
	const char*   name,
	void*         value)
{
	return lialg_strdic_insert (self->components, name, value) != NULL;
}

/**
 * \brief Pops a message from the message queue.
 *
 * This function is thread safe. It's specifically intended for thread safe
 * communication between scripted programs running in different threads.
 *
 * \param self Program.
 * \param queue Message queue.
 * \return Message or NULL.
 */
LIMaiMessage* limai_program_pop_message (
	LIMaiProgram* self,
	int           queue)
{
	LIMaiMessage* message;

	/* Pop a message from the queue. */
	lisys_mutex_lock (self->message_mutex);
	message = self->messages[queue];
	if (message != NULL)
	{
		if (message->next != NULL)
			message->next->prev = NULL;
		self->messages[queue] = message->next;
	}
	lisys_mutex_unlock (self->message_mutex);

	return message;
}

/**
 * \brief Pushes a message to the message queue.
 *
 * This function is thread safe. It's specifically intended for thread safe
 * communication between scripted programs running in different threads.
 *
 * \param queue Message queue.
 * \param type Message type.
 * \param name Message name.
 * \param data Message data to be soft copied.
 * \return Nonzero on success.
 */
int limai_program_push_message (
	LIMaiProgram* self,
	int           queue,
	int           type,
	const char*   name,
	const void*   data)
{
	LIMaiMessage* ptr;
	LIMaiMessage* message;

	/* Create the message. */
	message = limai_message_new (type, name, data);
	if (message == NULL)
		return 0;

	/* Append it to the message queue. */
	lisys_mutex_lock (self->message_mutex);
	if (self->messages[queue] != NULL)
	{
		for (ptr = self->messages[queue] ; ptr->next != NULL ; ptr = ptr->next)
			{}
		ptr->next = message;
		message->prev = ptr;
	}
	else
		self->messages[queue] = message;
	lisys_mutex_unlock (self->message_mutex);

	return 1;
}

/**
 * \brief Unregisters a component.
 *
 * \param self Program.
 * \param name Component name.
 */
void limai_program_remove_component (
	LIMaiProgram* self,
	const char*   name)
{
	lialg_strdic_remove (self->components, name);
}

/**
 * \brief Tells the server to shut down.
 *
 * \note Thread safe.
 * \param self Program.
 */
void limai_program_shutdown (
	LIMaiProgram* self)
{
	self->quit = 1;
}

/**
 * \brief Executes unit tests for the whole engine.
 * \param self Program.
 */
void limai_program_unittest (
	LIMaiProgram* self)
{
	limat_math_unittest ();
	livox_unittest ();
}

/**
 * \brief Updates the program state.
 * \param self Program.
 * \return Nonzero if the program is still running.
 */
int limai_program_update (
	LIMaiProgram* self)
{
	int i;
	float secs;

	/* Calculate time delta. */
	gettimeofday (&self->curr_tick, NULL);
	secs = self->curr_tick.tv_sec - self->prev_tick.tv_sec +
	      (self->curr_tick.tv_usec - self->prev_tick.tv_usec) * 0.000001f;
	self->prev_tick = self->curr_tick;

	/* Frames per second. */
	self->ticks[self->ticki++] = secs;
	if (self->ticki == LIMAI_PROGRAM_FPS_TICKS)
		self->ticki = 0;
	self->tick = 0.0f;
	for (i = 0 ; i < LIMAI_PROGRAM_FPS_TICKS ; i++)
		self->tick += self->ticks[i];
	self->fps = LIMAI_PROGRAM_FPS_TICKS / self->tick;
	self->tick = self->tick / LIMAI_PROGRAM_FPS_TICKS;

	/* Update subsystems. */
	/* Garbage collection is disabled so that we don't need to worry about
	   objects getting collected while the physics simulation or something
	   else is working on them and generating events. */
	liscr_script_set_gc (self->script, 0);
	lialg_sectors_update (self->sectors, secs);
	liscr_script_update (self->script, secs);
	lieng_engine_update (self->engine, secs);
	lical_callbacks_call (self->callbacks, "tick", lical_marshal_DATA_FLT, secs);
	liscr_script_set_gc (self->script, 1);

	/* Sleep until end of frame. */
	if (self->sleep > (int)(1000000 * secs))
		lisys_usleep (self->sleep - (int)(1000000 * secs));

	return !self->quit;
}

/**
 * \brief Gets time since program startup.
 *
 * \param self Program.
 * \return Time in seconds.
 */
double limai_program_get_time (
	const LIMaiProgram* self)
{
	struct timeval t;

	gettimeofday (&t, NULL);
	t.tv_sec -= self->start.tv_sec;
	t.tv_usec -= self->start.tv_usec;
	if (t.tv_usec < 0)
	{
		t.tv_sec -= 1;
		t.tv_usec += 1000000;
	}

	return (double) t.tv_sec + (double) t.tv_usec * 0.000001;
}

/****************************************************************************/

static int private_init (
	LIMaiProgram* self,
	const char*   path,
	const char*   name,
	const char*   args)
{
	/* Initialize paths. */
	self->paths = lipth_paths_new (path, name);
	if (self->paths == NULL)
		return 0;

	/* Initialize dictionaries. */
	self->components = lialg_strdic_new ();
	if (self->components == NULL)
		return 0;

	/* Initialize callbacks. */
	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
		return 0;

	/* Initialize sectors. */
#warning Hardcoded sector size
	self->sectors = lialg_sectors_new (128, 16.0f);
	if (self->sectors == NULL)
		return 0;

	/* Create engine. */
	self->engine = lieng_engine_new (self->callbacks, self->sectors, self->paths);
	if (self->engine == NULL)
		return 0;

	/* Allocate the script. */
	self->script = liscr_script_new ();
	if (self->script == NULL)
		return 0;

	/* Initialize timer. */
	gettimeofday (&self->start, NULL);
	self->prev_tick = self->start;
	self->curr_tick = self->start;

	/* Initialize messaging. */
	self->message_mutex = lisys_mutex_new ();
	if (self->message_mutex == NULL)
		return 0;

	/* Register classes. */
	liscr_script_set_userdata (self->script, LISCR_SCRIPT_PROGRAM, self);
	liscr_script_model (self->script);
	liscr_script_packet (self->script);
	liscr_script_program (self->script);
	liscr_script_object (self->script);
	liscr_script_quaternion (self->script);
	liscr_script_vector (self->script);

	/* Register callbacks. */
	if (!lical_callbacks_insert (self->callbacks, "object-motion", 63353, private_object_motion, self, self->calls + 0) ||
	    !lical_callbacks_insert (self->callbacks, "sector-free", 65535, private_sector_free, self, self->calls + 1) ||
	    !lical_callbacks_insert (self->callbacks, "sector-load", 65535, private_sector_load, self, self->calls + 2) ||
	    !lical_callbacks_insert (self->callbacks, "tick", 2, private_tick, self, self->calls + 3))
		return 0;

	lialg_random_init (&self->random, lisys_time (NULL));

	return 1;
}

static int private_object_motion (
	LIMaiProgram* self,
	LIEngObject*  object)
{
	float diff_pos;
	float diff_rot;
	const float POSITION_THRESHOLD = 0.05;
	const float ROTATION_THRESHOLD = 0.05;

	if (object->script != NULL)
	{
		/* Don't emit events for ridiculously small changes. */
		diff_pos = LIMAT_ABS (object->transform.position.x - object->transform_event.position.x) +
		           LIMAT_ABS (object->transform.position.y - object->transform_event.position.y) +
		           LIMAT_ABS (object->transform.position.z - object->transform_event.position.z);
		diff_rot = LIMAT_ABS (object->transform.rotation.x - object->transform_event.rotation.x) +
		           LIMAT_ABS (object->transform.rotation.y - object->transform_event.rotation.y) +
		           LIMAT_ABS (object->transform.rotation.z - object->transform_event.rotation.z) +
		           LIMAT_ABS (object->transform.rotation.w - object->transform_event.rotation.w);
		if (diff_pos < POSITION_THRESHOLD && diff_rot < ROTATION_THRESHOLD)
			return 1;
		object->transform_event = object->transform;

		/* Emit an object-motion event. */
		limai_program_event (self, "object-motion", "object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	}

	return 1;
}

static int private_sector_free (
	LIMaiProgram* self,
	int           sector)
{
	limai_program_event (self, "sector-free", "sector", LISCR_TYPE_INT, sector, NULL);

	return 1;
}

static int private_sector_load (
	LIMaiProgram* self,
	int           sector)
{
	limai_program_event (self, "sector-load", "sector", LISCR_TYPE_INT, sector, NULL);

	return 1;
}

static int private_tick (
	LIMaiProgram* self,
	float         secs)
{
	limai_program_event (self, "tick", "secs", LISCR_TYPE_FLOAT, secs, NULL);

	return 1;
}

/** @} */
/** @} */
