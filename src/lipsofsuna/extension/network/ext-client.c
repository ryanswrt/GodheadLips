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

/**
 * \brief Allocates a new client.
 * \param module Module.
 * \param peer Network peer.
 * \return New client or NULL.
 */
LIExtClient* liext_client_new (
	LIExtModule* module,
	ENetPeer*    peer)
{
	LIExtClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtClient));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->connected = 1;
	self->peer = peer;
	self->peer->data = self;

	/* Find free ID. */
	while (!self->id)
	{
		self->id = lialg_random_rand (&module->program->random);
		if (lialg_u32dic_find (module->clients, self->id) != NULL)
			self->id = 0;
	}

	/* Add to the client list. */
	if (!lialg_u32dic_insert (module->clients, self->id, self))
	{
		self->peer->data = NULL;
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the client.
 * \param self Client.
 */
void liext_client_free (
	LIExtClient* self)
{
	/* Signal logout. */
	limai_program_event (self->module->program, "logout", "client", LISCR_TYPE_INT, self->id, NULL);

	/* Remove from the client list. */
	lialg_u32dic_remove (self->module->clients, self->id);

	/* Mark peer data as removed. */
	/* We get a disconnection event even after the a manual disconnect but
	   the client struct is already freed. We use the NULL peer data pointer
	   to identify the condition and hence avoid a double free. */
	self->peer->data = NULL;

	lisys_free (self);
}

/**
 * \brief Marks the client as disconnected.
 * \param self Client.
 */
void liext_client_disconnect (
	LIExtClient* self)
{
	if (self->connected)
	{
		self->connected = 0;
		self->disconnect_time = lisys_time (NULL);
	}
}

/**
 * \brief Sends a network packet to the client.
 * \param self Client.
 * \param writer Packet.
 * \param reliable Nonzero for reliable.
 */
void liext_client_send (
	LIExtClient* self,
	LIArcWriter* writer,
	int          reliable)
{
	ENetPacket* packet;

	if (self->connected)
	{
		packet = enet_packet_create (
			liarc_writer_get_buffer (writer),
			liarc_writer_get_length (writer),
			(reliable)? ENET_PACKET_FLAG_RELIABLE : 0);
		if (packet != NULL)
			enet_peer_send (self->peer, 0, packet);
	}
}

/** @} */
/** @} */
