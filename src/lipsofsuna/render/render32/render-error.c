/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenError Error32
 * @{
 */

#include "render-error.h"

int liren_error32_get ()
{
	switch (glGetError ())
	{
		case GL_NO_ERROR:
			return 0;
		case GL_INVALID_ENUM:
			lisys_error_set (EINVAL, "OpenGL: invalid enum");
			break;
		case GL_INVALID_VALUE:
			lisys_error_set (EINVAL, "OpenGL: invalid value");
			break;
		case GL_INVALID_OPERATION:
			lisys_error_set (EINVAL, "OpenGL: invalid operation");
			break;
		case GL_STACK_OVERFLOW:
			lisys_error_set (EINVAL, "OpenGL: stack overflow");
			break;
		case GL_STACK_UNDERFLOW:
			lisys_error_set (EINVAL, "OpenGL: stack underflow");
			break;
		case GL_OUT_OF_MEMORY:
			lisys_error_set (EINVAL, "OpenGL: out of memory");
			break;
		default:
			lisys_error_set (EINVAL, "OpenGL: unknown error");
			break;
	}

	return 1;
}

#ifndef NDEBUG
void liren_error32_check ()
{
	if (liren_error32_get ())
	{
		lisys_error_report ();
		lisys_assert (0);
	}
}
#endif

/** @} */
/** @} */
