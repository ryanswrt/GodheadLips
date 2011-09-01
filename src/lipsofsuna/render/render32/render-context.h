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

#ifndef __RENDER32_CONTEXT_H__
#define __RENDER32_CONTEXT_H__

#include "lipsofsuna/image.h"
#include "lipsofsuna/model.h"
#include "render-buffer.h"
#include "render-buffer-texture.h"
#include "render-light.h"
#include "render-material.h"
#include "render-scene.h"
#include "render-types.h"
#include "render-mesh.h"
#include "render-uniforms.h"

/* If you changed this, also remember change LOS_LIGHT_MAX in shader program
 * and uniforms in the uniform list and in liren_uniforms32_setup(). */
#define LIREN_CONTEXT_MAX_LIGHTS 5

typedef struct _LIRenContextTexture32 LIRenContextTexture32;
struct _LIRenContextTexture32
{
	GLuint texture;
};

struct _LIRenContext32
{
	int deferred;
	int incomplete;
	int shadows;
	GLuint array;
	LIRenRender32* render;
	LIRenScene32* scene;
	LIRenShader32* shader;
	LIMatFrustum frustum;
	LIMatVector camera_position;
	LIRenBufferTexture32 buffer_texture;
	LIRenUniforms32 uniforms;
	GLuint shadow_texture;
	int alpha_to_coverage;
	int color_write;
	int shader_pass;
	struct
	{
		int enable;
		GLenum blend_src;
		GLenum blend_dst;
	} blend;
	struct
	{
		unsigned int blend : 1;
		unsigned int buffer : 1;
		unsigned int cull : 1;
		unsigned int depth : 1;
		unsigned int shader : 1;
	} changed;
	struct
	{
		int enable;
		GLenum front_face;
	} cull;
	struct
	{
		int enable_test;
		int enable_write;
		int depth_func;
	} depth;
	struct
	{
		LIMatMatrix model;
		LIMatMatrix modelview;
		LIMatMatrix modelviewinverse;
		LIMatMatrix projection;
		LIMatMatrix projectioninverse;
		LIMatMatrix view;
	} matrix;
	struct
	{
		int enabled;
		int rect[4];
	} scissor;
	struct
	{
		int count;
		LIRenContextTexture32 array[9];
		LIRenContextTexture32 cubemap;
	} textures;
};

LIAPICALL (void, liren_context32_init, (
	LIRenContext32* self));

LIAPICALL (void, liren_context32_bind, (
	LIRenContext32* self));

LIAPICALL (void, liren_context32_render_array, (
	LIRenContext32* self,
	GLenum          type,
	int             start,
	int             count));

LIAPICALL (int, liren_context32_render_immediate, (
	LIRenContext32*    self,
	GLenum             type,
	const LIRenVertex* vertices,
	int                count));

LIAPICALL (void, liren_context32_render_indexed, (
	LIRenContext32* self,
	int             start,
	int             count));

LIAPICALL (void, liren_context32_set_alpha_to_coverage, (
	LIRenContext32* self,
	int             value));

LIAPICALL (void, liren_context32_set_blend, (
	LIRenContext32* self,
	int             enable,
	GLenum          blend_src,
	GLenum          blend_dst));

LIAPICALL (void, liren_context32_set_buffer, (
	LIRenContext32* self,
	LIRenBuffer32*  vertex));

LIAPICALL (void, liren_context32_set_camera_position, (
	LIRenContext32*    self,
	const LIMatVector* value));

LIAPICALL (void, liren_context32_set_color_write, (
	LIRenContext32* self,
	int             value));

LIAPICALL (void, liren_context32_set_cull, (
	LIRenContext32* self,
	int             enable,
	int             front_face));

LIAPICALL (int, liren_context32_get_deferred, (
	LIRenContext32* self));

LIAPICALL (void, liren_context32_set_deferred, (
	LIRenContext32* self,
	int             value));

LIAPICALL (void, liren_context32_set_depth, (
	LIRenContext32* self,
	int             enable_test,
	int             enable_write,
	GLenum          depth_func));

LIAPICALL (void, liren_context32_set_diffuse, (
	LIRenContext32* self,
	const float*    value));

LIAPICALL (void, liren_context32_set_flags, (
	LIRenContext32* self,
	int             value));

LIAPICALL (void, liren_context32_set_frustum, (
	LIRenContext32*       self,
	const LIMatFrustum* frustum));

LIAPICALL (void, liren_context32_set_light, (
	LIRenContext32* self,
	int             index,
	LIRenLight32*   value));

LIAPICALL (void, liren_context32_set_material, (
	LIRenContext32*        self,
	const LIRenMaterial32* value));

LIAPICALL (void, liren_context32_set_mesh, (
	LIRenContext32* self,
	LIRenMesh32*    mesh));

LIAPICALL (void, liren_context32_set_modelmatrix, (
	LIRenContext32*    self,
	const LIMatMatrix* value));

LIAPICALL (void, liren_context32_set_param, (
	LIRenContext32* self,
	const float*    value));

LIAPICALL (int, liren_context32_set_pose, (
	LIRenContext32*  self,
	const LIMdlPose* pose));

LIAPICALL (void, liren_context32_set_viewmatrix, (
	LIRenContext32*    self,
	const LIMatMatrix* value));

LIAPICALL (void, liren_context32_set_projection, (
	LIRenContext32*    self,
	const LIMatMatrix* value));

LIAPICALL (void, liren_context32_set_scene, (
	LIRenContext32* self,
	LIRenScene32*   scene));

LIAPICALL (int, liren_context32_get_scissor, (
	LIRenContext32* self,
	int*            x,
	int*            y,
	int*            w,
	int*            h));

LIAPICALL (void, liren_context32_set_scissor, (
	LIRenContext32* self,
	int             enabled,
	int             x,
	int             y,
	int             w,
	int             h));

LIAPICALL (void, liren_context32_set_shader, (
	LIRenContext32* self,
	int             pass,
	LIRenShader32*  value));

LIAPICALL (void, liren_context32_set_textures, (
	LIRenContext32* self,
	LIRenImage32**  value,
	int             count));

LIAPICALL (void, liren_context32_set_textures_raw, (
	LIRenContext32* self,
	GLuint*         value,
	int             count));

LIAPICALL (void, liren_context32_set_time, (
	LIRenContext32* self,
	float           time));

#endif
