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
 * \addtogroup LIRenObject32 Object32
 * @{
 */

#include "lipsofsuna/network.h"
#include "lipsofsuna/system.h"
#include "render-lighting.h"
#include "render-object.h"
#include "render-private.h"
#include "render-scene.h"

static void private_bounds_update (
	LIRenObject32* self);

static void private_envmap_clear (
	LIRenObject32* self);

static int private_envmap_create (
	LIRenObject32* self);

static void private_envmap_update (
	LIRenObject32* self);

static void private_lights_clear (
	LIRenObject32* self);

static int private_lights_create (
	LIRenObject32* self,
	LIMdlPose*   pose);

static void private_lights_update (
	LIRenObject32* self);

/*****************************************************************************/

/**
 * \brief Creates a new render object and adds it to the scene.
 * \param scene Scene.
 * \param id Unique identifier.
 * \return New object or NULL.
 */
LIRenObject32* liren_object32_new (
	LIRenScene32* scene,
	int           id)
{
	LIRenObject32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenObject32));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	self->transform = limat_transform_identity ();
	self->orientation.matrix = limat_matrix_identity ();

	return self;
}

/**
 * \brief Frees the render object.
 *
 * The object is removed from its renderer and its memory is freed.
 *
 * \param self Object.
 */
void liren_object32_free (
	LIRenObject32* self)
{
	if (self->effect.material != NULL)
		liren_material32_free (self->effect.material);
	private_envmap_clear (self);
	private_lights_clear (self);
	lisys_free (self);
}

/**
 * \brief Deforms the object.
 * \param self Object.
 */
void liren_object32_deform (
	LIRenObject32* self)
{
	if (!liren_object32_get_realized (self))
		return;

	/* Deform the mesh. */
	liren_model32_deform (self->model, "skeletal", self->pose);

	/* Update light positions. */
	private_lights_update (self);
}

/**
 * \brief Sets the particle animation state of the object.
 * \param self Object.
 * \param start Animation offset in seconds.
 * \param loop Nonzero to enable looping.
 */
void liren_object32_particle_animation (
	LIRenObject32* self,
	float          start,
	int            loop)
{
	self->particle.time = start;
	self->particle.loop = loop;
}

/**
 * \brief Advances the timer of the object and deforms its mesh.
 * \param self Object.
 * \param secs Number of seconds since the last update.
 */
void liren_object32_update (
	LIRenObject32* self,
	float          secs)
{
	self->particle.time += secs;
	private_envmap_update (self);
}

/**
 * \brief Gets the bounding box of the object.
 * \param self Object.
 * \param result Return location for the bounding box.
 */
void liren_object32_get_bounds (
	const LIRenObject32* self,
	LIMatAabb*           result)
{
	*result = self->bounds;
}

/**
 * \brief Gets the center position of the object.
 *
 * The returned position depends on the current matrix.
 *
 * \param self Object.
 * \param center Return location for the position.
 */
void liren_object32_get_center (
	const LIRenObject32* self,
	LIMatVector*         center)
{
	*center = self->orientation.center;
}

/**
 * \brief Sets the effect layer of the object.
 * \param self Object.
 * \param shader Shader name or NULL to disable the effect.
 * \param params Effect parameters or NULL.
 * \return Nonzero on success.
 */
int liren_object32_set_effect (
	LIRenObject32* self,
	const char*    shader,
	const float*   params)
{
	LIRenShader32* shader_;

	/* Free the old effect. */
	if (self->effect.material != NULL)
	{
		liren_material32_free (self->effect.material);
		self->effect.material = NULL;
	}

	/* Find the effect shader. */
	if (shader != NULL)
		shader_ = liren_render32_find_shader (self->scene->render, shader);
	else
		shader_ = NULL;

	/* Create the effect material. */
	if (shader_ != NULL)
	{
		self->effect.material = liren_material32_new ();
		if (self->effect.material == NULL)
			return 0;
		liren_material32_set_shader (self->effect.material, shader_);
		liren_material32_set_params (self->effect.material, params);
	}

	return 1;
}

/**
 * \brief Gets the model of the object.
 * \param self Object.
 * \return Model or NULL.
 */
LIRenModel32* liren_object32_get_model (
	LIRenObject32* self)
{
	return self->model;
}

/**
 * \brief Sets the model of the object.
 *
 * Changes the model of the object and takes care of automatically registering
 * and unregistering lights to match the new apperance.
 *
 * \param self Object.
 * \param model Model.
 * \return Nonzero on success.
 */
int liren_object32_set_model (
	LIRenObject32* self,
	LIRenModel32*  model)
{
	/* Replace model. */
	self->model = model;

	/* Replace lights and environment map. */
	private_lights_clear (self);
	private_envmap_clear (self);
	if (liren_object32_get_realized (self))
	{
		private_lights_create (self, self->pose);
		private_envmap_create (self);
	}
	private_bounds_update (self);

	return 1;
}

/**
 * \brief Sets the pose of the object.
 * \param self Object.
 * \param pose Pose.
 * \return Nonzero on success.
 */
int liren_object32_set_pose (
	LIRenObject32* self,
	LIMdlPose*     pose)
{
	if (self->pose != pose)
	{
		self->pose = pose;
		return liren_object32_set_model (self, self->model);
	}

	return 1;
}

/**
 * \brief Checks if the object is renderable.
 *
 * Returns nonzero if the realized flag of the object is set and the object
 * has a model assigned to it. Otherwise returns zero.
 *
 * \param self Object.
 * \return Nonzero if realized.
 */
int liren_object32_get_realized (
	const LIRenObject32* self)
{
	return self->realized && self->model != NULL;
}

/**
 * \brief Sets the realized flag of the object.
 *
 * If the flag is set and the object has a model set, the model will be rendered
 * in subsequent frames. The light sources that are a part of the model are also
 * added to the scene.
 *
 * If the flag is cleared, the model assigned to the object is not rendered from
 * now on. The light sources associated with the model are also removed from the
 * scene.
 *
 * \param self Object.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
int liren_object32_set_realized (
	LIRenObject32* self,
	int            value)
{
	if (self->realized == value)
		return 1;
	self->realized = value;
	private_lights_clear (self);
	private_envmap_clear (self);
	if (liren_object32_get_realized (self))
	{
		private_lights_create (self, self->pose);
		private_envmap_create (self);
	}

	return 1;
}

/**
 * \brief Gets the transformation of the object.
 * \param self Object.
 * \param value Return location for the transformation.
 */
void liren_object32_get_transform (
	LIRenObject32*  self,
	LIMatTransform* value)
{
	*value = self->transform;
}

/**
 * \brief Sets the transformation of the object.
 * \param self Object.
 * \param value Transformation.
 */
void liren_object32_set_transform (
	LIRenObject32*        self,
	const LIMatTransform* value)
{
	/* Set transformation. */
	self->transform = *value;
	self->orientation.matrix = limat_convert_transform_to_matrix (*value);

	/* Recalculate bounding box. */
	private_bounds_update (self);

	/* Transform our light sources. */
	private_lights_update (self);
}

/*****************************************************************************/

static void private_bounds_update (
	LIRenObject32* self)
{
	LIMatVector center;

	/* Update bounding box. */
	if (self->model != NULL)
	{
		liren_model32_get_bounds (self->model, &self->bounds);
		self->bounds = limat_aabb_transform (self->bounds, &self->orientation.matrix);
	}
	else
		limat_aabb_init (&self->bounds);

	/* Update center point. */
	center = limat_vector_add (self->bounds.min, self->bounds.max);
	center = limat_vector_multiply (center, 0.5f);
	self->orientation.center = center;
}

static void private_envmap_clear (
	LIRenObject32* self)
{
	if (self->cubemap.map)
	{
		glDeleteFramebuffersEXT (6, self->cubemap.fbo);
		glDeleteTextures (1, &self->cubemap.depth);
		glDeleteTextures (1, &self->cubemap.map);
		memset (self->cubemap.fbo, 0, 6 * sizeof (GLuint));
		self->cubemap.depth = 0;
		self->cubemap.map = 0;
	}
}

static int private_envmap_create (
	LIRenObject32* self)
{
#warning Envmaps are disabled.
#if 0
	int i;
	int j;
	int width = 0;
	int height = 0;
	LIRenMaterial32* material;
	LIRenTexture32* texture;

	/* Check for requirements. */
	if (!GLEW_ARB_depth_texture ||
	    !GLEW_ARB_texture_cube_map ||
	    !GLEW_EXT_framebuffer_object)
		return 1;

	/* Check if needed by textures. */
	if (self->model == NULL)
		return 1;
	for (i = 0 ; i < self->model->materials.count ; i++)
	{
		material = self->model->materials.array[i];
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
			if (texture->type == LIMDL_TEXTURE_TYPE_ENVMAP)
			{
				width = LIMAT_MAX (width, texture->width);
				height = LIMAT_MAX (height, texture->height);
			}
		}
	}
	if (width < 2 || height < 2)
		return 1;

	/* Create depth texture. */
	glGenTextures (1, &self->cubemap.depth);
	glBindTexture (GL_TEXTURE_2D, self->cubemap.depth);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Create cubemap texture. */
	glGenTextures (1, &self->cubemap.map);
	glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, self->cubemap.map);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (i = 0 ; i < 6 ; i++)
	{
		glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, 0, GL_RGB,
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	/* Create framebuffer objects. */
	glGenFramebuffersEXT (6, self->cubemap.fbo);
	for (i = 0 ; i < 6 ; i++)
	{
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->cubemap.fbo[i]);
		glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, self->cubemap.map, 0);
		glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			GL_TEXTURE_2D, self->cubemap.depth, 0);
		glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
		switch (glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT))
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				lisys_error_set (EINVAL, "incomplete framebuffer attachment");
				goto error;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				lisys_error_set (EINVAL, "incomplete framebuffer dimensions");
				goto error;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				lisys_error_set (EINVAL, "missing framebuffer attachment");
				goto error;
			default:
				lisys_error_set (ENOTSUP, "unsupported framebuffer format");
				goto error;
		}
	}
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

	/* Setup virtual image to make binding easier. */
	self->cubemap.width = width;
	self->cubemap.height = height;

	/* Bind it to environment map textures. */
	for (i = 0 ; i < self->model->materials.count ; i++)
	{
		material = self->model->materials.array[i];
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
/*			if (texture->type == LIMDL_TEXTURE_TYPE_ENVMAP)
				texture->texture = self->cubemap.map;*/
		}
	}

	return 1;

error:
	lisys_error_report ();
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT (6, self->cubemap.fbo);
	glDeleteTextures (1, &self->cubemap.depth);
	glDeleteTextures (1, &self->cubemap.map);
	memset (self->cubemap.fbo, 0, 6 * sizeof (GLuint));
	self->cubemap.depth = 0;
	self->cubemap.map = 0;
#endif

	return 1;
}

static void private_envmap_update (
	LIRenObject32* self)
{
#if 0
	int i;
	LIAlgU32dicIter iter;
	LIMatFrustum frustum;
	LIMatVector ctr;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenContext32* context;
	const LIMatVector dir[6] =
	{
		{ 1.0f, 0.0f, 0.0f }, /* Back. */
		{ -1.0f, 0.0f, 0.0f }, /* Front. */
		{ 0.0f, 1.0f, 0.0f }, /* Up. */
		{ 0.0f, -1.0f, 0.0f }, /* Down. */
		{ 0.0f, 0.0f, 1.0f }, /* Left. */
		{ 0.0f, 0.0f, -1.0f } /* Right. */
	};
	const LIMatVector up[6] =
	{
		{ 0.0f, -1.0f, 0.0f }, /* Back. */
		{ 0.0f, -1.0f, 0.0f }, /* Front. */
		{ 0.0f, 0.0f, 1.0f }, /* Up. */
		{ 0.0f, 0.0f, -1.0f }, /* Down. */
		{ 0.0f, -1.0f, 0.0f }, /* Left. */
		{ 0.0f, -1.0f, 0.0f } /* Right. */
	};

	if (!self->cubemap.map)
		return;
	modelview = self->orientation.matrix;
	projection = limat_matrix_perspective (0.5 * M_PI, 1.0f, 1.0f, 100.0f);
	liren_object32_get_center (self, &ctr);

	/* Enable cube map rendering mode. */
	glPushAttrib (GL_VIEWPORT_BIT);
	glViewport (0, 0, self->cubemap.width, self->cubemap.height);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
	glFrontFace (GL_CCW);
	glDepthFunc (GL_LEQUAL);
	glBindTexture (GL_TEXTURE_2D, 0);

	/* Render each cube face. */
	for (i = 0 ; i < 6 ; i++)
	{
		modelview = limat_matrix_look (
			ctr.x, ctr.y, ctr.z,
			dir[i].x, dir[i].y, dir[i].z,
			up[i].x, up[i].y, up[i].z);
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->cubemap.fbo[i]);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		limat_frustum_init (&frustum, &modelview, &projection);
		context = liren_render32_get_context (self->scene->render);
		liren_context32_set_scene (context, self->scene);
		liren_context32_set_viewmatrix (context, &modelview);
		liren_context32_set_projection (context, &projection);
		liren_context32_set_frustum (context, &frustum);
		LIALG_U32DIC_FOREACH (iter, self->scene->objects)
			liren_draw_exclude (context, iter.value, self);
	}

	/* Disable cube map rendering mode. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib ();
#endif
}

static void private_lights_clear (
	LIRenObject32* self)
{
	int i;

	for (i = 0 ; i < self->lights.count ; i++)
	{
		if (self->lights.array[i] != NULL)
		{
			liren_lighting32_remove_light (self->scene->lighting, self->lights.array[i]);
			liren_light32_free (self->lights.array[i]);
		}
	}
	lisys_free (self->lights.array);
	self->lights.array = NULL;
	self->lights.count = 0;
}

static int private_lights_create (
	LIRenObject32* self,
	LIMdlPose*     pose)
{
	int i;
	LIMdlNode* node;
	LIMdlNodeIter iter;
	LIRenLight32* light;

	/* Create light sources. */
	if (pose != NULL)
	{
		LIMDL_FOREACH_NODE (iter, &pose->nodes)
		{
			node = iter.value;
			if (node->type != LIMDL_NODE_LIGHT)
				continue;
			light = liren_light32_new_from_model (self->scene, node);
			if (light == NULL)
				return 0;
			if (!lialg_array_append (&self->lights, &light))
			{
				liren_light32_free (light);
				return 0;
			}
		}
	}

	/* Register light sources. */
	for (i = 0 ; i < self->lights.count ; i++)
	{
		light = self->lights.array[i];
		if (!liren_lighting32_insert_light (self->scene->lighting, light))
		{
			while (i--)
			{
				light = self->lights.array[i];
				liren_lighting32_remove_light (self->scene->lighting, light);
			}
			return 0;
		}
	}

	return 1;
}

static void private_lights_update (
	LIRenObject32* self)
{
	int i;
	float scale;
	LIMatTransform transform;
	LIRenLight32* light;

	for (i = 0 ; i < self->lights.count ; i++)
	{
		light = self->lights.array[i];
		if (light->node != NULL)
		{
			limdl_node_get_world_transform (light->node, &scale, &transform);
			transform = limat_transform_multiply (self->transform, transform);
			liren_light32_set_transform (light, &transform);
		}
	}
}

/** @} */
/** @} */
