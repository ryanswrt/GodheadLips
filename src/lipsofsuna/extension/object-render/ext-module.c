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
 * \addtogroup LIExtObjectRender ObjectRender
 * @{
 */

#include "ext-module.h"

static int private_engine_free (
	LIExtModule* self,
	LIEngEngine* engine);

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model);

static int private_object_new (
	LIExtModule* self,
	LIEngObject* object);

static int private_object_free (
	LIExtModule* self,
	LIEngObject* object);

static int private_object_model (
	LIExtModule* self,
	LIEngObject* object,
	LIEngModel*  model);

static int private_object_realize (
	LIExtModule* self,
	LIEngObject* object,
	int          value);

static int private_object_transform (
	LIExtModule*    self,
	LIEngObject*    object,
	LIMatTransform* value);

static int private_engine_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_object_render_info =
{
	LIMAI_EXTENSION_VERSION, "ObjectRender",
	liext_object_render_new,
	liext_object_render_free
};

LIExtModule* liext_object_render_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Find the client compoent. */
	self->client = limai_program_find_component (program, "client");
	if (self->client == NULL)
	{
		liext_object_render_free (self);
		return NULL;
	}
	self->render = self->client->render;
	self->scene = self->client->scene;

	/* Make sure that the required extensions are loaded. */
	if (!limai_program_insert_extension (program, "render"))
	{
		liext_object_render_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "engine-free", 1, private_engine_free, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, "model-changed", 1, private_model_changed, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, "model-free", 1, private_model_free, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, "model-new", 1, private_model_new, self, self->calls + 3) ||
	    !lical_callbacks_insert (program->callbacks, "tick", 1, private_engine_tick, self, self->calls + 4) ||
	    !lical_callbacks_insert (program->callbacks, "object-new", 1, private_object_new, self, self->calls + 5) ||
	    !lical_callbacks_insert (program->callbacks, "object-free", 1, private_object_free, self, self->calls + 6) ||
	    !lical_callbacks_insert (program->callbacks, "object-model", 1, private_object_model, self, self->calls + 7) ||
	    !lical_callbacks_insert (program->callbacks, "object-visibility", 1, private_object_realize, self, self->calls + 8) ||
	    !lical_callbacks_insert (program->callbacks, "object-transform", 1, private_object_transform, self, self->calls + 9))
	{
		liext_object_render_free (self);
		return NULL;
	}

	/* Extend classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_RENDER_OBJECT, self);
	liext_script_render_object (program->script);

	return self;
}

void liext_object_render_free (
	LIExtModule* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int private_engine_free (
	LIExtModule* self,
	LIEngEngine* engine)
{
	if (self->scene != NULL)
		liren_scene_free (self->scene);
	if (self->render != NULL)
		liren_render_free (self->render);

	return 1;
}

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIRenModel* model_;

	lisys_assert (model != NULL);

	model_ = liren_render_find_model (self->render, model->id);
	if (model_ != NULL)
		liren_model_set_model (model_, model->model);

	return 1;
}

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIRenModel* model_;

	/* Find the model. */
	lisys_assert (model != NULL);
	model_ = liren_render_find_model (self->render, model->id);
	if (model_ == NULL)
		return 1;

	/* Remove from objects. */
	/* Keeping the model alive when it's assigned to objects is the job of scripts.
	   If they don't reference the model, we'll remove it even if it's in use. We
	   prevent crashing by removing it from objects in such a case. */
	liren_scene_remove_model (self->scene, model_);

	liren_model_free (model_);

	return 1;
}

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIRenModel* model_;

	lisys_assert (model != NULL);

	model_ = liren_render_find_model (self->render, model->id);
	if (model_ == NULL)
		liren_model_new (self->render, model->model, model->id);

	return 1;
}

static int private_object_new (
	LIExtModule* self,
	LIEngObject* object)
{
	liren_object_new (self->scene, object->id);

	return 1;
}

static int private_object_free (
	LIExtModule* self,
	LIEngObject* object)
{
	LIRenObject* object_;

	object_ = liren_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
		liren_object_free (object_);

	return 1;
}

static int private_object_model (
	LIExtModule* self,
	LIEngObject* object,
	LIEngModel*  model)
{
	LIRenObject* object_;
	LIRenModel* model_;

	object_ = liren_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
	{
		if (model != NULL)
		{
			model_ = liren_render_find_model (self->render, model->id);
			if (model_ != NULL)
			{
				liren_object_set_pose (object_, object->pose);
				liren_object_set_model (object_, model_);
				return 1;
			}
		}
		liren_object_set_pose (object_, NULL);
		liren_object_set_model (object_, NULL);
	}

	return 1;
}

static int private_object_realize (
	LIExtModule* self,
	LIEngObject* object,
	int          value)
{
	LIRenObject* object_;

	object_ = liren_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
		liren_object_set_realized (object_, value);

	return 1;
}

static int private_object_transform (
	LIExtModule*    self,
	LIEngObject*    object,
	LIMatTransform* value)
{
	LIRenObject* object_;

	object_ = liren_scene_find_object (self->scene, object->id);
	if (object_ != NULL)
		liren_object_set_transform (object_, value);

	return 1;
}

static int private_engine_tick (
	LIExtModule* self,
	float        secs)
{
	liren_render_update (self->render, secs);
	liren_scene_update (self->scene, secs);

	return 1;
}

/** @} */
/** @} */
