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
 * \addtogroup LIExtNetwork Network
 * @{
 */

#include "ext-module.h"
#include "ext-client.h"

static void Network_disconnect (LIScrArgs* args)
{
	int id;
	LIExtClient* client;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	if (!liscr_args_geti_int (args, 0, &id) &&
	    !liscr_args_gets_int (args, "client", &id))
		return;
	client = liext_network_find_client (module, id);
	if (client != NULL)
		liext_client_disconnect (client);
}

static void Network_host (LIScrArgs* args)
{
	int port = 10101;
	int udp = 0;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	liscr_args_gets_bool (args, "udp", &udp);
	liscr_args_gets_int (args, "port", &port);
	port = LIMAT_CLAMP (port, 1025, 32767);

	if (liext_network_host (module, port))
		liscr_args_seti_bool (args, 1);
}

static void Network_join (LIScrArgs* args)
{
	int port = 10101;
	const char* addr = "localhost";
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	liscr_args_gets_int (args, "port", &port);
	liscr_args_gets_string (args, "host", &addr);
	port = LIMAT_CLAMP (port, 1025, 32767);

	if (liext_network_join (module, port, addr))
		liscr_args_seti_bool (args, 1);
}

static void Network_send (LIScrArgs* args)
{
	int id = 0;
	int reliable = 1;
	LIExtModule* module;
	LIScrData* data;
	LIArcPacket* packet;

	/* Get packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);

	/* Get object if hosting. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	if (module->server_socket)
	{
		if (!liscr_args_gets_int (args, "client", &id))
			return;
	}

	/* Send packet. */
	liscr_args_gets_bool (args, "reliable", &reliable);
	if (packet->writer != NULL)
		liext_network_send (module, id, packet->writer, reliable);
}

static void Network_shutdown (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	liext_network_shutdown (module);
}

static void Network_update (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	liext_network_update (module, 1.0f);
}

static void Network_get_clients (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIExtClient* client;
	LIExtModule* module;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	LIALG_U32DIC_FOREACH (iter, module->clients)
	{
		client = iter.value;
		if (client->connected)
			liscr_args_seti_int (args, client->id);
	}
}

static void Network_get_closed (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	liscr_args_seti_bool (args, liext_network_get_closed (module));
}
static void Network_set_closed (LIScrArgs* args)
{
	int value;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	if (liscr_args_geti_bool (args, 0, &value))
		liext_network_set_closed (module, value);
}

static void Network_get_connected (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NETWORK);
	liscr_args_seti_bool (args, liext_network_get_connected (module));
}

void liext_script_network (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_disconnect", Network_disconnect);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_host", Network_host);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_join", Network_join);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_send", Network_send);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_shutdown", Network_shutdown);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_update", Network_update);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_get_clients", Network_get_clients);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_get_closed", Network_get_closed);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_set_closed", Network_set_closed);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NETWORK, "network_get_connected", Network_get_connected);
}

/** @} */
/** @} */
