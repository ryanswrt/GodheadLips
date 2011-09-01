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

#ifndef __EXT_CLIENT_H__
#define __EXT_CLIENT_H__

#include "ext-module.h"

struct _LIExtClient
{
	int id;
	int connected;
	time_t disconnect_time;
	ENetPeer* peer;
	LIExtModule* module;
};

LIExtClient* liext_client_new (
	LIExtModule* module,
	ENetPeer*    peer);

void liext_client_free (
	LIExtClient* self);

void liext_client_disconnect (
	LIExtClient* self);

void liext_client_send (
	LIExtClient* self,
	LIArcWriter* writer,
	int          reliable);

#endif
