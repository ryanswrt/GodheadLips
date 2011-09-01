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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <enet/enet.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/extension.h>

#define LIEXT_SCRIPT_NETWORK "Network"

typedef struct _LIExtModule LIExtModule;
typedef struct _LIExtClient LIExtClient;

struct _LIExtModule
{
	int closed;
	int connected;
	ENetHost* client_socket;
	ENetHost* server_socket;
	ENetPeer* client_peer;
	LIAlgU32dic* clients;
	LICalHandle calls[2];
	LIMaiProgram* program;
};

LIExtModule* liext_network_new (
	LIMaiProgram* program);

void liext_network_free (
	LIExtModule* self);

int liext_network_host (
	LIExtModule* self,
	int          port);

int liext_network_join (
	LIExtModule* self,
	int          port,
	const char*  addr);

void liext_network_update (
	LIExtModule* self,
	float        secs);

LIExtClient* liext_network_find_client (
	LIExtModule* self,
	int          id);

int liext_network_send (
	LIExtModule* self,
	int          id,
	LIArcWriter* writer,
	int          reliable);

void liext_network_shutdown (
	LIExtModule* self);

int liext_network_get_closed (
	LIExtModule* self);

void liext_network_set_closed (
	LIExtModule* self,
	int          value);

int liext_network_get_connected (
	LIExtModule* self);

/*****************************************************************************/

void liext_script_network (
	LIScrScript* self);

#endif
