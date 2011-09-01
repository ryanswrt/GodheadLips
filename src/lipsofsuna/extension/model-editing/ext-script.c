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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtModelEditing ModelEditing
 * @{
 */

#include "ext-module.h"

static void Model_add_material (LIScrArgs* args)
{
	int cull;
	float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* shader;
	LIMdlBuilder* builder;
	LIMdlMaterial material;
	LIEngModel* model;

	/* Create a model builder. */
	model = args->self;
	builder = limdl_builder_new (model->model);
	if (builder == NULL)
		return;

	/* Read material properties. */
	/* TODO: Support more. */
	limdl_material_init (&material);
	if (liscr_args_gets_bool (args, "cull", &cull) && !cull)
		material.flags &= ~LIMDL_MATERIAL_FLAG_CULLFACE;
	if (liscr_args_gets_floatv (args, "diffuse", 4, diffuse))
		limdl_material_set_diffuse (&material, diffuse);
	if (liscr_args_gets_string (args, "shader", &shader))
		limdl_material_set_shader (&material, shader);
	if (liscr_args_gets_floatv (args, "specular", 4, specular))
		limdl_material_set_specular (&material, specular);

	/* Insert the material. */
	limdl_builder_insert_material (builder, &material);
	limdl_builder_finish (builder);

	/* Cleanup. */
	limdl_builder_free (builder);
	limdl_material_free (&material);
}

static void Model_add_triangles (LIScrArgs* args)
{
	int i;
	int group = 1;
	int vertices_cap = 0;
	int vertices_num = 0;
	float* attrs[8];
	LIMdlIndex* indices = NULL;
	LIMatVector zero = { 0.0f, 0.0f, 0.0f };
	LIMdlBuilder* builder;
	LIMdlVertex* tmp;
	LIMdlVertex* vertex;
	LIMdlVertex* vertices = NULL;
	LIEngModel* model;

	/* Get the edited material group. */
	model = args->self;
	liscr_args_gets_int (args, "material", &group);
	if (group < 1 || group > model->model->materials.count)
		return;

	/* Read vertex data. */
	/* Vertices are stored to a sequential table. Each vertex is presented as
	   a sequential child table whose fields are presented in the order of
	   {x,y,z,nx,ny,nz,u,v}. */
	if (liscr_args_gets_table (args, "vertices"))
	{
		for (vertices_num = 0 ; ; vertices_num++)
		{
			/* Get the vertex table. */
			lua_pushnumber (args->lua, vertices_num + 1);
			lua_gettable (args->lua, -2);
			if (lua_type (args->lua, -1) != LUA_TTABLE)
			{
				lua_pop (args->lua, 1);
				break;
			}

			/* Allocate space for the vertex. */
			if (vertices_cap < vertices_num + 1)
			{
				if (!vertices_cap)
					vertices_cap = 32;
				else
					vertices_cap <<= 1;
				tmp = realloc (vertices, vertices_cap * sizeof (LIMdlVertex));
				if (tmp == NULL)
				{
					lisys_free (vertices);
					return;
				}
				vertices = tmp;
			}
			vertex = vertices + vertices_num;
			limdl_vertex_init (vertex, &zero, &zero, 0.0f, 0.0f);

			/* Read vertex attributes. */
			attrs[0] = &vertex->coord.x;
			attrs[1] = &vertex->coord.y;
			attrs[2] = &vertex->coord.z;
			attrs[3] = &vertex->normal.x;
			attrs[4] = &vertex->normal.y;
			attrs[5] = &vertex->normal.z;
			attrs[6] = vertex->texcoord + 0;
			attrs[7] = vertex->texcoord + 1;
			for (i = 0 ; i < 8 ; i++)
			{
				lua_pushnumber (args->lua, i + 1);
				lua_gettable (args->lua, -2);
				if (lua_type (args->lua, -1) == LUA_TNUMBER)
					*(attrs[i]) = lua_tonumber (args->lua, -1);
				else
					*(attrs[i]) = 0.0f;
				lua_pop (args->lua, 1);
			}
			lua_pop (args->lua, 1);
		}
		lua_pop (args->lua, 1);
	}

	/* Validate the vertex count. */
	if (!vertices_num)
		return;
	vertices_num -= vertices_num % 3;
	if (!vertices_num)
	{
		lisys_free (vertices);
		return;
	}

	/* Create the index buffer. */
	indices = calloc (vertices_num, sizeof (LIMdlIndex));
	if (indices == NULL)
	{
		lisys_free (vertices);
		return;
	}
	for (i = 0 ; i < vertices_num ; i++)
		indices[i] = model->model->vertices.count + i;

	/* Create a model builder. */
	builder = limdl_builder_new (model->model);
	if (builder == NULL)
	{
		lisys_free (vertices);
		lisys_free (indices);
		return;
	}

	/* Insert the vertices and indices. */
	limdl_builder_insert_vertices (builder, vertices, vertices_num, NULL);
	limdl_builder_insert_indices (builder, 0, group - 1, indices, vertices_num, 0);
	limdl_builder_finish (builder);

	/* Cleanup. */
	limdl_builder_free (builder);
	lisys_free (vertices);
	lisys_free (indices);
}

static void Model_calculate_lod (LIScrArgs* args)
{
	int levels = 5;
	float factor = 0.3f;
	LIEngModel* model;
	LIMdlBuilder* builder;

	model = args->self;
	if (liscr_args_geti_int (args, 0, &levels))
		levels = LIMAT_MAX (1, levels);
	if (liscr_args_geti_float (args, 1, &factor))
		factor = LIMAT_CLAMP (factor, 0.0f, 1.0f);

	builder = limdl_builder_new (model->model);
	if (builder == NULL)
		return;
	limdl_builder_calculate_lod (builder, levels, factor);
	limdl_builder_finish (builder);
	limdl_builder_free (builder);
	liscr_args_seti_bool (args, 1);
}

static void Model_edit_material (LIScrArgs* args)
{
	int i;
	int j;
	int n;
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* str;
	const char* shader = NULL;
	const char* texture = NULL;
	LIEngModel* model;
	LIMdlMaterial* material;

	/* Get the engine model. */
	model = args->self;
	liscr_args_gets_string (args, "match_shader", &shader);
	liscr_args_gets_string (args, "match_texture", &texture);

	/* Edit each matching material. */
	for (i = 0 ; i < model->model->materials.count ; i++)
	{
		material = model->model->materials.array + i;
		if (!limdl_material_compare_shader_and_texture (material, shader, texture))
			continue;
		if (liscr_args_gets_floatv (args, "diffuse", 4, color))
			limdl_material_set_diffuse (material, color);
		if (liscr_args_gets_floatv (args, "specular", 4, color))
			limdl_material_set_specular (material, color);
		if (liscr_args_gets_string (args, "shader", &str))
			limdl_material_set_shader (material, str);
		if (liscr_args_gets_table (args, "textures"))
		{
			/* Count textures. */
			for (n = 0 ; n < 4 ; n++)
			{
				lua_pushnumber (args->lua, n + 1);
				lua_gettable (args->lua, -2);
				j = lua_type (args->lua, -1);
				lua_pop (args->lua, 1);
				if (j != LUA_TSTRING)
					break;
			}
			/* Allocate textures. */
			if (material->textures.count != n)
			{
				if (!limdl_material_realloc_textures (material, n))
				{
					lua_pop (args->lua, 1);
					continue;
				}
			}
			/* Set textures. */
			for (j = 0 ; j < n ; j++)
			{
				lua_pushnumber (args->lua, j + 1);
				lua_gettable (args->lua, -2);
				limdl_material_set_texture (material, j, LIMDL_TEXTURE_TYPE_IMAGE,
					LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP |
					LIMDL_TEXTURE_FLAG_REPEAT, lua_tostring (args->lua, -1));
				lua_pop (args->lua, 1);
			}
			lua_pop (args->lua, 1);
		}
	}
}

static void Model_merge (LIScrArgs* args)
{
	LIScrData* data;
	LIEngModel* model1;
	LIEngModel* model2;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &data) ||
	    liscr_args_gets_data (args, "model", LISCR_SCRIPT_MODEL, &data))
	{
		model1 = args->self;
		model2 = liscr_data_get_data (data);
		if (!limdl_model_merge (model1->model, model2->model))
			lisys_error_report ();
	}
}

static void Model_morph (LIScrArgs* args)
{
	float value = 0.5f;
	const char* shape;
	LIEngModel* model;
	LIEngModel* ref = NULL;
	LIScrData* data;

	model = args->self;
	if (!liscr_args_geti_string (args, 0, &shape) &&
	    !liscr_args_gets_string (args, "shape", &shape))
		return;
	if (!liscr_args_geti_float (args, 1, &value))
		liscr_args_gets_float (args, "value", &value);
	if (!liscr_args_geti_data (args, 2, LISCR_SCRIPT_MODEL, &data))
		liscr_args_gets_data (args, "ref", LISCR_SCRIPT_MODEL, &data);
	if (data != NULL)
		ref = liscr_data_get_data (data);

	liscr_args_seti_bool (args, limdl_model_morph (model->model,
		(ref != NULL)? ref->model : NULL, shape, value));
}

static void Model_remove_vertices (LIScrArgs* args)
{
	LIEngModel* model;

	model = args->self;
	limdl_model_clear_vertices (model->model);
}

/*****************************************************************************/

void liext_script_model_editing (
	LIScrScript* self)
{
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_add_material", Model_add_material);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_add_triangles", Model_add_triangles);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_calculate_lod", Model_calculate_lod);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_edit_material", Model_edit_material);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_merge", Model_merge);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_morph", Model_morph);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_remove_vertices", Model_remove_vertices);
}

/** @} */
/** @} */
