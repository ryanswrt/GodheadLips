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
 * \addtogroup LISnd Sound 
 * @{
 * \addtogroup LISndSystem System 
 * @{
 */

#ifndef LI_DISABLE_SOUND

#include "lipsofsuna/system.h"
#include "sound-system.h"

/*****************************************************************************/

/**
 * \brief Initializes the sound system.
 *
 * \return Sound system.
 */
LISndSystem*
lisnd_system_new ()
{
	LISndSystem* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LISndSystem));
	if (self == NULL)
		return NULL;

	/* Open the device. */
	self->device = alcOpenDevice (NULL);
	if (self->device == NULL)
	{
		lisnd_system_free (self);
		return NULL;
	}
	self->context = alcCreateContext (self->device, NULL);
	if (self->context == NULL)
	{
		lisnd_system_free (self);
		return NULL;
	}
	alcMakeContextCurrent (self->context);
	alGetError();

	return self;
}

/**
 * \brief Deinitializes the sound system.
 *
 * \param self Sound system.
 */
void
lisnd_system_free (LISndSystem* self)
{
	if (self->context != NULL)
	{
		alcMakeContextCurrent (NULL);
		alcDestroyContext (self->context);
	}
	if (self->device != NULL)
		alcCloseDevice (self->device);
	lisys_free (self);
}

/**
 * \brief Sets the orientation of the listener.
 *
 * \param self Sound system.
 * \param pos Position of the listener.
 * \param vel Velocity of the listener.
 * \param dir Facing direction of the listener.
 * \param up Up direction of the listener.
 */
void
lisnd_system_set_listener (LISndSystem*       self,
                           const LIMatVector* pos,
                           const LIMatVector* vel,
                           const LIMatVector* dir,
                           const LIMatVector* up)
{
	ALfloat alpos[3] = { -pos->x, pos->y, -pos->z };
	ALfloat alvel[3] = { vel->x, vel->y, vel->z };
	ALfloat alori[6] = { -dir->x, dir->y, -dir->z, -up->x, up->y, -up->z };

	alListenerfv (AL_POSITION, alpos);
	alListenerfv (AL_VELOCITY, alvel);
	alListenerfv (AL_ORIENTATION, alori);
}

#endif

/** @} */
/** @} */

