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
 * \addtogroup LIVid Video
 * @{
 * \addtogroup LIVidVideo Video
 * @{
 */

#include <string.h>
#include "video.h"

/**
 * \brief Initializes the global video card information.
 * \return Nonzero on success.
 */
int livid_video_init ()
{
	int ver[3];
	GLenum error;
	const GLubyte* tmp;

	/* Initialize GLEW. */
	error = glewInit ();
	if (error != GLEW_OK)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "%s", glewGetErrorString (error));
		return 0;
	}

	/* Check for OpenGL 3.2 capabilities. */
	/* GLEW versions up to 1.5.3 had a bug that completely broke OpenGL 3.2
	   support. We try to detect it and warn the user of the problem. */
	if (GLEW_VERSION_3_2)
		return 1;
	tmp = glewGetString (GLEW_VERSION);
	if (sscanf ((const char*) tmp, "%d.%d.%d", ver, ver + 1, ver + 2) == 3 &&
	   (ver[0] < 1 || ver[1] < 5 || ver[2] <= 3))
	{
		lisys_error_set (EINVAL, "OpenGL 3.2 isn't supported because it requires GLEW 1.5.4 or newer while you have %s", tmp);
		lisys_error_report ();
	}
	else
	{
		lisys_error_set (EINVAL, "OpenGL 3.2 isn't supported by your graphics card or drivers");
		lisys_error_report ();
	}

	/* Check for OpenGL 2.1 capabilities. */
	if (GLEW_VERSION_2_1)
		return 1;
	lisys_error_set (EINVAL, "OpenGL 2.1 isn't supported by your graphics card or drivers");

	return 0;
}

/** @} */
/** @} */
