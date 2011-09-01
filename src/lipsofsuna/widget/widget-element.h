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

#ifndef __WIDGET_ELEMENT_H__
#define __WIDGET_ELEMENT_H__

#include "lipsofsuna/render.h"
#include "widget-types.h"

struct _LIWdgElement
{
	int dst_clip_enabled;
	int dst_clip[4];
	int dst_clip_screen[4];
	int dst_pos[2];
	int dst_size[2];
	int src_pos[2];
	int src_tiling_enabled;
	int src_tiling[6];
	float rotation;
	char* text;
	float text_align[2];
	float text_color[4];
	LIFntFont* font;
	LIMatVector center;
	LIRenBuffer* buffer;
	LIRenImage* image;
	LIWdgElement* next;
	struct
	{
		int count;
		int capacity;
		LIRenVertex* array;
	} vertices;
};

LIAPICALL (LIWdgElement*, liwdg_element_new_image, (
	LIRenImage*        image,
	const float*       color,
	const int*         dst_clip,
	const int*         dst_pos,
	const int*         dst_size,
	const int*         src_pos,
	const int*         src_tiling,
	float              rotation_angle,
	const LIMatVector* rotation_center));

LIAPICALL (LIWdgElement*, liwdg_element_new_text, (
	LIFntFont*         font,
	const char*        text,
	const int*         dst_clip,
	const int*         dst_pos,
	const int*         dst_size,
	const float*       text_align,
	const float*       text_color,
	float              rotation_angle,
	const LIMatVector* rotation_center));

LIAPICALL (void, liwdg_element_free, (
	LIWdgElement* self));

LIAPICALL (void, liwdg_element_paint, (
	LIWdgElement*      self,
	LIWdgManager*      manager,
	const LIMatMatrix* matrix));

LIAPICALL (void, liwdg_element_reload, (
	LIWdgElement* self,
	LIWdgManager* manager,
	int           pass));

LIAPICALL (void, liwdg_element_update, (
	LIWdgElement*    self,
	LIWdgManager*    manager,
	const LIWdgRect* rect));

#endif
