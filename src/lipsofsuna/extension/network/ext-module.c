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

#define DISCONNECT_TIMEOUT 2
#define MAX_CLIENTS 32
#define MAX_CHANNELS 1

static int private_init (
	LIExtModule* self);

static int private_connect (
	LIExtModule* self,
	ENetEvent*   event);

static int private_message_client (
	LIExtModule* self,
	ENetEvent*   event);

static int private_message_server (
	LIExtModule* self,
	ENetEvent*   event);

static int private_update (
	LIExtModule* self,
	float        secs);

static int private_shutdown (
	LIExtModule* self);

/*****************************************************************************/

LIMaiExtensionInfo liext_network_info =
{
	LIMAI_EXTENSION_VERSION, "Network",
	liext_network_new,
	liext_network_free
};

LIExtModule* liext_network_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Connect callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "program-shutdown", 0, private_shutdown, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, "tick", 0, private_update, self, self->calls + 1))
	{
		liext_network_free (self);
		return NULL;
	}

	/* Initialize self. */
	if (!private_init (self))
	{
		liext_network_free (self);
		return NULL;
	}

	return self;
}

void liext_network_free (
	LIExtModule* self)
{
	LIAlgU32dicIter iter0;

	if (self->clients != NULL)
	{
		lisys_assert (self->clients->size == 0);
		LIALG_U32DIC_FOREACH (iter0, self->clients)
			liext_client_free (iter0.value);
		lialg_u32dic_free (self->clients);
	}
	if (self->client_socket != NULL)
		enet_host_destroy (self->client_socket);
	if (self->server_socket != NULL)
		enet_host_destroy (self->server_socket);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
	enet_deinitialize ();
}

int liext_network_host (
	LIExtModule* self,
	int          port)
{
	ENetAddress address;

	/* Close old socket. */
	if (self->client_socket || self->server_socket)
		liext_network_shutdown (self);

	/* Initialize address. */
	memset (&address, 0, sizeof (ENetAddress));
	address.host = ENET_HOST_ANY;
	address.port = port;

	/* Create host. */
#if ENET_VERSION_MAJOR >= 1 && ENET_VERSION_MINOR >= 3
	self->server_socket = enet_host_create (&address, MAX_CLIENTS, MAX_CHANNELS, 0, 0);
#else
	self->server_socket = enet_host_create (&address, MAX_CLIENTS, MAX_CHANNELS, 0);
#endif
	if (self->server_socket == NULL)
	{
		lisys_error_set (EINVAL, "creating ENet server host failed");
		return 0;
	}
	self->connected = 1;

	return 1;
}

int liext_network_join (
	LIExtModule* self,
	int          port,
	const char*  addr)
{
	ENetAddress address;

	/* Close old socket. */
	if (self->client_socket || self->server_socket)
		liext_network_shutdown (self);

	/* Create host. */
#if ENET_VERSION_MAJOR >= 1 && ENET_VERSION_MINOR >= 3
	self->client_socket = enet_host_create (NULL, 1, MAX_CHANNELS, 0, 0);
#else
	self->client_socket = enet_host_create (NULL, 1, MAX_CHANNELS, 0);
#endif
	if (self->client_socket == NULL)
	{
		lisys_error_set (EINVAL, "creating ENet client host failed");
		return 0;
	}

	/* Format address. */
	memset (&address, 0, sizeof (ENetAddress));
	enet_address_set_host (&address, addr);
	address.port = port;

	/* Connect to server. */
#if ENET_VERSION_MAJOR >= 1 && ENET_VERSION_MINOR >= 3
	self->client_peer = enet_host_connect (self->client_socket, &address, MAX_CHANNELS, 0);
#else
	self->client_peer = enet_host_connect (self->client_socket, &address, MAX_CHANNELS);
#endif
	if (self->client_peer == NULL)
	{
		lisys_error_set (EINVAL, "creating ENet client peer failed");
		enet_host_destroy (self->client_socket);
		self->client_socket = NULL;
		return 0;
	}

	return 1;
}

void liext_network_update (
	LIExtModule* self,
	float        secs)
{
	ENetEvent event;
	LIAlgU32dicIter iter;
	LIExtClient* client;

	/* Handle client socket. */
	if (self->client_socket != NULL)
	{
		while (enet_host_service (self->client_socket, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					self->connected = 1;
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					private_message_client (self, &event);
					enet_packet_destroy (event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					enet_host_destroy (self->client_socket);
					self->client_socket = NULL;
					self->connected = 0;
					break;
				default:
					break;
			}
			if (self->client_socket == NULL)
				break;
		}
	}

	/* Handle server socket. */
	if (self->server_socket != NULL)
	{
		while (enet_host_service (self->server_socket, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					if (self->closed || !private_connect (self, &event))
						enet_peer_reset (event.peer);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					if (event.peer->data != NULL)
						private_message_server (self, &event);
					enet_packet_destroy (event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					if (event.peer->data != NULL)
					{
						liext_client_free (event.peer->data);
						event.peer->data = NULL;
					}
					break;
				default:
					break;
			}
		}
		LIALG_U32DIC_FOREACH (iter, self->clients)
		{
			client = iter.value;
			if (!client->connected && lisys_time (NULL) - client->disconnect_time > DISCONNECT_TIMEOUT)
			{
				enet_peer_reset (client->peer);
				liext_client_free (client);
			}
		}
	}
}

/**
 * \brief Find a client by ID.
 * \param self Network.
 * \param id Client ID.
 * \return Client or NULL.
 */
LIExtClient* liext_network_find_client (
	LIExtModule* self,
	int          id)
{
	return lialg_u32dic_find (self->clients, id);
}

int liext_network_send (
	LIExtModule* self,
	int          id,
	LIArcWriter* writer,
	int          reliable)
{
	ENetPacket* packet;
	LIExtClient* client;

	if (self->client_socket != NULL)
	{
		packet = enet_packet_create (
			liarc_writer_get_buffer (writer),
			liarc_writer_get_length (writer),
			(reliable)? ENET_PACKET_FLAG_RELIABLE : 0);
		if (packet != NULL)
			enet_peer_send (self->client_peer, 0, packet);
		return 1;
	}
	if (self->server_socket != NULL)
	{
		client = liext_network_find_client (self, id);
		if (client == NULL || !client->connected)
			return 0;
		liext_client_send (client, writer, reliable);
		return 1;
	}

	return 0;
}

void liext_network_shutdown (
	LIExtModule* self)
{
	ENetEvent event;
	LIAlgU32dicIter iter0;
	LIExtClient* client;

	if (self->server_socket != NULL)
	{
		/* Queue a disconnection event for each client. */
		LIALG_U32DIC_FOREACH (iter0, self->clients)
		{
			client = iter0.value;
			enet_peer_disconnect (client->peer, 0);
		}

		/* Wait for the disconnection events to be sent. */
		/* This blocks but it isn't a huge problem at this point since
		   there's no current use case for highly responsive shutdowns. */
		while (enet_host_service (self->server_socket, &event, 500))
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					enet_peer_reset (event.peer);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy (event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					break;
				default:
					break;
			}
		}

		/* Force disconnect and free clients. */
		LIALG_U32DIC_FOREACH (iter0, self->clients)
		{
			client = iter0.value;
			enet_peer_reset (client->peer);
			liext_client_free (client);
		}
		lialg_u32dic_clear (self->clients);

		/* Destroy the server socket. */
		if (self->server_socket)
		{
			enet_host_destroy (self->server_socket);
			self->server_socket = NULL;
		}
	}

	/* Destroy the client socket. */
	if (self->client_socket)
	{
		enet_host_destroy (self->client_socket);
		self->client_socket = NULL;
	}
	self->connected = 0;
}

int liext_network_get_closed (
	LIExtModule* self)
{
	return self->closed;
}

void liext_network_set_closed (
	LIExtModule* self,
	int          value)
{
	self->closed = value;
}

int liext_network_get_connected (
	LIExtModule* self)
{
	return self->connected;
}

/*****************************************************************************/

static int private_init (
	LIExtModule* self)
{
	/* Initialize ENet. */
	if (enet_initialize ())
	{
		lisys_error_set (EINVAL, "initializing ENet failed");
		return 0;
	}

	/* Allocate client list. */
	self->clients = lialg_u32dic_new ();
	if (self->clients == NULL)
		return 0;

	/* Register classes. */
	liscr_script_set_userdata (self->program->script, LIEXT_SCRIPT_NETWORK, self);
	liext_script_network (self->program->script);

	return 1;
}

static int private_connect (
	LIExtModule* self,
	ENetEvent*   event)
{
	LIExtClient* client;

	/* Create client. */
	client = liext_client_new (self, event->peer);
	if (client == NULL)
		return 0;

	/* Emit a login event. */
	limai_program_event (self->program, "login", "client", LISCR_TYPE_INT, client->id, NULL);

	return 1;
}

static int private_message_client (
	LIExtModule* self,
	ENetEvent*   event)
{
	int len;
	const uint8_t* data;
	LIArcPacket* reader;

	/* Check for valid length. */
	len = event->packet->dataLength;
	data = event->packet->data;
	if (len < 1)
	{
		lisys_error_set (EINVAL, "invalid packet size");
		return 0;
	}

	/* Create packet reader. */
	reader = liarc_packet_new_readable ((char*) data, len);
	if (reader == NULL)
		return 0;
	reader->reader->pos = 1;

	/* Emit an event. */
	limai_program_event (self->program, "packet", "message", LISCR_TYPE_INT, data[0], "packet", LISCR_SCRIPT_PACKET, reader, NULL);

	return 1;
}

static int private_message_server (
	LIExtModule* self,
	ENetEvent*   event)
{
	LIArcPacket* reader;
	LIExtClient* client;

	/* Get the client. */
	client = event->peer->data;
	if (!client->connected)
		return 0;

	/* Create packet reader. */
	if (event->packet->dataLength < 1)
		return 0;
	reader = liarc_packet_new_readable ((void*) event->packet->data, event->packet->dataLength);
	if (reader == NULL)
		return 0;
	reader->reader->pos = 1;

	/* Emit an event. */
	limai_program_event (self->program, "packet", "message", LISCR_TYPE_INT, ((uint8_t*) event->packet->data)[0], "client", LISCR_TYPE_INT, client->id, "packet", LISCR_SCRIPT_PACKET, reader, NULL);

	return 1;
}

static int private_update (
	LIExtModule* self,
	float        secs)
{
	liext_network_update (self, secs);

	return 1;
}

static int private_shutdown (
	LIExtModule* self)
{
	/* The program is shutting down so we need to disconnect the clients before
	   the script gets deleted. Since client logouts emit events, there must not
	   be any clients left after this callback. */
	liext_network_shutdown (self);

	return 1;
}

/** @} */
/** @} */
