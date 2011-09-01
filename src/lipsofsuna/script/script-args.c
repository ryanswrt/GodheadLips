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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrArgs Args
 * @{
 */

#include "script-args.h"
#include "script-library.h"
#include "script-private.h"
#include "script-util.h"

void liscr_args_init_func (
	LIScrArgs*   self,
	lua_State*   lua,
	LIScrScript* script,
	LIScrData*   data)
{
	memset (self, 0, sizeof (LIScrArgs));
	self->lua = lua;
	self->script = script;
	self->data = data;
	if (data != NULL)
	{
		self->self = data->data;
		self->args_start = 2;
	}
	else
		self->args_start = 1;
	self->args_count = lua_gettop (lua) + 1 - self->args_start;
	if (lua_type (self->lua, self->args_start) == LUA_TTABLE)
	{
		self->input_mode = LISCR_ARGS_INPUT_TABLE;
		self->input_table = self->args_start;
	}
}

int liscr_args_geti_bool (
	LIScrArgs*  self,
	int         index,
	int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_type (self->lua, -1) == LUA_TBOOLEAN)
		{
			*result = (int) lua_toboolean (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_type (self->lua, index) == LUA_TBOOLEAN)
		{
			*result = (int) lua_toboolean (self->lua, index);
			ret = 1;
		}
	}

	return ret;
}

int liscr_args_geti_bool_convert (
	LIScrArgs*  self,
	int         index,
	int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		*result = (int) lua_toboolean (self->lua, -1);
		ret = 1;
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		*result = (int) lua_toboolean (self->lua, index);
		ret = 1;
	}

	return ret;
}

int
liscr_args_geti_data (LIScrArgs*  self,
                      int         index,
                      const char* type,
                      LIScrData** result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (type != NULL)
			tmp = liscr_isdata (self->lua, -1, type);
		else
			tmp = liscr_isanydata (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (type != NULL)
			tmp = liscr_isdata (self->lua, index, type);
		else
			tmp = liscr_isanydata (self->lua, index);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

int liscr_args_geti_float (
	LIScrArgs*  self,
	int         index,
	float*      result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_isnumber (self->lua, -1))
		{
			*result = lua_tonumber (self->lua, -1);
			if (isnan (*result) || isinf (*result))
				*result = 0.0f;
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_isnumber (self->lua, index))
		{
			*result = lua_tonumber (self->lua, index);
			if (isnan (*result) || isinf (*result))
				*result = 0.0f;
			ret = 1;
		}
	}

	return ret;
}

int
liscr_args_geti_int (LIScrArgs*  self,
                     int         index,
                     int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_isnumber (self->lua, -1))
		{
			*result = (int) lua_tonumber (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_isnumber (self->lua, index))
		{
			*result = (int) lua_tonumber (self->lua, index);
			ret = 1;
		}
	}

	return ret;
}

int liscr_args_geti_intv (
	LIScrArgs* self,
	int        index,
	int        count,
	int*       result)
{
	int i;
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index);
		lua_gettable (self->lua, self->input_table);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
		{
			for (i = 0 ; i < count ; i++)
			{
				lua_pushnumber (self->lua, i + 1);
				lua_gettable (self->lua, -2);
				if (!lua_isnumber (self->lua, -1))
				{
					lua_pop (self->lua, 1);
					break;
				}
				result[i] = lua_tonumber (self->lua, -1);
				lua_pop (self->lua, 1);
			}
			ret = i;
		}
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_type (self->lua, index) == LUA_TTABLE)
		{
			for (i = 0 ; i < count ; i++)
			{
				lua_pushnumber (self->lua, i + 1);
				lua_gettable (self->lua, index);
				if (!lua_isnumber (self->lua, -1))
				{
					lua_pop (self->lua, 1);
					break;
				}
				result[i] = lua_tonumber (self->lua, -1);
				lua_pop (self->lua, 1);
			}
			ret = i;
		}
	}

	return ret;
}

int
liscr_args_geti_quaternion (LIScrArgs*       self,
                            int              index,
                            LIMatQuaternion* result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_QUATERNION);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((LIMatQuaternion*) tmp->data);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		tmp = liscr_isdata (self->lua, index, LISCR_SCRIPT_QUATERNION);
		if (tmp != NULL)
			*result = *((LIMatQuaternion*) tmp->data);
	}

	return tmp != NULL;
}

int
liscr_args_geti_string (LIScrArgs*   self,
                        int          index,
                        const char** result)
{
	const char* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_type (self->lua, -1) == LUA_TSTRING)
			tmp = lua_tostring (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_type (self->lua, index) == LUA_TSTRING)
			tmp = lua_tostring (self->lua, index);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

/**
 * \brief Gets a table by index and pushes it to the stack.
 * \param self Arguments.
 * \param index Argument index.
 * \return Nonzero on success.
 */
int liscr_args_geti_table (
	LIScrArgs* self,
	int        index)
{
	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
			return 1;
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_type (self->lua, index) == LUA_TTABLE)
		{
			lua_pushvalue (self->lua, index);
			return 1;
		}
	}

	return 0;
}

int
liscr_args_geti_vector (LIScrArgs*   self,
                        int          index,
                        LIMatVector* result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_VECTOR);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((LIMatVector*) tmp->data);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		tmp = liscr_isdata (self->lua, index, LISCR_SCRIPT_VECTOR);
		if (tmp != NULL)
			*result = *((LIMatVector*) tmp->data);
	}

	return tmp != NULL;
}

int
liscr_args_gets_bool (LIScrArgs*  self,
                      const char* name,
                      int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TBOOLEAN)
		{
			*result = (int) lua_toboolean (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int
liscr_args_gets_data (LIScrArgs*  self,
                      const char* name,
                      const char* type,
                      LIScrData** result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (type != NULL)
			tmp = liscr_isdata (self->lua, -1, type);
		else
			tmp = liscr_isanydata (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

int liscr_args_gets_float (
	LIScrArgs*  self,
	const char* name,
	float*      result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_isnumber (self->lua, -1))
		{
			*result = lua_tonumber (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int liscr_args_gets_floatv (
	LIScrArgs*  self,
	const char* name,
	int         count,
	float*      result)
{
	int i;
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
		{
			for (i = 0 ; i < count ; i++)
			{
				lua_pushnumber (self->lua, i + 1);
				lua_gettable (self->lua, -2);
				if (!lua_isnumber (self->lua, -1))
				{
					lua_pop (self->lua, 1);
					break;
				}
				result[i] = lua_tonumber (self->lua, -1);
				if (isnan (result[i]) || isinf (result[i]))
					result[i] = 0.0f;
				lua_pop (self->lua, 1);
			}
			ret = i;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int liscr_args_gets_int (
	LIScrArgs*  self,
	const char* name,
	int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_isnumber (self->lua, -1))
		{
			*result = (int) lua_tonumber (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int liscr_args_gets_intv (
	LIScrArgs*  self,
	const char* name,
	int         count,
	int*        result)
{
	int i;
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
		{
			for (i = 0 ; i < count ; i++)
			{
				lua_pushnumber (self->lua, i + 1);
				lua_gettable (self->lua, -2);
				if (!lua_isnumber (self->lua, -1))
				{
					lua_pop (self->lua, 1);
					break;
				}
				result[i] = lua_tonumber (self->lua, -1);
				lua_pop (self->lua, 1);
			}
			ret = i;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int
liscr_args_gets_quaternion (LIScrArgs*       self,
                            const char*      name,
                            LIMatQuaternion* result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_QUATERNION);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((LIMatQuaternion*) tmp->data);
	}

	return tmp != NULL;
}

int
liscr_args_gets_string (LIScrArgs*   self,
                        const char*  name,
                        const char** result)
{
	const char* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TSTRING)
			tmp = lua_tostring (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

/**
 * \brief Gets a table by name and pushes it to the stack.
 * \param self Arguments.
 * \param name Argument name.
 * \return Nonzero on success.
 */
int liscr_args_gets_table (
	LIScrArgs*  self,
	const char* name)
{
	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
			return 1;
		lua_pop (self->lua, 1);
	}

	return 0;
}

int
liscr_args_gets_vector (LIScrArgs*   self,
                        const char*  name,
                        LIMatVector* result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_VECTOR);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((LIMatVector*) tmp->data);
	}

	return tmp != NULL;
}

void
liscr_args_set_output (LIScrArgs* self,
                       int        value)
{
	lisys_assert (self->ret == 0);
	self->output_mode = value;

	if (value == LISCR_ARGS_OUTPUT_TABLE_FORCE)
	{
		lua_newtable (self->lua);
		self->output_table = lua_gettop (self->lua);
		self->output_mode = LISCR_ARGS_OUTPUT_TABLE;
	}
	else
		self->output_mode = value;
}

void
liscr_args_setf_data (LIScrArgs* self,
                      double     name,
                      LIScrData* value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			lua_pushnumber (self->lua, name);
			liscr_pushdata (self->lua, value);
			lua_settable (self->lua, self->output_table);
		}
	}
}

void
liscr_args_setf_float (LIScrArgs* self,
                       double     name,
                       double     value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, name);
		lua_pushnumber (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void
liscr_args_setf_string (LIScrArgs*  self,
                        double      name,
                        const char* value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, name);
		lua_pushstring (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void
liscr_args_seti_bool (LIScrArgs* self,
                      int        value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		lua_pushboolean (self->lua, value);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		lua_pushboolean (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

int liscr_args_seti_data (
	LIScrArgs* self,
	LIScrData* value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		if (value != NULL)
		{
			if (!liscr_pushdata (self->lua, value))
				return 0;
		}
		else
			lua_pushnil (self->lua);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			lua_pushnumber (self->lua, ++self->ret);
			if (!liscr_pushdata (self->lua, value))
			{
				lua_pop (self->lua, 1);
				return 0;
			}
			lua_settable (self->lua, self->output_table);
		}
	}

	return 1;
}

void
liscr_args_seti_float (LIScrArgs* self,
                       float      value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		lua_pushnumber (self->lua, value);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		lua_pushnumber (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void
liscr_args_seti_int (LIScrArgs* self,
                     int        value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		lua_pushnumber (self->lua, value);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		lua_pushnumber (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void liscr_args_seti_nil (
	LIScrArgs* self)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		lua_pushnil (self->lua);
		self->ret++;
	}
	else
		self->ret++;
}

void
liscr_args_seti_quaternion (LIScrArgs*             self,
                            const LIMatQuaternion* value)
{
	LIScrData* quat;

	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		quat = liscr_data_new_alloc (self->script, self->lua, sizeof (LIMatQuaternion), LISCR_SCRIPT_QUATERNION);
		if (quat != NULL)
		{
			*((LIMatQuaternion*) quat->data) = *value;
			self->ret++;
		}
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		quat = liscr_data_new_alloc (self->script, self->lua, sizeof (LIMatQuaternion), LISCR_SCRIPT_QUATERNION);
		if (quat != NULL)
		{
			*((LIMatQuaternion*) quat->data) = *value;
			lua_settable (self->lua, self->output_table);
		}
		else
			lua_pop (self->lua, 1);
	}
}

void
liscr_args_seti_stack (LIScrArgs* self)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			lua_pushnumber (self->lua, ++self->ret);
			lua_pushvalue (self->lua, -3);
			lua_remove (self->lua, -4);
			self->output_table = lua_gettop (self->lua) - 2;
			lua_settable (self->lua, self->output_table);
		}
		else
		{
			lua_pushnumber (self->lua, ++self->ret);
			lua_pushvalue (self->lua, -2);
			lua_settable (self->lua, self->output_table);
			lua_pop (self->lua, 1);
		}
	}
}

void
liscr_args_seti_string (LIScrArgs*  self,
                        const char* value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		if (value != NULL)
			lua_pushstring (self->lua, value);
		else
			lua_pushnil (self->lua);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			lua_pushnumber (self->lua, ++self->ret);
			lua_pushstring (self->lua, value);
			lua_settable (self->lua, self->output_table);
		}
	}
}

void liscr_args_seti_vector (
	LIScrArgs*         self,
	const LIMatVector* value)
{
	LIScrData* vector;

	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		vector = liscr_data_new_alloc (self->script, self->lua, sizeof (LIMatVector), LISCR_SCRIPT_VECTOR);
		if (vector != NULL)
		{
			*((LIMatVector*) vector->data) = *value;
			self->ret++;
		}
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		vector = liscr_data_new_alloc (self->script, self->lua, sizeof (LIMatVector), LISCR_SCRIPT_VECTOR);
		if (vector != NULL)
		{
			*((LIMatVector*) vector->data) = *value;
			lua_settable (self->lua, self->output_table);
		}
		else
			lua_pop (self->lua, 1);
	}
}

void
liscr_args_sets_bool (LIScrArgs*  self,
                      const char* name,
                      int         value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushboolean (self->lua, value);
		lua_setfield (self->lua, self->output_table, name);
	}
}

void
liscr_args_sets_data (LIScrArgs*  self,
                      const char* name,
                      LIScrData*  value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			liscr_pushdata (self->lua, value);
			lua_setfield (self->lua, self->output_table, name);
		}
	}
}

void
liscr_args_sets_float (LIScrArgs*  self,
                       const char* name,
                       float       value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, value);
		lua_setfield (self->lua, self->output_table, name);
	}
}

void
liscr_args_sets_int (LIScrArgs*  self,
                     const char* name,
                     int         value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, value);
		lua_setfield (self->lua, self->output_table, name);
	}
}

void liscr_args_sets_quaternion (
	LIScrArgs*             self,
	const char*            name,
	const LIMatQuaternion* value)
{
	LIScrData* quat;

	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		quat = liscr_data_new_alloc (self->script, self->lua, sizeof (LIMatQuaternion), LISCR_SCRIPT_QUATERNION);
		if (quat != NULL)
		{
			*((LIMatQuaternion*) quat->data) = *value;
			lua_setfield (self->lua, self->output_table, name);
		}
	}
}

void
liscr_args_sets_stack (LIScrArgs*  self,
                       const char* name)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			lua_pushvalue (self->lua, -2);
			lua_remove (self->lua, -3);
			self->output_table = lua_gettop (self->lua) - 1;
		}
		lua_setfield (self->lua, self->output_table, name);
	}
}

void
liscr_args_sets_string (LIScrArgs*  self,
                        const char* name,
                        const char* value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushstring (self->lua, value);
		lua_setfield (self->lua, self->output_table, name);
	}
}

void liscr_args_sets_vector (
	LIScrArgs*         self,
	const char*        name,
	const LIMatVector* value)
{
	LIScrData* vector;

	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		vector = liscr_data_new_alloc (self->script, self->lua, sizeof (LIMatVector), LISCR_SCRIPT_VECTOR);
		if (vector != NULL)
		{
			*((LIMatVector*) vector->data) = *value;
			lua_setfield (self->lua, self->output_table, name);
		}
	}
}

/*****************************************************************************/

int liscr_marshal_CLASS (lua_State* lua)
{
	LIScrArgs args;
	LIScrScript* script;
	void (*func)(LIScrArgs*);

	script = lua_touserdata (lua, lua_upvalueindex (1));
	func = lua_touserdata (lua, lua_upvalueindex (2));

	liscr_args_init_func (&args, lua, script, NULL);
	func (&args);

	if (args.output_table)
		return 1;
	else
		return args.ret;
}

int liscr_marshal_DATA (lua_State* lua)
{
	LIScrArgs args;
	LIScrData* data;
	LIScrScript* script;
	const char* type;
	void (*func)(LIScrArgs*);

	script = lua_touserdata (lua, lua_upvalueindex (1));
	type = lua_tostring (lua, lua_upvalueindex (2));
	func = lua_touserdata (lua, lua_upvalueindex (3));

	data = liscr_isdata (lua, 1, type);
	if (data == NULL)
		return 0;
	liscr_args_init_func (&args, lua, script, data);
	func (&args);

	if (args.output_table)
		return 1;
	else
		return args.ret;
}

/** @} */
/** @} */
