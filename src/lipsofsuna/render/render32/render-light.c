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
 * \addtogroup LIRenLight32 Light32
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-context.h"
#include "render-light.h"
#include "render-lighting.h"
#include "render-private.h"
#include "../render-scene.h"
#include "../render-private.h"

#define LIGHT_CONTRIBUTION_EPSILON 0.001f

static void private_update_bounds (
	LIRenLight32* self);

static void private_update_shadow (
	LIRenLight32* self);

/*****************************************************************************/

/**
 * \brief Creates a new light source.
 * \param scene Scene.
 * \param ambient Ambient color, array of 4 floats.
 * \param diffuse Diffuse color, array of 4 floats.
 * \param specular Specular color, array of 4 floats.
 * \param equation Attenuation equation, array of 3 floats.
 * \param cutoff Spot cutoff in radians.
 * \param exponent Spot expoent.
 * \param shadows Nonzero if the lamp casts shadows.
 * \return New light source or NULL.
 */
LIRenLight32* liren_light32_new (
	LIRenScene32* scene,
	const float*  ambient,
	const float*  diffuse,
	const float*  specular,
	const float*  equation,
	float         cutoff,
	float         exponent,
	int           shadows)
{
	LIRenLight32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenLight32));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	self->ambient[0] = ambient[0];
	self->ambient[1] = ambient[1];
	self->ambient[2] = ambient[2];
	self->ambient[3] = 1.0f;
	self->diffuse[0] = diffuse[0];
	self->diffuse[1] = diffuse[1];
	self->diffuse[2] = diffuse[2];
	self->diffuse[3] = 1.0f;
	self->specular[0] = specular[0];
	self->specular[1] = specular[1];
	self->specular[2] = specular[2];
	self->specular[3] = 1.0f;
	self->equation[0] = equation[0];
	self->equation[1] = equation[1];
	self->equation[2] = equation[2];
	self->cutoff = cutoff;
	self->exponent = exponent;
	self->shadow_far = 50.0f;
	self->shadow_near = 0.1f;
	self->projection = limat_matrix_identity ();
	self->modelview = limat_matrix_identity ();
	self->modelview_inverse = limat_matrix_identity ();
	self->transform = limat_transform_identity ();
	liren_light32_update_projection (self);
	liren_light32_set_shadow (self, shadows);

	return self;
}

/**
 * \brief Creates a new directional light source.
 * \param scene Scene.
 * \param ambient Ambient color, array of 4 floats.
 * \param diffuse Diffuse color, array of 4 floats.
 * \param specular Specular color, array of 4 floats.
 * \return New light source or NULL.
 */
LIRenLight32* liren_light32_new_directional (
	LIRenScene32* scene,
	const float*  ambient,
	const float*  diffuse,
	const float*  specular)
{
	const float equation[3] = { 1.0f, 0.0f, 0.0f };
	const LIMatVector direction = { 0.0f, 0.0f, -1.0f };
	LIRenLight32* self;

	/* Create the sun.  */
	self = liren_light32_new (scene, ambient, diffuse, specular, equation, M_PI, 0.0f, 0);
	if (self == NULL)
		return NULL;
	liren_light32_set_direction (self, &direction);

	return self;
}

/**
 * \brief Creates a new light from a model light.
 * \param scene Scene.
 * \param light Model light.
 * \return New light or NULL.
 */
LIRenLight32* liren_light32_new_from_model (
	LIRenScene32*    scene,
	const LIMdlNode* light)
{
	float scale;
	LIMatMatrix projection;
	LIMatTransform transform;
	LIRenLight32* self;

	/* Allocate self. */
	self = liren_light32_new (scene, light->light.ambient,
		light->light.diffuse, light->light.specular, light->light.equation,
		light->light.spot.cutoff, light->light.spot.exponent,
		light->light.flags & LIMDL_LIGHT_FLAG_SHADOW);
	if (self == NULL)
		return NULL;

	/* Set transform. */
	self->node = light;
	limdl_node_get_world_transform (light, &scale, &transform);
	liren_light32_set_transform (self, &transform);
	limdl_light_get_projection (light, &projection);
	liren_light32_set_projection (self, &projection);

	return self;
}

/**
 * \brief Frees a light source.
 * \param self Light source.
 */
void liren_light32_free (
	LIRenLight32* self)
{
	liren_light32_set_shadow (self, 0);
	liren_lighting32_remove_light (self->scene->lighting, self);
	lisys_free (self);
}

/**
 * \brief Compares the priorities of two lights.
 * \param self Light source.
 * \param light Light source.
 * \return Integer indicating which light contributes more.
 */
int liren_light32_compare (
	const LIRenLight32* self,
	const LIRenLight32* light)
{
	/* Sorting is done primarily by priority. */
	/* This allows the user to decide what lights are the most important
	   ones. If there's an ambient light in the scene, for example, the
	   user may want to give that higher priority than other lights. */
	if (self->priority < light->priority)
		return -1;
	if (self->priority > light->priority)
		return 1;

	return 0;
}

void liren_light32_update (
	LIRenLight32* self)
{
	if (self->shadow.map)
		private_update_shadow (self);
}

/**
 * \brief Caches the lighting values needed by shaders for fast access.
 * \param self Light.
 * \param context Context.
 */
void liren_light32_update_cache (
	LIRenLight32*   self,
	LIRenContext32* context)
{
	LIMatMatrix matrix;
	LIMatVector vector;
	LIMatMatrix bias =
	{{
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	}};

	/* Calculate position vectors. */
	vector = self->transform.position;
	self->cache.pos_world[0] = vector.x;
	self->cache.pos_world[1] = vector.y;
	self->cache.pos_world[2] = vector.z;
	vector = limat_matrix_transform (context->matrix.modelview, vector);
	self->cache.pos_view[0] = vector.x;
	self->cache.pos_view[1] = vector.y;
	self->cache.pos_view[2] = vector.z;

	/* Calculate direction vectors. */
	liren_light32_get_direction (self, &vector);
	self->cache.dir_world[0] = vector.x;
	self->cache.dir_world[1] = vector.y;
	self->cache.dir_world[2] = vector.z;
	matrix = limat_matrix_get_rotation (context->matrix.modelview);
	vector = limat_matrix_transform (matrix, vector);
	self->cache.dir_view[0] = vector.x;
	self->cache.dir_view[1] = vector.y;
	self->cache.dir_view[2] = vector.z;

	/* Calculate shadow buffer matrix. */
	matrix = limat_matrix_multiply (bias, context->matrix.projection);
	matrix = limat_matrix_multiply (matrix, context->matrix.view);
	matrix = limat_matrix_multiply (matrix, context->matrix.modelviewinverse);
	self->cache.matrix = matrix;

	/* Calculate spot light settings. */
	self->cache.spot[0] = self->cutoff;
	self->cache.spot[1] = cos (self->cutoff);
	self->cache.spot[2] = self->exponent;
}

void liren_light32_update_projection (
	LIRenLight32* self)
{
	self->projection = limat_matrix_perspective (2.0f * self->cutoff * M_PI,
		1.0f, self->shadow_near, self->shadow_far);
}

void liren_light32_get_ambient (
	LIRenLight32* self,
	float*        value)
{
	memcpy (value, self->ambient, 4 * sizeof (float));
}

void liren_light32_set_ambient (
	LIRenLight32* self,
	const float*  value)
{
	memcpy (self->ambient, value, 4 * sizeof (float));
}

/**
 * \brief Gets the area of effect of the light source.
 * \param self Light source.
 * \param result Return location for a bounding box.
 * \return Nonzero if has bounds.
 */
int liren_light32_get_bounds (
	const LIRenLight32* self,
	LIMatAabb*          result)
{
	*result = self->bounds;

	return 1;
}

void liren_light32_get_diffuse (
	LIRenLight32* self,
	float*        value)
{
	memcpy (value, self->diffuse, 4 * sizeof (float));
}

void liren_light32_set_diffuse (
	LIRenLight32* self,
	const float*  value)
{
	memcpy (self->diffuse, value, 4 * sizeof (float));
}

/**
 * \brief Gets the forwards direction of the light.
 *
 * The direction is derived from the transformation of the light.
 *
 * \param self Light source.
 * \param value Return location for the direction.
 */
void liren_light32_get_direction (
	const LIRenLight32* self,
	LIMatVector*        value)
{
	*value = limat_vector_init (0.0f, 0.0f, -1.0f);
	*value = limat_quaternion_transform (self->transform.rotation, *value);
}

/**
 * \brief Makes the light directional and sets it direction.
 * \param self Light source.
 * \param value Light direction.
 */
void liren_light32_set_direction (
	LIRenLight32*      self,
	const LIMatVector* value)
{
	float a;
	float b;
	LIMatMatrix projection;
	LIMatQuaternion rotation;
	LIMatTransform transform;
	LIMatVector direction;
	LIMatVector position;

	/* Calculate temporary position. */
	/* FIXME: Temporary position not supported. */
	direction = *value;
	position = limat_vector_init (0.0f, 0.0f, 0.0f);

	/* Calculate light rotation. */
	a = limat_vector_dot (direction, limat_vector_init (0.0f, 1.0f, 0.0f));
	b = limat_vector_dot (direction, limat_vector_init (0.0f, 0.0f, 1.0f));
	if (LIMAT_ABS (a) >= LIMAT_ABS (b))
		rotation = limat_quaternion_look (direction, limat_vector_init (0.0f, 1.0f, 0.0f));
	else
		rotation = limat_quaternion_look (direction, limat_vector_init (0.0f, 0.0f, 1.0f));

	/* Set light transformation. */
	transform = limat_transform_init (position, rotation);
	liren_light32_set_transform (self, &transform);
	projection = limat_matrix_ortho (200, -200, 200, -200, -1000, 1000);
	liren_light32_set_projection (self, &projection);
	self->directional = 1;
	private_update_bounds (self);
}

/**
 * \brief Enables or disables directional mode.
 * \param self Light source.
 * \param value Nonzero if the light is directional.
 */
void liren_light32_set_directional (
	LIRenLight32* self,
	int           value)
{
	self->directional = (value != 0);
}

/**
 * \brief Checks if the light is registered.
 * \param self Light source.
 * \return Nonzero if registered.
 */
int liren_light32_get_enabled (
	const LIRenLight32* self)
{
	return self->enabled;
}

void liren_light32_get_equation (
	LIRenLight32* self,
	float*        value)
{
	memcpy (value, self->equation, 3 * sizeof (float));
}

void liren_light32_set_equation (
	LIRenLight32* self,
	const float*  value)
{
	memcpy (self->equation, value, 3 * sizeof (float));
}

/**
 * \brief Gets the modelview matrix of the light.
 * \param self Light source.
 * \param value Return location for the matrix.
 */
void liren_light32_get_modelview (
	const LIRenLight32* self,
	LIMatMatrix*        value)
{
	*value = self->modelview;
}

/**
 * \brief Gets the OpenGL position vector for the light.
 * \param self Light source.
 * \param value Return location for 4 floats.
 */
void liren_light32_get_position (
	const LIRenLight32* self,
	GLfloat*            value)
{
	LIMatVector tmp;

	if (self->directional)
	{
		liren_light32_get_direction (self, &tmp);
		value[0] = -tmp.x;
		value[1] = -tmp.y;
		value[2] = -tmp.z;
		value[3] = 0.0f;
	}
	else
	{
		value[0] = self->transform.position.x;
		value[1] = self->transform.position.y;
		value[2] = self->transform.position.z;
		value[3] = 1.0f;
	}
}

/**
 * \brief Gets the priority of the light.
 * \param self Light source.
 * \return Priority value, higher means more important.
 */
float liren_light32_get_priority (
	LIRenLight32* self)
{
	return self->priority;
}

/**
 * \brief Sets the priority of the light.
 * \param self Light source.
 * \param value Priority value, higher means more important.
 */
void liren_light32_set_priority (
	LIRenLight32* self,
	float         value)
{
	self->priority = value;
}

/**
 * \brief Gets the projection matrix of the light.
 * \param self Light source.
 * \param value Return location for the matrix.
 */
void liren_light32_get_projection (
	const LIRenLight32* self,
	LIMatMatrix*        value)
{
	*value = self->projection;
}

/**
 * \brief Sets the projection matrix of the light.
 * \param self Light source.
 * \param value Matrix to set.
 */
void liren_light32_set_projection (
	LIRenLight32*      self,
	const LIMatMatrix* value)
{
	self->projection = *value;
	private_update_bounds (self);
}

LIRenScene32* liren_light32_get_scene (
	const LIRenLight32* self)
{
	return self->scene;
}

/**
 * \brief Gets the shadow casting mode of the light.
 * \param self Light source.
 * \return Nonzero if shadow casting is allowed, zero if disabled.
 */
int liren_light32_get_shadow (
	const LIRenLight32* self)
{
	return self->shadow.fbo != 0;
}

float liren_light32_get_shadow_far (
	const LIRenLight32* self)
{
	return self->shadow_far;
}

void liren_light32_set_shadow_far (
	LIRenLight32* self,
	float         value)
{
	self->shadow_far = value;
	liren_light32_update_projection (self);
}

float liren_light32_get_shadow_near (
	const LIRenLight32* self)
{
	return self->shadow_near;
}

void liren_light32_set_shadow_near (
	LIRenLight32* self,
	float         value)
{
	self->shadow_near = value;
	liren_light32_update_projection (self);
}

/**
 * \brief Sets the shadow casting mode of the light.
 * \param self Light source.
 * \param value Nonzero to allow shadow casting, zero to disable.
 */
void liren_light32_set_shadow (
	LIRenLight32* self,
	int           value)
{
	float border[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	if ((value != 0) == (self->shadow.fbo != 0))
		return;
	if (value)
	{
		/* Create shadow texture. */
		glGenTextures (1, &self->shadow.map);
		glBindTexture (GL_TEXTURE_2D, self->shadow.map);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOWMAPSIZE, SHADOWMAPSIZE,
			0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

		/* Create framebuffer object. */
		glGenFramebuffers (1, &self->shadow.fbo);
		glBindFramebuffer (GL_FRAMEBUFFER, self->shadow.fbo);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, self->shadow.map, 0);
		glDrawBuffer (GL_NONE);
		glReadBuffer (GL_NONE);
		switch (glCheckFramebufferStatus (GL_FRAMEBUFFER))
		{
			case GL_FRAMEBUFFER_COMPLETE:
				break;
			default:
				lisys_error_set (ENOTSUP, "cannot create framebuffer object");
				glDeleteFramebuffers (1, &self->shadow.fbo);
				glDeleteTextures (1, &self->shadow.map);
				self->shadow.fbo = 0;
				self->shadow.map = 0;
				break;
		}
		glBindFramebuffer (GL_FRAMEBUFFER, 0);
	}
	else
	{
		/* Delete the shadow map. */
		glDeleteFramebuffers (1, &self->shadow.fbo);
		glDeleteTextures (1, &self->shadow.map);
		self->shadow.fbo = 0;
		self->shadow.map = 0;
	}
}

void liren_light32_get_specular (
	LIRenLight32* self,
	float*        value)
{
	memcpy (value, self->specular, 4 * sizeof (float));
}

void liren_light32_set_specular (
	LIRenLight32* self,
	const float*  value)
{
	memcpy (self->specular, value, 4 * sizeof (float));
}

float liren_light32_get_spot_cutoff (
	const LIRenLight32* self)
{
	return self->cutoff;
}

void liren_light32_set_spot_cutoff (
	LIRenLight32* self,
	float         value)
{
	self->cutoff = value;
	liren_light32_update_projection (self);
}

float liren_light32_get_spot_exponent (
	const LIRenLight32* self)
{
	return self->exponent;
}

void liren_light32_set_spot_exponent (
	LIRenLight32* self,
	float         value)
{
	self->exponent = value;
}

/**
 * \brief Gets the transformation of the light.
 * \param self Light source.
 * \param value Return value for the transformation.
 */
void liren_light32_get_transform (
	LIRenLight32*   self,
	LIMatTransform* value)
{
	*value = self->transform;
}

/**
 * \brief Sets the transformation of the light.
 *
 * This modifies the modelview matrix.
 *
 * \param self Light source.
 * \param transform Transformation.
 */
void liren_light32_set_transform (
	LIRenLight32*         self,
	const LIMatTransform* transform)
{
	LIMatVector dir;
	LIMatVector pos;
	LIMatVector up;

	pos = transform->position;
	dir = limat_quaternion_transform (transform->rotation, limat_vector_init (0.0f, 0.0f, -1.0f));
	up = limat_quaternion_transform (transform->rotation, limat_vector_init (0.0f, 1.0f, 0.0f));
	self->transform = *transform;
	self->modelview = limat_matrix_look (pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, up.x, up.y, up.z);
	self->modelview_inverse = limat_matrix_invert (self->modelview);
	private_update_bounds (self);
}

int liren_light32_get_type (
	const LIRenLight32* self)
{
	if (self->directional)
		return LIREN_LIGHTTYPE_DIRECTIONAL;
	else if (LIMAT_ABS (self->cutoff - M_PI) < 0.001)
		return LIREN_LIGHTTYPE_POINT;
	else if (self->shadow.map)
		return LIREN_LIGHTTYPE_SPOTSHADOW;
	else
		return LIREN_LIGHTTYPE_SPOT;
}

/*****************************************************************************/

static void private_update_bounds (
	LIRenLight32* self)
{
	double a;
	double b;
	double c;
	double r;
	double det;
	double eps;

	/* Choose epsilon. */
	eps = LIMAT_MAX (LIMAT_MAX (self->diffuse[0], self->diffuse[1]), self->diffuse[2]);
	eps /= 256.0;
	if (eps < LIGHT_CONTRIBUTION_EPSILON)
		eps = LIGHT_CONTRIBUTION_EPSILON;

	/* Solve radius. */
	/* 1 / (A * r^2 + B * r + C) = E */
	/* (EA) * r^2 + (EB) * r + (Ec-1) = 0 */
	a = eps * self->equation[2];
	b = eps * self->equation[1];
	c = eps * self->equation[0] - 1.0;
	det = b * b - 4 * a * c;
	if (det < 0.0)
	{
		self->bounds.min = self->transform.position;
		self->bounds.max = self->transform.position;
		return;
	}
	r = (-b + sqrt (det)) / (2.0 * a);

	/* Create bounding box. */
	self->bounds.min = limat_vector_subtract (self->transform.position, limat_vector_init (r, r, r));
	self->bounds.max = limat_vector_add (self->transform.position, limat_vector_init (r, r, r));
}

static void private_update_shadow (
	LIRenLight32* self)
{
	LIAlgU32dicIter iter1;
	LIMatAabb aabb;
	LIMatFrustum frustum;
	LIRenContext32* context;
	LIRenLod32* lod;
	LIRenObject32* object;
	LIRenShader32* shader;

	/* Find the shader. */
	shader = liren_render32_find_shader (self->scene->render, "shadowmap");
	if (shader == NULL)
		return;

	/* Enable depth rendering mode. */
	glPushAttrib (GL_VIEWPORT_BIT | GL_SCISSOR_BIT);
	glBindFramebuffer (GL_FRAMEBUFFER, self->shadow.fbo);
	glViewport (0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);
	glDisable (GL_SCISSOR_TEST);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	limat_frustum_init (&frustum, &self->modelview, &self->projection);
	context = liren_render32_get_context (self->scene->render);
	liren_context32_init (context);
	liren_context32_set_scene (context, self->scene);
	liren_context32_set_cull (context, 1, GL_CCW);
	liren_context32_set_depth (context, 1, 1, GL_LEQUAL);
	liren_context32_set_frustum (context, &frustum);
	liren_context32_set_projection (context, &self->projection);
	liren_context32_set_shader (context, 0, shader);
	liren_context32_set_viewmatrix (context, &self->modelview);

	/* Render objects to the depth texture. */
	LIALG_U32DIC_FOREACH (iter1, ((LIRenScene*) self->scene->scene)->objects)
	{
		object = ((LIRenObject*) iter1.value)->v32;
		if (!liren_object32_get_realized (object))
			continue;
		liren_object32_get_bounds (object, &aabb);
		if (limat_frustum_cull_aabb (&frustum, &aabb))
			continue;
		lod = object->model->lod.array + 0;
		liren_context32_set_modelmatrix (context, &object->orientation.matrix);
		liren_context32_set_mesh (context, &lod->mesh);
		liren_context32_bind (context);
		liren_context32_render_indexed (context, 0, lod->mesh.counts[0]);
	}

	/* Disable depth rendering mode. */
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glPopAttrib ();
}

/** @} */
/** @} */
