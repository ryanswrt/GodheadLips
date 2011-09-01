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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/font.h>
#include <lipsofsuna/extension.h>

#define LIEXT_SCRIPT_SPEECH "Speech"

typedef struct _LIExtSpeech LIExtSpeech;

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LIAlgU32dic* objects;
	LICalHandle calls[1];
	LICliClient* client;
	LIMaiProgram* program;
	LIWdgManager* widgets;
};

LIExtModule* liext_speeches_new (
	LIMaiProgram* program);

void liext_speeches_free (
	LIExtModule* self);

void liext_speeches_render (
	LIExtModule*       self,
	const LIMatMatrix* projection,
	const LIMatMatrix* modelview,
	const int*         viewport);

LIExtSpeech* liext_speeches_set_speech (
	LIExtModule* self,
	uint32_t     object,
	const float* diffuse,
	const char*  font,
	const char*  message);

/*****************************************************************************/

typedef struct _LIExtObject LIExtObject;
struct _LIExtObject
{
	LIAlgList* speech;
	LIMatVector position;
};

LIExtObject* liext_speech_object_new ();

void liext_speech_object_free (
	LIExtObject* self);

/*****************************************************************************/

void liext_script_speech (
	LIScrScript* self);

#endif
