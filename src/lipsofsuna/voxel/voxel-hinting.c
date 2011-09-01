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
 * \addtogroup LIVox Voxel
 * @{
 * \addtogroup LIVoxHinting Hinting
 * @{
 */

#include "voxel-hinting.h"
#include "voxel-material.h"

#define ISFILLED(x,y,z) (materials[x][y][z] != NULL)
#define ISCUBE(x,y,z) (materials[x][y][z] != NULL && (materials[x][y][z]->type != LIVOX_MATERIAL_TYPE_SLOPED && materials[x][y][z]->type != LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL))
#define ISEMPTY(x,y,z) (materials[x][y][z] == NULL)
#define ISSLOPED(x,y,z) (materials[x][y][z] != NULL && (materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_SLOPED || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL))
#define ISROUNDED(x,y,z) (materials[x][y][z] != NULL && (materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_ROUNDED || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL))
#define ISROUNDEDSLOPED(x,y,z) (materials[x][y][z] != NULL && (ISROUNDED(x,y,z) || ISSLOPED(x,y,z)))
#define ISSOLID(x,y,z) (materials[x][y][z] != NULL && materials[x][y][z]->type != LIVOX_MATERIAL_TYPE_LIQUID)
#define ISLIQUID(x,y,z) (materials[x][y][z] != NULL && materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_LIQUID)
#define ISLIQUIDEMPTY(x,y,z) (materials[x][y][z] == NULL || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_LIQUID)
#define ISLIQUIDEMPTYFRACTAL(x,y,z) (ISLIQUIDEMPTY(x,y,z) || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL)
#define ISLIQUIDEMPTYROUNDED(x,y,z) (ISLIQUIDEMPTY(x,y,z) || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_ROUNDED || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL)
#define ISLIQUIDEMPTYSLOPED(x,y,z) (ISLIQUIDEMPTY(x,y,z) || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_SLOPED || materials[x][y][z]->type == LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL)

static void private_hint_liquid (
	LIVoxVoxel*    voxels[3][3][3],
	LIVoxMaterial* materials[3][3][3]);

static void private_hint_sloped (
	LIVoxVoxel*    voxels[3][3][3],
	LIVoxMaterial* materials[3][3][3]);

static void private_hint_rounded (
	LIVoxVoxel*    voxels[3][3][3],
	LIVoxMaterial* materials[3][3][3]);

/*****************************************************************************/

LIVoxVoxel* livox_hinting_process_area (
	LIVoxManager* manager,
	int           px,
	int           py,
	int           pz,
	int           sx,
	int           sy,
	int           sz)
{
	int x;
	int y;
	int z;
	LIVoxMaterial** material;
	LIVoxMaterial** materials;
	LIVoxVoxel* voxel;
	LIVoxVoxel* voxel1;
	LIVoxVoxel* voxels;

	/* Copy the voxels and their neighbors. */
	voxels = lisys_calloc ((sx + 2) * (sy + 2) * (sz + 2), sizeof (LIVoxVoxel));
	if (voxels == NULL)
		return NULL;
	livox_manager_copy_voxels (manager, px - 1, py - 1, pz - 1, sx + 2, sy + 2, sz + 2, voxels);

	/* Resolve materials. */
	materials = lisys_calloc ((sx + 2) * (sy + 2) * (sz + 2), sizeof (LIVoxMaterial*));
	if (materials == NULL)
	{
		lisys_free (voxels);
		return NULL;
	}
	for (z = 0 ; z < sz + 2 ; z++)
	for (y = 0 ; y < sy + 2 ; y++)
	for (x = 0 ; x < sx + 2 ; x++)
	{
		voxel = voxels + x + y * (sx + 2) + z * (sx + 2) * (sy + 2);
		material = materials + x + y * (sx + 2) + z * (sx + 2) * (sy + 2);
		*material = livox_manager_find_material (manager, voxel->type);
	}

	/* Hint the voxels. */
	for (z = 1 ; z < sz + 1 ; z++)
	for (y = 1 ; y < sy + 1 ; y++)
	for (x = 1 ; x < sx + 1 ; x++)
	{
		voxel = voxels + x + y * (sx + 2) + z * (sx + 2) * (sy + 2);
		if (voxel->type)
			livox_hinting_process_voxel (manager, voxels, materials, x, y, z, sx + 2, sy + 2, sz + 2);
		else
			voxel->hint = 0;
	}
	lisys_free (materials);

	/* Remove the neighbors. */
	for (z = 0 ; z < sz ; z++)
	for (y = 0 ; y < sy ; y++)
	for (x = 0 ; x < sx ; x++)
	{
		voxel = voxels + x + y * sx + z * sx * sy;
		voxel1 = voxels + (x + 1) + (y + 1) * (sx + 2) + (z + 1) * (sx + 2) * (sy + 2);
		*voxel = *voxel1;
	}

	return voxels;
}

/**
 * \brief Calculates triangulation hints for a voxel.
 * \param manager Voxel manager.
 * \param voxels_ Voxel buffer.
 * \param materials_ Material buffer.
 * \param vx Index of the triangulated voxel.
 * \param vy Index of the triangulated voxel.
 * \param vz Index of the triangulated voxel.
 * \param sx Size of the voxel buffer.
 * \param sy Size of the voxel buffer.
 * \param sz Size of the voxel buffer.
 */
void livox_hinting_process_voxel (
	LIVoxManager*   manager,
	LIVoxVoxel*     voxels_,
	LIVoxMaterial** materials_,
	int             vx,
	int             vy,
	int             vz,
	int             sx,
	int             sy,
	int             sz)
{
	int x;
	int y;
	int z;
	int index;
	int liquid = 0;
	LIVoxVoxel* voxel;
	LIVoxVoxel* voxels[3][3][3];
	LIVoxMaterial* materials[3][3][3];

	/* Get the hinted voxel. */
	voxel = voxels_ + vx + vy * sx + vz * sx * sy;
	if (!voxel->type)
	{
		voxel->hint = 0;
		return;
	}

	/* Get the neighborhood. */
	for (z = -1 ; z <= 1 ; z++)
	for (y = -1 ; y <= 1 ; y++)
	for (x = -1 ; x <= 1 ; x++)
	{
		index = (x + vx) + (y + vy) * sx + (z + vz) * sx * sy;
		voxels[x + 1][y + 1][z + 1] = voxels_ + index;
		materials[x + 1][y + 1][z + 1] = materials_[index];
	}
	if (materials[1][1][1] == NULL)
		return;

	/* Calculate deformation hints. */
	switch (materials[1][1][1]->type)
	{
		case LIVOX_MATERIAL_TYPE_LIQUID:
			liquid = 1;
			private_hint_liquid (voxels, materials);
			break;
		case LIVOX_MATERIAL_TYPE_ROUNDED:
		case LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL:
			private_hint_rounded (voxels, materials);
			break;
		case LIVOX_MATERIAL_TYPE_SLOPED:
		case LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL:
			private_hint_sloped (voxels, materials);
			break;
	}
}

/*****************************************************************************/

static void private_hint_liquid (
	LIVoxVoxel*    voxels[3][3][3],
	LIVoxMaterial* materials[3][3][3])
{
	/* TODO */
	voxels[1][1][1]->hint = 0;
}

static void private_hint_rounded (
	LIVoxVoxel*    voxels[3][3][3],
	LIVoxMaterial* materials[3][3][3])
{
	/* TODO */
	voxels[1][1][1]->hint = 0;
}

static void private_hint_sloped (
	LIVoxVoxel*    voxels[3][3][3],
	LIVoxMaterial* materials[3][3][3])
{
	int y;
	int y1;
	int slope[2][2] = { { 1, 1 }, { 1, 1 } };

	/* Decide whether to slope the bottom or the top side. */
	voxels[1][1][1]->hint = 0;
	if (ISLIQUIDEMPTY(1,0,1) + ISLIQUIDEMPTY(1,2,1) != 1)
		return;
	if (!ISLIQUIDEMPTY(1,0,1))
	{
		y = 2;
		y1 = 0;
	}
	else
	{
		y = 0;
		y1 = 2;
	}

	/* Determine the sloping states of corners. */
	/* Other tiles prevent slope creation by default. */
	if (!ISLIQUIDEMPTY(0,1,1)) slope[0][0] = slope[0][1] = 0;
	if (!ISLIQUIDEMPTY(2,1,1)) slope[1][0] = slope[1][1] = 0;
	if (!ISLIQUIDEMPTY(1,1,0)) slope[0][0] = slope[1][0] = 0;
	if (!ISLIQUIDEMPTY(1,1,2)) slope[0][1] = slope[1][1] = 0;
	/* Adjacent slopes are possible when facing empty tiles. */
	/* Facing negative X (west). */
	if (ISLIQUIDEMPTY(0,1,0) && ISLIQUIDEMPTY(0,1,1) && ISSLOPED(1,1,0) &&
	    ISLIQUIDEMPTY(1,y,0) && !ISLIQUIDEMPTY(1,y1,0) && 
	   !ISLIQUIDEMPTY(2,1,0) && !ISLIQUIDEMPTY(2,1,1)) slope[0][0] = 1;
	if (ISLIQUIDEMPTY(0,1,2) && ISLIQUIDEMPTY(0,1,1) && ISSLOPED(1,1,2) &&
	    ISLIQUIDEMPTY(1,y,2) && !ISLIQUIDEMPTY(1,y1,2) && 
	   !ISLIQUIDEMPTY(2,1,2) && !ISLIQUIDEMPTY(2,1,1)) slope[0][1] = 1;
	/* Facing positive X (east). */
	if (ISLIQUIDEMPTY(2,1,0) && ISLIQUIDEMPTY(2,1,1) && ISSLOPED(1,1,0) &&
	    ISLIQUIDEMPTY(1,y,0) && !ISLIQUIDEMPTY(1,y1,0) && 
	   !ISLIQUIDEMPTY(0,1,0) && !ISLIQUIDEMPTY(0,1,1)) slope[1][0] = 1;
	if (ISLIQUIDEMPTY(2,1,2) && ISLIQUIDEMPTY(2,1,1) && ISSLOPED(1,1,2) &&
	    ISLIQUIDEMPTY(1,y,2) && !ISLIQUIDEMPTY(1,y1,2) && 
	   !ISLIQUIDEMPTY(0,1,2) && !ISLIQUIDEMPTY(0,1,1)) slope[1][1] = 1;
	/* Facing negative Z (north). */
	if (ISLIQUIDEMPTY(1,1,0) && ISLIQUIDEMPTY(0,1,0) && ISSLOPED(0,1,1) &&
	    ISLIQUIDEMPTY(0,y,1) && !ISLIQUIDEMPTY(0,y1,1) && 
	   !ISLIQUIDEMPTY(1,1,2) && !ISLIQUIDEMPTY(0,1,2)) slope[0][0] = 1;
	if (ISLIQUIDEMPTY(1,1,0) && ISLIQUIDEMPTY(2,1,0) && ISSLOPED(2,1,1) &&
	    ISLIQUIDEMPTY(2,y,1) && !ISLIQUIDEMPTY(2,y1,1) && 
	   !ISLIQUIDEMPTY(1,1,2) && !ISLIQUIDEMPTY(2,1,2)) slope[1][0] = 1;
	/* Facing positive Z (south). */
	if (ISLIQUIDEMPTY(1,1,2) && ISLIQUIDEMPTY(0,1,2) && ISSLOPED(0,1,1) &&
	    ISLIQUIDEMPTY(0,y,1) && !ISLIQUIDEMPTY(0,y1,1) && 
	   !ISLIQUIDEMPTY(1,1,0) && !ISLIQUIDEMPTY(0,1,0)) slope[0][1] = 1;
	if (ISLIQUIDEMPTY(1,1,2) && ISLIQUIDEMPTY(2,1,2) && ISSLOPED(2,1,1) &&
	    ISLIQUIDEMPTY(2,y,1) && !ISLIQUIDEMPTY(2,y1,1) && 
	   !ISLIQUIDEMPTY(1,1,0) && !ISLIQUIDEMPTY(2,1,0)) slope[1][1] = 1;

	/* Store the sloping hints. */
	if (y == 2)
		voxels[1][1][1]->hint |= LIVOX_HINT_SLOPE_FACEUP;
	else
		voxels[1][1][1]->hint |= LIVOX_HINT_SLOPE_FACEDOWN;
	if (slope[0][0] && slope[1][0] && slope[0][1] && slope[1][1])
	{
		if (ISLIQUIDEMPTY(1,0,0) && ISLIQUIDEMPTY(1,0,2))
			voxels[1][1][1]->hint |= LIVOX_HINT_SLOPE_SPECIAL1;
		else if (ISLIQUIDEMPTY(0,0,1) && ISLIQUIDEMPTY(2,0,1))
			voxels[1][1][1]->hint |= LIVOX_HINT_SLOPE_SPECIAL2;
	}
	else
	{
		if (slope[0][0])
			voxels[1][1][1]->hint |= LIVOX_HINT_SLOPE_CORNER00;
		if (slope[1][0])
			voxels[1][1][1]->hint |= LIVOX_HINT_SLOPE_CORNER10;
		if (slope[0][1])
			voxels[1][1][1]->hint |= LIVOX_HINT_SLOPE_CORNER01;
		if (slope[1][1])
			voxels[1][1][1]->hint |= LIVOX_HINT_SLOPE_CORNER11;
	}
}

/** @} */
/** @} */
