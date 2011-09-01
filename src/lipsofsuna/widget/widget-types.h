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

#ifndef __WIDGET_TYPES_H__
#define __WIDGET_TYPES_H__

#define LIWDG_HANDLER(c) ((liwdgHandler)(c))

typedef int (*liwdgHandler)();
typedef int LIWdgWidgetState;
typedef struct _LIWdgElement LIWdgElement;
typedef struct _LIWdgManager LIWdgManager;
typedef struct _LIWdgWidget LIWdgWidget;
typedef struct _LIWdgRect LIWdgRect;
typedef struct _LIWdgSize LIWdgSize;
typedef struct _LIWdgStyle LIWdgStyle;
typedef struct _LIWdgStyles LIWdgStyles;

struct _LIWdgRect
{
	int x;
	int y;
	int width;
	int height;
};

struct _LIWdgSize
{
	int width;
	int height;
};

#endif
