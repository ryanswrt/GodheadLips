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

#ifndef __RENDER21_PRIVATE_H__
#define __RENDER21_PRIVATE_H__

#include "lipsofsuna/algorithm.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/paths.h"
#include "lipsofsuna/image.h"
#include "render-types.h"

struct _LIRenBuffer21
{
	int type;
	GLuint index_buffer;
	GLuint vertex_array;
	GLuint vertex_buffer;
	LIRenFormat vertex_format;
	struct
	{
		int count;
	} indices;
	struct
	{
		int count;
	} vertices;
};

struct _LIRenFramebuffer21
{
	int width;
	int height;
	int samples;
	LIRenRender21* render;
};

struct _LIRenImage21
{
	char* name;
	char* path;
	LIImgTexture* texture;
	LIRenRender21* render;
};

struct _LIRenLight21
{
	int enabled;
	float priority;
	float cutoff;
	float exponent;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float equation[3];
	LIMatAabb bounds;
	LIMatTransform transform;
	const LIMdlNode* node;
	LIRenScene21* scene;
};

struct _LIRenMaterial21
{
	float shininess;
	float diffuse[4];
	float specular[4];
	LIRenImage21* image;
};

typedef struct _LIRenModelGroup21 LIRenModelGroup21;
struct _LIRenModelGroup21
{
	int start;
	int count;
};

struct _LIRenModel21
{
	LIMatAabb bounds;
	LIRenRender21* render;
	LIRenBuffer21* buffer;
	struct
	{
		int count;
		LIRenModelGroup21* array;
	} groups;
	struct
	{
		int count;
		LIRenIndex* array;
	} indices;
	struct
	{
		int count;
		LIRenMaterial21* array;
	} materials;
	struct
	{
		int count;
		LIMdlVertex* array;
	} vertices;
};

struct _LIRenObject21
{
	int realized;
	LIMatAabb bounds;
	LIMatTransform transform;
	LIMdlPose* pose;
	LIRenScene21* scene;
	LIRenModel21* model;
	struct
	{
		int count;
		LIRenLight21** array;
	} lights;
	struct
	{
		LIMatVector center;
		LIMatMatrix matrix;
	} orientation;
};

struct _LIRenRender21
{
	LIAlgPtrdic* scenes;
	LIPthPaths* paths;
	struct _LIRenRender* render;
};

struct _LIRenScene21
{
	float time;
	LIAlgPtrdic* lights;
	LIRenRender21* render;
	struct _LIRenScene* scene;
};

struct _LIRenShader21
{
	int sort;
	char* name;
	LIRenRender21* render;
};

#endif
