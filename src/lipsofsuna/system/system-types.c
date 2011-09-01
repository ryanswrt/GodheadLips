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
 * \addtogroup LISysTypes Types
 * @{
 */

#include "system-endian.h"
#include "system-types.h"

/**
 * \brief Converts a system endianness integer to a big endian integer.
 *
 * \param value System endianness integer.
 * \return Big endian integer.
 */
uint16_t
lisys_htons (uint16_t value)
{
	union
	{
		uint8_t val[2];
		uint16_t ret;
	} tmp;

	if (lisys_endian_big ())
	{
		tmp.val[0] = ((uint8_t*) &value)[0];
		tmp.val[1] = ((uint8_t*) &value)[1];
	}
	else
	{
		tmp.val[0] = ((uint8_t*) &value)[1];
		tmp.val[1] = ((uint8_t*) &value)[0];
	}

	return tmp.ret;
}

/**
 * \brief Converts a system endianness integer to a big endian integer.
 *
 * \param value System endianness integer.
 * \return Big endian integer.
 */
uint32_t
lisys_htonl (uint32_t value)
{
	union
	{
		uint8_t val[4];
		uint32_t ret;
	} tmp;

	if (lisys_endian_big ())
	{
		tmp.val[0] = ((uint8_t*) &value)[0];
		tmp.val[1] = ((uint8_t*) &value)[1];
		tmp.val[2] = ((uint8_t*) &value)[2];
		tmp.val[3] = ((uint8_t*) &value)[3];
	}
	else
	{
		tmp.val[0] = ((uint8_t*) &value)[3];
		tmp.val[1] = ((uint8_t*) &value)[2];
		tmp.val[2] = ((uint8_t*) &value)[1];
		tmp.val[3] = ((uint8_t*) &value)[0];
	}

	return tmp.ret;
}

/** @} */
/** @} */
