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

#ifndef __ALGORITHM_CAMERA_H__
#define __ALGORITHM_CAMERA_H__

#include <lipsofsuna/math.h>

typedef struct _LIAlgCamera LIAlgCamera;
typedef int LIAlgCameraDriver;
typedef float (*LIAlgCameraClip)(void*, LIAlgCamera*, LIMatTransform*, LIMatTransform*);

enum 
{
	LIALG_CAMERA_FIRSTPERSON,
	LIALG_CAMERA_THIRDPERSON,
	LIALG_CAMERA_MANUAL,
	LIALG_CAMERA_MAX
};

struct _LIAlgCamera
{
	struct
	{
		int collision_mask;
		int collision_group;
		float distance;
		LIAlgCameraDriver driver;
		LIAlgCameraClip clip_func;
		void* clip_data;
	} config;
	struct
	{
		float move_rate;
		float tilt_rate;
		float turn_rate;
		float zoom_rate;
	} controls;
	struct
	{
		float pos;
		float rot;
		float timer;
	} smoothing;
	struct
	{
		LIMatTransform center;
		LIMatTransform current;
		LIMatTransform local;
		LIMatTransform target;
		LIMatTransform inverse;
	} transform;
	struct
	{
		float aspect;
		float fov;
		float nearplane;
		float farplane;
		int viewport[4];
		LIMatMatrix modelview;
		LIMatMatrix projection;
	} view;
};

LIAPICALL (LIAlgCamera*, lialg_camera_new, ());

LIAPICALL (void, lialg_camera_free, (
	LIAlgCamera* self));

LIAPICALL (void, lialg_camera_move, (
	LIAlgCamera* self,
	float        value));

LIAPICALL (int, lialg_camera_project, (
	LIAlgCamera*       self,
	const LIMatVector* object,
	LIMatVector*       window));

LIAPICALL (void, lialg_camera_tilt, (
	LIAlgCamera* self,
	float        value));

LIAPICALL (void, lialg_camera_turn, (
	LIAlgCamera* self,
	float        value));

LIAPICALL (int, lialg_camera_unproject, (
	LIAlgCamera*       self,
	const LIMatVector* window,
	LIMatVector*       object));

LIAPICALL (void, lialg_camera_update, (
	LIAlgCamera* self,
	float        secs));

LIAPICALL (void, lialg_camera_warp, (
	LIAlgCamera* self));

LIAPICALL (void, lialg_camera_zoom, (
	LIAlgCamera* self,
	float        value));

LIAPICALL (void, lialg_camera_get_bounds, (
	const LIAlgCamera* self,
	LIMatAabb*         aabb));

LIAPICALL (void, lialg_camera_get_center, (
	LIAlgCamera*    self,
	LIMatTransform* result));

LIAPICALL (void, lialg_camera_set_center, (
	LIAlgCamera*          self,
	const LIMatTransform* value));

LIAPICALL (void, lialg_camera_set_clipping, (
	LIAlgCamera*    self,
	LIAlgCameraClip func,
	void*           data));

LIAPICALL (LIAlgCameraDriver, lialg_camera_get_driver, (
	LIAlgCamera* self));

LIAPICALL (void, lialg_camera_set_driver, (
	LIAlgCamera*      self,
	LIAlgCameraDriver value));

LIAPICALL (void, lialg_camera_set_far, (
	LIAlgCamera* self,
	float        value));

LIAPICALL (float, lialg_camera_get_fov, (
	const LIAlgCamera* self));

LIAPICALL (void, lialg_camera_set_fov, (
	LIAlgCamera* self,
	float        value));

LIAPICALL (void, lialg_camera_get_frustum, (
	const LIAlgCamera* self,
	LIMatFrustum*      result));

LIAPICALL (void, lialg_camera_get_modelview, (
	const LIAlgCamera* self,
	LIMatMatrix*       value));

LIAPICALL (void, lialg_camera_set_near, (
	LIAlgCamera* self,
	float        value));

LIAPICALL (void, lialg_camera_get_projection, (
	const LIAlgCamera* self,
	LIMatMatrix*       value));

LIAPICALL (void, lialg_camera_set_projection, (
	LIAlgCamera* self,
	float        fov,
	float        aspect,
	float        nearplane,
	float        farplane));

LIAPICALL (void, lialg_camera_get_smoothing, (
	const LIAlgCamera* self,
	float*             pos,
	float*             rot));

LIAPICALL (void, lialg_camera_set_smoothing, (
	LIAlgCamera* self,
	float        pos,
	float        rot));

LIAPICALL (void, lialg_camera_get_transform, (
	const LIAlgCamera* self,
	LIMatTransform*    value));

LIAPICALL (void, lialg_camera_set_transform, (
	LIAlgCamera*          self,
	const LIMatTransform* value));

LIAPICALL (void, lialg_camera_get_up, (
	const LIAlgCamera* self,
	LIMatVector*       result));

LIAPICALL (void, lialg_camera_set_viewport, (
	LIAlgCamera* self,
	int          x,
	int          y,
	int          width,
	int          height));

#endif
