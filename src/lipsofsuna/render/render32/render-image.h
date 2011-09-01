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

#ifndef __RENDER32_IMAGE_H__
#define __RENDER32_IMAGE_H__

#include "lipsofsuna/image.h"
#include "lipsofsuna/system.h"
#include "render-types.h"

typedef struct _LIRenImage32 LIRenImage32;

LIAPICALL (LIRenImage32*, liren_image32_new_from_file, (
	LIRenRender32* render,
	const char*    name));

LIAPICALL (void, liren_image32_free, (
	LIRenImage32* self));

LIAPICALL (int, liren_image32_load, (
	LIRenImage32* self));

LIAPICALL (void, liren_image32_reload, (
	LIRenImage32* self,
	int           pass));

LIAPICALL (GLuint, liren_image32_get_handle, (
	const LIRenImage32* self));

LIAPICALL (int, liren_image32_get_height, (
	const LIRenImage32* self));

LIAPICALL (int, liren_image32_get_width, (
	const LIRenImage32* self));

#endif
