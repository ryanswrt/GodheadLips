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

#ifndef __SCRIPT_ARGS_H__
#define __SCRIPT_ARGS_H__

#include <lipsofsuna/system.h>
#include "script-types.h"

enum
{
	LISCR_ARGS_INPUT_NORMAL,
	LISCR_ARGS_INPUT_TABLE
};

enum
{
	LISCR_ARGS_OUTPUT_NORMAL,
	LISCR_ARGS_OUTPUT_TABLE,
	LISCR_ARGS_OUTPUT_TABLE_FORCE
};

struct _LIScrArgs
{
	lua_State* lua;
	LIScrScript* script;
	LIScrData* data;
	void* self;
	int ret;
	int args_start;
	int args_count;
	int input_mode;
	int input_table;
	int output_mode;
	int output_table;
};

LIAPICALL (void, liscr_args_init_func, (
	LIScrArgs*   self,
	lua_State*   lua,
	LIScrScript* script,
	LIScrData*   data));

LIAPICALL (int, liscr_args_geti_bool, (
	LIScrArgs*  self,
	int         index,
	int*        result));

LIAPICALL (int, liscr_args_geti_bool_convert, (
	LIScrArgs*  self,
	int         index,
	int*        result));

LIAPICALL (int, liscr_args_geti_data, (
	LIScrArgs*  self,
	int         index,
	const char* type,
	LIScrData** result));

LIAPICALL (int, liscr_args_geti_float, (
	LIScrArgs*  self,
	int         index,
	float*      result));

LIAPICALL (int, liscr_args_geti_int, (
	LIScrArgs*  self,
	int         index,
	int*        result));

LIAPICALL (int, liscr_args_geti_intv, (
	LIScrArgs* self,
	int        index,
	int        count,
	int*       result));

LIAPICALL (int, liscr_args_geti_quaternion, (
	LIScrArgs*       self,
	int              index,
	LIMatQuaternion* result));

LIAPICALL (int, liscr_args_geti_string, (
	LIScrArgs*   self,
	int          index,
	const char** result));

LIAPICALL (int, liscr_args_geti_table, (
	LIScrArgs* self,
	int        index));

LIAPICALL (int, liscr_args_geti_vector, (
	LIScrArgs*   self,
	int          index,
	LIMatVector* result));

LIAPICALL (int, liscr_args_gets_bool, (
	LIScrArgs*  self,
	const char* name,
	int*        result));

LIAPICALL (int, liscr_args_gets_data, (
	LIScrArgs*  self,
	const char* name,
	const char* type,
	LIScrData** result));

LIAPICALL (int, liscr_args_gets_float, (
	LIScrArgs*  self,
	const char* name,
	float*      result));

LIAPICALL (int, liscr_args_gets_floatv, (
	LIScrArgs*  self,
	const char* name,
	int         count,
	float*      result));

LIAPICALL (int, liscr_args_gets_int, (
	LIScrArgs*  self,
	const char* name,
	int*        result));

LIAPICALL (int, liscr_args_gets_intv, (
	LIScrArgs*  self,
	const char* name,
	int         count,
	int*        result));

LIAPICALL (int, liscr_args_gets_quaternion, (
	LIScrArgs*       self,
	const char*      name,
	LIMatQuaternion* result));

LIAPICALL (int, liscr_args_gets_string, (
	LIScrArgs*   self,
	const char*  name,
	const char** result));

LIAPICALL (int, liscr_args_gets_table, (
	LIScrArgs*  self,
	const char* name));

LIAPICALL (int, liscr_args_gets_vector, (
	LIScrArgs*   self,
	const char*  name,
	LIMatVector* result));

LIAPICALL (void, liscr_args_set_output, (
	LIScrArgs* self,
	int        value));

LIAPICALL (void, liscr_args_setf_data, (
	LIScrArgs* self,
	double     name,
	LIScrData* value));

LIAPICALL (void, liscr_args_setf_float, (
	LIScrArgs* self,
	double     name,
	double     value));

LIAPICALL (void, liscr_args_setf_string, (
	LIScrArgs*  self,
	double      name,
	const char* value));

LIAPICALL (void, liscr_args_seti_bool, (
	LIScrArgs* self,
	int        value));

LIAPICALL (int, liscr_args_seti_data, (
	LIScrArgs* self,
	LIScrData* value));

LIAPICALL (void, liscr_args_seti_float, (
	LIScrArgs* self,
	float      value));

LIAPICALL (void, liscr_args_seti_int, (
	LIScrArgs* self,
	int        value));

LIAPICALL (void, liscr_args_seti_nil, (
	LIScrArgs* self));

LIAPICALL (void, liscr_args_seti_quaternion, (
	LIScrArgs*             self,
	const LIMatQuaternion* value));

LIAPICALL (void, liscr_args_seti_stack, (
	LIScrArgs* self));

LIAPICALL (void, liscr_args_seti_string, (
	LIScrArgs*  self,
	const char* value));

LIAPICALL (void, liscr_args_seti_vector, (
	LIScrArgs*         self,
	const LIMatVector* value));

LIAPICALL (void, liscr_args_sets_bool, (
	LIScrArgs*  self,
	const char* name,
	int         value));

LIAPICALL (void, liscr_args_sets_data, (
	LIScrArgs*  self,
	const char* name,
	LIScrData*  value));

LIAPICALL (void, liscr_args_sets_float, (
	LIScrArgs*  self,
	const char* name,
	float       value));

LIAPICALL (void, liscr_args_sets_int, (
	LIScrArgs*  self,
	const char* name,
	int         value));

LIAPICALL (void, liscr_args_sets_quaternion, (
	LIScrArgs*             self,
	const char*            name,
	const LIMatQuaternion* value));

LIAPICALL (void, liscr_args_sets_stack, (
	LIScrArgs*  self,
	const char* name));

LIAPICALL (void, liscr_args_sets_string, (
	LIScrArgs*  self,
	const char* name,
	const char* value));

LIAPICALL (void, liscr_args_sets_vector, (
	LIScrArgs*         self,
	const char*        name,
	const LIMatVector* value));

/*****************************************************************************/

int liscr_marshal_CLASS (lua_State* lua);

int liscr_marshal_DATA (lua_State* lua);

#endif
