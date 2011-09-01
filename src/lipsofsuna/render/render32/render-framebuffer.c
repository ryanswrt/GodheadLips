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
 * \addtogroup LIRenFramebuffer32 Framebuffer32
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-error.h"
#include "render-framebuffer.h"
#include "render-private.h"

static int private_check (
	LIRenFramebuffer32* self);

static int private_rebuild (
	LIRenFramebuffer32* self,
	int                 width,
	int                 height,
	int                 samples,
	int                 hdr);

/*****************************************************************************/

/**
 * \brief Creates a rendering framebuffer.
 * \param render Renderer.
 * \param width Framebuffer width.
 * \param height Framebuffer height.
 * \param samples Number of multisamples.
 * \param hdr Nonzero to use floating point framebuffer to enable HDR.
 * \return New deferred framebuffer or NULL.
 */
LIRenFramebuffer32* liren_framebuffer32_new (
	LIRenRender32* render,
	int            width,
	int            height,
	int            samples,
	int            hdr)
{
	LIRenFramebuffer32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenFramebuffer32));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->width = width;
	self->height = height;

	/* Create frame buffer object. */
	for ( ; samples > 0 ; samples--)
	{
		if (!liren_framebuffer32_resize (self, width, height, samples, hdr))
			lisys_error_report ();
		else
			break;
	}
	if (samples == 0)
	{
		liren_framebuffer32_free (self);
		return NULL;
	}

	/* Add to dictionary. */
	if (!lialg_ptrdic_insert (render->framebuffers, self, self))
	{
		liren_framebuffer32_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the framebuffer.
 * \param self Framebuffer.
 */
void liren_framebuffer32_free (
	LIRenFramebuffer32* self)
{
	lialg_ptrdic_remove (self->render->framebuffers, self);
	glDeleteFramebuffers (1, &self->render_framebuffer);
	glDeleteFramebuffers (2, self->postproc_framebuffers);
	glDeleteTextures (2, self->render_textures);
	glDeleteTextures (3, self->postproc_textures);
	lisys_free (self);
}

/**
 * \brief Reloads the framebuffer.
 * \param self Framebuffer.
 * \param pass Reload pass.
 */
void liren_framebuffer32_reload (
	LIRenFramebuffer32* self,
	int                 pass)
{
	if (!pass)
	{
		glDeleteFramebuffers (1, &self->render_framebuffer);
		glDeleteFramebuffers (2, self->postproc_framebuffers);
		glDeleteTextures (2, self->render_textures);
		glDeleteTextures (3, self->postproc_textures);
		self->render_framebuffer = 0;
		self->postproc_framebuffers[0] = 0;
		self->postproc_framebuffers[1] = 0;
		self->render_textures[0] = 0;
		self->render_textures[1] = 0;
		self->postproc_textures[0] = 0;
		self->postproc_textures[1] = 0;
		self->postproc_textures[2] = 0;
	}
	else
		private_rebuild (self, self->width, self->height, self->samples, self->hdr);
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
int liren_framebuffer32_resize (
	LIRenFramebuffer32* self,
	int                 width,
	int                 height,
	int                 samples,
	int                 hdr)
{
	int max;
	int request;
	GLint max_color;
	GLint max_depth;

	/* Make sure that the sample count is valid. */
	/* The sample count must not be greater than the maximum sample count.
	   It must also be a power of two and greater than zero. */
	glGetIntegerv (GL_MAX_COLOR_TEXTURE_SAMPLES, &max_color);
	glGetIntegerv (GL_MAX_DEPTH_TEXTURE_SAMPLES, &max_depth);
	request = samples;
	max = LIMAT_MIN (max_color, max_depth);
	for (samples = 1 ; samples < max && samples < request ; samples <<= 1) {}
	samples = LIMAT_MIN (samples, max);

	/* Check if a resize is actually needed. */
	if (self->hdr == hdr && self->width == width && self->height == height && self->samples == samples)
		return 1;

	/* Recreate the framebuffer objects. */
	/* If the driver fails to create an HDR framebuffer, we fall back to a non-HDR
	   one. To avoid retrying in the next frame, the HDR flag is set regardless. */
	if (private_rebuild (self, width, height, samples, hdr) ||
	    private_rebuild (self, width, height, samples, 0))
	{
		self->width = width;
		self->height = height;
		self->samples = samples;
		self->hdr = hdr;
		return 1;
	}

	return 0;
}

/*****************************************************************************/

static int private_check (
	LIRenFramebuffer32* self)
{
	int ret;

	ret = glCheckFramebufferStatus (GL_FRAMEBUFFER);
	if (ret != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (ret)
		{
			case GL_FRAMEBUFFER_UNSUPPORTED:
				lisys_error_set (ENOTSUP, "OpenGL: framebuffer object unsupported");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer attachment");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer draw buffer");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer dimensions");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer formats");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				lisys_error_set (ENOTSUP, "OpenGL: missing framebuffer attachment");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer multisample");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer read buffer");
				break;
			default:
				lisys_error_set (ENOTSUP, "OpenGL: unknown framebuffer creation error");
				break;
		}
		return 0;
	}

	return 1;
}

static int private_rebuild (
	LIRenFramebuffer32* self,
	int                 width,
	int                 height,
	int                 samples,
	int                 hdr)
{
	int i;
	GLenum fmt;
	GLenum error;
	GLuint render_framebuffer;
	GLuint render_textures[2];
	GLuint postproc_framebuffers[2];
	GLuint postproc_textures[3];
	static const GLenum fragdata[] = { GL_COLOR_ATTACHMENT0 };

	/* Choose pixel formats. */
	if (hdr && GLEW_ARB_texture_float)
		fmt = GL_RGBA32F;
	else
		fmt = GL_RGBA;
	error = glGetError ();

	if (samples > 1)
	{
		/* Create multisample render textures. */
		glGenTextures (2, render_textures);
		glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, render_textures[0]);
		glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, samples, fmt, width, height, GL_FALSE);
		glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, render_textures[1]);
		glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, samples,
			GL_DEPTH_COMPONENT24, width, height, GL_FALSE);

		/* Create multisample render framebuffer object. */
		glGenFramebuffers (1, &render_framebuffer);
		glBindFramebuffer (GL_FRAMEBUFFER, render_framebuffer);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D_MULTISAMPLE, render_textures[0], 0);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D_MULTISAMPLE, render_textures[1], 0);
		if (!private_check (self))
		{
			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers (1, &render_framebuffer);
			glDeleteTextures (2, render_textures);
			return 0;
		}
		glDrawBuffers (1, fragdata);
	}
	else
	{
		/* Create render textures. */
		glGenTextures (2, render_textures);
		glBindTexture (GL_TEXTURE_2D, render_textures[0]);
		glTexImage2D (GL_TEXTURE_2D, 0, fmt, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture (GL_TEXTURE_2D, render_textures[1]);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		/* Create render framebuffer object. */
		glGenFramebuffers (1, &render_framebuffer);
		glBindFramebuffer (GL_FRAMEBUFFER, render_framebuffer);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, render_textures[0], 0);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, render_textures[1], 0);
		if (!private_check (self))
		{
			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers (1, &render_framebuffer);
			glDeleteTextures (2, render_textures);
			return 0;
		}
		glDrawBuffers (1, fragdata);
	}

	/* Create postprocessing textures. */
	glGenTextures (3, postproc_textures);
	for (i = 0 ; i < 2 ; i++)
	{
		glBindTexture (GL_TEXTURE_2D, postproc_textures[i]);
		glTexImage2D (GL_TEXTURE_2D, 0, fmt, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glGenerateMipmap (GL_TEXTURE_2D);
	}
	glBindTexture (GL_TEXTURE_2D, postproc_textures[2]);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Create post-processing framebuffer objects. */
	glGenFramebuffers (2, postproc_framebuffers);
	for (i = 0 ; i < 2 ; i++)
	{
		glBindFramebuffer (GL_FRAMEBUFFER, postproc_framebuffers[i]);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, postproc_textures[i], 0);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, postproc_textures[2], 0);
		if (!private_check (self))
		{
			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers (2, postproc_framebuffers);
			glDeleteFramebuffers (1, &render_framebuffer);
			glDeleteTextures (3, postproc_textures);
			glDeleteTextures (2, render_textures);
			return 0;
		}
		glDrawBuffers (1, fragdata);
	}

	/* There's a danger of running out of memory here since the G-buffer
	   can consume a lot of video memory with high multisample settings.
	   If something goes wrong, it's better to revert to the old buffer
	   than to try to use the broken one just created. */
	if (liren_error32_get ())
		return 0;

	/* Accept successful rebuild. */
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers (1, &self->render_framebuffer);
	glDeleteFramebuffers (2, self->postproc_framebuffers);
	glDeleteTextures (2, self->render_textures);
	glDeleteTextures (3, self->postproc_textures);
	self->render_framebuffer = render_framebuffer;
	self->render_textures[0] = render_textures[0];
	self->render_textures[1] = render_textures[1];
	self->postproc_framebuffers[0] = postproc_framebuffers[0];
	self->postproc_framebuffers[1] = postproc_framebuffers[1];
	self->postproc_textures[0] = postproc_textures[0];
	self->postproc_textures[1] = postproc_textures[1];
	self->postproc_textures[2] = postproc_textures[2];

	return 1;
}

/** @} */
/** @} */

