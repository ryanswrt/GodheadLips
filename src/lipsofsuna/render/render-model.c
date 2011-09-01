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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenModel Model
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-model.h"
#include "render-private.h"
#include "render32/render-private.h"

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param id Unique model ID.
 * \return New model or NULL.
 */
LIRenModel* liren_model_new (
	LIRenRender* render,
	LIMdlModel*  model,
	int          id)
{
	LIRenModel* self;

	self = lisys_calloc (1, sizeof (LIRenModel));
	if (self == NULL)
		return NULL;
	self->id = id;
	self->render = render;

	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_model32_new (render->v32, model, id);
		if (self->v32 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}
	else
	{
		self->v21 = liren_model21_new (render->v21, model, id);
		if (self->v21 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	/* Add to the dictionary. */
	if (id)
	{
		if (!lialg_u32dic_insert (render->models, id, self))
		{
			liren_model_free (self);
			return NULL;
		}
	}
	if (!lialg_ptrdic_insert (render->models_ptr, self, self))
	{
		liren_model_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the model.
 * \param self Model.
 */
void liren_model_free (
	LIRenModel* self)
{
	lialg_ptrdic_remove (self->render->models_ptr, self);
	if (self->id)
		lialg_u32dic_remove (self->render->models, self->id);
	if (self->v32 != NULL)
		liren_model32_free (self->v32);
	if (self->v21 != NULL)
		liren_model21_free (self->v21);
	lisys_free (self);
}

int liren_model_set_model (
	LIRenModel* self,
	LIMdlModel* model)
{
	if (self->v32 != NULL)
		return liren_model32_set_model (self->v32, model);
	else
		return liren_model21_set_model (self->v21, model);
}

/** @} */
/** @} */
