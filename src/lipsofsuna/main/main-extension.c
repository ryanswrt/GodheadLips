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
 * \addtogroup LIMai Main
 * @{
 * \addtogroup LIMaiExtension Extension
 * @{
 */

#include <string.h>
#include "main-extension.h"

extern LIMaiExtensionInfo liext_ai_info;
extern LIMaiExtensionInfo liext_animation_info;
extern LIMaiExtensionInfo liext_camera_info;
extern LIMaiExtensionInfo liext_config_file_info;
extern LIMaiExtensionInfo liext_database_info;
extern LIMaiExtensionInfo liext_file_info;
extern LIMaiExtensionInfo liext_graphics_info;
extern LIMaiExtensionInfo liext_lobby_info;
extern LIMaiExtensionInfo liext_model_editing_info;
extern LIMaiExtensionInfo liext_network_info;
extern LIMaiExtensionInfo liext_noise_info;
extern LIMaiExtensionInfo liext_object_physics_info;
extern LIMaiExtensionInfo liext_object_render_info;
extern LIMaiExtensionInfo liext_password_info;
extern LIMaiExtensionInfo liext_physics_info;
extern LIMaiExtensionInfo liext_reload_info;
extern LIMaiExtensionInfo liext_render_info;
extern LIMaiExtensionInfo liext_skeleton_info;
extern LIMaiExtensionInfo liext_speech_info;
extern LIMaiExtensionInfo liext_sound_info;
extern LIMaiExtensionInfo liext_thread_info;
extern LIMaiExtensionInfo liext_tiles_info;
extern LIMaiExtensionInfo liext_tiles_physics_info;
extern LIMaiExtensionInfo liext_tiles_render_info;
extern LIMaiExtensionInfo liext_vision_info;
extern LIMaiExtensionInfo liext_watchdog_info;
extern LIMaiExtensionInfo liext_widgets_info;
extern LIMaiExtensionInfo liext_heightmap_info;

/**
 * \brief Gets a built-in extension by name.
 * \param name Mangled extension name.
 */
LIMaiExtensionInfo* limai_extension_get_builtin (
	const char* name)
{
	int i;
	static const struct
	{
		const char* name;
		LIMaiExtensionInfo* info;
	}
	info[] =
	{
		{ "ai", &liext_ai_info },
		{ "animation", &liext_animation_info },
		{ "heightmap", &liext_heightmap_info },
		{ "camera", &liext_camera_info },
		{ "config-file", &liext_config_file_info },
		{ "database", &liext_database_info },
		{ "file", &liext_file_info },
		{ "graphics", &liext_graphics_info },
		{ "lobby", &liext_lobby_info },
		{ "model-editing", &liext_model_editing_info },
		{ "network", &liext_network_info },
		{ "noise", &liext_noise_info },
		{ "object-physics", &liext_object_physics_info },
		{ "object-render", &liext_object_render_info },
		{ "password", &liext_password_info },
		{ "physics", &liext_physics_info },
		{ "reload", &liext_reload_info },
		{ "render", &liext_render_info },
		{ "skeleton", &liext_skeleton_info },
		{ "speech", &liext_speech_info },
		{ "sound", &liext_sound_info },
		{ "thread", &liext_thread_info },
		{ "tiles", &liext_tiles_info },
		{ "tiles-physics", &liext_tiles_physics_info },
		{ "tiles-render", &liext_tiles_render_info },
		{ "vision", &liext_vision_info },
		{ "watchdog", &liext_watchdog_info },
		{ "widgets", &liext_widgets_info }
	};

	for (i = 0 ; i < sizeof (info) / sizeof (*info) ; i++)
	{
		if (!strcmp (info[i].name, name))
			return info[i].info;
	}

	return NULL;
}

/** @} */
/** @} */
