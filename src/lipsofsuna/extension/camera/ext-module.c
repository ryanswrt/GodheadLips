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
 * \addtogroup LIExtCamera Camera
 * @{
 */

#include "ext-module.h"

LIMaiExtensionInfo liext_camera_info =
{
	LIMAI_EXTENSION_VERSION, "Camera",
	liext_cameras_new,
	liext_cameras_free
};

LIExtModule* liext_cameras_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->client = limai_program_find_component (program, "client");

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_CAMERA, self);
	liext_script_camera (program->script);

	return self;
}

void liext_cameras_free (
	LIExtModule* self)
{
	lisys_free (self);
}

float liext_cameras_clip_camera (
	LIExtModule*    self,
	LIAlgCamera*    camera,
	LIMatTransform* start,
	LIMatTransform* end)
{
	int hit;
	LIPhyCollision tmp;
	LIPhyPhysics* physics;

	/* Find the physics manager. */
	physics = limai_program_find_component (self->program, "physics");
	if (physics == NULL)
		return 1.0f;

	/* Find the clip distance with a ray cast. */
	/* A convex cast might sound like a better idea but it makes the behavior
	   less predictable so scripting will suffer. The gain is also relatively
	   small so we just use a ray cast now. */
	hit = liphy_physics_cast_ray (physics, &start->position, &end->position,
		camera->config.collision_group, camera->config.collision_mask, NULL, 0, &tmp);

	/* Return the clip distance. */
	if (hit)
		return tmp.fraction;
	else
		return 1.0f;
}

void liext_cameras_update (
	LIExtModule* self,
	LIAlgCamera* camera,
	float        secs)
{
	lialg_camera_update (camera, secs);
}

/** @} */
/** @} */
