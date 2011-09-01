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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtSpeech Speech
 * @{
 */

#include "ext-module.h"
#include "ext-speech.h"

LIExtSpeech* liext_speech_new (
	LIExtModule* module,
	const char*  font,
	const char*  text)
{
	LIExtSpeech* self;
	LIFntFont* font1;

	self = lisys_malloc (sizeof (LIExtSpeech));
	if (self == NULL)
		return NULL;
	self->diffuse[0] = 1.0f;
	self->diffuse[1] = 1.0f;
	self->diffuse[2] = 1.0f;
	self->diffuse[3] = 1.0f;
	self->timer = 0.0f;
	self->fade_exponent = 4;
	self->fade_time = 5;
	self->life_time = 10;
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	lifnt_layout_set_width_limit (self->text, 150);
	font1 = liwdg_manager_find_font (module->widgets, font);
	if (font1 != NULL)
		lifnt_layout_append_string (self->text, font1, text);

	return self;
}

void
liext_speech_free (LIExtSpeech* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self);
}

/** @} */
/** @} */
