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
 * \addtogroup LIExtLobby Lobby
 * @{
 */

#include "ext-module.h"
#ifdef HAVE_CURL
#include <curl/curl.h>
#endif

#ifdef HAVE_CURL
static size_t private_write (
	void*        ptr,
	size_t       size,
	size_t       nmemb,
	LIArcWriter* writer)
{
    liarc_writer_append_raw (writer, ptr, size * nmemb);

    return size * nmemb;
}
#endif

/*****************************************************************************/

static void Lobby_download_server_list (LIScrArgs* args)
{
#ifdef HAVE_CURL
	int ret;
	int port = 0;
	int players = 0;
	char* url;
	char* desc;
	char* ip;
	char* name;
	const char* master;
	LIArcReader* reader;
	LIArcWriter* writer;
	LIExtModule* module;
	CURL* curl;

	/* Get arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_LOBBY);
	if (!liscr_args_gets_string (args, "master", &master))
		return;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);

	/* Format the script URL. */
	url = lisys_string_concat (master, "/lossrvapi.php");
	if (url == NULL)
		return;

	/* GET from the master server. */
	curl = curl_easy_init();
	if (curl != NULL)
	{
		writer = liarc_writer_new ();
		curl_easy_setopt (curl, CURLOPT_URL, url);
		curl_easy_setopt (curl, CURLOPT_WRITEDATA, writer);
		curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, private_write);
		curl_easy_perform (curl);
		curl_easy_cleanup (curl);
		liarc_writer_append_nul (writer);
		reader = liarc_reader_new (
			liarc_writer_get_buffer (writer),
			liarc_writer_get_length (writer));
		while (!liarc_reader_check_end (reader))
		{
			ip = NULL;
			name = NULL;
			desc = NULL;
			ret = liarc_reader_get_text (reader, "\t", &ip) &&
				liarc_reader_get_text_int (reader, &port) &&
				liarc_reader_skip_bytes (reader, 1) &&
				liarc_reader_get_text_int (reader, &players) &&
				liarc_reader_skip_bytes (reader, 1) &&
				liarc_reader_get_text (reader, "\t", &name) &&
				liarc_reader_get_text (reader, "\n", &desc);
			if (ret)
			{
				lua_newtable (args->lua);
				lua_pushstring (args->lua, ip);
				lua_setfield (args->lua, -2, "ip");
				lua_pushnumber (args->lua, port);
				lua_setfield (args->lua, -2, "port");
				lua_pushnumber (args->lua, players);
				lua_setfield (args->lua, -2, "players");
				lua_pushstring (args->lua, name);
				lua_setfield (args->lua, -2, "name");
				lua_pushstring (args->lua, desc);
				lua_setfield (args->lua, -2, "desc");
				liscr_args_seti_stack (args);
			}
			lisys_free (ip);
			lisys_free (name);
			lisys_free (desc);
			if (!ret)
				break;
		}
		liarc_reader_free (reader);
		liarc_writer_free (writer);
	}
	lisys_free (url);
#endif
}

static void Lobby_upload_server_info (LIScrArgs* args)
{
#ifdef HAVE_CURL
	int port = 0;
	int players = 0;
	char* url;
	char* decoded;
	const char* desc = NULL;
	const char* master = NULL;
	const char* name = NULL;
	LIExtModule* module;
	CURL* curl;

	/* Get arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_LOBBY);
	if (!liscr_args_gets_string (args, "desc", &desc) ||
	    !liscr_args_gets_string (args, "master", &master) ||
	    !liscr_args_gets_string (args, "name", &name) ||
	    !liscr_args_gets_int (args, "players", &players) ||
	    !liscr_args_gets_int (args, "port", &port))
		return;
	players = LIMAT_CLAMP (players, 0, 256);
	port = LIMAT_CLAMP (port, 1, 65535);

	/* Format the script URL. */
	url = lisys_string_concat (master, "/lossrvapi.php");
	if (url == NULL)
		return;

	/* POST to the master server. */
	curl = curl_easy_init();
	if (curl != NULL)
	{
		decoded = lisys_string_format ("u=%d|%d|%s|%s", port, players, name, desc);
		curl_easy_setopt (curl, CURLOPT_URL, url);
		curl_easy_setopt (curl, CURLOPT_POSTFIELDS, decoded);
		curl_easy_perform (curl);
		curl_easy_cleanup (curl);
		lisys_free (decoded);
	}
	lisys_free (url);
#endif
}

/*****************************************************************************/

void liext_script_lobby (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_LOBBY, "lobby_download_server_list", Lobby_download_server_list);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_LOBBY, "lobby_upload_server_info", Lobby_upload_server_info);
}

/** @} */
/** @} */
