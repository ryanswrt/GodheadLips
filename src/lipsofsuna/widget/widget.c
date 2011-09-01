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
 * \addtogroup LIWdg Widget
 * @{
 * \addtogroup LIWdgWidget Widget
 * @{
 */

#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-element.h"

#define LIWDG_DEFAULT_SPACING 5

enum
{
	PRIVATE_REBUILD_CHILDREN = 0x01,
	PRIVATE_REBUILD_HORZ = 0x02,
	PRIVATE_REBUILD_REQUEST = 0x04,
	PRIVATE_REBUILD_VERT = 0x08
};

static void private_call_attach (
	LIWdgWidget* self,
	int          x,
	int          y);

static void private_call_detach (
	LIWdgWidget* self,
	int          x,
	int          y);

static void private_cell_changed (
	LIWdgWidget* self,
	int          x,
	int          y);

static int private_get_col_size (
	LIWdgWidget* self,
	int          x);

static void private_rebuild (
	LIWdgWidget* self,
	int          flags);

static int private_get_row_size (
	LIWdgWidget* self,
	int          y);

static LIWdgWidget* private_table_child_at (
	LIWdgWidget* self,
	int          pixx,
	int          pixy);

/*****************************************************************************/

LIWdgWidget* liwdg_widget_new (
	LIWdgManager* manager)
{
	LIWdgWidget* self;

	self = lisys_calloc (1, sizeof (LIWdgWidget));
	if (self == NULL)
		return NULL;
	self->manager = manager;
	self->request[1].width = -1;
	self->request[1].height = -1;
	self->request[2].width = -1;
	self->request[2].height = -1;
	self->visible = 1;
	self->cols = NULL;
	self->rows = NULL;
	self->cells = NULL;
	self->width = 0;
	self->height = 0;
	self->col_expand = 0;
	self->row_expand = 0;
	self->col_spacing = LIWDG_DEFAULT_SPACING;
	self->row_spacing = LIWDG_DEFAULT_SPACING;
	self->margin_left = 0;
	self->margin_right = 0;
	self->margin_top = 0;
	self->margin_bottom = 0;

	/* Add to dictionary. */
	if (!lialg_ptrdic_insert (manager->widgets.all, self, self))
	{
		lisys_free (self);
		return NULL;
	}

	private_rebuild (self, PRIVATE_REBUILD_REQUEST);

	return self;
}

/**
 * \brief Frees the widget.
 * \param self Widget.
 */
void liwdg_widget_free (LIWdgWidget* self)
{
	int x;
	int y;
	LIWdgWidget* widget;

	for (widget = self->children ; widget != NULL ; widget = widget->next)
	{
		widget->next = NULL;
		widget->prev = NULL;
		widget->parent = NULL;
	}
	for (y = 0 ; y < self->height ; y++)
	for (x = 0 ; x < self->width ; x++)
		private_call_detach (self, x, y);
	liwdg_widget_canvas_clear (self);
	liwdg_widget_detach (self);
	lisys_free (self->cols);
	lisys_free (self->rows);
	lisys_free (self->cells);
	lialg_ptrdic_remove (self->manager->widgets.all, self);
	lisys_free (self);
}

void liwdg_widget_add_child (
	LIWdgWidget* self,
	LIWdgWidget* child)
{
	child->prev = NULL;
	child->next = self->children;
	if (self->children != NULL)
		self->children->prev = child;
	self->children = child;
	child->parent = self;
	liwdg_widget_child_request (self, child);
}

/**
 * \brief Appends an empty column to the widget.
 * \param self Widget.
 * \return Nonzero on success.
 */
int liwdg_widget_append_col (
	LIWdgWidget* self)
{
	return liwdg_widget_set_size (self, self->width + 1, self->height);
}

/**
 * \brief Appends an empty row to the widget.
 * \param self Widget.
 * \return Nonzero on success.
 */
int liwdg_widget_append_row (
	LIWdgWidget* self)
{
	return liwdg_widget_set_size (self, self->width, self->height + 1);
}

void liwdg_widget_canvas_clear (
	LIWdgWidget* self)
{
	LIWdgElement* elem;
	LIWdgElement* elem_next;

	for (elem = self->elements ; elem != NULL ; elem = elem_next)
	{
		elem_next = elem->next;
		liwdg_element_free (elem);
	}
	self->elements = NULL;
}

void liwdg_widget_canvas_compile (
	LIWdgWidget* self)
{
	LIWdgElement* elem;

	for (elem = self->elements ; elem != NULL ; elem = elem->next)
		liwdg_element_update (elem, self->manager, &self->allocation);
}

int liwdg_widget_canvas_insert (
	LIWdgWidget*  self,
	LIWdgElement* element)
{
	LIWdgElement* elem;

	if (self->elements != NULL)
	{
		for (elem = self->elements ; elem->next != NULL ; elem = elem->next) {}
		elem->next = element;
	}
	else
		self->elements = element;

	return 1;
}

/**
 * \brief Gets a child widget under the cursor position.
 *
 * \param self Container.
 * \param pixx Cursor position in pixels.
 * \param pixy Cursor position in pixels.
 * \return Widget owned by the widget or NULL.
 */
LIWdgWidget* liwdg_widget_child_at (
	LIWdgWidget* self,
	int          pixx,
	int          pixy)
{
	LIWdgWidget* widget;

	/* Try table packet widgets. */
	widget = private_table_child_at (self, pixx, pixy);
	if (widget != NULL)
		return widget;

	/* Try manually packed widgets. */
	if (self->children != NULL)
	{
		for (widget = self->children ; widget->next != NULL ; widget = widget->next)
			{}
		for ( ; widget != NULL ; widget = widget->prev)
		{
			if (widget->visible &&
			    widget->allocation.x <= pixx && pixx < widget->allocation.x + widget->allocation.width &&
			    widget->allocation.y <= pixy && pixy < widget->allocation.y + widget->allocation.height)
				return widget;
		}
	}

	return NULL;
}

/**
 * \brief Updates the layout after the size of a child has changed.
 *
 * \param self Container.
 * \param child Child widget.
 */
void liwdg_widget_child_request (
	LIWdgWidget* self,
	LIWdgWidget* child)
{
	int x;
	int y;
	LIWdgSize size;
	LIWdgGroupCell* cell;

	/* Table layout. */
	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			cell = self->cells + x + y * self->width;
			if (cell->child == child)
			{
				size.width = private_get_col_size (self, x);
				size.height = private_get_row_size (self, y);
				if (self->cols[x].request != size.width ||
				    self->rows[y].request != size.height)
					private_cell_changed (self, x, y);
				return;
			}
		}
	}

	/* Manual layout. */
	liwdg_widget_get_request (child, &size);
	liwdg_widget_set_allocation (child,
		self->allocation.x + child->offset.x,
		self->allocation.y + child->offset.y,
		size.width, size.height);
}

LIWdgWidget* liwdg_widget_cycle_focus (
	LIWdgWidget* self,
	LIWdgWidget* curr,
	int          next)
{
	int x = 0;
	int y = 0;
	int found;
	LIWdgWidget* tmp;
	LIWdgWidget* child;

	/* Find old focused widget. */
	found = 0;
	if (curr != NULL)
	{
		for (y = self->height - 1 ; y >= 0 ; y--)
		{
			for (x = self->width - 1 ; x >= 0 ; x--)
			{
				child = self->cells[x + y * self->width].child;
				if (child == curr)
				{
					found = 1;
					break;
				}
			}
			if (found)
				break;
		}
		lisys_assert (found);
	}

	/* Find new focused widget. */
	if (next)
	{
		/* Set start position. */
		if (!found)
		{
			x = 0;
			y = 0;
		}

		/* Iterate forward. */
		for ( ; y < self->height ; y++, x = 0)
		for ( ; x < self->width ; x++)
		{
			if (!found)
			{
				child = self->cells[x + y * self->width].child;
				if (child == NULL)
					continue;
				tmp = liwdg_widget_cycle_focus (child, NULL, 1);
				if (tmp != NULL)
					return tmp;
			}
			else
				found = 0;
		}
	}
	else
	{
		/* Set start position. */
		if (!found)
		{
			x = self->width - 1;
			y = self->height - 1;
		}

		/* Iterate backward. */
		for ( ; y >= 0 ; y--, x = self->width - 1)
		for ( ; x >= 0 ; x--)
		{
			if (!found)
			{
				child = self->cells[x + y * self->width].child;
				if (child == NULL)
					continue;
				tmp = liwdg_widget_cycle_focus (child, NULL, 0);
				if (tmp != NULL)
					return tmp;
			}
			else
				found = 0;
		}
	}

	return NULL;
}

/**
 * \brief Unparents the widget and removes from the dialog list.
 *
 * If the widget is a child widget of another widget, it is removed from the
 * parent container. If it is the root widget of the widget manager or a dialog,
 * it is removed from the manager.
 *
 * \param self Widget.
 * \return Nonzero if was detached from something.
 */
int liwdg_widget_detach (
	LIWdgWidget* self)
{
	int changed = 0;

	if (self->floating)
	{
		liwdg_manager_remove_window (self->manager, self);
		self->floating = 0;
		changed = 1;
	}
	else if (self->parent != NULL)
	{
		liwdg_widget_detach_child (self->parent, self);
		lisys_assert (self->parent == NULL);
		changed = 1;
	}

	return changed;
}

/**
 * \brief Finds and unparents a child widget.
 *
 * \param self Container.
 * \param child Child widget.
 */
void liwdg_widget_detach_child (
	LIWdgWidget* self,
	LIWdgWidget* child)
{
	int x;
	int y;
	LIWdgGroupCell* cell;

	/* Table packing. */
	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			cell = self->cells + x + y * self->width;
			if (cell->child == child)
			{
				liwdg_widget_set_child (self, x, y, NULL);
				return;
			}
		}
	}

	/* Manual packing. */
	if (child->prev != NULL)
		child->prev->next = child->next;
	else
		self->children = child->next;
	if (child->next != NULL)
		child->next->prev = child->prev;
	child->next = NULL;
	child->prev = NULL;
	child->parent = NULL;
}

void liwdg_widget_draw (
	LIWdgWidget* self)
{
	int x;
	int y;
	LIMatMatrix matrix;
	LIWdgElement* elem;
	LIWdgGroupCell* cell;
	LIWdgWidget* widget;

	/* Paint custom. */
	lical_callbacks_call (self->manager->callbacks, "widget-paint", lical_marshal_DATA_PTR, self);

	/* Paint canvas. */
	if (self->elements != NULL)
	{
		matrix = limat_matrix_translation (self->allocation.x, self->allocation.y, 0.0f);
		for (elem = self->elements ; elem != NULL ; elem = elem->next)
			liwdg_element_paint (elem, self->manager, &matrix);
	}

	/* Paint table packed children. */
	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			cell = self->cells + x + y * self->width;
			if (cell->child != NULL && cell->child->visible)
				liwdg_widget_draw (cell->child);
		}
	}

	/* Paint manually packed children. */
	for (widget = self->children ; widget != NULL ; widget = widget->next)
	{
		if (widget->visible)
			liwdg_widget_draw (widget);
	}
}

/**
 * \brief Calls the passed function for each child.
 * \param self Container.
 * \param call Function to call.
 * \param data Data to pass to the function.
 */
void liwdg_widget_foreach_child (
	LIWdgWidget* self,
	void       (*call)(),
	void*        data)
{
	int x;
	int y;
	LIWdgGroupCell* cell;

	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			cell = self->cells + x + y * self->width;
			if (cell->child != NULL)
				call (data, cell->child);
		}
	}
}

/**
 * \brief Inserts an empty column to the widget.
 * \param self Widget.
 * \param index Column index.
 * \return Nonzero on success.
 */
int liwdg_widget_insert_col (
	LIWdgWidget* self,
	int          index)
{
	int x;
	int y;

	lisys_assert (index >= 0);
	lisys_assert (index <= self->width);

	/* Resize. */
	if (!liwdg_widget_set_size (self, self->width + 1, self->height))
		return 0;

	/* Shift columns. */
	for (x = self->width - 1 ; x > index ; x--)
	{
		self->cols[x] = self->cols[x - 1];
		for (y = 0 ; y < self->height ; y++)
			self->cells[x + y * self->width] = self->cells[(x - 1) + y * self->width];
	}

	/* Clear new column. */
	memset (self->cols + index, 0, sizeof (LIWdgGroupCol));
	for (y = 0 ; y < self->height ; y++)
		self->cells[index + x * self->width].child = NULL;

	/* Rebuild columns. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_CHILDREN);

	return 1;
}

/**
 * \brief Inserts an empty row to the widget.
 * \param self Widget.
 * \param index Row index.
 * \return Nonzero on success.
 */
int liwdg_widget_insert_row (
	LIWdgWidget* self,
	int          index)
{
	int x;
	int y;

	lisys_assert (index >= 0);
	lisys_assert (index <= self->height);

	/* Resize. */
	if (!liwdg_widget_set_size (self, self->width, self->height + 1))
		return 0;

	/* Shift rows. */
	for (y = self->height - 1 ; y > index ; y--)
	{
		self->rows[y] = self->rows[y - 1];
		for (x = 0 ; x < self->width ; x++)
			self->cells[x + y * self->width] = self->cells[x + (y - 1) * self->width];
	}

	/* Clear new row. */
	memset (self->rows + index, 0, sizeof (LIWdgGroupRow));
	for (x = 0 ; x < self->width ; x++)
		self->cells[x + index * self->width].child = NULL;

	/* Rebuild rows. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);

	return 1;
}

void liwdg_widget_move (
	LIWdgWidget* self,
	int          x,
	int          y)
{
	liwdg_widget_set_allocation (self, x, y,
		self->allocation.width,
		self->allocation.height);
}

void liwdg_widget_reload (
	LIWdgWidget* self,
	int          pass)
{
	LIWdgElement* elem;

	/* Reload canvas. */
	for (elem = self->elements ; elem != NULL ; elem = elem->next)
		liwdg_element_reload (elem, self->manager, pass);
}

/**
 * \brief Removes a column from the widget.
 * \param self Widget.
 * \param index Column index.
 */
void liwdg_widget_remove_col (
	LIWdgWidget* self,
	int          index)
{
	int x;
	int y;

	lisys_assert (index >= 0);
	lisys_assert (index < self->width);

	/* Delete widgets. */
	for (y = 0 ; y < self->height ; y++)
		private_call_detach (self, index, y);

	/* Shift columns. */
	for (x = index ; x < self->width - 1 ; x++)
	{
		self->cols[x] = self->cols[x + 1];
		for (y = 0 ; y < self->height ; y++)
			self->cells[x + y * self->width] = self->cells[(x + 1) + y * self->width];
	}

	/* Clear last column. */
	for (y = 0 ; y < self->height ; y++)
		self->cells[(self->width - 1) + y * self->width].child = NULL;

	/* Resize. */
	liwdg_widget_set_size (self, self->width - 1, self->height);

	/* Rebuild all. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
}

/**
 * \brief Removes a row from the widget.
 * \param self Widget.
 * \param index Row index.
 */
void liwdg_widget_remove_row (
	LIWdgWidget* self,
	int          index)
{
	int x;
	int y;

	lisys_assert (index >= 0);
	lisys_assert (index < self->height);

	/* Delete widgets. */
	for (x = 0 ; x < self->width ; x++)
		private_call_detach (self, x, index);

	/* Shift rows. */
	for (y = index ; y < self->height - 1 ; y++)
	{
		self->rows[y] = self->rows[y + 1];
		for (x = 0 ; x < self->width ; x++)
			self->cells[x + y * self->width] = self->cells[x + (y + 1) * self->width];
	}

	/* Clear last row. */
	for (x = 0 ; x < self->width ; x++)
		self->cells[x + (self->height - 1) * self->width].child = NULL;

	/* Resize. */
	liwdg_widget_set_size (self, self->width, self->height - 1);

	/* Rebuild all. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
}

void liwdg_widget_get_allocation (
	LIWdgWidget* self,
	LIWdgRect*   allocation)
{
	*allocation = self->allocation;
}

void liwdg_widget_set_allocation (
	LIWdgWidget* self,
	int          x,
	int          y,
	int          w,
	int          h)
{
	if (self->allocation.x != x ||
	    self->allocation.y != y ||
	    self->allocation.width != w ||
	    self->allocation.height != h)
	{
		self->allocation.x = x;
		self->allocation.y = y;
		self->allocation.width = w;
		self->allocation.height = h;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
		lical_callbacks_call (self->manager->callbacks, "widget-allocation", lical_marshal_DATA_PTR, self);
	}
}

int liwdg_widget_get_behind (
	LIWdgWidget* self)
{
	return self->behind;
}

void liwdg_widget_set_behind (
	LIWdgWidget* self,
	int          value)
{
	self->behind = value;
	if (self->floating)
	{
		if (value)
		{
			/* Move to bottom. */
			if (self != self->manager->dialogs.bottom)
			{
				if (self->next != NULL)
					self->next->prev = self->prev;
				if (self->prev != NULL)
					self->prev->next = self->next;
				self->prev = self->manager->dialogs.bottom;
				if (self->prev != NULL)
					self->prev->next = self;
				self->manager->dialogs.bottom = self;
			}
		}
		else
		{
			/* Move out of bottom. */
			if (self == self->manager->dialogs.bottom && self->prev != NULL)
			{
				self->prev->next = NULL;
				self->manager->dialogs.bottom = self->prev;
				self->next = self->manager->dialogs.bottom;
				self->prev = self->manager->dialogs.bottom->prev;
				self->manager->dialogs.bottom->prev = self;
			}
		}
	}
}

/**
 * \brief Gets the size of a cell.
 * \param self Widget.
 * \param x Column number.
 * \param y Row number.
 * \param rect Return location for the size.
 */
void liwdg_widget_get_cell_rect (
	LIWdgWidget* self,
	int          x,
	int          y,
	LIWdgRect*   rect)
{
	liwdg_widget_get_allocation (self, rect);
	rect->x += self->cols[x].start;
	rect->y += self->rows[y].start;
	rect->width = self->cols[x].allocation;
	rect->height = self->rows[y].allocation;
}

/**
 * \brief Gets a child widget.
 * \param self Widget.
 * \param x Column number.
 * \param y Row number.
 * \return Widget owned by the widget or NULL.
 */
LIWdgWidget* liwdg_widget_get_child (
	LIWdgWidget* self,
	int          x,
	int          y)
{
	lisys_assert (x < self->width);
	lisys_assert (y < self->height);
	return self->cells[x + y * self->width].child;
}

/**
 * \brief Sets a child widget in the given cell position.
 * \param self Widget.
 * \param x Column number.
 * \param y Row number.
 * \param child Widget or NULL.
 */
void liwdg_widget_set_child (
	LIWdgWidget* self,
	int          x,
	int          y,
	LIWdgWidget* child)
{
	LIWdgGroupCell* cell;

	lisys_assert (x < self->width);
	lisys_assert (y < self->height);

	/* Check for same widget. */
	cell = self->cells + x + y * self->width;
	if (cell->child == child)
		return;

	/* Detach the old child. */
	if (cell->child != NULL)
	{
		lisys_assert (!cell->child->floating);
		lisys_assert (cell->child->parent == self);
		private_call_detach (self, x, y);
	}

	/* Attach the new child. */
	cell->child = child;
	if (child != NULL)
	{
		lisys_assert (!child->floating);
		lisys_assert (child->parent == NULL);
		child->parent = self;
		private_call_attach (self, x, y);
	}

	/* Update the size of the cell. */
	private_cell_changed (self, x, y);
}

/**
 * \brief Gets the column expand status of a column.
 * \param self Widget.
 * \param x Column number.
 * \return Nonzero if the column is set to expand.
 */
int liwdg_widget_get_col_expand (
	LIWdgWidget* self,
	int          x)
{
	return self->cols[x].expand;
}

/**
 * \brief Sets the expand attribute of a column.
 * \param self Widget.
 * \param x Column number.
 * \param expand Nonzero if should expand.
 */
void liwdg_widget_set_col_expand (
	LIWdgWidget* self,
	int          x,
	int          expand) 
{
	if (self->cols[x].expand != expand)
	{
		if (expand)
			self->col_expand++;
		else
			self->col_expand--;
		self->cols[x].expand = expand;
		private_rebuild (self, PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_CHILDREN);
	}
}

/**
 * \brief Gets the allocation of a column in pixels.
 * \param self Widget.
 * \param x Column number.
 * \return Horizontal allocation of the column in pixels.
 */
int liwdg_widget_get_col_size (
	LIWdgWidget* self,
	int          x)
{
	return self->cols[x].allocation;
}

int liwdg_widget_get_fixed_size (
	LIWdgWidget* self)
{
	return self->fixed_size;
}

void liwdg_widget_set_fixed_size (
	LIWdgWidget* self,
	int          value)
{
	self->fixed_size = (value != 0);
}

int liwdg_widget_get_floating (
	LIWdgWidget* self)
{
	return self->floating;
}

void liwdg_widget_set_floating (
	LIWdgWidget* self,
	int          value)
{
	liwdg_widget_detach (self);
	if (value)
	{
		if (liwdg_manager_insert_window (self->manager, self))
		{
			self->floating = 1;
			self->visible = 1;
		}
	}
	else
	{
		self->floating = 0;
		self->visible = 0;
	}
}

int liwdg_widget_get_fullscreen (
	LIWdgWidget* self)
{
	return self->fullscreen;
}

void liwdg_widget_set_fullscreen (
	LIWdgWidget* self,
	int           value)
{
	self->fullscreen = value;
}

int liwdg_widget_get_grab (
	const LIWdgWidget* self)
{
	return self->manager->widgets.grab == self;
}

void liwdg_widget_set_grab (
	LIWdgWidget* self,
	int          value)
{
	int cx;
	int cy;

	if (value)
	{
		cx = self->manager->width / 2;
		cy = self->manager->height / 2;
		SDL_ShowCursor (SDL_DISABLE);
		SDL_WarpMouse (cx, cy);
		SDL_WM_GrabInput (SDL_GRAB_OFF);
		self->manager->widgets.grab = self;
	}
	else
	{
		SDL_ShowCursor (SDL_ENABLE);
		SDL_WM_GrabInput (SDL_GRAB_OFF);
		self->manager->widgets.grab = NULL;
	}
}

/**
 * \brief Get the homogeneousness flag.
 * \param self Widget.
 * \return value Nonzero if homogeneous.
 */
int liwdg_widget_get_homogeneous (
	const LIWdgWidget* self)
{
	return self->homogeneous;
}

/**
 * \brief Set the homogeneousness flag.
 * \param self Widget.
 * \param value Nonzero if homogeneous.
 */
void liwdg_widget_set_homogeneous (
	LIWdgWidget* self,
	int          value)
{
	self->homogeneous = value;
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
}

/**
 * \brief Gets the margins of widget in pixels.
 * \param self Widget.
 * \param left Return location for the left margin.
 * \param right Return location for the right margin.
 * \param top Return location for the top margin.
 * \param bottom Return location for the bottom margin.
 */
void liwdg_widget_get_margins (
	LIWdgWidget* self,
	int*         left,
	int*         right,
	int*         top,
	int*         bottom)
{
	if (left != NULL) *left = self->margin_left;
	if (right != NULL) *right = self->margin_right;
	if (top != NULL) *top = self->margin_top;
	if (bottom != NULL) *bottom = self->margin_bottom;
}

/**
 * \brief Sets the margins of the widget.
 * \param self Widget.
 * \param left Left margin in pixels.
 * \param right Right margin in pixels.
 * \param top Top margin in pixels.
 * \param bottom Bottom margin in pixels.
 */
void liwdg_widget_set_margins (
	LIWdgWidget* self,
	int          left,
	int          right,
	int          top,
	int          bottom)
{
	/* Set margins. */
	self->margin_left = left;
	self->margin_right = right;
	self->margin_top = top;
	self->margin_bottom = bottom;

	/* Rebuild the layout. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
}

void liwdg_widget_get_offset (
	LIWdgWidget* self,
	int*         x,
	int*         y)
{
	*x = self->offset.x;
	*y = self->offset.y;
}

void liwdg_widget_set_offset (
	LIWdgWidget* self,
	int          x,
	int          y)
{
	if (self->offset.x != x || self->offset.y != y)
	{
		self->offset.x = x;
		self->offset.y = y;
		if (self->parent != NULL)
			liwdg_widget_child_request (self->parent, self);
	}
}

/**
 * \brief Gets the full size request of the widget.
 *
 * Returns the larger of the user and hard size requests, combined with the
 * style paddings of the widget.
 *
 * \param self Widget.
 * \param request Return location for the size.
 */
void liwdg_widget_get_request (
	LIWdgWidget* self,
	LIWdgSize*   request)
{
	request->width = self->request[0].width;
	request->height = self->request[0].height;
	if (self->request[1].width != -1)
		request->width = LIMAT_MAX (request->width, self->request[1].width);
	if (self->request[1].height != -1)
		request->height = LIMAT_MAX (request->height, self->request[1].height);
	if (self->request[2].width != -1)
		request->width = LIMAT_MAX (request->width, self->request[2].width);
	if (self->request[2].height != -1)
		request->height = LIMAT_MAX (request->height, self->request[2].height);
}

/**
 * \brief Sets or unsets the user overridden size request of the widget.
 *
 * \param self Widget.
 * \param level Request level.
 * \param w Width or -1 to unset.
 * \param h Height or -1 to unset.
 */
void liwdg_widget_set_request (
	LIWdgWidget* self,
	int          level,
	int          w,
	int          h)
{
	/* Fixed size widgets have no size request. */
	if (self->fixed_size && !level)
		w = h = 0;

	/* Update the size request. */
	if (self->request[level].width != w ||
	    self->request[level].height != h)
	{
		self->request[level].width = w;
		self->request[level].height = h;
		if (self->parent != NULL)
			liwdg_widget_child_request (self->parent, self);
	}
}

LIWdgWidget* liwdg_widget_get_root (
	LIWdgWidget* self)
{
	LIWdgWidget* widget;

	for (widget = self ; widget->parent != NULL ; widget = widget->parent) { }
	return widget;
}

/**
 * \brief Gets the row expand status of a row.
 * \param self Widget.
 * \param y Row number.
 * \return Nonzero if the row is set to expand.
 */
int liwdg_widget_get_row_expand (
	LIWdgWidget* self,
	int          y)
{
	return self->rows[y].expand;
}

/**
 * \brief Sets the expand attribute of a row.
 * \param self Widget.
 * \param y Row number.
 * \param expand Nonzero if should expand.
 */
void liwdg_widget_set_row_expand (
	LIWdgWidget* self,
	int          y,
	int          expand)
{
	if (self->rows[y].expand != expand)
	{
		if (expand)
			self->row_expand++;
		else
			self->row_expand--;
		self->rows[y].expand = expand;
		private_rebuild (self, PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
}

/**
 * \brief Gets the allocation of a row in pixels.
 * \param self Widget.
 * \param y Row number.
 * \return Vertical allocation of the row in pixels.
 */
int liwdg_widget_get_row_size (
	LIWdgWidget* self,
	int          y)
{
	return self->rows[y].allocation;
}

LIScrData* liwdg_widget_get_script (
	LIWdgWidget* self)
{
	return self->script;
}

void liwdg_widget_set_script (
	LIWdgWidget* self,
	LIScrData*   value)
{
	self->script = value;
}

/**
 * \brief Gets the number of columns and rows.
 * \param self Widget.
 * \param cols Return location for the number of columns.
 * \param rows Return location for the number of rows.
 */
void liwdg_widget_get_size (
	LIWdgWidget* self,
	int*         cols,
	int*         rows)
{
	if (cols != NULL) *cols = self->width;
	if (rows != NULL) *rows = self->height;
}

/**
 * \brief Sets the number of columns and rows.
 * \param self Widget.
 * \param width Number of columns.
 * \param height Number of rows.
 * \return Nonzero on success.
 */
int liwdg_widget_set_size (
	LIWdgWidget* self,
	int          width,
	int          height)
{
	int x;
	int y;
	LIWdgGroupCol* mem0 = NULL;
	LIWdgGroupRow* mem1 = NULL;
	LIWdgGroupCell* mem2 = NULL;

	/* Allocate memory. */
	if (width > 0)
	{
		mem0 = (LIWdgGroupCol*) lisys_calloc (width, sizeof (LIWdgGroupCol));
		if (mem0 == NULL)
			return 0;
	}
	if (height > 0)
	{
		mem1 = (LIWdgGroupRow*) lisys_calloc (height, sizeof (LIWdgGroupRow));
		if (mem1 == NULL)
		{
			lisys_free (mem0);
			return 0;
		}
	}
	if (width > 0 && height > 0)
	{
		mem2 = (LIWdgGroupCell*) lisys_calloc (width * height, sizeof (LIWdgGroupCell));
		if (mem2 == NULL)
		{
			lisys_free (mem0);
			lisys_free (mem1);
			return 0;
		}
	}

	/* Free widgets that don't fit. */
	for (y = 0 ; y < height ; y++)
	{
		for (x = width ; x < self->width ; x++)
			private_call_detach (self, x, y);
	}
	for (y = height ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
			private_call_detach (self, x, y);
	}

	/* Copy over the column data. */
	if (self->width < width)
		memcpy (mem0, self->cols, self->width * sizeof (LIWdgGroupCol));
	else
		memcpy (mem0, self->cols, width * sizeof (LIWdgGroupCol));
	lisys_free (self->cols);
	self->cols = mem0;

	/* Copy over the row data. */
	if (self->height < height)
		memcpy (mem1, self->rows, self->height * sizeof (LIWdgGroupRow));
	else
		memcpy (mem1, self->rows, height * sizeof (LIWdgGroupRow));
	lisys_free (self->rows);
	self->rows = mem1;

	/* Copy over the cell data. */
	for (y = 0 ; y < height && y < self->height ; y++)
	{
		for (x = 0 ; x < width && x < self->width ; x++)
		{
			mem2[x + y * width].child = self->cells[x + y * self->width].child;
		}
	}
	lisys_free (self->cells);
	self->cells = mem2;
	self->width = width;
	self->height = height;

	/* Update the expansion information. */
	self->col_expand = 0;
	self->row_expand = 0;
	for (x = 0 ; x < width ; x++)
	{
		if (self->cols[x].expand)
			self->col_expand++;
	}
	for (y = 0 ; y < height ; y++)
	{
		if (self->rows[y].expand)
			self->row_expand++;
	}

	/* Update the size request. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST);
	return 1;
}

/**
 * \brief Gets the row and column spacings in pixels.
 * \param self Widget.
 * \param column Return location for the column spacing.
 * \param row Return location for the row spacing.
 */
void liwdg_widget_get_spacings (
	LIWdgWidget* self,
	int*         column,
	int*         row)
{
	if (column != NULL) *column = self->col_spacing;
	if (row != NULL) *row = self->row_spacing;
}

/**
 * \brief Sets row and column spacings.
 * \param self Widget.
 * \param column Spacing between columns.
 * \param row Spacing between rows.
 */
void liwdg_widget_set_spacings (
	LIWdgWidget* self,
	int          column,
	int          row)
{
	if (self->col_spacing != column &&
	    self->row_spacing != row)
	{
		self->col_spacing = column;
		self->row_spacing = row;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
	else if (self->col_spacing != column)
	{
		self->col_spacing = column;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_CHILDREN);
	}
	else if (self->row_spacing != row)
	{
		self->row_spacing = row;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
}

int liwdg_widget_get_visible (
	LIWdgWidget* self)
{
	return self->visible;
}

void liwdg_widget_set_visible (
	LIWdgWidget* self,
	int          visible)
{
	self->visible = (visible != 0);
	if (self->parent != NULL)
		liwdg_widget_child_request (self->parent, self);
	if (self->floating)
		liwdg_manager_remove_window (self->manager, self);
}

/*****************************************************************************/

static void private_call_attach (
	LIWdgWidget* self,
	int          x,
	int          y)
{
}

static void private_call_detach (
	LIWdgWidget* self,
	int          x,
	int          y)
{
	LIWdgWidget* child;

	child = self->cells[x + y * self->width].child;
	if (child != NULL)
	{
		child->parent = NULL;
		self->cells[x + y * self->width].child = NULL;
	}
}

static void private_cell_changed (
	LIWdgWidget* self,
	int          x,
	int          y)
{
	LIWdgSize size;
	LIWdgWidget* child;

	/* Get the row and column size requests. */
	size.width = private_get_col_size (self, x);
	size.height = private_get_row_size (self, y);
	child = self->cells[x + y * self->width].child;

	/* Rebuild if the requests have changed. */
	if (size.width  != self->cols[x].request &&
	    size.height != self->rows[y].request)
	{
		/* Both horizontal and vertical layout changed. */
		self->cols[x].request = size.width;
		self->rows[y].request = size.height;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
	else if (size.width != self->cols[x].request)
	{
		/* Only vertical layout changed. */
		self->cols[x].request = size.width;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_CHILDREN);
	}
	else if (size.height != self->rows[y].request)
	{
		/* Only horizontal layout changed. */
		self->rows[y].request = size.height;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
	else if (child != NULL && child->visible)
	{
		/* Only set allocation of the widget. */
		lisys_assert (self->cols[x].allocation >= size.width);
		lisys_assert (self->rows[y].allocation >= size.height);
		liwdg_widget_set_allocation (child,
			self->allocation.x + self->cols[x].start,
			self->allocation.y + self->rows[y].start,
			self->cols[x].allocation,
			self->rows[y].allocation);
	}

	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
}

static int private_get_col_size (
	LIWdgWidget* self,
	int          x)
{
	int y;
	int width;
	LIWdgSize size;
	LIWdgWidget* child;

	width = 0;
	for (y = 0 ; y < self->height ; y++)
	{
		child = self->cells[x + y * self->width].child;
		if (child != NULL && child->visible)
		{
			liwdg_widget_get_request (child, &size);
			if (width < size.width)
				width = size.width;
		}
	}
	return width;
}

static void private_rebuild (
	LIWdgWidget* self,
	int          flags)
{
	int x;
	int y;
	int wmax;
	int hmax;
	int wpad;
	int hpad;
	int wreq;
	int hreq;
	int start;
	int expand;
	LIWdgWidget* child;
	LIWdgRect rect;
	LIWdgSize size;

	if (self->rebuilding)
		return;
	self->rebuilding = 1;

	if (self->homogeneous)
	{
		if (1)
		{
			/* Calculate the width request. */
			wmax = 0;
			wpad = self->margin_right + self->margin_left;
			for (x = 0 ; x < self->width ; x++)
			{
				if (self->cols[x].request > 0)
				{
					if (x != self->width - 1)
						wpad += self->col_spacing;
					if (wmax < self->cols[x].request)
						wmax = self->cols[x].request;
				}
			}
			wreq = wpad + wmax * self->width;

			/* Calculate the height request. */
			hmax = 0;
			hpad = self->margin_top + self->margin_bottom;
			for (y = 0 ; y < self->height ; y++)
			{
				if (self->rows[y].request > 0)
				{
					if (y != self->height - 1)
						hpad += self->row_spacing;
					if (hmax < self->rows[y].request)
						hmax = self->rows[y].request;
				}
			}
			hreq = hpad + hmax * self->height;

			/* Set the size request. */
			liwdg_widget_set_request (self, 0, wreq, hreq);
			liwdg_widget_get_allocation (self, &rect);
			liwdg_widget_get_request (self, &size);
			rect.width = LIMAT_MAX (size.width, rect.width);
			rect.height = LIMAT_MAX (size.height, rect.height);
			liwdg_widget_set_allocation (self, rect.x, rect.y, rect.width, rect.height);
		}

		liwdg_widget_get_allocation (self, &rect);

		if (flags & PRIVATE_REBUILD_HORZ)
		{
			/* Get horizontal expansion. */
			if (self->col_expand > 0)
			{
				expand = rect.width - wreq;
				lisys_assert (expand >= 0);
				expand /= self->width;
			}
			else
				expand = 0;

			/* Set horizontal allocations. */
			start = self->margin_left;
			for (x = 0 ; x < self->width ; x++)
			{
				self->cols[x].start = start;
				self->cols[x].allocation = wmax;
				self->cols[x].allocation += expand;
				start += self->cols[x].allocation;
				start += self->col_spacing;
			}
		}

		if (flags & PRIVATE_REBUILD_VERT)
		{
			/* Get vertical expansion. */
			if (self->row_expand > 0)
			{
				expand = rect.height - hreq;
				lisys_assert (expand >= 0);
				expand /= self->height;
			}
			else
				expand = 0;

			/* Set vertical allocations. */
			start = self->margin_top;
			for (y = 0 ; y < self->height ; y++)
			{
				self->rows[y].start = start;
				self->rows[y].allocation = hmax;
				self->rows[y].allocation += expand;
				start += self->rows[y].allocation;
				start += self->row_spacing;
			}
		}
	}
	else
	{
		if (flags & PRIVATE_REBUILD_REQUEST)
		{
			/* Calculate the width request. */
			wreq = self->margin_right + self->margin_left;
			for (x = 0 ; x < self->width ; x++)
			{
				if (self->cols[x].request > 0)
				{
					if (x != self->width - 1)
						wreq += self->col_spacing;
					wreq += self->cols[x].request;
				}
			}

			/* Calculate the height request. */
			hreq = self->margin_top + self->margin_bottom;
			for (y = 0 ; y < self->height ; y++)
			{
				if (self->rows[y].request > 0)
				{
					if (y != self->height - 1)
						hreq += self->row_spacing;
					hreq += self->rows[y].request;
				}
			}

			/* Set the size request. */
			liwdg_widget_set_request (self, 0, wreq, hreq);
			liwdg_widget_get_allocation (self, &rect);
			liwdg_widget_get_request (self, &size);
			rect.width = LIMAT_MAX (size.width, rect.width);
			rect.height = LIMAT_MAX (size.height, rect.height);
			liwdg_widget_set_allocation (self, rect.x, rect.y, rect.width, rect.height);
		}

		liwdg_widget_get_allocation (self, &rect);

		if (flags & PRIVATE_REBUILD_HORZ)
		{
			/* Get horizontal expansion. */
			if (self->col_expand > 0)
			{
				expand = rect.width - self->margin_left - self->margin_right;
				lisys_assert (expand >= 0);
				for (x = 0 ; x < self->width ; x++)
				{
					if (self->cols[x].request)
					{
						expand -= self->cols[x].request;
						if (x < self->width - 1)
							expand -= self->col_spacing;
					}
				}
				lisys_assert (expand >= 0);
				expand /= self->col_expand;
			}
			else
				expand = 0;

			/* Set horizontal allocations. */
			start = self->margin_left;
			for (x = 0 ; x < self->width ; x++)
			{
				self->cols[x].start = start;
				self->cols[x].allocation = self->cols[x].request;
				if (self->cols[x].expand)
					self->cols[x].allocation += expand;
				start += self->cols[x].allocation;
				if (self->cols[x].request)
					start += self->col_spacing;
			}
		}

		if (flags & PRIVATE_REBUILD_VERT)
		{
			/* Get vertical expansion. */
			if (self->row_expand > 0)
			{
				expand = rect.height - self->margin_top - self->margin_bottom;
				lisys_assert (expand >= 0);
				for (y = 0 ; y < self->height ; y++)
				{
					if (self->rows[y].request)
					{
						expand -= self->rows[y].request;
						if (y < self->height - 1)
							expand -= self->row_spacing;
					}
				}
				lisys_assert (expand >= 0);
				expand /= self->row_expand;
			}
			else
				expand = 0;

			/* Set vertical allocations. */
			start = self->margin_top;
			for (y = 0 ; y < self->height ; y++)
			{
				self->rows[y].start = start;
				self->rows[y].allocation = self->rows[y].request;
				if (self->rows[y].expand)
					self->rows[y].allocation += expand;
				start += self->rows[y].allocation;
				if (self->rows[y].request)
					start += self->row_spacing;
			}
		}
	}

	if (flags & PRIVATE_REBUILD_CHILDREN)
	{
		liwdg_widget_get_allocation (self, &rect);

		/* Set positions of widgets. */
		for (x = 0 ; x < self->width ; x++)
		{
			for (y = 0 ; y < self->height ; y++)
			{
				child = self->cells[x + y * self->width].child;
				if (child != NULL)
				{
					liwdg_widget_set_allocation (child,
						rect.x + self->cols[x].start,
						rect.y + self->rows[y].start,
						self->cols[x].allocation,
						self->rows[y].allocation);
				}
			}
		}
		for (child = self->children ; child != NULL ; child = child->next)
			liwdg_widget_child_request (self, child);
	}

	self->rebuilding = 0;
}

static int private_get_row_size (
	LIWdgWidget* self,
	int          y)
{
	int x;
	int height;
	LIWdgSize size;
	LIWdgWidget* child;

	height = 0;
	for (x = 0 ; x < self->width ; x++)
	{
		child = self->cells[x + y * self->width].child;
		if (child != NULL && child->visible)
		{
			liwdg_widget_get_request (child, &size);
			if (height < size.height)
				height = size.height;
		}
	}
	return height;
}

static LIWdgWidget* private_table_child_at (
	LIWdgWidget* self,
	int          pixx,
	int          pixy)
{
	int x;
	int y;
	LIWdgRect rect;

	liwdg_widget_get_allocation (self, &rect);
	pixx -= rect.x;
	pixy -= rect.y;

	/* Get column. */
	for (x = 0 ; x < self->width ; x++)
	{
		if (pixx >= self->cols[x].start + self->cols[x].allocation)
			continue;
		if (pixx >= self->cols[x].start)
			break;
		return NULL;
	}
	if (x == self->width)
		return NULL;

	/* Get row. */
	for (y = 0 ; y < self->height ; y++)
	{
		if (pixy >= self->rows[y].start + self->rows[y].allocation)
			continue;
		if (pixy >= self->rows[y].start)
			break;
		return NULL;
	}
	if (y == self->height)
		return NULL;

	/* Return the child. */
	return self->cells[x + y * self->width].child;
}

/** @} */
/** @} */
