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
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include <lipsofsuna/render.h>
#include "ext-module.h"

static void Scene_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenFramebuffer* self;
	LIScrData* data;

	/* Allocate self. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SCENE);
	self = liren_framebuffer_new (module->client->render, 32, 32, 1, 0);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_SCENE, liren_framebuffer_free);
	if (data == NULL)
	{
		liren_framebuffer_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Scene_render (LIScrArgs* args)
{
	int i;
	int pass;
	int sort;
	int mipmap;
	int hdr = 0;
	int multisamples = 1;
	int postproc_passes_num = 0;
	int render_passes_num = 0;
	char* shader;
	GLint viewport[4];
	LIExtModule* module;
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenPassPostproc postproc_passes[10];
	LIRenPassRender render_passes[10];
	LIRenScene* scene;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;

	/* Get arguments. */
	modelview = limat_matrix_identity ();
	projection = limat_matrix_identity ();
	liscr_args_gets_bool (args, "hdr", &hdr);
	liscr_args_gets_floatv (args, "modelview", 16, modelview.m);
	if (liscr_args_gets_int (args, "multisamples", &multisamples))
		multisamples = LIMAT_MAX (1, multisamples);
	liscr_args_gets_floatv (args, "projection", 16, projection.m);
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = module->client->window->mode.width;
	viewport[3] = module->client->window->mode.height;
	liscr_args_gets_intv (args, "viewport", 4, viewport);
	viewport[0] = LIMAT_MAX (0, viewport[0]);
	viewport[1] = LIMAT_MAX (0, viewport[1]);
	viewport[1] = module->client->window->mode.height - viewport[1] - viewport[3];
	viewport[2] = LIMAT_MAX (2, viewport[2]);
	viewport[3] = LIMAT_MAX (2, viewport[3]);

	/* Collect render passes. */
	if (liscr_args_gets_table (args, "render_passes"))
	{
		for (i = 1 ; i < 10 ; i++)
		{
			/* Get the pass table. */
			lua_pushnumber (args->lua, i);
			lua_gettable (args->lua, -2);
			if (lua_type (args->lua, -1) != LUA_TTABLE)
			{
				lua_pop (args->lua, 1);
				break;
			}

			/* Get shader pass. */
			lua_getfield (args->lua, -1, "pass");
			if (lua_type (args->lua, -1) != LUA_TNUMBER)
			{
				lua_pop (args->lua, 2);
				continue;
			}
			pass = lua_tointeger (args->lua, -1);
			if (pass < 1 || pass > LIREN_SHADER_PASS_COUNT)
			{
				lua_pop (args->lua, 2);
				continue;
			}
			lua_pop (args->lua, 1);

			/* Get sorting. */
			lua_getfield (args->lua, -1, "sorting");
			sort = lua_toboolean (args->lua, -1);
			lua_pop (args->lua, 2);

			/* Add the pass. */
			render_passes[render_passes_num].pass = pass - 1;
			render_passes[render_passes_num].sort = sort;
			render_passes_num++;
		}
		lua_pop (args->lua, 1);
	}

	/* Collect post-processing passes. */
	if (liscr_args_gets_table (args, "postproc_passes"))
	{
		for (i = 1 ; i < 10 ; i++)
		{
			/* Get the pass table. */
			lua_pushnumber (args->lua, i);
			lua_gettable (args->lua, -2);
			if (lua_type (args->lua, -1) != LUA_TTABLE)
			{
				lua_pop (args->lua, 1);
				break;
			}

			/* Get shader name. */
			lua_getfield (args->lua, -1, "shader");
			if (lua_type (args->lua, -1) != LUA_TSTRING)
			{
				lua_pop (args->lua, 2);
				continue;
			}
			shader = lisys_string_dup (lua_tostring (args->lua, -1));
			lua_pop (args->lua, 1);

			/* Get mipmapping. */
			lua_getfield (args->lua, -1, "mipmaps");
			mipmap = lua_toboolean (args->lua, -1);
			lua_pop (args->lua, 2);

			/* Add the pass. */
			postproc_passes[postproc_passes_num].shader = shader;
			postproc_passes[postproc_passes_num].mipmap = mipmap;
			postproc_passes_num++;
		}
		lua_pop (args->lua, 1);
	}

	/* Render the scene. */
	limat_frustum_init (&frustum, &modelview, &projection);
	if (liren_framebuffer_resize (args->self, viewport[2], viewport[3], multisamples, hdr))
	{
		liren_scene_render (scene, args->self, viewport, &modelview, &projection, &frustum,
			render_passes, render_passes_num, postproc_passes, postproc_passes_num);
	}
	else
		lisys_error_report ();

	/* Free the shader names we allocated. */
	for (i = 0 ; i < postproc_passes_num ; i++)
		lisys_free ((char*) postproc_passes[i].shader);
}

/*****************************************************************************/

void liext_script_scene (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SCENE, "scene_new", Scene_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SCENE, "scene_render", Scene_render);
}

/** @} */
/** @} */
