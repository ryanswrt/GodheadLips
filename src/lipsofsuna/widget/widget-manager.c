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

/**
 * \addtogroup LIWdg Widget
 * @{
 * \addtogroup LIWdgManager Manager
 * @{
 */

#include "lipsofsuna/system.h"
#include "widget.h"
#include "widget-manager.h"

enum
{
	LIWDG_MATCH_BOTTOM,
	LIWDG_MATCH_BOTTOMLEFT,
	LIWDG_MATCH_BOTTOMRIGHT,
	LIWDG_MATCH_INSIDE,
	LIWDG_MATCH_LEFT,
	LIWDG_MATCH_RIGHT,
	LIWDG_MATCH_TITLEBAR,
	LIWDG_MATCH_TOP,
	LIWDG_MATCH_TOPLEFT,
	LIWDG_MATCH_TOPRIGHT,
};

static void private_attach_window (
	LIWdgManager* self,
	LIWdgWidget*  widget);

static void private_detach_window (
	LIWdgManager* self,
	LIWdgWidget*  widget);

static LIWdgWidget* private_find_window (
	LIWdgManager* self,
	int           x,
	int           y,
	int*          match);

static void private_resize_window (
	LIWdgManager* self,
	LIWdgWidget*  window);

static int private_load_config (
	LIWdgManager* self,
	LIPthPaths*   paths);

/*****************************************************************************/

/**
 * \brief Creates a new widget manager.
 * \param render Renderer.
 * \param callbacks Callback manager.
 * \param paths Paths used for loading data files.
 * \return New widget manager or NULL.
 */
LIWdgManager* liwdg_manager_new (
	LIRenRender*    render,
	LICalCallbacks* callbacks,
	LIPthPaths*     paths)
{
	LIWdgManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIWdgManager));
	if (self == NULL)
		return NULL;
	self->callbacks = callbacks;
	self->width = 640;
	self->height = 480;
	self->render = render;
	self->projection = limat_matrix_identity ();

	/* Initialize widget dictionary. */
	self->widgets.all = lialg_ptrdic_new ();
	if (self->widgets.all == NULL)
	{
		liwdg_manager_free (self);
		return NULL;
	}

	/* Load config and resources. */
	if (!private_load_config (self, paths))
	{
		liwdg_manager_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Freed the widget manager.
 * \param self Widget manager.
 */
void liwdg_manager_free (
	LIWdgManager* self)
{
	lisys_assert (self->dialogs.bottom == NULL);
	lisys_assert (self->dialogs.top == NULL);

	if (self->widgets.all != NULL)
		lialg_ptrdic_free (self->widgets.all);
	if (self->styles != NULL)
		liwdg_styles_free (self->styles);
	lisys_free (self);
}

/**
 * \brief Stores allocated widgets to pointers.
 * \param self Widget manager.
 * \param ... List of pointer-to-pointer and pointer pairs terminated by NULL.
 */
int liwdg_manager_alloc_widgets (
	LIWdgManager* self,
	              ...)
{
	int fail;
	va_list args;
	LIWdgWidget* ptr;
	LIWdgWidget** pptr;

	/* Check for errors. */
	fail = 0;
	va_start (args, self);
	while (1)
	{
		pptr = va_arg (args, LIWdgWidget**);
		if (pptr == NULL)
			break;
		ptr = va_arg (args, LIWdgWidget*);
		if (ptr == NULL)
		{
			fail = 1;
			break;
		}
	}
	va_end (args);
	if (fail)
	{
		va_start (args, self);
		while (1)
		{
			pptr = va_arg (args, LIWdgWidget**);
			if (pptr == NULL)
				break;
			ptr = va_arg (args, LIWdgWidget*);
			if (ptr != NULL)
				liwdg_widget_free (ptr);
		}
		va_end (args);
		return 0;
	}

	/* Copy widgets to target variables. */
	va_start (args, self);
	while (1)
	{
		pptr = va_arg (args, LIWdgWidget**);
		if (pptr == NULL)
			break;
		ptr = va_arg (args, LIWdgWidget*);
		lisys_assert (ptr != NULL);
		*pptr = ptr;
	}
	va_end (args);

	return 1;
}

LIFntFont* liwdg_manager_find_font (
	LIWdgManager* self,
	const char*   name)
{
	return lialg_strdic_find (self->styles->fonts, name);
}

LIRenImage* liwdg_manager_find_image (
	LIWdgManager* self,
	const char*   name)
{
	LIRenImage* image;

	image = liren_render_find_image (self->render, name);
	if (image == NULL)
	{
		liren_render_load_image (self->render, name);
		image = liren_render_find_image (self->render, name);
	}

	return image;
}

/**
 * \brief Finds a widget by screen position.
 * \param self Widget manager.
 * \param x Screen X coordinate.
 * \param y Screen Y coordinate.
 * \return Widget or NULL.
 */
LIWdgWidget* liwdg_manager_find_widget_by_point (
	LIWdgManager* self,
	int           x,
	int           y)
{
	int match;
	LIWdgWidget* widget;
	LIWdgWidget* child;

	/* Find window. */
	widget = private_find_window (self, x, y, &match);
	if (widget == NULL)
		return NULL;

	/* Find widget. */
	while (1)
	{
		child = liwdg_widget_child_at (widget, x, y);
		if (child == NULL)
			break;
		widget = child;
	}

	return widget;
}

/**
 * \brief Finds a window by screen position.
 *
 * The root widget is considered a dialog widget as well and
 * is returned if no other widget matched.
 *
 * \param self Widget manager.
 * \param x Screen X coordinate.
 * \param y Screen Y coordinate.
 * \return Widget or NULL.
 */
LIWdgWidget* liwdg_manager_find_window_by_point (
	LIWdgManager* self,
	int           x,
	int           y)
{
	int match;

	return private_find_window (self, x, y, &match);
}

int liwdg_manager_insert_window (
	LIWdgManager* self,
	LIWdgWidget*  widget)
{
	LIWdgSize size;

	liwdg_widget_detach (widget);
	widget->floating = 1;
	private_attach_window (self, widget);

	liwdg_widget_get_request (widget, &size);
	size.width = (self->width - size.width) / 2;
	size.height = (self->height - size.height) / 2;
	liwdg_widget_move (widget, size.width, size.height);
	private_resize_window (self, widget);

	return 1;
}

/**
 * \brief Reloads all fonts.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads all fonts that were lost when the context was erased.
 *
 * \param self Renderer.
 * \param pass Reload pass.
 */
void liwdg_manager_reload (
	LIWdgManager* self,
	int           pass)
{
	LIAlgStrdicIter iter;
	LIAlgPtrdicIter iter1;

	LIALG_STRDIC_FOREACH (iter, self->styles->fonts)
		lifnt_font_reload (iter.value, pass);
	LIALG_PTRDIC_FOREACH (iter1, self->widgets.all)
		liwdg_widget_reload (iter1.value, pass);
}

int liwdg_manager_remove_window (
	LIWdgManager* self,
	LIWdgWidget*  widget)
{
	lisys_assert (widget->floating);
	lisys_assert (widget->prev != NULL || widget == self->dialogs.top);
	lisys_assert (widget->next != NULL || widget == self->dialogs.bottom);

	/* Make sure that the update loop doesn't break. */
	if (self->widgets.iter == widget)
		self->widgets.iter = widget->next;

	/* Remove from stack. */
	private_detach_window (self, widget);
	widget->floating = 0;

	return 1;
}

void liwdg_manager_render (
	LIWdgManager* self)
{
	LIWdgWidget* widget;

	/* Find the widget shader. */
	self->shader = liren_render_find_shader (self->render, "widget");
	if (self->shader == NULL)
		return;

	/* Setup viewport. */
	glViewport (0, 0, self->width, self->height);

	/* Render widgets. */
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	for (widget = self->dialogs.bottom ; widget != NULL ; widget = widget->prev)
	{
		if (liwdg_widget_get_visible (widget))
			liwdg_widget_draw (widget);
	}
}

void liwdg_manager_update (
	LIWdgManager* self,
	float         secs)
{
	int x;
	int y;
	int cx;
	int cy;
	int buttons;
	LIWdgWidget* widget;

	if (self->widgets.grab != NULL)
	{
		cx = self->width / 2;
		cy = self->height / 2;
		buttons = SDL_GetMouseState (&x, &y);
		if (x != cx || y != cy)
		{
			SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouse (cx, cy);
			SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
		}
	}
	for (widget = self->dialogs.top ; widget != NULL ; widget = self->widgets.iter)
	{
		self->widgets.iter = widget->next;
		private_resize_window (self, widget);
	}
}

/**
 * \brief Gets the projection matrix used for rendering widgets.
 * \param self Widget manager.
 * \param matrix Return location for the projection matrix.
 */
void liwdg_manager_get_projection (
	LIWdgManager* self,
	LIMatMatrix*  matrix)
{
	*matrix = self->projection;
}

/**
 * \brief Gets the screen size.
 * \param self Widget manager.
 * \param width Return location for the width or NULL.
 * \param height Return location for the height or NULL.
 */
void liwdg_manager_get_size (
	LIWdgManager* self,
	int*          width,
	int*          height)
{
	if (width != NULL)
		*width = self->width;
	if (height != NULL)
		*height = self->height;
}

/**
 * \brief Sets the root window size.
 * \param self Widget manager.
 * \param width Width in pixels.
 * \param height Height in pixels.
 */
void liwdg_manager_set_size (
	LIWdgManager* self,
	int           width,
	int           height)
{
	/* Update projection matrix. */
	self->width = width;
	self->height = height;
	self->projection = limat_matrix_ortho (0.0f, width, height, 0.0f, -100.0f, 100.0f);
}

/*****************************************************************************/

static void private_attach_window (
	LIWdgManager* self,
	LIWdgWidget*  widget)
{
	widget->prev = NULL;
	widget->next = self->dialogs.top;
	if (self->dialogs.top != NULL)
		self->dialogs.top->prev = widget;
	else
		self->dialogs.bottom = widget;
	self->dialogs.top = widget;
}

static void private_detach_window (
	LIWdgManager* self,
	LIWdgWidget*  widget)
{
	if (widget->next != NULL)
		widget->next->prev = widget->prev;
	else
		self->dialogs.bottom = widget->prev;
	if (widget->prev != NULL)
		widget->prev->next = widget->next;
	else
		self->dialogs.top = widget->next;
}

static LIWdgWidget* private_find_window (
	LIWdgManager* self,
	int           x,
	int           y,
	int*          match)
{
	LIWdgRect rect;
	LIWdgWidget* widget;

	for (widget = self->dialogs.top ; widget != NULL ; widget = widget->next)
	{
		lisys_assert (liwdg_widget_get_visible (widget));
		liwdg_widget_get_allocation (widget, &rect);

		if (rect.x <= x && x < rect.x + rect.width &&
			rect.y <= y && y < rect.y + rect.height)
		{
			*match = LIWDG_MATCH_INSIDE;
			return widget;
		}
	}

	return NULL;
}

static void private_resize_window (
	LIWdgManager* self,
	LIWdgWidget*  window)
{
	LIWdgRect rect;
	LIWdgSize size;

	if (window->fullscreen)
	{
		size.width = self->width;
		size.height = self->height;
		liwdg_widget_get_allocation (window, &rect);
		if (rect.x != 0 || rect.y != 0 || rect.width != size.width || rect.height != size.height)
			liwdg_widget_set_allocation (window, 0, 0, size.width, size.height);
	}
	else
	{
		liwdg_widget_get_request (window, &size);
		liwdg_widget_get_allocation (window, &rect);
		if (rect.width != size.width || rect.height != size.height)
			liwdg_widget_set_allocation (window, rect.x, rect.y, size.width, size.height);
	}
}

static int private_load_config (
	LIWdgManager* self,
	LIPthPaths*   paths)
{
	self->styles = liwdg_styles_new (self, paths);
	if (self->styles == NULL)
		return 0;

	return 1;
}

/** @} */
/** @} */
