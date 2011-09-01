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
 * \addtogroup LIWdgElement Element
 * @{
 */

#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-element.h"

static const LIRenFormat private_format = { 32, GL_FLOAT, 24, GL_FLOAT, 12, GL_FLOAT, 0 };

static void private_pack_quad (
	LIWdgElement* self,
	float         u0,
	float         v0,
	float         x0,
	float         y0,
	float         u1,
	float         v1,
	float         x1,
	float         y1);

static void private_pack_scaled (
	LIWdgElement* self);

static void private_pack_text (
	LIWdgElement* self);

static void private_pack_tiled (
	LIWdgElement* self);

static void private_pack_verts (
	LIWdgElement*      self,
	const LIRenVertex* verts,
	int                count);

/*****************************************************************************/

LIWdgElement* liwdg_element_new_image (
	LIRenImage*        image,
	const float*       color,
	const int*         dst_clip,
	const int*         dst_pos,
	const int*         dst_size,
	const int*         src_pos,
	const int*         src_tiling,
	float              rotation_angle,
	const LIMatVector* rotation_center)
{
	const float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	LIWdgElement* self;

	self = lisys_calloc (1, sizeof (LIWdgElement));
	if (self == NULL)
		return 0;
	self->image = image;
	if (color != NULL)
		memcpy (self->text_color, color, 4 * sizeof (float));
	else
		memcpy (self->text_color, white, 4 * sizeof (float));
	if (dst_clip != NULL)
	{
		self->dst_clip_enabled = 1;
		memcpy (self->dst_clip, dst_clip, 4 * sizeof (int));
	}
	if (dst_pos != NULL)
		memcpy (self->dst_pos, dst_pos, 2 * sizeof (int));
	if (dst_size != NULL)
		memcpy (self->dst_size, dst_size, 2 * sizeof (int));
	if (src_pos != NULL)
		memcpy (self->src_pos, src_pos, 2 * sizeof (int));
	if (src_tiling != NULL)
	{
		self->src_tiling_enabled = 1;
		memcpy (self->src_tiling, src_tiling, 6 * sizeof (int));
	}
	else
	{
		self->src_tiling[1] = liren_image_get_width (self->image);
		self->src_tiling[4] = liren_image_get_height (self->image);
	}
	self->rotation = rotation_angle;
	if (rotation_center != NULL)
		self->center = *rotation_center;

	return self;
}

LIWdgElement* liwdg_element_new_text (
	LIFntFont*         font,
	const char*        text,
	const int*         dst_clip,
	const int*         dst_pos,
	const int*         dst_size,
	const float*       text_align,
	const float*       text_color,
	float              rotation_angle,
	const LIMatVector* rotation_center)
{
	LIWdgElement* self;

	self = lisys_calloc (1, sizeof (LIWdgElement));
	if (self == NULL)
		return 0;
	self->font = font;
	self->text = lisys_string_dup (text);
	if (self->text == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	if (dst_clip != NULL)
	{
		self->dst_clip_enabled = 1;
		memcpy (self->dst_clip, dst_clip, 4 * sizeof (int));
	}
	if (dst_pos != NULL)
		memcpy (self->dst_pos, dst_pos, 2 * sizeof (int));
	if (dst_size != NULL)
		memcpy (self->dst_size, dst_size, 2 * sizeof (int));
	if (text_align != NULL)
		memcpy (self->text_align, text_align, 2 * sizeof (float));
	if (text_color != NULL)
		memcpy (self->text_color, text_color, 4 * sizeof (float));
	self->rotation = rotation_angle;
	if (rotation_center != NULL)
		self->center = *rotation_center;

	return self;
}

void liwdg_element_free (
	LIWdgElement* self)
{
	if (self->buffer != NULL)
		liren_buffer_free (self->buffer);
	lisys_free (self->vertices.array);
	lisys_free (self->text);
	lisys_free (self);
}

void liwdg_element_paint (
	LIWdgElement*      self,
	LIWdgManager*      manager,
	const LIMatMatrix* matrix)
{
	int scissor[5];
	GLuint texture;

	if (self->buffer != NULL)
	{
		if (self->dst_clip_enabled)
		{
			scissor[0] = self->dst_clip_screen[0];
			scissor[1] = manager->height - self->dst_clip_screen[1] - self->dst_clip_screen[3];
			scissor[2] = self->dst_clip_screen[2];
			scissor[3] = self->dst_clip_screen[3];
		}
		else
		{
			scissor[0] = 0;
			scissor[1] = 0;
			scissor[2] = manager->width;
			scissor[3] = manager->height;
		}
		if (self->image != NULL)
			texture = liren_image_get_handle (self->image);
		else
			texture = self->font->texture;
		liren_render_draw_clipped_buffer (manager->render, manager->shader, matrix,
			&manager->projection, texture, self->text_color, scissor, self->buffer);
	}
}

void liwdg_element_reload (
	LIWdgElement* self,
	LIWdgManager* manager,
	int           pass)
{
	if (!pass)
	{
		if (self->buffer != NULL)
		{
			liren_buffer_free (self->buffer);
			self->buffer = NULL;
		}
	}
	else if (self->vertices.count)
	{
		self->buffer = liren_buffer_new (manager->render, NULL, 0, &private_format,
			 self->vertices.array, self->vertices.count, LIREN_BUFFER_TYPE_STATIC);
	}
}

void liwdg_element_update (
	LIWdgElement*    self,
	LIWdgManager*    manager,
	const LIWdgRect* rect)
{
	/* Free old vertices. */
	if (self->buffer != NULL)
	{
		liren_buffer_free (self->buffer);
		self->buffer = NULL;
	}
	lisys_free (self->vertices.array);
	self->vertices.array = NULL;
	self->vertices.count = 0;
	self->vertices.capacity = 0;

	/* Format vertices. */
	if (self->image != NULL)
	{
		if (self->src_tiling_enabled)
			private_pack_tiled (self);
		else
			private_pack_scaled (self);
	}
	else if (self->text != NULL)
		private_pack_text (self);

	/* Create vertex buffer. */
	if (self->vertices.count)
	{
		self->buffer = liren_buffer_new (manager->render, NULL, 0, &private_format,
			 self->vertices.array, self->vertices.count, LIREN_BUFFER_TYPE_STATIC);
		if (self->buffer == NULL)
			return;
	}

	/* Update scissor rectangle. */
	self->dst_clip_screen[0] = self->dst_clip[0] + rect->x;
	self->dst_clip_screen[1] = self->dst_clip[1] + rect->y;
	self->dst_clip_screen[2] = self->dst_clip[2];
	self->dst_clip_screen[3] = self->dst_clip[3];
}

/*****************************************************************************/

static void private_pack_quad (
	LIWdgElement* self,
	float         u0,
	float         v0,
	float         x0,
	float         y0,
	float         u1,
	float         v1,
	float         x1,
	float         y1)
{
	LIRenVertex vertices[6] =
	{
		{ { x0, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v0 } },
		{ { x1, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v0 } },
		{ { x0, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v1 } },
		{ { x1, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v0 } },
		{ { x0, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v1 } },
		{ { x1, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v1 } }
	};

	private_pack_verts (self, vertices, 6);
}

static void private_pack_scaled (
	LIWdgElement* self)
{
	float center;
	float size;
	float xs;
	float ys;
	float tx[2];
	float ty[2];

	/* Calculate texture coordinates. */
	tx[0] = (float)(self->src_pos[0]) / liren_image_get_width (self->image);
	tx[1] = (float)(self->src_pos[0] + self->src_tiling[1]) / liren_image_get_width (self->image);
	ty[0] = (float)(self->src_pos[1]) / liren_image_get_height (self->image);
	ty[1] = (float)(self->src_pos[1] + self->src_tiling[4]) / liren_image_get_height (self->image);

	/* Calculate pixels per texture unit. */
	xs = tx[1] - tx[0];
	ys = ty[1] - ty[0];
	if (xs < LIMAT_EPSILON || ys < LIMAT_EPSILON)
		return;
	xs = self->dst_size[0] / xs;
	ys = self->dst_size[1] / ys;

	/* Scale and translate to fill widget area. */
	if (ty[1] - ty[0] >= self->dst_size[1] / xs)
	{
		center = 0.5f * (ty[0] + ty[1]);
		size = ty[1] - ty[0];
		ty[0] = center - 0.5f * self->dst_size[1] / xs;
		ty[1] = center + 0.5f * self->dst_size[1] / xs;
	}
	else
	{
		center = 0.5f * (tx[0] + tx[1]);
		size = tx[1] - tx[0];
		tx[0] = center - 0.5f * self->dst_size[0] / ys;
		tx[1] = center + 0.5f * self->dst_size[0] / ys;
	}

	/* Pack fill. */
	private_pack_quad (self, tx[0], ty[0],
		self->dst_pos[0], self->dst_pos[1], tx[1], ty[1],
		self->dst_pos[0] + self->dst_size[0],
		self->dst_pos[0] + self->dst_size[1]);
}

static void private_pack_text (
	LIWdgElement* self)
{
	int i;
	int j;
	int x;
	int y;
	int w;
	int h;
	float* vertex;
	float* vertex_data;
	LIMdlIndex* index_data;
	LIFntLayout* text;
	LIFntLayoutGlyph* glyph;
	LIRenVertex vertices[6];

	/* Layout the text. */
	if (!strlen (self->text))
		return;
	text = lifnt_layout_new ();
	if (text == NULL)
		return;
	lifnt_layout_set_width_limit (text, self->dst_size[0]);
	lifnt_layout_append_string (text, self->font, self->text);
	if (!lifnt_layout_get_vertices (text, &index_data, &vertex_data))
	{
		lifnt_layout_free (text);
		return;
	}

	/* Apply translation. */
	w = lifnt_layout_get_width (text);
	h = lifnt_layout_get_height (text);
	x = self->dst_pos[0] + (int)(self->text_align[0] * (self->dst_size[0] - w));
	y = self->dst_pos[1] + (int)(self->text_align[1] * (self->dst_size[1] - h));
	for (i = 2 ; i < text->n_glyphs * 20 ; i += 5)
	{
		glyph = text->glyphs + i;
		vertex_data[i + 0] += x;
		vertex_data[i + 1] += y;
	}

	/* Pack glyphs. */
	for (i = 0 ; i < text->n_glyphs ; i++)
	{
		glyph = text->glyphs + i;
		for (j = 0 ; j < 6 ; j++)
		{
			vertex = vertex_data + 5 * index_data[6 * i + j];
			memcpy (vertices[j].coord, vertex + 2, 3 * sizeof (float));
			memset (vertices[j].normal, 0, 3 * sizeof (float));
			memcpy (vertices[j].texcoord, vertex + 0, 2 * sizeof (float));
		}
		private_pack_verts (self, vertices, 6);
	}

	lifnt_layout_free (text);
	lisys_free (index_data);
	lisys_free (vertex_data);
}

static void private_pack_tiled (
	LIWdgElement* self)
{
	int px;
	int py;
	int iw;
	int ih;
	float fw;
	float fh;
	float fu;
	float fv;
	float w[3];
	float h[3];
	float tx[4];
	float ty[4];
	LIWdgRect r;

	/* Calculate destination rectangle. */
	r.x = self->dst_pos[0];
	r.y = self->dst_pos[1];
	r.width = self->dst_size[0];
	r.height = self->dst_size[1];

	/* Calculate repeat counts. */
	w[0] = self->src_tiling[0];
	w[1] = LIMAT_MAX (1, self->src_tiling[1]);
	w[2] = self->src_tiling[2];
	h[0] = self->src_tiling[3];
	h[1] = LIMAT_MAX (1, self->src_tiling[4]);
	h[2] = self->src_tiling[5];

	/* Calculate texture coordinates. */
	iw = liren_image_get_width (self->image);
	ih = liren_image_get_height (self->image);
	tx[0] = (float)(self->src_pos[0]) / iw;
	tx[1] = (float)(self->src_pos[0] + self->src_tiling[0]) / iw;
	tx[2] = (float)(self->src_pos[0] + self->src_tiling[0] + self->src_tiling[1]) / iw;
	tx[3] = (float)(self->src_pos[0] + self->src_tiling[0] + self->src_tiling[1] + self->src_tiling[2]) / iw;
	ty[0] = (float)(self->src_pos[1]) / ih;
	ty[1] = (float)(self->src_pos[1] + self->src_tiling[3]) / ih;
	ty[2] = (float)(self->src_pos[1] + self->src_tiling[3] + self->src_tiling[4]) / ih;
	ty[3] = (float)(self->src_pos[1] + self->src_tiling[3] + self->src_tiling[4] + self->src_tiling[5]) / ih;

	/* Pack corners. */
	px = r.x;
	py = r.y;
	private_pack_quad (self,
		tx[0], ty[0], px, py,
		tx[1], ty[1], px + w[0], py + h[0]);
	px = r.x + r.width - w[2] - 1;
	private_pack_quad (self,
		tx[2], ty[0], px, py,
		tx[3], ty[1], px + w[2], py + h[0]);
	py = r.y + r.height - h[2] - 1;
	private_pack_quad (self,
		tx[2], ty[2], px, py,
		tx[3], ty[3], px + w[2], py + h[2]);
	px = r.x;
	private_pack_quad (self,
		tx[0], ty[2], px, py,
		tx[1], ty[3], px + w[0], py + h[2]);

	/* Pack horizontal borders. */
	for (px = r.x + w[0] ; px < r.x + r.width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], r.x + r.width - px - w[2] - 1);
		fu = (tx[2] - tx[1]) * fw / w[1];
		py = r.y;
		private_pack_quad (self,
			tx[1], ty[0], px, py,
			tx[1] + fu, ty[1], px + fw, py + h[0]);
		py = r.y + r.height - h[2] - 1;
		private_pack_quad (self,
			tx[1], ty[2], px, py,
			tx[1] + fu, ty[3], px + fw, py + h[2]);
	}

	/* Pack vertical borders. */
	for (py = r.y + h[0] ; py < r.y + r.height - h[2] ; py += h[1])
	{
		fh = LIMAT_MIN (h[1], r.y + r.height - py - h[2] - 1);
		fv = (ty[2] - ty[1]) * fh / h[1];
		px = r.x;
		private_pack_quad (self,
			tx[0], ty[1], px, py,
			tx[1], ty[1] + fv, px + w[0], py + fh);
		px = r.x + r.width - w[2] - 1;
		private_pack_quad (self,
			tx[2], ty[1], px, py,
			tx[3], ty[1] + fv, px + w[2], py + fh);
	}

	/* Pack fill. */
	for (py = r.y + h[0] ; py < r.y + r.height - h[2] ; py += h[1])
	for (px = r.x + w[0] ; px < r.x + r.width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], r.x + r.width - px - w[2] - 1);
		fh = LIMAT_MIN (h[1], r.y + r.height - py - h[2] - 1);
		fu = (tx[2] - tx[1]) * fw / w[1];
		fv = (ty[2] - ty[1]) * fh / h[1];
		private_pack_quad (self,
			tx[1], ty[1], px, py,
			tx[1] + fu, ty[1] + fv, px + fw, py + fh);
	}
}

static void private_pack_verts (
	LIWdgElement*      self,
	const LIRenVertex* verts,
	int                count)
{
	int i;
	int cap;
	LIMatQuaternion q;
	LIMatVector v;
	LIRenVertex* tmp;

	/* Allocate space for vertices. */
	cap = self->vertices.capacity;
	if (cap < self->vertices.count + count)
	{
		if (!cap)
			cap = 32;
		while (cap < self->vertices.count + count)
			cap <<= 1;
		tmp = lisys_realloc (self->vertices.array, cap * sizeof (LIRenVertex));
		if (tmp == NULL)
			return;
		self->vertices.array = tmp;
		self->vertices.capacity = cap;
	}

	/* Append vertices. */
	memcpy (self->vertices.array + self->vertices.count, verts, count * sizeof (LIRenVertex));
	self->vertices.count += count;

	/* Apply rotation. */
	if (self->rotation != 0.0f)
	{
		v = limat_vector_init (0.0f, 0.0f, -1.0f);
		q = limat_quaternion_rotation (self->rotation, v);
		for (i = self->vertices.count - count ; i < self->vertices.count ; i++)
		{
			tmp = self->vertices.array + i;
			v = limat_vector_init (tmp->coord[0], tmp->coord[1], tmp->coord[2]);
			v = limat_vector_subtract (v, self->center);
			v = limat_quaternion_transform (q, v);
			v = limat_vector_add (v, self->center);
			tmp->coord[0] = v.x;
			tmp->coord[1] = v.y;
			tmp->coord[2] = v.z;
		}
	}
}

/** @} */
/** @} */
