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
 * \addtogroup LIRenRender32 Render32
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.h"
#include "render-context.h"
#include "render-lighting.h"
#include "render-private.h"
#include "render-scene.h"
#include "render-sort.h"
#include "../render-private.h"
#include "../render-scene.h"

#define LIREN_LIGHT_MAXIMUM_RATING 100.0f
#define LIREN_PARTICLE_MAXIMUM_COUNT 1000

static int private_init (
	LIRenScene32* self);

static void private_render_pass_nosort (
	LIRenScene32*   self,
	LIRenContext32* context,
	LIMatFrustum*   frustum,
	int             pass);

static void private_render_pass_sort (
	LIRenScene32*   self,
	LIRenContext32* context,
	int             pass);

static int private_render_postproc (
	LIRenScene32*       self,
	LIRenContext32*     context,
	LIRenFramebuffer32* framebuffer,
	int                 pass,
	const char*         name,
	int                 mipmaps);

static int private_sort_scene (
	LIRenScene32*   self,
	LIRenContext32* context);

/*****************************************************************************/

LIRenScene32* liren_scene32_new (
	void*          scene,
	LIRenRender32* render)
{
	LIRenScene32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenScene32));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	self->render = render;

	/* Register self. */
	if (!lialg_ptrdic_insert (render->scenes, self, self))
	{
		liren_scene32_free (self);
		return NULL;
	}

	/* Initialize subsystems. */
	if (!private_init (self))
	{
		liren_scene32_free (self);
		return NULL;
	}

	return self;
}

void liren_scene32_free (
	LIRenScene32* self)
{
	if (self->lighting != NULL)
		liren_lighting32_free (self->lighting);
	if (self->sort != NULL)
		liren_sort32_free (self->sort);
	lialg_ptrdic_remove (self->render->scenes, self);
	lisys_free (self);
}

void liren_scene32_insert_light (
	LIRenScene32* self,
	LIRenLight32* light)
{
	liren_lighting32_insert_light (self->lighting, light);
}

void liren_scene32_remove_light (
	LIRenScene32* self,
	LIRenLight32* light)
{
	liren_lighting32_remove_light (self->lighting, light);
}

void liren_scene32_remove_model (
	LIRenScene32* self,
	LIRenModel32* model)
{
	LIAlgU32dicIter iter;
	LIRenObject32* object;

	LIALG_U32DIC_FOREACH (iter, self->scene->objects)
	{
		object = ((LIRenObject*) iter.value)->v32;
		if (object->model == model)
			liren_object32_set_model (object, NULL);
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
void liren_scene32_render (
	LIRenScene32*       self,
	LIRenFramebuffer32* framebuffer,
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
	int postproc_passes_done;
	GLint orig_viewport[4];
	LIMatMatrix inv;
	LIMatVector eye;
	LIRenContext32* context;

	lisys_assert (modelview != NULL);
	lisys_assert (projection != NULL);
	lisys_assert (frustum != NULL);
	lisys_assert (framebuffer != NULL);
	lisys_assert (viewport[2] == framebuffer->width);
	lisys_assert (viewport[3] == framebuffer->height);
	glGetIntegerv (GL_VIEWPORT, orig_viewport);

	/* Initialize the context. */
	context = liren_render32_get_context (self->render);
	liren_context32_init (context);
	liren_context32_set_scene (context, self);
	liren_context32_set_frustum (context, frustum);
	liren_context32_set_projection (context, projection);
	liren_context32_set_viewmatrix (context, modelview);
	liren_context32_set_time (context, self->time);

	/* Calculate camera position. */
	inv = limat_matrix_invert (*modelview);
	eye = limat_matrix_transform (inv, limat_vector_init (0.0f, 0.0f, 0.0f));
	liren_context32_set_camera_position (context, &eye);

	/* Depth sort the scene. */
	if (!private_sort_scene (self, context))
		return;

	/* Upload light settings. */
	liren_lighting32_upload (self->lighting, context, &eye);

	/* Reset profiling. */
#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.objects = 0;
	self->render->profiling.materials = 0;
	self->render->profiling.faces = 0;
	self->render->profiling.vertices = 0;
#endif

	/* Enable backbuffer viewport. */
	glBindFramebuffer (GL_FRAMEBUFFER, framebuffer->render_framebuffer);
	glViewport (0, 0, viewport[2], viewport[3]);
	glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	/* Render scene passes. */
	for (i = 0 ; i < render_passes_num ; i++)
	{
		if (!render_passes[i].sort)
			private_render_pass_nosort (self, context, frustum, render_passes[i].pass);
		else
			private_render_pass_sort (self, context, render_passes[i].pass);
	}

	/* Render post-processing effects. */
	postproc_passes_done = 0;
	for (i = 0 ; i < postproc_passes_num ; i++)
	{
		postproc_passes_done += private_render_postproc (self, context, framebuffer,
			postproc_passes_done, postproc_passes[i].shader, postproc_passes[i].mipmap);
	}

	/* Blit to the screen. */
	/* If no post-processing steps were done, we blit directly from the render
	   buffer. Otherwise, the final image is in the first post-processing
	   buffer and we need to blit from there. */
	if (postproc_passes_done)
		glBindFramebuffer (GL_READ_FRAMEBUFFER, framebuffer->postproc_framebuffers[0]);
	else
		glBindFramebuffer (GL_READ_FRAMEBUFFER, framebuffer->render_framebuffer);
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer (0, 0, viewport[2], viewport[3], viewport[0], viewport[1],
		viewport[0] + viewport[2], viewport[1] + viewport[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);

	/* Restore the original viewport. */
	glViewport (orig_viewport[0], orig_viewport[1], orig_viewport[2], orig_viewport[3]);

	/* Profiling report. */
#ifdef LIREN_ENABLE_PROFILING
	printf ("RENDER PROFILING: objects=%d materials=%d polys=%d verts=%d\n",
		self->render->profiling.objects, self->render->profiling.materials,
		self->render->profiling.faces, self->render->profiling.vertices);
#endif
}

/**
 * \brief Updates the scene.
 * \param self Scene.
 * \param secs Number of seconds since the last update.
 */
void liren_scene32_update (
	LIRenScene32* self,
	float         secs)
{
	LIAlgU32dicIter iter;
	LIRenObject32* object;

	/* Update the effect timer. */
	self->time += secs;

	/* Update objects. */
	LIALG_U32DIC_FOREACH (iter, self->scene->objects)
	{
		object = ((LIRenObject*) iter.value)->v32;
		liren_object32_update (object, secs);
	}

	/* Update lights. */
	liren_lighting32_update (self->lighting);
}

/*****************************************************************************/

static int private_init (
	LIRenScene32* self)
{
	self->lighting = liren_lighting32_new (self->render);
	if (self->lighting == NULL)
		return 0;
	self->sort = liren_sort32_new (self->render);
	if (self->sort == NULL)
		return 0;
	return 1;
}

static void private_render_pass_nosort (
	LIRenScene32*   self,
	LIRenContext32* context,
	LIMatFrustum*   frustum,
	int             pass)
{
	int i;
	int start;
	int count;
	int first;
	LIAlgU32dicIter iter;
	LIMatAabb bounds;
	LIRenLod32* lod;
	LIRenMaterial32* material;
	LIRenObject32* object;

	LIALG_U32DIC_FOREACH (iter, self->scene->objects)
	{
		object = ((LIRenObject*) iter.value)->v32;

		/* Check for visibility. */
		if (!liren_object32_get_realized (object))
			continue;
		liren_object32_get_bounds (object, &bounds);
		if (limat_frustum_cull_aabb (frustum, &bounds))
			continue;

		/* Select the level of detail. */
		lod = liren_model32_get_distance_lod (object->model,
			&object->transform.position, &context->camera_position);

		/* Bind the object data. */
		liren_context32_set_modelmatrix (context, &object->orientation.matrix);
		liren_context32_set_mesh (context, &lod->mesh);
		first = 1;

		/* Render each material group that has the pass. */
		for (i = 0 ; i < object->model->materials.count ; i++)
		{
			material = object->model->materials.array[i];
			if (!material->shader->passes[pass].program)
				continue;
			start = lod->groups.array[i].start;
			count = lod->groups.array[i].count;
			liren_context32_set_material (context, material);
			liren_context32_set_shader (context, pass, material->shader);
			liren_context32_set_textures (context, material->textures.array, material->textures.count);
			if (first && material->shader->passes[pass].animated)
			{
				liren_context32_set_pose (context, object->pose);
				first = 0;
			}
			liren_context32_bind (context);
			liren_context32_render_indexed (context, start, count);
		}

		/* Render the effect layer. */
		material = object->effect.material;
		if (material != NULL && lod->groups.count && material->shader->passes[pass].program)
		{
			start = lod->groups.array[lod->groups.count - 1].start;
			count = lod->groups.array[lod->groups.count - 1].count;
			liren_context32_set_material (context, material);
			liren_context32_set_shader (context, pass, material->shader);
			liren_context32_set_textures (context, material->textures.array, material->textures.count);
			if (first && material->shader->passes[pass].animated)
				liren_context32_set_pose (context, object->pose);
			liren_context32_bind (context);
			liren_context32_render_indexed (context, 0, start + count);
		}
	}
}

static void private_render_pass_sort (
	LIRenScene32*   self,
	LIRenContext32* context,
	int             pass)
{
	int i;
	LIMatMatrix identity;
	LIMatVector position;
	LIRenSortface32* face;

	/* Initialize pass. */
	identity = limat_matrix_identity ();

	/* Render sorted groups. */
	for (i = self->sort->buckets.count - 1 ; i >= 0 ; i--)
	{
		for (face = self->sort->buckets.array[i] ; face != NULL ; face = face->next)
		{
			if (face->type == LIREN_SORT_TYPE_FACE)
			{
				/* Render a single transparent triangle. */
				if (!face->face.material->shader->passes[pass].program)
					continue;
				liren_context32_set_material (context, face->face.material);
				liren_context32_set_modelmatrix (context, &face->face.matrix);
				liren_context32_set_shader (context, pass, face->face.material->shader);
				liren_context32_set_textures (context, face->face.material->textures.array, face->face.material->textures.count);
				liren_context32_set_mesh (context, face->face.mesh);
				liren_context32_bind (context);
				liren_context32_render_indexed (context, face->face.index, 3);
			}
			else if (face->type == LIREN_SORT_TYPE_GROUP)
			{
				/* Render a group of transparent triangles. */
				if (!face->group.material->shader->passes[pass].program)
					continue;
				liren_context32_set_material (context, face->group.material);
				liren_context32_set_modelmatrix (context, &face->group.matrix);
				liren_context32_set_shader (context, pass, face->group.material->shader);
				liren_context32_set_textures (context, face->group.material->textures.array, face->group.material->textures.count);
				liren_context32_set_mesh (context, face->group.mesh);
				liren_context32_set_pose (context, face->group.pose);
				liren_context32_bind (context);
				liren_context32_render_indexed (context, face->group.index, face->group.count);
			}
			else if (face->type == LIREN_SORT_TYPE_PARTICLE)
			{
				/* Render a particle. */
				if (!face->particle.shader->passes[pass].program)
					continue;
				liren_context32_set_modelmatrix (context, &identity);
				liren_context32_set_shader (context, pass, face->particle.shader);
				liren_context32_set_textures_raw (context, &face->particle.image->texture->texture, 1);
				liren_context32_bind (context);
				glBegin (GL_TRIANGLES);
				glVertexAttrib2f (LIREN_ATTRIBUTE_TEXCOORD, face->particle.diffuse[3], face->particle.size);
				position = face->particle.position;
				glVertexAttrib3fv (LIREN_ATTRIBUTE_NORMAL, face->particle.diffuse);
				glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
				glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
				glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
				glEnd ();
			}
		}
	}
}

static int private_render_postproc (
	LIRenScene32*       self,
	LIRenContext32*     context,
	LIRenFramebuffer32* framebuffer,
	int                 pass,
	const char*         name,
	int                 mipmaps)
{
	GLuint tmp;
	float param[4];
	LIRenShader32* shader;

	/* Find the shader. */
	shader = liren_render32_find_shader (self->render, name);
	if (shader == NULL)
		return 0;

	/* Blit to the post-processing buffer. */
	/* This is needed because the render buffer is multisampled but the
	   post-processing buffer isn't due to fragment shader performance.
	   Multisamples needs to be resolved before post-processing. */
	if (!pass)
	{
		glBindFramebuffer (GL_READ_FRAMEBUFFER, framebuffer->render_framebuffer);
		glBindFramebuffer (GL_DRAW_FRAMEBUFFER, framebuffer->postproc_framebuffers[0]);
		glBlitFramebuffer (0, 0, framebuffer->width, framebuffer->height, 0, 0,
			framebuffer->width, framebuffer->height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	/* Calculate the pixel size in texture units. */
	param[0] = 1.0f / framebuffer->width;
	param[1] = 1.0f / framebuffer->height;
	param[2] = 0.0;
	param[3] = 0.0;

	/* Render from the first buffer to the second. */
	glBindFramebuffer (GL_FRAMEBUFFER, framebuffer->postproc_framebuffers[1]);
	liren_context32_set_buffer (context, self->render->helpers.unit_quad);
	liren_context32_set_blend (context, 0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	liren_context32_set_cull (context, 0, GL_CCW);
	liren_context32_set_param (context, param);
	liren_context32_set_shader (context, 0, shader);
	liren_context32_set_textures_raw (context, framebuffer->postproc_textures, 1);
	liren_context32_bind (context);
	if (mipmaps)
	{
		glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0);
		glGenerateMipmap (GL_TEXTURE_2D);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		liren_context32_render_indexed (context, 0, 6);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
		liren_context32_render_indexed (context, 0, 6);

	/* Swap the buffers so that we can chain passes. */
	tmp = framebuffer->postproc_framebuffers[0];
	framebuffer->postproc_framebuffers[0] = framebuffer->postproc_framebuffers[1];
	framebuffer->postproc_framebuffers[1] = tmp;
	tmp = framebuffer->postproc_textures[0];
	framebuffer->postproc_textures[0] = framebuffer->postproc_textures[1];
	framebuffer->postproc_textures[1] = tmp;

	return 1;
}

static int private_sort_scene (
	LIRenScene32*   self,
	LIRenContext32* context)
{
	LIAlgU32dicIter iter;
	LIRenObject32* rndobj;

	/* Initialize sorting. */
	liren_sort32_clear (self->sort, &context->matrix.view, &context->matrix.projection);
	liren_context32_bind (context);

	/* Collect scene objects. */
	LIALG_U32DIC_FOREACH (iter, self->scene->objects)
	{
		rndobj = ((LIRenObject*) iter.value)->v32;
		if (!liren_object32_get_realized (rndobj))
			continue;
		liren_sort32_add_object (self->sort, rndobj);
	}

	return 1;
}

/** @} */
/** @} */
