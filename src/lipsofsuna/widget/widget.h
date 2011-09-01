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

#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <lipsofsuna/system.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/video.h>
#include "widget-manager.h"
#include "widget-types.h"

typedef struct _LIWdgGroupRow LIWdgGroupRow;
struct _LIWdgGroupRow
{
	int start;
	int expand;
	int request;
	int allocation;
};

typedef struct _LIWdgGroupCol LIWdgGroupCol;
struct _LIWdgGroupCol
{
	int start;
	int expand;
	int request;
	int allocation;
};

typedef struct _LIWdgGroupCell LIWdgGroupCell;
struct _LIWdgGroupCell
{
	int width;
	int height;
	LIWdgWidget* child;
};

struct _LIWdgWidget
{
	LIWdgManager* manager;
	LIWdgWidget* parent;
	LIWdgWidget* prev;
	LIWdgWidget* next;
	LIScrData* script;
	unsigned int behind : 1;
	unsigned int fixed_size : 1;
	unsigned int floating : 1;
	unsigned int fullscreen : 1;
	unsigned int visible : 1;

	/* Canvas. */
	LIWdgElement* elements;
	LIWdgRect allocation;
	LIWdgSize request[3];

	/* Table layout. */
	int width;
	int height;
	int homogeneous;
	int col_expand;
	int row_expand;
	int col_spacing;
	int row_spacing;
	int margin_left;
	int margin_right;
	int margin_top;
	int margin_bottom;
	int rebuilding;
	LIWdgGroupRow* rows;
	LIWdgGroupCol* cols;
	LIWdgGroupCell* cells;

	/* Manual layout. */
	LIWdgRect offset;
	LIWdgWidget* children;
};

LIAPICALL (LIWdgWidget*, liwdg_widget_new, (
	LIWdgManager* manager));

LIAPICALL (void, liwdg_widget_free, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_add_child, (
	LIWdgWidget* self,
	LIWdgWidget* child));

LIAPICALL (int, liwdg_widget_append_col, (
	LIWdgWidget* self));

LIAPICALL (int, liwdg_widget_append_row, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_canvas_clear, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_canvas_compile, (
	LIWdgWidget* self));

LIAPICALL (int, liwdg_widget_canvas_insert, (
	LIWdgWidget*  self,
	LIWdgElement* element));

LIAPICALL (LIWdgWidget*, liwdg_widget_child_at, (
	LIWdgWidget* self,
	int          x,
	int          y));

LIAPICALL (void, liwdg_widget_child_request, (
	LIWdgWidget* self,
	LIWdgWidget* child));

LIAPICALL (LIWdgWidget*, liwdg_widget_cycle_focus, (
	LIWdgWidget* self,
	LIWdgWidget* curr,
	int          dir));

LIAPICALL (int, liwdg_widget_detach, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_detach_child, (
	LIWdgWidget* self,
	LIWdgWidget* child));

LIAPICALL (void, liwdg_widget_draw, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_foreach_child, (
	LIWdgWidget* self,
	void       (*call)(),
	void*        data));

LIAPICALL (int, liwdg_widget_insert_col, (
	LIWdgWidget* self,
	int          index));

LIAPICALL (int, liwdg_widget_insert_row, (
	LIWdgWidget* self,
	int          index));

LIAPICALL (void, liwdg_widget_move, (
	LIWdgWidget* self,
	int          x,
	int          y));

LIAPICALL (void, liwdg_widget_paint, (
	LIWdgWidget* self,
	LIWdgRect*   rect));

LIAPICALL (void, liwdg_widget_reload, (
	LIWdgWidget* self,
	int          pass));

LIAPICALL (void, liwdg_widget_remove_col, (
	LIWdgWidget* self,
	int          index));

LIAPICALL (void, liwdg_widget_remove_row, (
	LIWdgWidget* self,
	int          index));

LIAPICALL (void, liwdg_widget_get_allocation, (
	LIWdgWidget* self,
	LIWdgRect*   allocation));

LIAPICALL (void, liwdg_widget_set_allocation, (
	LIWdgWidget* self,
	int          x,
	int          y,
	int          w,
	int          h));

LIAPICALL (int, liwdg_widget_get_behind, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_behind, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (void, liwdg_widget_get_cell_rect, (
	LIWdgWidget* self,
	int          x,
	int          y,
	LIWdgRect*   rect));

LIAPICALL (LIWdgWidget*, liwdg_widget_get_child, (
	LIWdgWidget* self,
	int         x,
	int         y));

LIAPICALL (void, liwdg_widget_set_child, (
	LIWdgWidget* self,
	int          x,
	int          y,
	LIWdgWidget* child));

LIAPICALL (int, liwdg_widget_get_col_expand, (
	LIWdgWidget* self,
	int          x));

LIAPICALL (void, liwdg_widget_set_col_expand, (
	LIWdgWidget* self,
	int          x,
	int          expand));

LIAPICALL (int, liwdg_widget_get_col_size, (
	LIWdgWidget* self,
	int          x));

LIAPICALL (int, liwdg_widget_get_fixed_size, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_fixed_size, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (int, liwdg_widget_get_floating, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_floating, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (int, liwdg_widget_get_fullscreen, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_fullscreen, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (int, liwdg_widget_get_grab, (
	const LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_grab, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (int, liwdg_widget_get_homogeneous, (
	const LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_homogeneous, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (void, liwdg_widget_get_margins, (
	LIWdgWidget* self,
	int*         left,
	int*         right,
	int*         top,
	int*         bottom));

LIAPICALL (void, liwdg_widget_set_margins, (
	LIWdgWidget* self,
	int          left,
	int          right,
	int          top,
	int          bottom));

LIAPICALL (void, liwdg_widget_get_request, (
	LIWdgWidget* self,
	LIWdgSize*   request));

LIAPICALL (void, liwdg_widget_set_request, (
	LIWdgWidget* self,
	int          level,
	int          w,
	int          h));

LIAPICALL (LIWdgWidget*, liwdg_widget_get_root, (
	LIWdgWidget* self));

LIAPICALL (int, liwdg_widget_get_row_expand, (
	LIWdgWidget* self,
	int          y));

LIAPICALL (void, liwdg_widget_set_row_expand, (
	LIWdgWidget* self,
	int          y,
	int          expand));

LIAPICALL (void, liwdg_widget_get_offset, (
	LIWdgWidget* self,
	int*         x,
	int*         y));

LIAPICALL (void, liwdg_widget_set_offset, (
	LIWdgWidget* self,
	int          x,
	int          y));

LIAPICALL (LIScrData*, liwdg_widget_get_script, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_script, (
	LIWdgWidget* self,
	LIScrData*   value));

LIAPICALL (void, liwdg_widget_get_size, (
	LIWdgWidget* self,
	int*         cols,
	int*         rows));

LIAPICALL (int, liwdg_widget_set_size, (
	LIWdgWidget* self,
	int          cols,
	int          rows));

LIAPICALL (void, liwdg_widget_get_spacings, (
	LIWdgWidget* self,
	int*         column,
	int*         row));

LIAPICALL (void, liwdg_widget_set_spacings, (
	LIWdgWidget* self,
	int          column,
	int          row));

LIAPICALL (int, liwdg_widget_get_visible, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_visible, (
	LIWdgWidget* self,
	int          visible));

#endif
