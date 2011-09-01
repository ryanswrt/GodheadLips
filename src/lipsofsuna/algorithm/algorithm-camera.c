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
 * \addtogroup LIAlg Algorithm
 * @{
 * \addtogroup LIAlgCamera Camera
 * @{
 */

#include <lipsofsuna/system.h>
#include "algorithm-camera.h"

#define LIALG_CAMERA_DEFAULT_FOV (M_PI / 5.0f)
#define LIALG_CAMERA_DEFAULT_NEAR 1.0f
#define LIALG_CAMERA_DEFAULT_FAR 75.0f
#define LIALG_CAMERA_DEFAULT_ZOOM 14.0f
#define LIALG_CAMERA_INTERPOLATION_WARP 50.0f
#define LIALG_CAMERA_MINIMUM_ZOOM 1.5f
#define LIALG_CAMERA_MAXIMUM_ZOOM 100.0f
#define LIALG_CAMERA_SENSITIVITY_ZOOM 1.0f
#define LIALG_CAMERA_SMOOTHING_TIMESTEP (1.0f / 60.0f)

static void private_update_1st_person (
	LIAlgCamera* self);

static void private_update_3rd_person (
	LIAlgCamera* self,
	float        dist);

static void private_update_modelview (
	LIAlgCamera* self);

static void private_update_orientation (
	LIAlgCamera* self,
	float        secs);

static void private_update_projection (
	LIAlgCamera* self);

/*****************************************************************************/

/**
 * \brief Creates a new camera.
 * \return New camera or NULL.
 */
LIAlgCamera* lialg_camera_new ()
{
	LIAlgCamera* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIAlgCamera));
	if (self == NULL)
		return NULL;
	self->config.collision_group = 0xFFFF;
	self->config.collision_mask = 0xFFFF;
	self->config.driver = LIALG_CAMERA_FIRSTPERSON;
	self->config.distance = LIALG_CAMERA_DEFAULT_ZOOM;
	self->smoothing.pos = 0.5f;
	self->smoothing.rot = 0.5f;
	self->transform.center = limat_transform_identity ();
	self->transform.current = limat_transform_identity ();
	self->transform.local = limat_transform_identity ();
	self->transform.target = limat_transform_identity ();
	self->transform.inverse = limat_transform_identity ();
	self->view.fov = LIALG_CAMERA_DEFAULT_FOV;
	self->view.nearplane = LIALG_CAMERA_DEFAULT_NEAR;
	self->view.farplane = LIALG_CAMERA_DEFAULT_FAR;
	self->view.aspect = 1.0f;

	private_update_modelview (self);
	private_update_projection (self);

	return self;
}

/**
 * \brief Frees the camera.
 * \param self Camera.
 */
void lialg_camera_free (
	LIAlgCamera* self)
{
	lisys_free (self);
}

/**
 * \brief Moves the camera by the specified amount.
 * \param self Camera.
 * \param value Movement amount.
 */
void lialg_camera_move (
	LIAlgCamera* self,
	float        value)
{
	LIMatVector dir;
	LIMatVector src;
	LIMatVector dst;
	LIMatQuaternion rot;

	/* Calculate eye position. */
	rot = limat_quaternion_conjugate (self->transform.current.rotation);
	dir = limat_quaternion_get_basis (rot, 2);
	src = self->transform.current.position;
	dst = limat_vector_add (src, limat_vector_multiply (dir, -value));
	self->transform.target.position = dst;
	private_update_modelview (self);
}

/**
 * \brief Projects a point to the viewport plane of the camera.
 * \param self Camera.
 * \param object Point in object space.
 * \param window Return location for a point in window space.
 * \return Nonzero on success.
 */
int lialg_camera_project (
	LIAlgCamera*       self,
	const LIMatVector* object,
	LIMatVector*       window)
{
	return limat_matrix_project (
		self->view.projection, self->view.modelview,
		self->view.viewport, object, window);
}

/**
 * \brief Tilts the camera by the specified amount.
 * \param self Camera.
 * \param value Rotation in radians.
 */
void lialg_camera_tilt (
	LIAlgCamera* self,
	float        value)
{
	LIMatQuaternion rot;
	LIMatTransform transform;
	LIMatVector axis;

	axis = limat_vector_init (1.0f, 0.0f, 0.0f);
	rot = limat_quaternion_rotation (value, axis);
	if (self->config.driver == LIALG_CAMERA_MANUAL)
	{
		transform = limat_convert_quaternion_to_transform (rot);
		transform = limat_transform_multiply (self->transform.target, transform);
		transform.rotation = limat_quaternion_normalize (transform.rotation);
		self->transform.target = transform;
	}
	else
	{
		transform = limat_convert_quaternion_to_transform (rot);
		transform = limat_transform_multiply (self->transform.local, transform);
		transform.rotation = limat_quaternion_normalize (transform.rotation);
		self->transform.local = transform;
	}
	private_update_modelview (self);
}

/**
 * \brief Turns the camera by the specified amount.
 * \param self Camera.
 * \param value Rotation in radians.
 */
void lialg_camera_turn (
	LIAlgCamera* self,
	float        value)
{
	LIMatQuaternion rot;
	LIMatTransform transform;
	LIMatVector axis;

	if (self->config.driver == LIALG_CAMERA_MANUAL)
	{
		rot = limat_quaternion_conjugate (self->transform.target.rotation);
		axis = limat_vector_init (0.0f, 1.0f, 0.0f);
		axis = limat_quaternion_transform (rot, axis);
		rot = limat_quaternion_rotation (value, axis);
		transform = limat_convert_quaternion_to_transform (rot);
		transform = limat_transform_multiply (self->transform.target, transform);
		transform.rotation = limat_quaternion_normalize (transform.rotation);
		self->transform.target = transform;
	}
	else
	{
		rot = limat_quaternion_conjugate (self->transform.local.rotation);
		axis = limat_vector_init (0.0f, 1.0f, 0.0f);
		axis = limat_quaternion_transform (rot, axis);
		rot = limat_quaternion_rotation (value, axis);
		transform = limat_convert_quaternion_to_transform (rot);
		transform = limat_transform_multiply (self->transform.local, transform);
		transform.rotation = limat_quaternion_normalize (transform.rotation);
		self->transform.local = transform;
	}
	private_update_modelview (self);
}

/**
 * \brief Projects a point on the viewport plane to the scene.
 * \param self Camera.
 * \param object Point in viewport.
 * \param window Return location for a point in world space.
 * \return Nonzero on success.
 */
int lialg_camera_unproject (
	LIAlgCamera*       self,
	const LIMatVector* window,
	LIMatVector*       object)
{
	return limat_matrix_unproject (
		self->view.projection, self->view.modelview,
		self->view.viewport, window, object);
}

/**
 * \brief Updates the position of the camera.
 * \param self Camera.
 * \param secs Number of seconds since the last update.
 */
void lialg_camera_update (
	LIAlgCamera* self,
	float        secs)
{
	lialg_camera_move (self, secs * self->controls.move_rate);
	lialg_camera_tilt (self, secs * self->controls.tilt_rate);
	lialg_camera_turn (self, secs * self->controls.turn_rate);
	lialg_camera_zoom (self, secs * self->controls.zoom_rate);
	switch (self->config.driver)
	{
		case LIALG_CAMERA_FIRSTPERSON:
			private_update_1st_person (self);
			private_update_orientation (self, secs);
			private_update_modelview (self);
			break;
		case LIALG_CAMERA_THIRDPERSON:
			private_update_3rd_person (self, self->config.distance);
			private_update_orientation (self, secs);
			private_update_modelview (self);
			break;
		default:
			private_update_orientation (self, secs);
			private_update_modelview (self);
			break;
	}
}

/**
 * \brief Warps the camera to the target position.
 * \param self Camera.
 */
void lialg_camera_warp (
	LIAlgCamera* self)
{
	switch (self->config.driver)
	{
		case LIALG_CAMERA_FIRSTPERSON:
			private_update_1st_person (self);
			break;
		case LIALG_CAMERA_THIRDPERSON:
			private_update_3rd_person (self, self->config.distance);
			break;
		default:
			break;
	}
	self->transform.current = self->transform.target;
	private_update_modelview (self);
}

/**
 * \brief Zooms in or out.
 * \param self Camera.
 * \param value Amount and direction of zoom.
 */
void lialg_camera_zoom (
	LIAlgCamera* self,
	float        value)
{
	self->config.distance += value * LIALG_CAMERA_SENSITIVITY_ZOOM;
	if (self->config.distance < LIALG_CAMERA_MINIMUM_ZOOM)
		self->config.distance = LIALG_CAMERA_MINIMUM_ZOOM;
	if (self->config.distance > LIALG_CAMERA_MAXIMUM_ZOOM)
		self->config.distance = LIALG_CAMERA_MAXIMUM_ZOOM;
	private_update_modelview (self);
}

/**
 * \brief Gets the size of the camera.
 * \param self Camera.
 * \param aabb Return location for the bounding box.
 */
void lialg_camera_get_bounds (
	const LIAlgCamera* self,
	LIMatAabb*         aabb)
{
	float max;
	float top;
	float right;
	float nearplane;
	LIMatVector size;
	LIMatVector zero;

	nearplane = self->view.nearplane;
	top = tan (self->view.fov * M_PI / 360.0f) * nearplane;
	right = top * self->view.aspect;

	max = 1.7f * LIMAT_MAX (LIMAT_MAX (top, right), nearplane);
	size = limat_vector_init (max, max, max);
	zero = limat_vector_init (0.0f, 0.0f, 0.0f);
	limat_aabb_init_from_center (aabb, &zero, &size);
}

/**
 * \brief Gets the point of interest for automatic camera modes.
 * \param self Camera.
 * \param result Return location for transformation.
 */
void lialg_camera_get_center (
	LIAlgCamera*    self,
	LIMatTransform* result)
{
	*result = self->transform.center;
}

/**
 * \brief Sets the point of interest for automatic camera modes.
 * \param self Camera.
 * \param value Center transformation.
 */
void lialg_camera_set_center (
	LIAlgCamera*          self,
	const LIMatTransform* value)
{
	self->transform.center = *value;
}

/**
 * \brief Sets the external clipping function of the camera.
 *
 * When the clipping function is set, the camera ensures that its distance to the
 * target in the third person mode isn't more than that returned by the clip
 * function.
 *
 * \param self Camera.
 * \param func Clipping function or NULL to disable.
 * \param data Userdata to be passed to the clipping function.
 */
void lialg_camera_set_clipping (
	LIAlgCamera*    self,
	LIAlgCameraClip func,
	void*           data)
{
	self->config.clip_func = func;
	self->config.clip_data = data;
}

/**
 * \brief Gets the driver type of the camera.
 * \param self Camera.
 * \return Camera driver type.
 */
LIAlgCameraDriver lialg_camera_get_driver (
	LIAlgCamera* self)
{
	return self->config.driver;
}

/**
 * \brief Sets the driver type of the camera.
 * \param self Camera.
 * \param value Camera driver type.
 */
void lialg_camera_set_driver (
	LIAlgCamera*      self,
	LIAlgCameraDriver value)
{
	self->config.driver = value;
}

/**
 * \brief Sets the far plane of the camera.
 * \param self Camera.
 * \param value Far plane distance.
 */
void lialg_camera_set_far (
	LIAlgCamera* self,
	float        value)
{
	self->view.farplane = value;
	private_update_projection (self);
}

/**
 * \brief Gets the field of view of the camera.
 * \param self Camera.
 * \return Field of view in radians.
 */
float lialg_camera_get_fov (
	const LIAlgCamera* self)
{
	return self->view.fov;
}

/**
 * \brief Sets the field of view of the camera.
 * \param self Camera.
 * \param value Field of view in radians.
 */
void lialg_camera_set_fov (
	LIAlgCamera* self,
	float        value)
{
	self->view.fov = value;
	private_update_projection (self);
}

/**
 * \brief Gets the frustum of the camera.
 * \param self Camera.
 * \param result Return location for frustum.
 */
void lialg_camera_get_frustum (
	const LIAlgCamera* self,
	LIMatFrustum*      result)
{
	limat_frustum_init (result,
		&self->view.modelview,
		&self->view.projection);
}

/**
 * \brief Gets the modelview matrix of the camera.
 * \param self Camera.
 * \param value Return location for the matrix.
 */
void lialg_camera_get_modelview (
	const LIAlgCamera* self,
	LIMatMatrix*       value)
{
	*value = self->view.modelview;
}

/**
 * \brief Sets the near plane of the camera.
 * \param self Camera.
 * \param value Near plane distance.
 */
void lialg_camera_set_near (
	LIAlgCamera* self,
	float        value)
{
	self->view.nearplane = value;
	private_update_projection (self);
}

/**
 * \brief Gets the projection matrix of the camera.
 * \param self Camera.
 * \param value Return location for the matrix.
 */
void lialg_camera_get_projection (
	const LIAlgCamera* self,
	LIMatMatrix*          value)
{
	*value = self->view.projection;
}

/**
 * \brief Sets the projection settings of the camera.
 * \param self Camera.
 * \param fov Field of view.
 * \param aspect Ascpect ratio of the viewport.
 * \param nearplane Near plane distance.
 * \param farplane Far plane distance.
 */
void lialg_camera_set_projection (
	LIAlgCamera* self,
	float        fov,
	float        aspect,
	float        nearplane,
	float        farplane)
{
	self->view.fov = fov;
	self->view.aspect = aspect;
	self->view.nearplane = nearplane;
	self->view.farplane = farplane;
	private_update_projection (self);
}

/**
 * \brief Gets the smoothing rates of the camera.
 * \param self Camera.
 * \param pos Return location for the position smoothing rate.
 * \param rot Return location for the rotation smoothing rate.
 */
void lialg_camera_get_smoothing (
	const LIAlgCamera* self,
	float*             pos,
	float*             rot)
{
	*pos = self->smoothing.pos;
	*rot = self->smoothing.rot;
}

/**
 * \brief Sets the smoothing rates of the camera.
 * \param self Camera.
 * \param pos Position smoothing rate.
 * \param rot Rotation smoothing rate.
 */
void lialg_camera_set_smoothing (
	LIAlgCamera* self,
	float        pos,
	float        rot)
{
	self->smoothing.pos = pos;
	self->smoothing.rot = rot;
}

/**
 * \brief Gets the current transformation of the camera.
 *
 * \param self Camera.
 * \param value Return location for the transformation.
 */
void lialg_camera_get_transform (
	const LIAlgCamera* self,
	LIMatTransform*    value)
{
	*value = limat_transform_init (
		self->transform.current.position,
		self->transform.current.rotation);
}

/**
 * \brief Sets the target transformation of the camera.
 * \param self Camera.
 * \param value Transformation.
 */
void lialg_camera_set_transform (
	LIAlgCamera*          self,
	const LIMatTransform* value)
{
	self->transform.target = *value;
	private_update_modelview (self);
}

/**
 * \brief Gets the up vector of the camera.
 * \param self Camera.
 * \param result Return location for the up vector.
 */
void lialg_camera_get_up (
	const LIAlgCamera* self,
	LIMatVector*       result)
{
	result->x = self->view.modelview.m[1];
	result->y = self->view.modelview.m[4];
	result->z = self->view.modelview.m[9];
}

/**
 * \brief Sets the viewport of the camera.
 * \param self Camera.
 * \param x Left border of the viewport.
 * \param y Bottom border of the viewport.
 * \param width Width of the viewport.
 * \param height Height of the viewport.
 */
void lialg_camera_set_viewport (
	LIAlgCamera* self,
	int          x,
	int          y,
	int          width,
	int          height)
{
	self->view.viewport[0] = x;
	self->view.viewport[1] = y;
	self->view.viewport[2] = width;
	self->view.viewport[3] = height;
	self->view.aspect = (float) width / height;
	private_update_projection (self);
}

/*****************************************************************************/

static void private_update_1st_person (
	LIAlgCamera* self)
{
	/* Copy center position and rotation. */
	self->transform.target = self->transform.center;
	self->transform.current.position = self->transform.target.position;

	/* Apply local rotation. */
	self->transform.target = limat_transform_multiply (self->transform.target, self->transform.local);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);
}

static void private_update_3rd_person (
	LIAlgCamera* self,
	float        dist)
{
	float frac;
	LIMatTransform transform;
	LIMatTransform target;
	LIMatTransform center;

	/* Copy center position and rotation. */
	center = self->transform.center;

	/* Apply local rotation. */
	center = limat_transform_multiply (center, self->transform.local);
	center.rotation = limat_quaternion_normalize (center.rotation);

	/* Project backwards. */
	transform = limat_transform_init (
		limat_vector_init (0.0f, 0.0f, dist),
		limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f));
	target = limat_transform_multiply (center, transform);
	target.rotation = limat_quaternion_normalize (target.rotation);

	/* Apply clipping. */
	if (self->config.clip_func != NULL)
	{
		frac = self->config.clip_func (self->config.clip_data, self, &center, &target);
		transform = limat_transform_init (
			limat_vector_init (0.0f, 0.0f, dist * frac),
			limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f));
		target = limat_transform_multiply (center, transform);
		target.rotation = limat_quaternion_normalize (target.rotation);
	}

	/* Set the target position. */
	self->transform.target = target;
}

static void private_update_modelview (
	LIAlgCamera* self)
{
	LIMatTransform t;

	self->transform.current.rotation = limat_quaternion_normalize (self->transform.current.rotation);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);
	t = limat_transform_invert (self->transform.current);
	self->transform.inverse = t;
	self->view.modelview = limat_convert_transform_to_matrix (t);
}

static void private_update_orientation (
	LIAlgCamera* self,
	float        secs)
{
	float dist;
	LIMatVector disp;
	LIMatTransform transform;
	LIMatTransform transform0;
	LIMatTransform transform1;

	/* Update timer. */
	self->smoothing.timer += secs;
	if (self->smoothing.timer < LIALG_CAMERA_SMOOTHING_TIMESTEP)
		return;

	/* Check if it'd be better to warp than interpolate. */
	disp = limat_vector_subtract (self->transform.target.position, self->transform.current.position);
	dist = limat_vector_get_length (disp);
	if (self->smoothing.timer >= 1.0f || dist >= LIALG_CAMERA_INTERPOLATION_WARP)
	{
		self->transform.current = self->transform.target;
		self->smoothing.timer = 0.0f;
		return;
	}

	/* Interpolate the transformation. */
	while (self->smoothing.timer >= LIALG_CAMERA_SMOOTHING_TIMESTEP)
	{
		transform0 = self->transform.current;
		transform1 = self->transform.target;
		transform0.rotation = limat_quaternion_get_nearest (transform0.rotation, transform1.rotation);
		transform.position = limat_vector_lerp (
			transform1.position, transform0.position, self->smoothing.pos);
		transform.rotation = limat_quaternion_nlerp (
			transform1.rotation, transform0.rotation, self->smoothing.rot);
		self->transform.current = transform;
		self->smoothing.timer -= LIALG_CAMERA_SMOOTHING_TIMESTEP;
	}
}

static void private_update_projection (
	LIAlgCamera* self)
{
	self->view.projection = limat_matrix_perspective (
		self->view.fov, self->view.aspect,
		self->view.nearplane, self->view.farplane);
}

/** @} */
/** @} */
