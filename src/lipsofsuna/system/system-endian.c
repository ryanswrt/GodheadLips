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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysEndian Endian
 * @{
 */

#include "system-endian.h"
#include "system-types.h"

/**
 * \brief Returns nonzero if the system is big endian.
 *
 * \return Nonzero if the system is big endian.
 */
int
lisys_endian_big ()
{
	union { uint16_t a; uint8_t b[2]; } tmp;
	tmp.b[0] = 0x12;
	tmp.b[1] = 0x34;
	return (tmp.a == LISYS_ENDIAN_BIG);
}

/**
 * \brief Returns the byte order of the system.
 *
 * \return LISYS_ENDIAN_BIG or LISYS_ENDIAN_LITTLE.
 */
int
lisys_endian_get ()
{
	union { uint16_t a; uint8_t b[2]; } tmp;
	tmp.b[0] = 0x12;
	tmp.b[1] = 0x34;
	return tmp.a;
}

/** @} */
/** @} */
