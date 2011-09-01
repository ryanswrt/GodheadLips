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

#ifndef __ALGORITHM_SECTORS_H__
#define __ALGORITHM_SECTORS_H__

#include <lipsofsuna/math.h>
#include "algorithm-strdic.h"
#include "algorithm-u32dic.h"

typedef struct _LIAlgSector LIAlgSector;
typedef struct _LIAlgSectors LIAlgSectors;
typedef struct _LIAlgSectorsCallback LIAlgSectorsCallback;
typedef void (*LIAlgSectorFreeFunc)(void*);
typedef void* (*LIAlgSectorLoadFunc)(LIAlgSector*);

enum
{
	LIALG_SECTORS_CONTENT_AI,
	LIALG_SECTORS_CONTENT_ENGINE,
	LIALG_SECTORS_CONTENT_VOXEL,
	LIALG_SECTORS_CONTENT_MAX
};

struct _LIAlgSectorsCallback
{
	void (*callback)(void*, LIAlgSector*);
	void* userdata;
};

struct _LIAlgSector
{
	int index;
	int stamp;
	int x;
	int y;
	int z;
	void* content[LIALG_SECTORS_CONTENT_MAX];
	LIAlgSectors* manager;
	LIMatVector position;
};

struct _LIAlgSectors
{
	int count;
	int loading;
	float width;
	void* content[LIALG_SECTORS_CONTENT_MAX];
	LIAlgU32dic* sectors;
	LIAlgSectorsCallback sector_free_callback;
	LIAlgSectorsCallback sector_load_callback;
};

LIAPICALL (LIAlgSectors*, lialg_sectors_new, (
	int   count,
	float width));

LIAPICALL (void, lialg_sectors_free, (
	LIAlgSectors* self));

LIAPICALL (void, lialg_sectors_clear, (
	LIAlgSectors* self));

LIAPICALL (void*, lialg_sectors_data_index, (
	LIAlgSectors* self,
	int           name,
	int           index,
	int           create));

LIAPICALL (void*, lialg_sectors_data_offset, (
	LIAlgSectors* self,
	int           name,
	int           x,
	int           y,
	int           z,
	int           create));

LIAPICALL (void*, lialg_sectors_data_point, (
	LIAlgSectors*      self,
	int                name,
	const LIMatVector* point,
	int                create));

LIAPICALL (LIAlgSector*, lialg_sectors_sector_index, (
	LIAlgSectors* self,
	int           index,
	int           create));

LIAPICALL (LIAlgSector*, lialg_sectors_sector_offset, (
	LIAlgSectors* self,
	int           x,
	int           y,
	int           z,
	int           create));

LIAPICALL (LIAlgSector*, lialg_sectors_sector_point, (
	LIAlgSectors*      self,
	const LIMatVector* point,
	int                create));

LIAPICALL (void, lialg_sectors_index_to_offset, (
	LIAlgSectors* self,
	int           index,
	int*          x,
	int*          y,
	int*          z));

LIAPICALL (void, lialg_sectors_index_to_offset_static, (
	int  count,
	int  index,
	int* x,
	int* y,
	int* z));

LIAPICALL (int, lialg_sectors_insert_content, (
	LIAlgSectors*       self,
	int                 name,
	void*               data,
	LIAlgSectorFreeFunc free,
	LIAlgSectorLoadFunc load));

LIAPICALL (int, lialg_sectors_offset_to_index, (
	LIAlgSectors* self,
	int           x,
	int           y,
	int           z));

LIAPICALL (int, lialg_sectors_offset_to_index_static, (
	int count,
	int x,
	int y,
	int z));

LIAPICALL (int, lialg_sectors_point_to_index, (
	LIAlgSectors*      self,
	const LIMatVector* point));

LIAPICALL (int, lialg_sectors_point_to_index_static, (
	int                count,
	float              width,
	const LIMatVector* point));

LIAPICALL (void, lialg_sectors_refresh_point, (
	LIAlgSectors*      self,
	const LIMatVector* point,
	float              radius));

LIAPICALL (void, lialg_sectors_remove, (
	LIAlgSectors* self,
	int           index));

LIAPICALL (void, lialg_sectors_remove_content, (
	LIAlgSectors* self,
	int           name));

LIAPICALL (void, lialg_sectors_update, (
	LIAlgSectors* self,
	float         secs));

LIAPICALL (void*, lialg_sectors_get_userdata, (
	LIAlgSectors* self,
	int           name));

#endif
