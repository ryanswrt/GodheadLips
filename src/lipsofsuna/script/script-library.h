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

#ifndef __SCRIPT_LIBRARY_H__
#define __SCRIPT_LIBRARY_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/system.h>

#define LISCR_SCRIPT_CLASS "Class"
#define LISCR_SCRIPT_EVENT "Event"
#define LISCR_SCRIPT_MODEL "Model"
#define LISCR_SCRIPT_OBJECT "Object"
#define LISCR_SCRIPT_PACKET "Packet"
#define LISCR_SCRIPT_PROGRAM "Program"
#define LISCR_SCRIPT_QUATERNION "Quaternion"
#define LISCR_SCRIPT_VECTOR "Vector"

LIAPICALL (void, liscr_script_event, (
	LIScrScript* self));

LIAPICALL (void, liscr_script_model, (
	LIScrScript* self));

LIAPICALL (void, liscr_script_object, (
	LIScrScript* self));

LIAPICALL (void, liscr_script_packet, (
	LIScrScript* self));

LIAPICALL (void, liscr_script_program, (
	LIScrScript* self));

LIAPICALL (void, liscr_script_quaternion, (
	LIScrScript* self));

LIAPICALL (void, liscr_script_vector, (
	LIScrScript* self));

#include "script-packet.h"

#endif

