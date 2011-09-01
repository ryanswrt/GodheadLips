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

#ifndef __MODEL_ANIMATION_H__
#define __MODEL_ANIMATION_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>

typedef struct _LIMdlFrame LIMdlFrame;
struct _LIMdlFrame
{
	float scale;
	LIMatTransform transform;
};

typedef struct _LIMdlAnimation LIMdlAnimation;
struct _LIMdlAnimation
{
	int length;
	char* name;
	float blendin; /* TODO: Not used anymore, remove. */
	float blendout; /* TODO: Not used anymore, remove. */
	struct
	{
		int count;
		LIMdlFrame* array;
	} buffer;
	struct
	{
		int count;
		char** array;
	} channels;
};

LIAPICALL (int, limdl_animation_init_copy, (
	LIMdlAnimation* self,
	LIMdlAnimation* anim));

LIAPICALL (LIMdlAnimation*, limdl_animation_new_copy, (
	LIMdlAnimation* anim));

LIAPICALL (void, limdl_animation_free, (
	LIMdlAnimation* self));

LIAPICALL (int, limdl_animation_insert_channel, (
	LIMdlAnimation* self,
	const char*     name));

LIAPICALL (void, limdl_animation_clear, (
	LIMdlAnimation* self));

LIAPICALL (int, limdl_animation_read, (
	LIMdlAnimation* self,
	LIArcReader*    reader));

LIAPICALL (int, limdl_animation_get_channel, (
	LIMdlAnimation* self,
	const char*     name));

LIAPICALL (float, limdl_animation_get_duration, (
	const LIMdlAnimation* self));

LIAPICALL (int, limdl_animation_set_length, (
	LIMdlAnimation* self,
	int             value));

LIAPICALL (int, limdl_animation_set_transform, (
	LIMdlAnimation*       self,
	const char*           name,
	int                   frame,
	float                 scale,
	const LIMatTransform* value));

LIAPICALL (int, limdl_animation_get_transform, (
	LIMdlAnimation* self,
	const char*     name,
	float           secs,
	float*          scale,
	LIMatTransform* value));

#endif
