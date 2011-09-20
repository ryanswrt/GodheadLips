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
 * \addtogroup LIExtTiles Tiles
 * @{
 */

#include "ext-module.h"

static void Voxel_copy_region (LIScrArgs* args)
{
	int i;
	int length;
	int sector;
	int offset[3];
	LIArcPacket* packet;
	LIScrData* data;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector size;
	LIVoxVoxel* result;

	/* Get region offset and size. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_gets_int (args, "sector", &sector))
	{
		lialg_sectors_index_to_offset (module->program->sectors, sector,
			offset + 0, offset + 1, offset + 2);
		point = limat_vector_init (offset[0], offset[1], offset[2]);
		point = limat_vector_multiply (point, module->voxels->tiles_per_line);
		size.x = size.y = size.z = module->voxels->tiles_per_line;
		length = module->voxels->tiles_per_sector;
	}
	else if (liscr_args_gets_vector (args, "point", &point) &&
	         liscr_args_gets_vector (args, "size", &size))
	{
		if (point.x < 0.0f || point.y < 0.0f || point.z < 0.0f ||
		    size.x < 1.0f || size.y < 1.0f || size.z < 1.0f)
			return;
		length = (int) size.x * (int) size.y * (int) size.z;
	}
	else
		return;

	/* Read voxel data. */
	result = lisys_calloc (length, sizeof (LIVoxVoxel));
	if (result == NULL)
		return;
	livox_manager_copy_voxels (module->voxels,
		(int) point.x, (int) point.y, (int) point.z,
		(int) size.x, (int) size.y, (int) size.z, result);

	/* Create a packet writer. */
	packet = liarc_packet_new_writable (0);
	if (packet == NULL)
	{
		lisys_free (result);
		return;
	}

	/* Write the dimensions. */
	if (!liarc_writer_append_uint32 (packet->writer, (int) size.x) ||
		!liarc_writer_append_uint32 (packet->writer, (int) size.y) ||
		!liarc_writer_append_uint32 (packet->writer, (int) size.z))
	{
		lisys_free (result);
		return;
	}

	/* Write voxel data. */
	for (i = 0 ; i < length ; i++)
	{
		if (!livox_voxel_write (result + i, packet->writer))
		{
			lisys_free (result);
			return;
		}
	}
	lisys_free (result);

	/* Return data. */
	data = liscr_data_new (args->script, args->lua, packet, LISCR_SCRIPT_PACKET, liarc_packet_free);
	if (data == NULL)
	{
		liarc_packet_free (packet);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Voxel_fill_region (LIScrArgs* args)
{
	int i;
	int count;
	int type = 0;
	LIExtModule* module;
	LIMatVector pos;
	LIMatVector size;
	LIVoxVoxel tile;
	LIVoxVoxel* tiles;

	/* Handle arguments. */
	if (!liscr_args_gets_vector (args, "point", &pos) ||
	    !liscr_args_gets_vector (args, "size", &size))
		return;
	if (size.x < 1.0f || size.y < 1.0f || size.z < 1.0f)
		return;
	liscr_args_gets_int (args, "tile", &type);
	livox_voxel_init (&tile, type);

	/* Allocate tiles. */
	count = (int) size.x * (int) size.y * (int) size.z;
	tiles = lisys_calloc (count, sizeof (LIVoxVoxel));
	if (tiles == NULL)
		return;
	for (i = 0 ; i < count ; i++)
		tiles[i] = tile;

	/* Paste tiles to the map. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	livox_manager_paste_voxels (module->voxels,
		(int) pos.x, (int) pos.y, (int) pos.z,
		(int) size.x, (int) size.y, (int) size.z, tiles);

	lisys_free (tiles);
}

static void Voxel_find_blocks (LIScrArgs* args)
{
	int sx;
	int sy;
	int sz;
	int index;
	int line;
	int stamp;
	float radius;
	LIAlgRange sectors;
	LIAlgRange blocks;
	LIAlgRange range;
	LIAlgRangeIter iter0;
	LIAlgRangeIter iter1;
	LIExtModule* module;
	LIMatVector min;
	LIMatVector max;
	LIMatVector point;
	LIMatVector size;
	LIVoxBlock* block;
	LIVoxSector* sector;

	/* Initialize arguments. */
	if (!liscr_args_gets_vector (args, "point", &point))
		return;
	liscr_args_gets_float (args, "radius", &radius);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	line = module->voxels->blocks_per_line * module->voxels->sectors->count;

	/* Calculate sight volume. */
	size = limat_vector_init (radius, radius, radius);
	min = limat_vector_subtract (point, size);
	max = limat_vector_add (point, size);
	sectors = lialg_range_new_from_aabb (&min, &max, module->voxels->sectors->width);
	sectors = lialg_range_clamp (sectors, 0, module->voxels->sectors->count - 1);
	blocks = lialg_range_new_from_aabb (&min, &max, module->voxels->sectors->width / module->voxels->blocks_per_line);
	blocks = lialg_range_clamp (blocks, 0, module->voxels->blocks_per_line * module->voxels->sectors->count - 1);

	/* Loop through visible sectors. */
	LIALG_RANGE_FOREACH (iter0, sectors)
	{
		/* Get voxel sector. */
		sector = lialg_sectors_data_index (module->voxels->sectors, LIALG_SECTORS_CONTENT_VOXEL, iter0.index, 0);
		if (sector == NULL)
			continue;

		/* Calculate visible block range. */
		livox_sector_get_offset (sector, &sx, &sy, &sz);
		sx *= module->voxels->blocks_per_line;
		sy *= module->voxels->blocks_per_line;
		sz *= module->voxels->blocks_per_line;
		range.min = 0;
		range.max = module->voxels->blocks_per_line;
		range.minx = LIMAT_MAX (blocks.minx - sx, 0);
		range.miny = LIMAT_MAX (blocks.miny - sy, 0);
		range.minz = LIMAT_MAX (blocks.minz - sz, 0);
		range.maxx = LIMAT_MIN (blocks.maxx - sx, module->voxels->blocks_per_line - 1);
		range.maxy = LIMAT_MIN (blocks.maxy - sy, module->voxels->blocks_per_line - 1);
		range.maxz = LIMAT_MIN (blocks.maxz - sz, module->voxels->blocks_per_line - 1);

		/* Loop through visible blocks. */
		LIALG_RANGE_FOREACH (iter1, range)
		{
			block = livox_sector_get_block (sector, iter1.x, iter1.y, iter1.z);
			stamp = livox_block_get_stamp (block);
			index = (sx + iter1.x) + (sy + iter1.y) * line + (sz + iter1.z) * line * line;
			liscr_args_setf_float (args, index, stamp);
		}
	}
}

static void Voxel_find_tile (LIScrArgs* args)
{
	int index[3];
	int flags = LIVOX_FIND_ALL;
	float radius = 0.0f;
	const char* tmp;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector result;
	LIVoxVoxel* voxel;

	if (liscr_args_gets_vector (args, "point", &point))
	{
		/* Search mode. */
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
		if (liscr_args_gets_string (args, "match", &tmp))
		{
			if (!strcmp (tmp, "all")) flags = LIVOX_FIND_ALL;
			else if (!strcmp (tmp, "empty")) flags = LIVOX_FIND_EMPTY;
			else if (!strcmp (tmp, "full")) flags = LIVOX_FIND_FULL;
		}

		/* Search radius. */
		liscr_args_gets_float (args, "radius", &radius);
		radius = LIMAT_MAX (0.0f, radius);

		/* Find voxel. */
		voxel = livox_manager_find_voxel (module->voxels, flags, &point, radius, index);
		if (voxel == NULL)
			return;

		/* Return values. */
		liscr_args_seti_int (args, voxel->type);
		result = limat_vector_init (index[0], index[1], index[2]);
		liscr_args_seti_vector (args, &result);
	}
}

static void Voxel_get_block (LIScrArgs* args)
{
	int tmp;
	int index;
	int type = 1;
	LIArcPacket* packet;
	LIExtModule* module;
	LIScrData* data = NULL;
	LIVoxSector* sector;
	LIVoxBlockAddr addr;

	/* Get block index. */
	if (!liscr_args_gets_int (args, "index", &index))
		return;
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	tmp = index;
	addr.block[0] = tmp % module->voxels->blocks_per_line;
	addr.sector[0] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;
	tmp /= module->voxels->blocks_per_line * module->voxels->sectors->count;
	addr.block[1] = tmp % module->voxels->blocks_per_line;
	addr.sector[1] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;
	tmp /= module->voxels->blocks_per_line * module->voxels->sectors->count;
	addr.block[2] = tmp % module->voxels->blocks_per_line;
	addr.sector[2] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;

	/* Get block. */
	sector = lialg_sectors_data_offset (module->program->sectors, LIALG_SECTORS_CONTENT_VOXEL,
		addr.sector[0], addr.sector[1], addr.sector[2], 0);
	if (sector == NULL)
		return;

	/* Get or create packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
	{
		liscr_args_gets_int (args, "type", &type);
		packet = liarc_packet_new_writable (type);
		if (packet == NULL)
			return;
		data = liscr_data_new (args->script, args->lua, packet, LISCR_SCRIPT_PACKET, liarc_packet_free);
		if (data == NULL)
		{
			liarc_packet_free (packet);
			return;
		}
		liscr_args_seti_stack (args);
	}
	else
	{
		packet = liscr_data_get_data (data);
		if (packet->writer == NULL)
			return;
	}

	/* Build the packet. */
	liarc_writer_append_uint32 (packet->writer, index);
	livox_sector_write_block (sector, addr.block[0], addr.block[1], addr.block[2], packet->writer);
}

static void Voxel_get_tile (LIScrArgs* args)
{
	int lim;
	LIExtModule* module;
	LIMatVector point;
	LIVoxVoxel voxel;

	if (!liscr_args_geti_vector (args, 0, &point) &&
	    !liscr_args_gets_vector (args, "point", &point))
		return;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	lim = module->voxels->tiles_per_line * module->program->sectors->count;
	if (point.x < 0.0f || point.x >= lim ||
	    point.y < 0.0f || point.y >= lim ||
	    point.z < 0.0f || point.z >= lim)
		return;
	livox_manager_get_voxel (module->voxels, (int) point.x, (int) point.y, (int) point.z, &voxel);
	liscr_args_seti_int (args, voxel.type);
}

static void Voxel_intersect_ray (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector ray0;
	LIMatVector ray1;
	LIMatVector point;
	LIMatVector tile;

	/* Get arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	if (!liscr_args_geti_vector (args, 0, &ray0) &&
	    !liscr_args_gets_vector (args, "start_point", &ray0))
		return;
	if (!liscr_args_geti_vector (args, 1, &ray1) &&
	    !liscr_args_gets_vector (args, "end_point", &ray1))
		return;

	/* Intersect with terrain. */
	ray0 = limat_vector_multiply (ray0, 1.0f / module->voxels->tile_width);
	ray1 = limat_vector_multiply (ray1, 1.0f / module->voxels->tile_width);
	if (livox_manager_intersect_ray (module->voxels, &ray0, &ray1, &point, &tile))
	{
		point = limat_vector_multiply (point, module->voxels->tile_width);
		liscr_args_seti_vector (args, &point);
		liscr_args_seti_vector (args, &tile);
	}
}

static void Voxel_paste_region (LIScrArgs* args)
{
	int i;
	int length;
	int sector;
	int offset[3];
	uint8_t tmp;
	uint32_t size[3];
	LIArcPacket* packet;
	LIArcReader* reader;
	LIExtModule* module;
	LIMatVector point;
	LIScrData* data;
	LIVoxVoxel* voxels;

	/* Get region offset. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_gets_int (args, "sector", &sector))
	{
		lialg_sectors_index_to_offset (module->program->sectors, sector,
			offset + 0, offset + 1, offset + 2);
		point = limat_vector_init (offset[0], offset[1], offset[2]);
		point = limat_vector_multiply (point, module->voxels->tiles_per_line);
	}
	else if (liscr_args_gets_vector (args, "point", &point))
	{
		if (point.x < 0.0f || point.y < 0.0f || point.z < 0.0f)
			return;
	}
	else
		return;

	/* Get terrain data. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->reader == NULL)
	{
		reader = liarc_reader_new (
			liarc_writer_get_buffer (packet->writer),
			liarc_writer_get_length (packet->writer));
		if (reader == NULL)
			return;
	}
	else
		reader = packet->reader;

	/* Read dimensions. */
	if (!liarc_reader_get_uint8 (reader, &tmp) ||
	    !liarc_reader_get_uint32 (reader, size + 0) ||
	    !liarc_reader_get_uint32 (reader, size + 1) ||
	    !liarc_reader_get_uint32 (reader, size + 2))
	{
		if (packet->reader == NULL)
			liarc_reader_free (reader);
		return;
	}
	length = size[0] * size[1] * size[2];
	if (!length)
	{
		if (packet->reader == NULL)
			liarc_reader_free (reader);
		return;
	}

	/* Allocate space for voxel data. */
	voxels = lisys_calloc (length, sizeof (LIVoxVoxel));
	if (voxels == NULL)
	{
		if (packet->reader == NULL)
			liarc_reader_free (reader);
		return;
	}

	/* Read voxel data. */
	for (i = 0 ; i < length ; i++)
	{
		if (!livox_voxel_read (voxels + i, reader))
		{
			if (packet->reader == NULL)
				liarc_reader_free (reader);
			lisys_free (voxels);
			return;
		}
	}

	/* Paste voxel data to the map. */
	livox_manager_paste_voxels (module->voxels,
		(int) point.x, (int) point.y, (int) point.z,
		size[0], size[1], size[2], voxels);
	if (packet->reader == NULL)
		liarc_reader_free (reader);
	lisys_free (voxels);
}

static void Voxel_update (LIScrArgs* args)
{
	float secs = 3.0f;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	liscr_args_gets_float (args, "secs", &secs);

	livox_manager_update (module->voxels, secs);
}

static void Voxel_set_block (LIScrArgs* args)
{
	uint8_t skip;
	uint32_t index;
	uint32_t tmp;
	LIArcPacket* packet;
	LIExtModule* module;
	LIVoxBlockAddr addr;
	LIVoxSector* sector;
	LIScrData* data;

	/* Get packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (!packet->reader)
		return;

	/* Skip type. */
	if (!packet->reader->pos && !liarc_reader_get_uint8 (packet->reader, &skip))
		return;

	/* Read block offset. */
	if (!liarc_reader_get_uint32 (packet->reader, &index))
		return;
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	tmp = index;
	addr.block[0] = tmp % module->voxels->blocks_per_line;
	addr.sector[0] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;
	tmp /= module->voxels->blocks_per_line * module->voxels->sectors->count;
	addr.block[1] = tmp % module->voxels->blocks_per_line;
	addr.sector[1] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;
	tmp /= module->voxels->blocks_per_line * module->voxels->sectors->count;
	addr.block[2] = tmp % module->voxels->blocks_per_line;
	addr.sector[2] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;

	/* Find or create sector. */
	sector = lialg_sectors_data_offset (module->voxels->sectors, LIALG_SECTORS_CONTENT_VOXEL,
		addr.sector[0], addr.sector[1], addr.sector[2], 1);
	if (sector == NULL)
		return;

	/* Read block data. */
	if (!livox_sector_read_block (sector, addr.block[0], addr.block[1], addr.block[2], packet->reader))
		return;

	/* Indicate success. */
	liscr_args_seti_bool (args, 1);
}

static void Voxel_set_tile (LIScrArgs* args)
{
	int lim;
	int type;
	LIExtModule* module;
	LIMatVector point;
	LIVoxVoxel voxel;

	if (!liscr_args_geti_vector (args, 0, &point) &&
	    !liscr_args_gets_vector (args, "point", &point))
		return;
	if (!liscr_args_geti_int (args, 1, &type) &&
	    !liscr_args_gets_int (args, "tile", &type))
		return;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	lim = module->voxels->tiles_per_line * module->program->sectors->count;
	if (point.x < 0.0f || point.x >= lim ||
	    point.y < 0.0f || point.y >= lim ||
	    point.z < 0.0f || point.z >= lim)
		return;
	livox_voxel_init (&voxel, type);
	livox_manager_set_voxel (module->voxels, (int) point.x, (int) point.y, (int) point.z, &voxel);
}

static void Voxel_set_tiles (LIScrArgs* args)
{
	int i;
	int lim;
	int type = 0;
	int top;
	LIExtModule* module;
	LIMatVector* point;
	LIScrData* data;
	LIVoxVoxel voxel;

	if (!liscr_args_geti_int (args, 0, &type) &&
	    !liscr_args_gets_int (args, "tile", &type))
		return;
	if (!liscr_args_geti_table (args, 1) &&
	    !liscr_args_gets_table (args, "points"))
		return;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	lim = module->voxels->tiles_per_line * module->program->sectors->count;
	top = lua_gettop (args->lua) + 1;
	livox_voxel_init (&voxel, type);

	for (i = 1 ; ; i++, lua_pop (args->lua, 1))
	{
		lua_pushnumber (args->lua, i);
		lua_gettable (args->lua, -2);
		data = liscr_isdata (args->lua, top, LISCR_SCRIPT_VECTOR);
		if (data == NULL)
		{
			lua_pop (args->lua, 1);
			break;
		}
		point = liscr_data_get_data (data);
		if (point->x < 0.0f || point->x >= lim ||
		    point->y < 0.0f || point->y >= lim ||
		    point->z < 0.0f || point->z >= lim)
			continue;
		livox_manager_set_voxel (module->voxels, (int) point->x, (int) point->y, (int) point->z, &voxel);
	}
	lua_pop (args->lua, 1);
}

static void Voxel_get_blocks_per_line (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	liscr_args_seti_int (args, module->voxels->blocks_per_line);
}
static void Voxel_set_blocks_per_line (LIScrArgs* args)
{
	int count;
	LIExtModule* module;

	if (liscr_args_geti_int (args, 0, &count))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
		if (!livox_manager_configure (module->voxels, count, module->voxels->tiles_per_line))
			lisys_error_report ();
	}
}

static void Voxel_get_fill (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	if (module->voxels->fill)
		liscr_args_seti_int (args, module->voxels->fill);
}
static void Voxel_set_fill (LIScrArgs* args)
{
	int type = 0;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	liscr_args_geti_int (args, 0, &type);
	livox_manager_set_fill (module->voxels, type);
}

static void Voxel_get_materials (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIExtModule* module;
	LIVoxMaterial* material;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, module->voxels->materials)
	{
		material = iter.value;
		liscr_args_seti_int (args, material->id);
	}
}

static void Voxel_get_memory_used (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	liscr_args_seti_int (args, livox_manager_get_memory (module->voxels));
}

static void Voxel_get_tiles_per_line (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	liscr_args_seti_int (args, module->voxels->tiles_per_line);
}
static void Voxel_set_tiles_per_line (LIScrArgs* args)
{
	int count;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_geti_int (args, 0, &count))
	{
		if (!livox_manager_configure (module->voxels, module->voxels->blocks_per_line, count))
			lisys_error_report ();
	}
}

/*****************************************************************************/

void liext_script_voxel (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_copy_region", Voxel_copy_region);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_fill_region", Voxel_fill_region);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_find_blocks", Voxel_find_blocks);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_find_tile", Voxel_find_tile);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_get_block", Voxel_get_block);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_get_tile", Voxel_get_tile);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_intersect_ray", Voxel_intersect_ray);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_paste_region", Voxel_paste_region);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_update", Voxel_update);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_set_block", Voxel_set_block);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_set_tile", Voxel_set_tile);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_set_tiles", Voxel_set_tiles);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_get_blocks_per_line", Voxel_get_blocks_per_line);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_set_blocks_per_line", Voxel_set_blocks_per_line);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_get_fill", Voxel_get_fill);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_set_fill", Voxel_set_fill);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_get_materials", Voxel_get_materials);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_get_memory_used", Voxel_get_memory_used);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_get_tiles_per_line", Voxel_get_tiles_per_line);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL, "voxel_set_tiles_per_line", Voxel_set_tiles_per_line);
}

/** @} */
/** @} */
