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
 * \addtogroup LIExtPhysics Physics
 * @{
 */

#include "ext-module.h"

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model);

static int private_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_physics_info =
{
	LIMAI_EXTENSION_VERSION, "Physics",
	liext_physics_new,
	liext_physics_free
};

LIExtModule* liext_physics_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Initialize physics. */
	self->physics = liphy_physics_new (program->callbacks);
	if (self->physics == NULL)
	{
		liext_physics_free (self);
		return NULL;
	}

	/* Register component. */
	if (!limai_program_insert_component (program, "physics", self->physics))
	{
		liext_physics_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_PHYSICS, self);
	liext_script_physics (program->script);

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "model-changed", 1, private_model_changed, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, "model-free", 1, private_model_free, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, "model-new", 1, private_model_new, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, "tick", -65535, private_tick, self, self->calls + 3))
	{
		liext_physics_free (self);
		return NULL;
	}

	return self;
}

void liext_physics_free (
	LIExtModule* self)
{
	/* Unregister component. */
	if (self->physics != NULL)
		limai_program_remove_component (self->program, "physics");

	/* Free physics. */
	if (self->physics != NULL)
		liphy_physics_free (self->physics);

	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIPhyModel* model_;

	lisys_assert (model != NULL);

	model_ = liphy_physics_find_model (self->physics, model->id);
	if (model_ != NULL)
		liphy_model_set_model (model_, model->model);

	return 1;
}

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIPhyModel* model_;

	lisys_assert (model != NULL);

	model_ = liphy_physics_find_model (self->physics, model->id);
	if (model_ == NULL)
		return 1;

	/* Remove from objects. */
	/* Keeping the model alive when it's assigned to objects is the job of scripts.
	   If they don't reference the model, we'll remove it even if it's in use. We
	   prevent crashing by removing it from objects in such a case. */
	liphy_physics_remove_model (self->physics, model_);

	/* Free the model. */
	liphy_model_free (model_);

	return 1;
}

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIPhyModel* model_;

	lisys_assert (model != NULL);

	model_ = liphy_physics_find_model (self->physics, model->id);
	if (model_ == NULL)
		liphy_model_new (self->physics, model->model, model->id);

	return 1;
}

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	/* Update physics. */
	if (self->simulate)
		liphy_physics_update (self->physics, secs);

	return 1;
}

/** @} */
/** @} */
