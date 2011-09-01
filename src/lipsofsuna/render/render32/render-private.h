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

#ifndef __RENDER32_PRIVATE_H__
#define __RENDER32_PRIVATE_H__

#include "lipsofsuna/algorithm.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/paths.h"
#include "lipsofsuna/image.h"
#include "render-lod.h"
#include "render-mesh.h"
#include "render-particles.h"
#include "render-program.h"
#include "render-types.h"

#define LIREN_SHADER_PASS_COUNT 10

/* #define LIREN_ENABLE_PROFILING */

struct _LIRenBuffer32
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

struct _LIRenFramebuffer32
{
	int hdr;
	int width;
	int height;
	int samples;
	GLuint render_framebuffer;
	GLuint render_textures[2];
	GLuint postproc_framebuffers[2];
	GLuint postproc_textures[3];
	LIRenRender32* render;
};

struct _LIRenImage32
{
	char* name;
	char* path;
	LIImgTexture* texture;
	LIRenRender32* render;
};

struct _LIRenLight32
{
	int directional;
	int enabled;
	float priority;
	float cutoff;
	float exponent;
	float shadow_far;
	float shadow_near;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float equation[3];
	LIMatAabb bounds;
	LIMatMatrix projection;
	LIMatMatrix modelview;
	LIMatMatrix modelview_inverse;
	LIMatTransform transform;
	const LIMdlNode* node;
	LIRenScene32* scene;
	struct
	{
		float pos_world[3];
		float pos_view[3];
		float dir_world[3];
		float dir_view[3];
		float spot[3];
		LIMatMatrix matrix;
	} cache;
	struct
	{
		GLuint fbo;
		GLuint map;
	} shadow;
};

struct _LIRenMaterial32
{
	int flags;
	float parameters[4];
	float shininess;
	float diffuse[4];
	float specular[4];
	float strand_start;
	float strand_end;
	float strand_shape;
	LIRenShader32* shader;
	struct
	{
		int count;
		LIRenImage32** array;
	} textures;
};

struct _LIRenModel32
{
	LIMatAabb bounds;
	LIRenParticles32 particles;
	LIRenRender32* render;
	struct
	{
		int count;
		LIRenLod32* array;
	} lod;
	struct
	{
		int count;
		LIRenMaterial32** array;
	} materials;
};

struct _LIRenObject32
{
	int realized;
	float sort;
	LIMatAabb bounds;
	LIMatTransform transform;
	LIMdlPose* pose;
	LIRenScene32* scene;
	LIRenModel32* model;
	struct
	{
		int width;
		int height;
		GLuint depth;
		GLuint map;
		GLuint fbo[6];
	} cubemap;
	struct
	{
		LIRenMaterial32* material;
	} effect;
	struct
	{
		int count;
		LIRenLight32** array;
	} lights;
	struct
	{
		LIMatVector center;
		LIMatMatrix matrix;
	} orientation;
	struct
	{
		int loop;
		float time;
	} particle;
};

struct _LIRenRender32
{
	int anisotrophy;
	float lod_near;
	float lod_far;
	LIAlgPtrdic* framebuffers;
	LIAlgPtrdic* scenes;
	LIPthPaths* paths;
	LIRenContext32* context;
	struct _LIRenRender* render;
	struct
	{
		float time;
		GLuint noise;
		GLuint depth_texture_max;
		GLuint empty_texture;
		LIRenBuffer32* unit_quad;
	} helpers;
	struct
	{
		int offset;
		LIRenBuffer32* buffer;
	} immediate;
#ifdef LIREN_ENABLE_PROFILING
	struct
	{
		int objects;
		int materials;
		int faces;
		int vertices;
	} profiling;
#endif
};

struct _LIRenScene32
{
	float time;
	LIRenRender32* render;
	LIRenSort32* sort;
	LIRenLighting32* lighting;
	struct _LIRenScene* scene;
};

struct _LIRenShader32
{
	int sort;
	char* name;
	LIRenRender32* render;
	LIRenProgram32 passes[LIREN_SHADER_PASS_COUNT];
};

#endif
