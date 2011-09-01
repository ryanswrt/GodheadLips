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

#ifndef __RENDER_IMAGE_H__
#define __RENDER_IMAGE_H__

#include "lipsofsuna/image.h"
#include "lipsofsuna/system.h"
#include "render-types.h"

typedef struct _LIRenImage LIRenImage;

LIAPICALL (LIRenImage*, liren_image_new, (
	LIRenRender* render,
	const char*  name));

LIAPICALL (void, liren_image_free, (
	LIRenImage* self));

LIAPICALL (GLuint, liren_image_get_handle, (
	const LIRenImage* self));

LIAPICALL (int, liren_image_get_height, (
	const LIRenImage* self));

LIAPICALL (int, liren_image_get_width, (
	const LIRenImage* self));

#endif
