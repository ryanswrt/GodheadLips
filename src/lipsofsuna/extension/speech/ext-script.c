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

static void Speech_add (LIScrArgs* args)
{
	int id;
	float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* msg;
	const char* font = "default";
	LIScrData* object;
	LIExtModule* module;
	LIExtSpeech* speech;

	if (!liscr_args_gets_string (args, "message", &msg))
		return;
	liscr_args_gets_floatv (args, "diffuse", 4, diffuse);
	liscr_args_gets_string (args, "font", &font);
	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &object))
		id = ((LIEngObject*) liscr_data_get_data (object))->id;
	else
		return;
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SPEECH);
	speech = liext_speeches_set_speech (module, id, diffuse, font, msg);
	if (speech != NULL)
	{
		if (liscr_args_gets_float (args, "fade_exponent", &speech->fade_exponent))
			speech->fade_exponent = LIMAT_MAX (1.0f, speech->fade_exponent);
		if (liscr_args_gets_float (args, "fade_time", &speech->fade_time))
			speech->fade_time = LIMAT_MAX (0.0f, speech->fade_time);
		if (liscr_args_gets_float (args, "life_time", &speech->life_time))
			speech->life_time = LIMAT_MAX (0.0f, speech->life_time);
	}
}

static void Speech_draw (LIScrArgs* args)
{
	GLint viewport[4];
	LIExtModule* module;
	LIMatMatrix modelview;
	LIMatMatrix projection;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SPEECH);

	/* Get arguments. */
	modelview = limat_matrix_identity ();
	projection = limat_matrix_identity ();
	liscr_args_gets_floatv (args, "modelview", 16, modelview.m);
	liscr_args_gets_floatv (args, "projection", 16, projection.m);
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = module->client->window->mode.width;
	viewport[3] = module->client->window->mode.height;
	liscr_args_gets_intv (args, "viewport", 4, viewport);
	viewport[0] = LIMAT_MAX (0, viewport[0]);
	viewport[1] = LIMAT_MAX (0, viewport[1]);
	viewport[1] = module->client->window->mode.height - viewport[1] - viewport[3];
	viewport[2] = LIMAT_MAX (2, viewport[2]);
	viewport[3] = LIMAT_MAX (2, viewport[3]);

	/* Render all speech. */
	liext_speeches_render (module, &projection, &modelview, viewport);
}

/*****************************************************************************/

void liext_script_speech (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SPEECH, "speech_add", Speech_add);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SPEECH, "speech_draw", Speech_draw);
}

/** @} */
/** @} */
