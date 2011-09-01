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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include "ext-module.h"

static void Widget_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate userdata. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_WIDGET);
	self = liwdg_widget_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_WIDGET, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_script (self, data);
	liscr_args_seti_stack (args);
}

static void Widget_add_child (LIScrArgs* args)
{
	LIScrData* data;

	/* Append rows. */
	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_WIDGET, &data))
	{
		liwdg_widget_detach (liscr_data_get_data (data));
		liwdg_widget_add_child (args->self, liscr_data_get_data (data));
	}
}

static void Widget_append_col (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	LIScrData* data;

	/* Append column. */
	liwdg_widget_get_size (args->self, &w, &h);
	if (!liwdg_widget_append_col (args->self))
		return;

	/* Append rows. */
	for (i = 0 ; i < h && liscr_args_geti_data (args, i, LIEXT_SCRIPT_WIDGET, &data) ; i++)
	{
		liwdg_widget_detach (liscr_data_get_data (data));
		liwdg_widget_set_child (args->self, w, i, liscr_data_get_data (data));
	}
}

static void Widget_append_row (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	LIScrData* data;

	/* Append row. */
	liwdg_widget_get_size (args->self, &w, &h);
	if (!liwdg_widget_append_row (args->self))
		return;

	/* Append columns. */
	for (i = 0 ; i < w && liscr_args_geti_data (args, i, LIEXT_SCRIPT_WIDGET, &data) ; i++)
	{
		liwdg_widget_detach (liscr_data_get_data (data));
		liwdg_widget_set_child (args->self, i, h, liscr_data_get_data (data));
	}
}

static void Widget_canvas_clear (LIScrArgs* args)
{
	liwdg_widget_canvas_clear (args->self);
}

static void Widget_canvas_compile (LIScrArgs* args)
{
	liwdg_widget_canvas_compile (args->self);
}

static void Widget_canvas_image (LIScrArgs* args)
{
	float color[4];
	int dest_clip[4];
	int dest_position[2];
	int dest_size[2];
	int source_position[2];
	int source_tiling[6];
	float* color_ptr = NULL;
	int* dest_position_ptr = NULL;
	int* dest_size_ptr = NULL;
	int* dest_clip_ptr = NULL;
	int* source_position_ptr = NULL;
	int* source_tiling_ptr = NULL;
	const char* source_image;
	float rotation = 0.0f;
	LIScrData* vector = NULL;
	LIMatVector* rotation_center_ptr = NULL;
	LIRenImage* image;
	LIWdgWidget* widget;
	LIWdgElement* elem;

	/* Process arguments. */
	widget = args->self;
	if (!liscr_args_gets_string (args, "source_image", &source_image))
		return;
	image = liwdg_manager_find_image (widget->manager, source_image);
	if (image == NULL)
		return;
	if (liscr_args_gets_floatv (args, "color", 4, color) == 4)
		color_ptr = color;
	if (liscr_args_gets_intv (args, "dest_clip", 4, dest_clip) == 4)
		dest_clip_ptr = dest_clip;
	if (liscr_args_gets_intv (args, "dest_position", 2, dest_position) == 2)
		dest_position_ptr = dest_position;
	if (liscr_args_gets_intv (args, "dest_size", 2, dest_size) == 2)
		dest_size_ptr = dest_size;
	if (liscr_args_gets_intv (args, "source_position", 2, source_position) == 2)
		source_position_ptr = source_position;
	if (liscr_args_gets_intv (args, "source_tiling", 6, source_tiling) == 6)
		source_tiling_ptr = source_tiling;
	liscr_args_gets_float (args, "rotation", &rotation);
	if (liscr_args_gets_data (args, "rotation_center", LISCR_SCRIPT_VECTOR, &vector))
		rotation_center_ptr = liscr_data_get_data (vector);

	/* Create the canvas element. */
	elem = liwdg_element_new_image (image, color_ptr, dest_clip_ptr, dest_position_ptr,
		dest_size_ptr, source_position_ptr, source_tiling_ptr, rotation, rotation_center_ptr);
	if (elem == NULL)
		return;
	if (!liwdg_widget_canvas_insert (widget, elem))
	{
		liwdg_element_free (elem);
		return;
	}
}

static void Widget_canvas_text (LIScrArgs* args)
{
	int dest_clip[4];
	int dest_position[2];
	int dest_size[2];
	float text_align[2];
	float text_color[6];
	int* dest_position_ptr = NULL;
	int* dest_size_ptr = NULL;
	int* dest_clip_ptr = NULL;
	float* text_align_ptr = NULL;
	float* text_color_ptr = NULL;
	const char* text;
	const char* text_font = "default";
	float rotation = 0.0f;
	LIScrData* vector = NULL;
	LIMatVector* rotation_center_ptr = NULL;
	LIFntFont* font;
	LIWdgWidget* widget;
	LIWdgElement* elem;

	/* Process arguments. */
	widget = args->self;
	if (!liscr_args_gets_string (args, "text", &text))
		return;
	liscr_args_gets_string (args, "text_font", &text_font);
	font = lialg_strdic_find (widget->manager->styles->fonts, text_font);
	if (font == NULL)
		return;
	if (liscr_args_gets_intv (args, "dest_clip", 4, dest_clip) == 4)
		dest_clip_ptr = dest_clip;
	if (liscr_args_gets_intv (args, "dest_position", 2, dest_position) == 2)
		dest_position_ptr = dest_position;
	if (liscr_args_gets_intv (args, "dest_size", 2, dest_size) == 2)
		dest_size_ptr = dest_size;
	if (liscr_args_gets_floatv (args, "text_alignment", 2, text_align) == 2)
		text_align_ptr = text_align;
	if (liscr_args_gets_floatv (args, "text_color", 4, text_color) == 4)
		text_color_ptr = text_color;
	liscr_args_gets_float (args, "rotation", &rotation);
	if (liscr_args_gets_data (args, "rotation_center", LISCR_SCRIPT_VECTOR, &vector))
		rotation_center_ptr = liscr_data_get_data (vector);

	/* Create the canvas element. */
	elem = liwdg_element_new_text (font, text, dest_clip_ptr, dest_position_ptr,
		dest_size_ptr, text_align_ptr, text_color_ptr, rotation, rotation_center_ptr);
	if (elem == NULL)
		return;
	if (!liwdg_widget_canvas_insert (widget, elem))
	{
		liwdg_element_free (elem);
		return;
	}
}

static void Widget_detach (LIScrArgs* args)
{
	liwdg_widget_detach (args->self);
}

static void Widget_get_child (LIScrArgs* args)
{
	int x = 1;
	int y = 1;
	int w;
	int h;
	LIWdgWidget* widget;

	liwdg_widget_get_size (args->self, &w, &h);
	liscr_args_geti_int (args, 0, &x);
	liscr_args_geti_int (args, 1, &y);
	if (x <= 0 || x > w || y <= 0 || y > h)
		return;	
	widget = liwdg_widget_get_child (args->self, x - 1, y - 1);
	if (widget != NULL)
		liscr_args_seti_data (args, liwdg_widget_get_script (widget));
}

static void Widget_get_request (LIScrArgs* args)
{
	int internal = 0;
	LIWdgWidget* widget;

	widget = args->self;
	liscr_args_gets_bool (args, "internal", &internal);
	if (internal)
	{
		liscr_args_seti_int (args, widget->request[1].width);
		liscr_args_seti_int (args, widget->request[1].height);
	}
	else
	{
		if (widget->request[2].width >= 0)
			liscr_args_seti_int (args, widget->request[2].width);
		else
			liscr_args_seti_nil (args);
		if (widget->request[2].height >= 0)
			liscr_args_seti_int (args, widget->request[2].height);
	}
}

static void Widget_insert_col (LIScrArgs* args)
{
	int w;
	int h;
	int col = 0;
	LIScrData* widget = NULL;

	if (!liscr_args_geti_int (args, 0, &col))
		liscr_args_gets_int (args, "col", &col);
	if (!liscr_args_geti_data (args, 1, LIEXT_SCRIPT_WIDGET, &widget))
		liscr_args_gets_data (args, "widget", LIEXT_SCRIPT_WIDGET, &widget);
	col--;
	liwdg_widget_get_size (args->self, &w, &h);
	if (col < 0) col = 0;
	if (col > w) col = w;

	/* Insert row. */
	if (!liwdg_widget_insert_col (args->self, col))
		return;

	/* Set widget. */
	if (h && widget != NULL)
	{
		liwdg_widget_detach (liscr_data_get_data (widget));
		liwdg_widget_set_child (args->self, col, 0, liscr_data_get_data (widget));
	}
}

static void Widget_insert_row (LIScrArgs* args)
{
	int w;
	int h;
	int row = 0;
	LIScrData* widget = NULL;

	if (!liscr_args_geti_int (args, 0, &row))
		liscr_args_gets_int (args, "row", &row);
	if (!liscr_args_geti_data (args, 1, LIEXT_SCRIPT_WIDGET, &widget))
		liscr_args_gets_data (args, "widget", LIEXT_SCRIPT_WIDGET, &widget);
	row--;
	liwdg_widget_get_size (args->self, &w, &h);
	if (row < 0) row = 0;
	if (row > w) row = w;

	/* Insert row. */
	if (!liwdg_widget_insert_row (args->self, row))
		return;

	/* Set widget. */
	if (w && widget != NULL)
	{
		liwdg_widget_detach (liscr_data_get_data (widget));
		liwdg_widget_set_child (args->self, 0, row, liscr_data_get_data (widget));
	}
}

static void Widget_popup (LIScrArgs* args)
{
	const char* dir;
	LIWdgRect rect;
	LIWdgSize screen;
	LIWdgSize size;
	LIWdgWidget* widget;

	widget = args->self;
	liwdg_widget_detach (widget);

	/* Calculate position. */
	liwdg_manager_get_size (widget->manager, &screen.width, &screen.height);
	liwdg_widget_get_request (widget, &size);
	rect.x = (screen.width - size.width) / 2;
	rect.y = (screen.height - size.height) / 2;
	rect.width = size.width;
	rect.height = size.height;
	liscr_args_gets_int (args, "x", &rect.x);
	liscr_args_gets_int (args, "y", &rect.y);
	liscr_args_gets_int (args, "width", &rect.width);
	liscr_args_gets_int (args, "height", &rect.height);
	if (liscr_args_gets_string (args, "dir", &dir))
	{
		if (!strcmp (dir, "left")) rect.x -= size.width;
		else if (!strcmp (dir, "right")) rect.x += rect.width;
		else if (!strcmp (dir, "up")) rect.y -= size.height;
		else if (!strcmp (dir, "down")) rect.y += rect.height;
	}

	/* Popup the widget. */
	liwdg_widget_set_visible (widget, 1);
	liwdg_manager_insert_window (widget->manager, widget);
	liwdg_widget_set_allocation (widget, rect.x, rect.y, rect.width, rect.height);
}

static void Widget_remove (LIScrArgs* args)
{
	int w;
	int h;
	int col;
	int row;

	liwdg_widget_get_size (args->self, &w, &h);
	if (liscr_args_gets_int (args, "col", &col) && col >= 1 && col <= w)
		liwdg_widget_remove_col (args->self, col - 1);
	if (liscr_args_gets_int (args, "row", &row) && row >= 1 && row <= h)
		liwdg_widget_remove_row (args->self, row - 1);
}

static void Widget_set_child (LIScrArgs* args)
{
	int x = 1;
	int y = 1;
	int w;
	int h;
	LIScrData* data = NULL;

	/* Arguments. */
	if (!liscr_args_geti_int (args, 0, &x) ||
	    !liscr_args_geti_int (args, 1, &y))
		return;
	liscr_args_geti_data (args, 2, LIEXT_SCRIPT_WIDGET, &data);
	liwdg_widget_get_size (args->self, &w, &h);
	if (x < 1 || x > w || y < 1 || y > h)
		return;

	/* Attach new widget. */
	if (data != NULL)
	{
		liwdg_widget_detach (liscr_data_get_data (data));
		liwdg_widget_set_child (args->self, x - 1, y - 1, liscr_data_get_data (data));
	}
	else
		liwdg_widget_set_child (args->self, x - 1, y - 1, NULL);
}

static void Widget_set_expand (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	int expand = 1;

	liwdg_widget_get_size (args->self, &w, &h);
	if (!liscr_args_geti_bool (args, 2, &expand))
		liscr_args_gets_bool (args, "expand", &expand);
	if (liscr_args_geti_int (args, 0, &i) ||
	    liscr_args_gets_int (args, "col", &i))
	{
		if (i >= 1 && i <= w)
			liwdg_widget_set_col_expand (args->self, i - 1, expand);
	}
	if (liscr_args_geti_int (args, 2, &i) ||
	    liscr_args_gets_int (args, "row", &i))
	{
		if (i >= 1 && i <= h)
			liwdg_widget_set_row_expand (args->self, i - 1, expand);
	}
}

static void Widget_set_request (LIScrArgs* args)
{
	int internal = 0;
	int paddings[4];
	const char* text;
	const char* font_name;
	LIFntFont* font;
	LIFntLayout* layout;
	LIMatVector vector;
	LIWdgSize size = { -1, -1 };
	LIWdgWidget* widget;

	widget = args->self;
	liscr_args_gets_bool (args, "internal", &internal);
	if (liscr_args_geti_vector (args, 0, &vector))
	{
		size.width = (int) vector.x;
		size.height = (int) vector.y;
	}
	else
	{
		if (!liscr_args_geti_int (args, 0, &size.width))
			liscr_args_gets_int (args, "width", &size.width);
		if (!liscr_args_geti_int (args, 1, &size.width))
			liscr_args_gets_int (args, "height", &size.height);
	}

	/* Calculate from text if given. */
	if (liscr_args_gets_string (args, "font", &font_name) &&
	    liscr_args_gets_string (args, "text", &text))
	{
		font = lialg_strdic_find (widget->manager->styles->fonts, font_name);
		if (font != NULL)
		{
			layout = lifnt_layout_new ();
			if (layout != NULL)
			{
				if (size.width != -1)
					lifnt_layout_set_width_limit (layout, size.width);
				lifnt_layout_append_string (layout, font, text);
				if (size.width == -1)
					size.width = lifnt_layout_get_width (layout);
				if (size.height == -1)
					size.height = lifnt_layout_get_height (layout);
				lifnt_layout_free (layout);
			}
		}
	}

	/* Add paddings if given. */
	if (liscr_args_gets_intv (args, "paddings", 4, paddings) == 4)
	{
		size.width += paddings[1] + paddings[2];
		size.height += paddings[0] + paddings[3];
	}

	/* Set the request. */
	if (internal)
		liwdg_widget_set_request (args->self, 1, size.width, size.height);
	else
		liwdg_widget_set_request (args->self, 2, size.width, size.height);
}

static void Widget_get_behind (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_behind (args->self));
}
static void Widget_set_behind (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_behind (args->self, value);
}

static void Widget_get_cols (LIScrArgs* args)
{
	int w;
	int h;

	liwdg_widget_get_size (args->self, &w, &h);
	liscr_args_seti_int (args, w);
}
static void Widget_set_cols (LIScrArgs* args)
{
	int w;
	int h;
	int cols;

	liwdg_widget_get_size (args->self, &w, &h);
	if (liscr_args_geti_int (args, 0, &cols) && cols >= 0)
		liwdg_widget_set_size (args->self, cols, h);
}

static void Widget_get_fixed_size (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_fixed_size (args->self));
}
static void Widget_set_fixed_size (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_fixed_size (args->self, value);
}

static void Widget_get_floating (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_floating (args->self));
}
static void Widget_set_floating (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_floating (args->self, value);
}

static void Widget_get_fullscreen (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_fullscreen (args->self));
}
static void Widget_set_fullscreen (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_fullscreen (args->self, value);
}

static void Widget_get_height (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.height);
}

static void Widget_get_margins (LIScrArgs* args)
{
	int v[4];

	liwdg_widget_get_margins (args->self, v + 0, v + 1, v + 2, v + 3);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_float (args, "left", v[0]);
	liscr_args_sets_float (args, "right", v[1]);
	liscr_args_sets_float (args, "top", v[2]);
	liscr_args_sets_float (args, "bottom", v[3]);
}
static void Widget_set_margins (LIScrArgs* args)
{
	int v[4];

	liwdg_widget_get_margins (args->self, v + 0, v + 1, v + 2, v + 3);
	liscr_args_geti_int (args, 0, v + 0);
	liscr_args_geti_int (args, 1, v + 1);
	liscr_args_geti_int (args, 2, v + 2);
	liscr_args_geti_int (args, 3, v + 3);
	liscr_args_gets_int (args, "left", v + 0);
	liscr_args_gets_int (args, "right", v + 1);
	liscr_args_gets_int (args, "top", v + 2);
	liscr_args_gets_int (args, "bottom", v + 3);
	liwdg_widget_set_margins (args->self, v[0], v[1], v[2], v[3]);
}

static void Widget_get_offset (LIScrArgs* args)
{
	int x;
	int y;
	LIMatVector v;

	liwdg_widget_get_offset (args->self, &x, &y);
	v = limat_vector_init (x, y, 0.0f);
	liscr_args_seti_vector (args, &v);
}
static void Widget_set_offset (LIScrArgs* args)
{
	LIMatVector value;

	if (liscr_args_geti_vector (args, 0, &value))
		liwdg_widget_set_offset (args->self, (int) value.x, (int) value.y);
}

static void Widget_get_parent (LIScrArgs* args)
{
	LIWdgWidget* self = args->self;

	if (self->parent != NULL)
		liscr_args_seti_data (args, self->parent->script);
}

static void Widget_get_rows (LIScrArgs* args)
{
	int w;
	int h;

	liwdg_widget_get_size (args->self, &w, &h);
	liscr_args_seti_int (args, h);
}
static void Widget_set_rows (LIScrArgs* args)
{
	int w;
	int h;
	int rows;

	liwdg_widget_get_size (args->self, &w, &h);
	if (liscr_args_geti_int (args, 0, &rows) || rows >= 0)
		liwdg_widget_set_size (args->self, w, rows);
}

static void Widget_get_spacings (LIScrArgs* args)
{
	int v[2];

	liwdg_widget_get_spacings (args->self, v + 0, v + 1);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_int (args, "horz", v[0]);
	liscr_args_sets_int (args, "vert", v[1]);
}
static void Widget_set_spacings (LIScrArgs* args)
{
	int v[4];

	liwdg_widget_get_spacings (args->self, v + 0, v + 1);
	liscr_args_geti_int (args, 0, v + 0);
	liscr_args_geti_int (args, 1, v + 1);
	liscr_args_gets_int (args, "horz", v + 0);
	liscr_args_gets_int (args, "vert", v + 1);
	liwdg_widget_set_spacings (args->self, v[0], v[1]);
}

static void Widget_get_visible (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_visible (args->self));
}
static void Widget_set_visible (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_visible (args->self, value);
}

static void Widget_get_width (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.width);
}

static void Widget_get_x (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.x);
}
static void Widget_set_x (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_geti_int (args, 0, &rect.x);
	liwdg_widget_set_allocation (args->self, rect.x, rect.y, rect.width, rect.height);
}

static void Widget_get_y (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.y);
}
static void Widget_set_y (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_geti_int (args, 0, &rect.y);
	liwdg_widget_set_allocation (args->self, rect.x, rect.y, rect.width, rect.height);
}

/*****************************************************************************/

void liext_script_widget (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WIDGET, "widget_new", Widget_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_add_child", Widget_add_child);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_append_col", Widget_append_col);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_append_row", Widget_append_row);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_canvas_clear", Widget_canvas_clear);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_canvas_compile", Widget_canvas_compile);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_canvas_image", Widget_canvas_image);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_canvas_text", Widget_canvas_text);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_detach", Widget_detach);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_child", Widget_get_child);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_request", Widget_get_request);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_insert_col", Widget_insert_col);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_insert_row", Widget_insert_row);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_popup", Widget_popup);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_remove", Widget_remove);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_child", Widget_set_child);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_expand", Widget_set_expand);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_request", Widget_set_request);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_behind", Widget_get_behind);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_behind", Widget_set_behind);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_cols", Widget_get_cols);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_cols", Widget_set_cols);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_fixed_size", Widget_get_fixed_size);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_fixed_size", Widget_set_fixed_size);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_floating", Widget_get_floating);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_floating", Widget_set_floating);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_fullscreen", Widget_get_fullscreen);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_fullscreen", Widget_set_fullscreen);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_height", Widget_get_height);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_margins", Widget_get_margins);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_margins", Widget_set_margins);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_offset", Widget_get_offset);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_offset", Widget_set_offset);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_parent", Widget_get_parent);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_rows", Widget_get_rows);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_rows", Widget_set_rows);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_spacings", Widget_get_spacings);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_spacings", Widget_set_spacings);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_visible", Widget_get_visible);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_visible", Widget_set_visible);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_width", Widget_get_width);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_x", Widget_get_x);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_x", Widget_set_x);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_get_y", Widget_get_y);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_WIDGET, "widget_set_y", Widget_set_y);
}

/** @} */
/** @} */
