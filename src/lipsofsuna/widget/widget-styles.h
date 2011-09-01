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

#ifndef __WIDGET_STYLES_H__
#define __WIDGET_STYLES_H__

#include <lipsofsuna/font.h>
#include <lipsofsuna/image.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/system.h>
#include "widget-types.h"

struct _LIWdgStyles
{
	LIAlgStrdic* fonts;
	LIPthPaths* paths;
	LIWdgManager* manager;
};

LIAPICALL (LIWdgStyles*, liwdg_styles_new, (
	LIWdgManager* manager,
	LIPthPaths*   paths));

LIAPICALL (void, liwdg_styles_free, (
	LIWdgStyles* self));

LIAPICALL (LIFntFont*, liwdg_styles_load_font, (
	LIWdgStyles* self,
	const char*  name,
	const char*  file,
	int          size));

#endif
