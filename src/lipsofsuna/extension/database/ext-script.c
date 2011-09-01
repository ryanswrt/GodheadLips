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
 * \addtogroup LIExtDatabase Database
 * @{
 */

#include <sqlite3.h>
#include "ext-module.h"

static void private_free_database (
	sqlite3* self)
{
	/* Make sure we aren't leaking statements. */
	lisys_assert (sqlite3_next_stmt (self, NULL) == NULL);

	sqlite3_close (self);
}

/*****************************************************************************/

static void Database_get_memory_used (LIScrArgs* args)
{
	liscr_args_seti_int (args, sqlite3_memory_used ());
}

static void Database_new (LIScrArgs* args)
{
	int ok;
	char* path;
	char* path1;
	const char* ptr;
	const char* name;
	sqlite3* sql;
	LIExtModule* module;
	LIScrData* data;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_DATABASE);

	/* Get and validate the filename. */
	if (!liscr_args_geti_string (args, 0, &name) &&
	    !liscr_args_gets_string (args, "name", &name))
		return;
	for (ptr = name ; *ptr != '\0' ; ptr++)
	{
		if (*ptr >= 'a' && *ptr <= 'z') ok = 1;
		else if (*ptr >= 'A' && *ptr <= 'Z') ok = 1;
		else if (*ptr >= '0' && *ptr <= '9') ok = 1;
		else if (*ptr == '-') ok = 1;
		else if (*ptr == '_') ok = 1;
		else if (*ptr == '.') ok = 1;
		else ok = 0;
		if (!ok)
		{
			lisys_error_set (EINVAL, "invalid database name `%s'", name);
			lisys_error_report ();
			return;
		}
	}

	/* Format the path. */
	path = lipth_paths_get_sql (module->program->paths, name);
	if (path == NULL)
	{
		lisys_error_report ();
		return;
	}

	/* Convert the path to UTF-8. */
	/* Unlike other file functions we use, SQLite requires the filename to
	   be in UTF-8 instead of the system locale. */
	path1 = lisys_string_convert_sys_to_utf8 (path);
	if (path1 != NULL)
	{
		lisys_free (path);
		path = path1;
	}

	/* Open database. */
	if (sqlite3_open_v2 (path, &sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (sql));
		lisys_error_report ();
		lisys_free (path);
		lisys_free (sql);
		return;
	}
	lisys_free (path);

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, sql, LIEXT_SCRIPT_DATABASE, private_free_database);
	if (data == NULL)
	{
		sqlite3_close (sql);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Database_query (LIScrArgs* args)
{
	int i;
	int col;
	int row;
	int ret;
	int size;
	const char* query;
	const char* str;
	sqlite3* self;
	LIArcPacket* packet;
	LIScrData* data;
	sqlite3_stmt* statement;

	self = args->self;
	if (!liscr_args_geti_string (args, 0, &query) &&
	    !liscr_args_gets_string (args, "query", &query))
		return;

	/* Create a statement. */
	if (sqlite3_prepare_v2 (self, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self));
		lisys_error_report ();
		return;
	}

	/* Bind variables. */
	if (liscr_args_geti_table (args, 1) || liscr_args_gets_table (args, "bind"))
	{
		for (i = 1 ; i < sqlite3_bind_parameter_count (statement) + 1 ; i++)
		{
			/* We got a table that has the bound variables in fields matching
			   the indices of the bound variables. We can simply loop through
			   the table and use the binding index as the key. */
			lua_pushnumber (args->lua, i);
			lua_gettable (args->lua, -2);
			switch (lua_type (args->lua, -1))
			{
				/* Bind numbers as doubles. */
				case LUA_TNUMBER:
					if (sqlite3_bind_double (statement, i, lua_tonumber (args->lua, -1)) != SQLITE_OK)
					{
						lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
						lisys_error_report ();
						sqlite3_finalize (statement);
						return;
					}
					break;

				/* Bind strings as text. */
				case LUA_TSTRING:
					if (sqlite3_bind_text (statement, i, lua_tostring (args->lua, -1), -1, SQLITE_TRANSIENT) != SQLITE_OK)
					{
						lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
						lisys_error_report ();
						sqlite3_finalize (statement);
						return;
					}
					break;

				/* Bind packets as blobs. */
				case LUA_TUSERDATA:
					data = liscr_isdata (args->lua, -1, LISCR_SCRIPT_PACKET);
					if (data == NULL)
						break;
					packet = liscr_data_get_data (data);
					if (packet->writer != NULL)
					{
						if (sqlite3_bind_blob (statement, i, packet->writer->memory.buffer,
							packet->writer->memory.length, SQLITE_TRANSIENT) != SQLITE_OK)
						{
							lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
							lisys_error_report ();
							sqlite3_finalize (statement);
							return;
						}
					}
					else
					{
						if (sqlite3_bind_blob (statement, i, packet->reader->buffer,
							packet->reader->length, SQLITE_TRANSIENT) != SQLITE_OK)
						{
							lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
							lisys_error_report ();
							sqlite3_finalize (statement);
							return;
						}
					}
					break;

				/* Bind any other values as NULL. */
				default:
					if (sqlite3_bind_null (statement, i) != SQLITE_OK)
					{
						lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
						lisys_error_report ();
						sqlite3_finalize (statement);
						return;
					}
					break;
			}
			lua_pop (args->lua, 1);
		}
		lua_pop (args->lua, 1);
	}

	/* Execute the statement and process results. */
	for (row = 0, ret = sqlite3_step (statement) ; ret != SQLITE_DONE ; ret = sqlite3_step (statement), row++)
	{
		/* Check for errors. */
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self));
			lisys_error_report ();
			sqlite3_finalize (statement);
			return;
		}
		if (!row)
			liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);

		/* Create a row table. */
		lua_newtable (args->lua);

		/* Push the columns to the table. */
		for (col = 0 ; col < sqlite3_column_count (statement) ; col++)
		{
			switch (sqlite3_column_type (statement, col))
			{
				case SQLITE_INTEGER:
					lua_pushnumber (args->lua, col + 1);
					lua_pushnumber (args->lua, sqlite3_column_int (statement, col));
					lua_settable (args->lua, -3);
					break;
				case SQLITE_FLOAT:
					lua_pushnumber (args->lua, col + 1);
					lua_pushnumber (args->lua, sqlite3_column_double (statement, col));
					lua_settable (args->lua, -3);
					break;
				case SQLITE_TEXT:
					str = (const char*) sqlite3_column_text (statement, col);
					size = sqlite3_column_bytes (statement, col);
					lua_pushnumber (args->lua, col + 1);
					if (size > 0 && str != NULL)
						lua_pushstring (args->lua, str);
					else
						lua_pushstring (args->lua, str);
					lua_settable (args->lua, -3);
					break;
				case SQLITE_BLOB:
					str = sqlite3_column_blob (statement, col);
					size = sqlite3_column_bytes (statement, col);
					packet = liarc_packet_new_readable (str, size);
					if (packet != NULL)
					{
						lua_pushnumber (args->lua, col + 1);
						data = liscr_data_new (args->script, args->lua, packet, LISCR_SCRIPT_PACKET, liarc_packet_free);
						if (data != NULL)
							lua_settable (args->lua, -3);
						else
						{
							lua_pop (args->lua, 1);
							liarc_packet_free (packet);
						}
					}
					break;
				case SQLITE_NULL:
					break;
				default:
					lisys_assert (0 && "invalid column type");
					break;
			}
		}

		/* Add the row to the return values. */
		liscr_args_seti_stack (args);
	}
	if (!row)
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	sqlite3_finalize (statement);
}

/*****************************************************************************/

void liext_script_database (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_DATABASE, "database_get_memory_used", Database_get_memory_used);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_DATABASE, "database_new", Database_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_DATABASE, "database_query", Database_query);
}

/** @} */
/** @} */
