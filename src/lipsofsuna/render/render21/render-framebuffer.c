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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenFramebuffer21 Framebuffer21
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-framebuffer.h"
#include "render-private.h"

/**
 * \brief Creates a rendering framebuffer.
 * \param render Renderer.
 * \param width Framebuffer width.
 * \param height Framebuffer height.
 * \param samples Number of multisamples.
 * \param hdr Nonzero to use floating point framebuffer to enable HDR.
 * \return New deferred framebuffer or NULL.
 */
LIRenFramebuffer21* liren_framebuffer21_new (
	LIRenRender21* render,
	int            width,
	int            height,
	int            samples,
	int            hdr)
{
	LIRenFramebuffer21* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenFramebuffer21));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->width = width;
	self->height = height;

	/* TODO */

	return self;
}

/**
 * \brief Frees the framebuffer.
 * \param self Framebuffer.
 */
void liren_framebuffer21_free (
	LIRenFramebuffer21* self)
{
	/* TODO */
	lisys_free (self);
}

/**
 * \brief Resizes the framebuffer.
 * \param self Framebuffer.
 * \param width New width.
 * \param height New height.
 * \param samples Number of multisamples.
 * \param hdr Nonzero to use floating point framebuffer to enable HDR.
 * \return Nonzero on success.
 */
int liren_framebuffer21_resize (
	LIRenFramebuffer21* self,
	int                 width,
	int                 height,
	int                 samples,
	int                 hdr)
{
	/* TODO */
	self->width = width;
	self->height = height;
	self->samples = samples;

	return 1;
}

/** @} */
/** @} */

