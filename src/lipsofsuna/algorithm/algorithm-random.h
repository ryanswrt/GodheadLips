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

#ifndef __ALGORITHM_RANDOM_H__
#define __ALGORITHM_RANDOM_H__

#include <lipsofsuna/system.h>

#define LIALG_RANDOM_MAX 0x7FFFFFFE

typedef struct _LIAlgRandom LIAlgRandom;
struct _LIAlgRandom
{
	uint32_t state;
};

/**
 * \brief Initializes the random number generator with a seed.
 *
 * LIAlgRandom implements a linear congruential pseudo-random number
 * generator similar to, though not identical in output, to what glibc
 * uses. The class exists because of the subpar quality of the rand()
 * implementation in Windows.
 *
 * \param self Random number generator.
 * \param seed Seed.
 */
static inline void lialg_random_init (
	LIAlgRandom* self,
	uint32_t     seed)
{
	if (seed)
		self->state = seed;
	else
		self->state = 1;
}

/**
 * \brief Generates a random number.
 * \param self Random number generator.
 * \return Random number in range [0, 1].
 */
static inline uint32_t lialg_random_rand (
	LIAlgRandom* self)
{
	const uint32_t a = 1103515245;
	const uint32_t c = 12345;
	self->state = (a * self->state + c) & 0x7FFFFFFF;
	return self->state;
}

/**
 * \brief Generates a random number.
 * \param self Random number generator.
 * \return Random number in range [0, 1].
 */
static inline float lialg_random_float (
	LIAlgRandom* self)
{
	lialg_random_rand (self);
	return (double) self->state / LIALG_RANDOM_MAX;
}

/**
 * \brief Generates a random number.
 * \param self Random number generator.
 * \param end Maximum value plus 1.
 * \return Random number.
 */
static inline uint32_t lialg_random_max (
	LIAlgRandom* self,
	uint32_t     end)
{
	lialg_random_rand (self);
	return self->state % end;
}

/**
 * \brief Generates a random number.
 * \param self Random number generator.
 * \param start Minimum value.
 * \param end Maximum value plus 1.
 * \return Random number.
 */
static inline uint32_t lialg_random_range (
	LIAlgRandom* self,
	uint32_t     start,
	uint32_t     end)
{
	lialg_random_rand (self);
	return start + self->state % (end - start);
}

#endif
