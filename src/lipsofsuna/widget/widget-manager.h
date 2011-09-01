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

#ifndef __WIDGET_MANAGER_H__
#define __WIDGET_MANAGER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/font.h>
#include <lipsofsuna/image.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/render.h>
#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-styles.h"
#include "widget-types.h"

struct _LIWdgManager
{
	int width;
	int height;
	LIWdgStyles* styles;
	LICalCallbacks* callbacks;
	LIMatMatrix projection;
	LIPthPaths* paths;
	LIRenRender* render;
	LIRenShader* shader;
	struct
	{
		LIWdgWidget* top;
		LIWdgWidget* bottom;
	} dialogs;
	struct
	{
		LIAlgPtrdic* all;
		LIWdgWidget* iter;
		LIWdgWidget* grab;
	} widgets;
};

LIAPICALL (LIWdgManager*, liwdg_manager_new, (
	LIRenRender*    render,
	LICalCallbacks* callbacks,
	LIPthPaths*     paths));

LIAPICALL (void, liwdg_manager_free, (
	LIWdgManager* self));

LIAPICALL (int, liwdg_manager_alloc_widgets, (
	LIWdgManager* self,
	              ...));

LIAPICALL (LIFntFont*, liwdg_manager_find_font, (
	LIWdgManager* self,
	const char*   name));

LIAPICALL (LIRenImage*, liwdg_manager_find_image, (
	LIWdgManager* self,
	const char*   name));

LIAPICALL (LIWdgWidget*, liwdg_manager_find_widget_by_point, (
	LIWdgManager* self,
	int           x,
	int           y));

LIAPICALL (LIWdgWidget*, liwdg_manager_find_window_by_point, (
	LIWdgManager* self,
	int           x,
	int           y));

LIAPICALL (int, liwdg_manager_insert_window, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (void, liwdg_manager_reload, (
	LIWdgManager* self,
	int           pass));

LIAPICALL (int, liwdg_manager_remove_window, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (void, liwdg_manager_render, (
	LIWdgManager* self));

LIAPICALL (void, liwdg_manager_update, (
	LIWdgManager* self,
	float         secs));

LIAPICALL (void, liwdg_manager_get_projection, (
	LIWdgManager* self,
	LIMatMatrix*  matrix));

LIAPICALL (LIWdgWidget*, liwdg_manager_get_root, (
	LIWdgManager* self));

LIAPICALL (void, liwdg_manager_set_root, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (void, liwdg_manager_get_size, (
	LIWdgManager* self,
	int*          x,
	int*          y));

LIAPICALL (void, liwdg_manager_set_size, (
	LIWdgManager* self,
	int           width,
	int           height));

#endif
