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

#ifndef __EXT_VISION_H__
#define __EXT_VISION_H__

#include "lipsofsuna/extension.h"
#include "ext-module.h"

typedef struct _LIExtVisionListener LIExtVisionListener;
struct _LIExtVisionListener
{
	float cone_angle;
	float cone_cosine;
	float cone_cutoff;
	float cone_factor;
	float scan_radius;
	float keep_threshold;
	LIAlgU32dic* objects;
	LIAlgU32dic* terrain;
	LIExtModule* module;
	LIMatVector position;
	LIMatVector direction;
	LIScrData* script;
};

LIExtVisionListener* liext_vision_listener_new (
	LIExtModule* module);

void liext_vision_listener_free (
	LIExtVisionListener* self);

void liext_vision_listener_clear (
	LIExtVisionListener* self);

void liext_vision_listener_update (
	LIExtVisionListener* self,
	lua_State*           lua);

#endif
