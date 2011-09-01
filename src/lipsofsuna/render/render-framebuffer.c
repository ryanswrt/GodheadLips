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
 * \addtogroup LIRenFramebuffer Framebuffer
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-framebuffer.h"
#include "render-private.h"
#include "render21/render-framebuffer.h"
#include "render32/render-framebuffer.h"

/**
 * \brief Creates a rendering framebuffer.
 * \param render Renderer.
 * \param width Framebuffer width.
 * \param height Framebuffer height.
 * \param samples Number of multisamples.
 * \param hdr Nonzero to use floating point framebuffer to enable HDR.
 * \return New deferred framebuffer or NULL.
 */
LIRenFramebuffer* liren_framebuffer_new (
	LIRenRender* render,
	int          width,
	int          height,
	int          samples,
	int          hdr)
{
	LIRenFramebuffer* self;

	self = lisys_calloc (1, sizeof (LIRenFramebuffer));
	if (self == NULL)
		return NULL;

	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_framebuffer32_new (render->v32, width, height, samples, hdr);
		if (self->v32 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}
	else
	{
		self->v21 = liren_framebuffer21_new (render->v21, width, height, samples, hdr);
		if (self->v21 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	return self;
}

/**
 * \brief Frees the framebuffer.
 * \param self Framebuffer.
 */
void liren_framebuffer_free (
	LIRenFramebuffer* self)
{
	if (self->v32 != NULL)
		liren_framebuffer32_free (self->v32);
	if (self->v21 != NULL)
		liren_framebuffer21_free (self->v21);
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
int liren_framebuffer_resize (
	LIRenFramebuffer* self,
	int               width,
	int               height,
	int               samples,
	int               hdr)
{
	if (self->v32 != NULL)
		return liren_framebuffer32_resize (self->v32, width, height, samples, hdr);
	else
		return liren_framebuffer21_resize (self->v21, width, height, samples, hdr);
}

/** @} */
/** @} */

