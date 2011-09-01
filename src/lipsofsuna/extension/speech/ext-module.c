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

static int private_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_speech_info =
{
	LIMAI_EXTENSION_VERSION, "Speech",
	liext_speeches_new,
	liext_speeches_free
};

LIExtModule* liext_speeches_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Make sure the widgets extension is loaded. */
	if (!limai_program_insert_extension (program, "widgets"))
	{
		lisys_free (self);
		return NULL;
	}
	self->client = limai_program_find_component (program, "client");
	self->widgets = limai_program_find_component (program, "widgets");

	/* Allocate objects. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "tick", 1, private_tick, self, self->calls + 0))
	{
		liext_speeches_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_SPEECH, self);
	liext_script_speech (program->script);

	return self;
}

void liext_speeches_free (
	LIExtModule* self)
{
	LIAlgU32dicIter iter;

	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Free objects. */
	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
			liext_speech_object_free (iter.value);
		lialg_u32dic_free (self->objects);
	}

	lisys_free (self);
}

void liext_speeches_render (
	LIExtModule*       self,
	const LIMatMatrix* projection,
	const LIMatMatrix* modelview,
	const int*         viewport)
{
	int i;
	int x;
	int y;
	int w;
	int h;
	int width;
	float* vertex_data;
	LIRenIndex* index_data;
	LIAlgU32dicIter iter;
	LIAlgList* ptr;
	LIExtObject* object;
	LIExtSpeech* speech;
	LIFntLayout* text;
	LIFntLayoutGlyph* glyph;
	LIMatMatrix matrix;
	LIMatVector win;
	LIRenShader* shader;
	LIRenRender* render = self->client->render;

	shader = liren_render_find_shader (render, "widget");
	if (shader == NULL)
		return;
	matrix = limat_matrix_ortho (0, viewport[2], 0, viewport[3], -100.0f, 100.0f);
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		object = iter.value;

		/* Project start offset. */
		if (!limat_matrix_project (*projection, *modelview, viewport, &object->position, &win))
			continue;
		if (win.z < 0.0f)
			continue;
		win.y -= 5;

		/* Render all messages. */
		for (ptr = object->speech ; ptr != NULL ; ptr = ptr->next)
		{
			speech = ptr->data;
			text = speech->text;
			win.y += lifnt_layout_get_height (text);
			width = lifnt_layout_get_width (text) / 2;

			/* Get vertex data. */
			if (!text->n_glyphs)
				continue;
			if (!lifnt_layout_get_vertices (text, &index_data, &vertex_data))
				continue;

			/* Apply translation and scaling. */
			w = lifnt_layout_get_width (text);
			h = lifnt_layout_get_height (text);
			x = win.x - width;
			y = win.y;
			for (i = 2 ; i < text->n_glyphs * 20 ; i += 5)
			{
				glyph = text->glyphs + i;
				vertex_data[i + 0] += x;
				vertex_data[i + 1] *= -1.0f;
				vertex_data[i + 1] += y;
			}

			/* Render glyphs. */
			for (i = 0 ; i < text->n_glyphs ; i++)
			{
				glyph = text->glyphs + i;
				liren_render_draw_indexed_triangles_T2V3 (render, shader, &matrix,
					glyph->font->texture, speech->diffuse, vertex_data, index_data + 6 * i, 6);
			}

			lisys_free (index_data);
			lisys_free (vertex_data);
		}
	}
}

/**
 * \brief Sets the latest chat message for an object.
 *
 * \param self Module.
 * \param object Object.
 * \param diffuse Diffuse color.
 * \param font Font name.
 * \param message String.
 */
LIExtSpeech* liext_speeches_set_speech (
	LIExtModule* self,
	uint32_t     object,
	const float* diffuse,
	const char*  font,
	const char*  message)
{
	int create;
	LIEngObject* engobj;
	LIExtObject* extobj;
	LIExtSpeech* speech;

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
		extobj = liext_speech_object_new ();
		if (extobj == NULL)
			return NULL;
		if (!lialg_u32dic_insert (self->objects, object, extobj))
		{
			liext_speech_object_free (extobj);
			return NULL;
		}
	}

	/* Allocate new speech. */
	speech = liext_speech_new (self, font, message);
	if (speech == NULL)
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_speech_object_free (extobj);
		}
		return NULL;
	}
	memcpy (speech->diffuse, diffuse, 4 * sizeof (float));
	if (!lialg_list_prepend (&extobj->speech, speech))
	{
		if (create)
		{
			lialg_u32dic_remove (self->objects, object);
			liext_speech_object_free (extobj);
		}
		liext_speech_free (speech);
		return NULL;
	}

	return speech;
}

/*****************************************************************************/

LIExtObject* liext_speech_object_new ()
{
	return lisys_calloc (1, sizeof (LIExtObject));
}

void liext_speech_object_free (
	LIExtObject* self)
{
	LIAlgList* ptr;

	for (ptr = self->speech ; ptr != NULL ; ptr = ptr->next)
		liext_speech_free (ptr->data);
	lialg_list_free (self->speech);
	lisys_free (self);
}

/*****************************************************************************/

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	float t;
	LIAlgU32dicIter iter;
	LIAlgList* ptr;
	LIAlgList* next;
	LIEngObject* engobj;
	LIExtObject* extobj;
	LIExtSpeech* speech;
	LIMatAabb bounds;
	LIMatTransform transform;

	/* Update speech. */
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		extobj = iter.value;
		engobj = lieng_engine_find_object (self->program->engine, iter.key);
		if (engobj != NULL)
		{
			/* Update speech offset. */
			lieng_object_get_transform (engobj, &transform);
			extobj->position = transform.position;
			if (engobj->model != NULL && engobj->model->model != NULL)
			{
				bounds = engobj->model->model->bounds;
				extobj->position.y += bounds.max.y;
			}

			/* Update speech timing. */
			for (ptr = extobj->speech ; ptr != NULL ; ptr = next)
			{
				next = ptr->next;
				speech = ptr->data;
				speech->timer += secs;
				if (speech->timer > speech->life_time)
				{
					liext_speech_free (speech);
					lialg_list_remove (&extobj->speech, ptr);
				}
				else
				{
					t = (speech->timer - speech->life_time + speech->fade_time) / speech->fade_time;
					if (t < 0)
						speech->diffuse[3] = 1.0f;
					else
						speech->diffuse[3] = 1.0f - powf (t, speech->fade_exponent);
				}
			}
		}
		if (engobj == NULL || extobj->speech == NULL)
		{
			lialg_u32dic_remove (self->objects, iter.key);
			liext_speech_object_free (extobj);
		}
	}

	return 1;
}

/** @} */
/** @} */
