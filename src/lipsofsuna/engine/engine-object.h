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

#ifndef __ENGINE_OBJECT_H__
#define __ENGINE_OBJECT_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/physics.h>
#include <lipsofsuna/system.h>
#include "engine-model.h"
#include "engine-types.h"

#ifndef LIENG_DISABLE_SCRIPT
#include <lipsofsuna/script.h>
#endif

struct _LIEngObject
{
	uint32_t id;
	LIEngEngine* engine;
	LIEngModel* model;
	LIEngSector* sector;
	LIMatTransform transform;
	LIMatTransform transform_event;
	LIMdlPose* pose;
#ifndef LIENG_DISABLE_SCRIPTS
	LIScrData* script;
#endif
};

LIAPICALL (LIEngObject*, lieng_object_new, (
	LIEngEngine* engine));

LIAPICALL (void, lieng_object_free, (
	LIEngObject* self));

LIAPICALL (int, lieng_object_moved, (
	LIEngObject* self));

LIAPICALL (void, lieng_object_refresh, (
	LIEngObject* self,
	float        radius));

LIAPICALL (void, lieng_object_get_bounds, (
	const LIEngObject* self,
	LIMatAabb*         bounds));

LIAPICALL (int, lieng_object_get_dirty, (
	const LIEngObject* self));

LIAPICALL (void, lieng_object_set_dirty, (
	LIEngObject* self,
	int          value));

LIAPICALL (float, lieng_object_get_distance, (
	const LIEngObject* self,
	const LIEngObject* object));

LIAPICALL (int, lieng_object_set_model, (
	LIEngObject* self,
	LIEngModel*  model));

LIAPICALL (int, lieng_object_get_realized, (
	const LIEngObject* self));

LIAPICALL (int, lieng_object_set_realized, (
	LIEngObject* self,
	int          value));

LIAPICALL (LIEngSector*, lieng_object_get_sector, (
	LIEngObject* self));

LIAPICALL (void, lieng_object_get_transform, (
	const LIEngObject* self,
	LIMatTransform*    value));

LIAPICALL (int, lieng_object_set_transform, (
	LIEngObject*          self,
	const LIMatTransform* value));

#endif
