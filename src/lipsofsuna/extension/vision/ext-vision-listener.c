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

#include "lipsofsuna/extension.h"
#include "ext-vision-listener.h"

static float private_cone_factor (
	LIExtVisionListener* self,
	const LIMatVector*   pos);

static void private_update_terrain (
	LIExtVisionListener* self,
	LIVoxManager*        voxels,
	lua_State*           lua);

/*****************************************************************************/

LIExtVisionListener* liext_vision_listener_new (
	LIExtModule* module)
{
	LIExtVisionListener* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtVisionListener));
	if (self == NULL)
		return NULL;
	self->cone_angle = 0.0f;
	self->cone_cosine = 0.5f * M_PI;
	self->cone_factor = 0.5f;
	self->keep_threshold = 5.0f;
	self->scan_radius = 32.0f;
	self->direction = limat_vector_init (0.0f, 0.0f, -1.0f);
	self->module = module;

	/* Allocate the object dictionary. */
	self->objects = lialg_u32dic_new (self->objects);
	if (self->objects == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate the terrain dictionary. */
	self->terrain = lialg_u32dic_new (self->objects);
	if (self->terrain == NULL)
	{
		lialg_u32dic_free (self->objects);
		lisys_free (self);
		return NULL;
	}

	return self;
}

void liext_vision_listener_free (
	LIExtVisionListener* self)
{
	lialg_u32dic_free (self->objects);
	lialg_u32dic_free (self->terrain);
	lisys_free (self);
}

void liext_vision_listener_clear (
	LIExtVisionListener* self)
{
	lialg_u32dic_clear (self->objects);
	lialg_u32dic_clear (self->terrain);
}

/**
 * \brief Updates the vision and added the events to a table in the Lua stack.
 *
 * This function is intended to be called only from the scripting API. The
 * table where the events are added is already expected to be at the top of
 * the stack.
 *
 * \param self Vision listener.
 * \param lua Lua state.
 */
void liext_vision_listener_update (
	LIExtVisionListener* self,
	lua_State*           lua)
{
	float dist;
	float mult;
	float radius_add;
	float radius_del;
	float radius_add_obj;
	float radius_del_obj;
	LIAlgU32dicIter iter;
	LIEngObject* object;
	LIMatVector diff;
	LIVoxManager* voxels;

	/* Calculate the second powers of the vision radii. */
	radius_add = self->scan_radius;
	radius_del = self->scan_radius + self->keep_threshold;

	/* Add and remove vision objects. */
	LIALG_U32DIC_FOREACH (iter, self->module->program->engine->objects)
	{
		object = iter.value;

		/* Make sure the object is realized. */
		if (!lieng_object_get_realized (object))
		{
			if (lialg_u32dic_find (self->objects, object->id) != NULL)
			{
				lialg_u32dic_remove (self->objects, object->id);
				lua_pushnumber (lua, lua_objlen (lua, -1) + 1);
				lua_newtable (lua);
				if (liscr_pushdata (lua, object->script))
				{
					lua_setfield (lua, -2, "object");
					lua_pushstring (lua, "object-hidden");
					lua_setfield (lua, -2, "type");
					lua_settable (lua, -3);
				}
				else
					lua_pop (lua, 2);
			}
			continue;
		}

		/* Calculate the distance to the object. */
		diff = limat_vector_subtract (object->transform.position, self->position);
		dist = limat_vector_get_length (diff);
		mult = private_cone_factor (self, &diff);
		radius_add_obj = mult * radius_add;
		radius_del_obj = mult * radius_del;

		/* Check if the object just entered the vision sphere. */
		if (lialg_u32dic_find (self->objects, object->id) == NULL)
		{
			if (dist <= radius_add_obj)
			{
				lialg_u32dic_insert (self->objects, object->id, NULL + 1);
				lua_pushnumber (lua, lua_objlen (lua, -1) + 1);
				lua_newtable (lua);
				if (liscr_pushdata (lua, object->script))
				{
					lua_setfield (lua, -2, "object");
					lua_pushstring (lua, "object-shown");
					lua_setfield (lua, -2, "type");
					lua_settable (lua, -3);
				}
				else
					lua_pop (lua, 2);
			}
		}

		/* Check if the object has just left the vision sphere. */
		else
		{
			if (dist > radius_del_obj)
			{
				lialg_u32dic_remove (self->objects, object->id);
				lua_pushnumber (lua, lua_objlen (lua, -1) + 1);
				lua_newtable (lua);
				if (liscr_pushdata (lua, object->script))
				{
					lua_setfield (lua, -2, "object");
					lua_pushstring (lua, "object-hidden");
					lua_setfield (lua, -2, "type");
					lua_settable (lua, -3);
				}
				else
					lua_pop (lua, 2);
			}
		}
	}

	/* TODO: Silently remove garbage collected objects. */

	/* Update terrain blocks. */
	voxels = limai_program_find_component (self->module->program, "voxels");
	if (voxels != NULL)
		private_update_terrain (self, voxels, lua);
}

/*****************************************************************************/

static float private_cone_factor (
	LIExtVisionListener* self,
	const LIMatVector*   diff)
{
	float dot;
	float mult;

	dot = limat_vector_dot (self->direction, limat_vector_normalize (*diff));
	if (dot >= self->cone_cosine)
		return 1.0f;
	mult = (dot + 1.0f) / (self->cone_cosine + 1.0f);
	mult = (1.0f - mult) * self->cone_factor + mult;

	return mult;
}

static void private_update_terrain (
	LIExtVisionListener* self,
	LIVoxManager*        voxels,
	lua_State*           lua)
{
	int sx;
	int sy;
	int sz;
	int index;
	int line;
	int stamp;
	float block_size;
	float dist;
	float mult;
	float radius;
	LIAlgRange sectors;
	LIAlgRange blocks;
	LIAlgRange range;
	LIAlgRangeIter iter0;
	LIAlgRangeIter iter1;
	LIMatVector min;
	LIMatVector max;
	LIMatVector point;
	LIMatVector point1;
	LIMatVector diff;
	LIMatVector size;
	LIVoxBlock* block;
	LIVoxSector* sector;

	block_size = voxels->sectors->width / voxels->blocks_per_line;
	line = voxels->blocks_per_line * voxels->sectors->count;
	radius = self->scan_radius;
	point = self->position;

	/* Calculate the sight volume. */
	size = limat_vector_init (radius, radius, radius);
	min = limat_vector_subtract (point, size);
	max = limat_vector_add (point, size);
	sectors = lialg_range_new_from_aabb (&min, &max, voxels->sectors->width);
	sectors = lialg_range_clamp (sectors, 0, voxels->sectors->count - 1);
	blocks = lialg_range_new_from_aabb (&min, &max, block_size);
	blocks = lialg_range_clamp (blocks, 0, voxels->blocks_per_line * voxels->sectors->count - 1);

	/* Loop through nearby sectors. */
	LIALG_RANGE_FOREACH (iter0, sectors)
	{
		/* Get voxel sector. */
		sector = lialg_sectors_data_index (voxels->sectors, LIALG_SECTORS_CONTENT_VOXEL, iter0.index, 0);
		if (sector == NULL)
			continue;

		/* Calculate the block range in the sector. */
		livox_sector_get_offset (sector, &sx, &sy, &sz);
		sx *= voxels->blocks_per_line;
		sy *= voxels->blocks_per_line;
		sz *= voxels->blocks_per_line;
		range.min = 0;
		range.max = voxels->blocks_per_line;
		range.minx = LIMAT_MAX (blocks.minx - sx, 0);
		range.miny = LIMAT_MAX (blocks.miny - sy, 0);
		range.minz = LIMAT_MAX (blocks.minz - sz, 0);
		range.maxx = LIMAT_MIN (blocks.maxx - sx, voxels->blocks_per_line - 1);
		range.maxy = LIMAT_MIN (blocks.maxy - sy, voxels->blocks_per_line - 1);
		range.maxz = LIMAT_MIN (blocks.maxz - sz, voxels->blocks_per_line - 1);

		/* Loop through nearby blocks. */
		LIALG_RANGE_FOREACH (iter1, range)
		{
			/* Vision cone check. */
			point1 = limat_vector_init (sx + iter1.x + 0.5f, sy + iter1.y + 0.5f, sz + iter1.z + 0.5f);
			point1 = limat_vector_multiply (point1, block_size);
			diff = limat_vector_subtract (point1, point);
			dist = limat_vector_get_length (diff);
			mult = private_cone_factor (self, &diff);
			if (dist - 0.3f * block_size > radius * mult)
				continue;

			/* Check for changes. */
			block = livox_sector_get_block (sector, iter1.x, iter1.y, iter1.z);
			stamp = livox_block_get_stamp (block);
			index = (sx + iter1.x) + (sy + iter1.y) * line + (sz + iter1.z) * line * line;
			if (lialg_u32dic_find (self->terrain, index) == (void*)(intptr_t)(stamp + 1))
				continue;

			/* Store the new block stamp. */
			if (!lialg_u32dic_insert (self->terrain, index, (void*)(intptr_t)(stamp + 1)))
				continue;

			/* Create an event. */
			lua_pushnumber (lua, lua_objlen (lua, -1) + 1);
			lua_newtable (lua);
			lua_pushnumber (lua, index);
			lua_setfield (lua, -2, "index");
			lua_pushnumber (lua, stamp);
			lua_setfield (lua, -2, "stamp");
			lua_pushstring (lua, "voxel-block-changed");
			lua_setfield (lua, -2, "type");
			lua_settable (lua, -3);
		}
	}
}
