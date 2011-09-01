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
 * \addtogroup LIRenRender21 Render21
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.h"
#include "render-private.h"
#include "render-scene.h"
#include "../render-private.h"
#include "../render-scene.h"

#define LIREN_SCENE21_MAX_LIGHTS 8

LIRenScene21* liren_scene21_new (
	void*          scene,
	LIRenRender21* render)
{
	LIRenScene21* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenScene21));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	self->render = render;

	/* Allocate light list. */
	self->lights = lialg_ptrdic_new ();
	if (self->lights == NULL)
	{
		liren_scene21_free (self);
		return NULL;
	}

	/* Register self. */
	if (!lialg_ptrdic_insert (render->scenes, self, self))
	{
		liren_scene21_free (self);
		return NULL;
	}

	return self;
}

void liren_scene21_free (
	LIRenScene21* self)
{
	lialg_ptrdic_remove (self->render->scenes, self);
	if (self->lights != NULL)
		lialg_ptrdic_free (self->lights);
	lisys_free (self);
}

int liren_scene21_insert_light (
	LIRenScene21* self,
	LIRenLight21* light)
{
	if (!lialg_ptrdic_insert (self->lights, light, light))
		return 0;
	light->enabled = 1;

	return 1;
}

void liren_scene21_remove_light (
	LIRenScene21* self,
	LIRenLight21* light)
{
	lialg_ptrdic_remove (self->lights, light);
	light->enabled = 0;
}

void liren_scene21_remove_model (
	LIRenScene21* self,
	LIRenModel21* model)
{
	LIAlgU32dicIter iter;
	LIRenObject21* object;

	LIALG_U32DIC_FOREACH (iter, self->scene->objects)
	{
		object = ((LIRenObject*) iter.value)->v21;
		if (object->model == model)
			liren_object21_set_model (object, NULL);
	}
}

/**
 * \brief Renders the scene.
 * \param self Scene.
 * \param framebuffer Render target framebuffer.
 * \param viewport Viewport array.
 * \param modelview Modelview matrix of the camera.
 * \param projection Projeciton matrix of the camera.
 * \param frustum Frustum of the camera.
 * \param render_passes Array of render passes.
 * \param render_passes_num Number of render passes.
 * \param postproc_passes Array of post-processing passes.
 * \param postproc_passes_num Number of post-processing passes.
 */
void liren_scene21_render (
	LIRenScene21*       self,
	LIRenFramebuffer21* framebuffer,
	const GLint*        viewport,
	LIMatMatrix*        modelview,
	LIMatMatrix*        projection,
	LIMatFrustum*       frustum,
	LIRenPassRender*    render_passes,
	int                 render_passes_num,
	LIRenPassPostproc*  postproc_passes,
	int                 postproc_passes_num)
{
	int i;
	int lights_num;
	float pos[4];
	LIAlgPtrdicIter iter1;
	LIAlgU32dicIter iter;
	LIRenBuffer21* buffer;
	LIRenLight21* light;
	LIRenLight21* lights[LIREN_SCENE21_MAX_LIGHTS + 1];
	LIRenMaterial21* material;
	LIRenModel21* model;
	LIRenModelGroup21* group;
	LIRenObject21* object;

	/* Set the rendering mode. */
	glPushAttrib (GL_VIEWPORT_BIT);
	glViewport (viewport[0], viewport[1], viewport[2], viewport[3]);
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (projection->m);
	glMatrixMode (GL_MODELVIEW);
	glLoadMatrixf (modelview->m);
	glDisable (GL_BLEND);
	glEnable (GL_CULL_FACE);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHTING);
	glEnable (GL_NORMALIZE);

	/* Setup lighting. */
	lights_num = 0;
	LIALG_PTRDIC_FOREACH (iter1, self->lights)
	{
		light = iter1.value;
		for (i = lights_num ; i > 0 ; i--)
		{
			if (liren_light21_compare (light, lights[i - 1]) < 0)
				break;
			lights[i] = lights[i - 1];
		}
		lights[i] = light;
		lights_num = LIMAT_MIN (LIREN_SCENE21_MAX_LIGHTS, lights_num + 1);
	}
	for (i = 0 ; i < lights_num ; i++)
	{
		light = lights[i];
		glEnable (GL_LIGHT0 + i);
		glLightf (GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, light->equation[0]);
		glLightf (GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, light->equation[1]);
		glLightf (GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, light->equation[2]);
		glLightfv (GL_LIGHT0 + i, GL_AMBIENT, light->ambient);
		glLightfv (GL_LIGHT0 + i, GL_DIFFUSE, light->diffuse);
		glLightfv (GL_LIGHT0 + i, GL_SPECULAR, light->specular);
		liren_light21_get_position (light, pos);
		glLightfv (GL_LIGHT0 + i, GL_POSITION, pos);
	}

	/* Render each object. */
	LIALG_U32DIC_FOREACH (iter, self->scene->objects)
	{
		object = ((LIRenObject*) iter.value)->v21;
		model = object->model;
		if (!object->realized || model == NULL || model->buffer == NULL)
			continue;

		/* Enable the vertex buffer. */
		buffer = model->buffer;
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, buffer->index_buffer);
		glBindBuffer (GL_ARRAY_BUFFER, buffer->vertex_buffer);
		glEnableClientState (GL_VERTEX_ARRAY);
		glVertexPointer (3, buffer->vertex_format.vtx_format,
			buffer->vertex_format.size, NULL + buffer->vertex_format.vtx_offset);
		glEnableClientState (GL_NORMAL_ARRAY);
		glNormalPointer (buffer->vertex_format.nml_format,
			buffer->vertex_format.size, NULL + buffer->vertex_format.nml_offset);
		glEnableClientState (GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer (2, buffer->vertex_format.tex_format,
			buffer->vertex_format.size, NULL + buffer->vertex_format.tex_offset);

		/* Render each face group. */
		glPushMatrix ();
		glMultMatrixf (object->orientation.matrix.m);
		for (i = 0 ; i < model->groups.count ; i++)
		{
			group = model->groups.array + i;
			material = model->materials.array + i;
			glMaterialf (GL_FRONT, GL_SHININESS, material->shininess);
			glMaterialfv (GL_FRONT, GL_DIFFUSE, material->diffuse);
			glMaterialfv (GL_FRONT, GL_SPECULAR, material->specular);
			if (material->image != NULL && material->image->texture != NULL)
				glBindTexture (GL_TEXTURE_2D, material->image->texture->texture);
			else
				glBindTexture (GL_TEXTURE_2D, 0);
			if (sizeof (LIRenIndex) == 2)
				glDrawElements (GL_TRIANGLES, group->count, GL_UNSIGNED_SHORT, NULL + group->start * sizeof (LIRenIndex));
			else if (sizeof (LIRenIndex) == 4)
				glDrawElements (GL_TRIANGLES, group->count, GL_UNSIGNED_INT, NULL + group->start * sizeof (LIRenIndex));
			else
				lisys_assert (0);
		}
		glPopMatrix ();
	}

	/* Disable vertex buffers. */
	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_NORMAL_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);

	/* Restore the rendering mode. */
	for (i = 0 ; i < lights_num ; i++)
		glDisable (GL_LIGHT0 + i);
	glDisable (GL_LIGHTING);
	glDisable (GL_NORMALIZE);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glPopAttrib ();
}

/**
 * \brief Updates the scene.
 * \param self Scene.
 * \param secs Number of seconds since the last update.
 */
void liren_scene21_update (
	LIRenScene21* self,
	float         secs)
{
	LIAlgU32dicIter iter;
	LIRenObject21* object;

	/* Update the effect timer. */
	self->time += secs;

	/* Update objects. */
	LIALG_U32DIC_FOREACH (iter, self->scene->objects)
	{
		object = ((LIRenObject*) iter.value)->v21;
		liren_object21_update (object, secs);
	}
}

/** @} */
/** @} */
