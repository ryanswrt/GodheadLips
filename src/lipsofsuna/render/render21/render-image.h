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

#ifndef __RENDER21_IMAGE_H__
#define __RENDER21_IMAGE_H__

#include "lipsofsuna/image.h"
#include "lipsofsuna/system.h"
#include "render-types.h"

LIAPICALL (LIRenImage21*, liren_image21_new_from_file, (
	LIRenRender21* render,
	const char*    name));

LIAPICALL (void, liren_image21_free, (
	LIRenImage21* self));

LIAPICALL (int, liren_image21_load, (
	LIRenImage21* self));

LIAPICALL (void, liren_image21_reload, (
	LIRenImage21* self,
	int           pass));

LIAPICALL (GLuint, liren_image21_get_handle, (
	const LIRenImage21* self));

LIAPICALL (int, liren_image21_get_height, (
	const LIRenImage21* self));

LIAPICALL (int, liren_image21_get_width, (
	const LIRenImage21* self));

#endif
