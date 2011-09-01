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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrPacket Packet
 * @{
 */

#include "lipsofsuna/archive.h"
#include "lipsofsuna/script.h"
#include "lipsofsuna/system.h"
#include "script-private.h"

static void private_read (
	LIArcPacket* self,
	LIScrArgs*   args);

static void private_write (
	LIArcPacket* self,
	LIScrArgs*   args,
	int          start);

/*****************************************************************************/

static void Packet_new (LIScrArgs* args)
{
	int type = 0;
	LIArcPacket* self;
	LIScrData* data;

	liscr_args_geti_int (args, 0, &type);

	/* Allocate the packet. */
	self = liarc_packet_new_writable (type);
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LISCR_SCRIPT_PACKET, liarc_packet_free);
	if (data == NULL)
	{
		liarc_packet_free (self);
		return;
	}
	liscr_args_seti_stack (args);

	/* Write content. */
	private_write (data->data, args, 1);
}

static void Packet_read (LIScrArgs* args)
{
	LIArcPacket* self;

	self = args->self;
	if (self->reader != NULL)
	{
		self->reader->pos = 1;
		private_read (self, args);
	}
}

static void Packet_resume (LIScrArgs* args)
{
	LIArcPacket* self;

	self = args->self;
	if (self->reader != NULL)
		private_read (self, args);
}

static void Packet_write (LIScrArgs* args)
{
	LIArcPacket* self;

	self = args->self;
	if (self->writer != NULL)
		private_write (self, args, 0);
}

static void Packet_get_size (LIScrArgs* args)
{
	LIArcPacket* self;

	self = args->self;
	if (self->reader != NULL)
		liscr_args_seti_int (args, self->reader->length);
	else
		liscr_args_seti_int (args, liarc_writer_get_length (self->writer));
}

static void Packet_get_type (LIScrArgs* args)
{
	LIArcPacket* self;

	self = args->self;
	if (self->reader != NULL)
		liscr_args_seti_int (args, ((uint8_t*) self->reader->buffer)[0]);
	else
		liscr_args_seti_int (args, ((uint8_t*) liarc_writer_get_buffer (self->writer))[0]);
}
static void Packet_set_type (LIScrArgs* args)
{
	int value;
	uint8_t* buffer;
	LIArcPacket* self;

	self = args->self;
	if (self->writer != NULL)
	{
		if (liscr_args_geti_int (args, 0, &value))
		{
			buffer = (uint8_t*) liarc_writer_get_buffer (self->writer);
			*buffer = value;
		}
	}
}

/*****************************************************************************/

void liscr_script_packet (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PACKET, "packet_new", Packet_new);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_PACKET, "packet_read", Packet_read);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_PACKET, "packet_resume", Packet_resume);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_PACKET, "packet_write", Packet_write);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_PACKET, "packet_get_size", Packet_get_size);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_PACKET, "packet_get_type", Packet_get_type);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_PACKET, "packet_set_type", Packet_set_type);
}

/*****************************************************************************/

static void private_read (
	LIArcPacket* self,
	LIScrArgs*   args)
{
	int i;
	int p;
	int ok = 1;
	const char* type;
	union
	{
		int8_t i8;
		int16_t i16;
		int32_t i32;
		uint8_t u8;
		uint16_t u16;
		uint32_t u32;
		float flt;
		char* str;
	} tmp;

	/* Check for valid format. */
	p = self->reader->pos;
	for (i = 0 ; liscr_args_geti_string (args, i, &type) ; i++)
	{
		if (!strcmp (type, "bool"))
			ok = liarc_reader_get_int8 (self->reader, &tmp.i8);
		else if (!strcmp (type, "float"))
			ok = liarc_reader_get_float (self->reader, &tmp.flt);
		else if (!strcmp (type, "int8"))
			ok = liarc_reader_get_int8 (self->reader, &tmp.i8);
		else if (!strcmp (type, "int16"))
			ok = liarc_reader_get_int16 (self->reader, &tmp.i16);
		else if (!strcmp (type, "int32"))
			ok = liarc_reader_get_int32 (self->reader, &tmp.i32);
		else if (!strcmp (type, "string"))
		{
			if (liarc_reader_get_text (self->reader, "", &tmp.str))
			{
				ok = lisys_utf8_get_valid (tmp.str);
				lisys_free (tmp.str);
			}
			else
				ok = 0;
		}
		else if (!strcmp (type, "uint8"))
			ok = liarc_reader_get_uint8 (self->reader, &tmp.u8);
		else if (!strcmp (type, "uint16"))
			ok = liarc_reader_get_uint16 (self->reader, &tmp.u16);
		else if (!strcmp (type, "uint32"))
			ok = liarc_reader_get_uint32 (self->reader, &tmp.u32);
		else
			ok = 0;
		if (!ok)
			return;
	}
	liscr_args_seti_bool (args, 1);
	self->reader->pos = p;

	/* Read and return contents. */
	for (i = 0 ; liscr_args_geti_string (args, i, &type) ; i++)
	{
		liscr_args_geti_string (args, i, &type);
		if (!strcmp (type, "bool"))
		{
			liarc_reader_get_uint8 (self->reader, &tmp.u8);
			liscr_args_seti_bool (args, tmp.u8);
		}
		else if (!strcmp (type, "float"))
		{
			liarc_reader_get_float (self->reader, &tmp.flt);
			liscr_args_seti_float (args, tmp.flt);
		}
		else if (!strcmp (type, "int8"))
		{
			liarc_reader_get_int8 (self->reader, &tmp.i8);
			liscr_args_seti_int (args, tmp.i8);
		}
		else if (!strcmp (type, "int16"))
		{
			liarc_reader_get_int16 (self->reader, &tmp.i16);
			liscr_args_seti_int (args, tmp.i16);
		}
		else if (!strcmp (type, "int32"))
		{
			liarc_reader_get_int32 (self->reader, &tmp.i32);
			liscr_args_seti_float (args, tmp.i32);
		}
		else if (!strcmp (type, "string"))
		{
			if (liarc_reader_get_text (self->reader, "", &tmp.str))
			{
				liscr_args_seti_string (args, tmp.str);
				lisys_free (tmp.str);
			}
		}
		else if (!strcmp (type, "uint8"))
		{
			liarc_reader_get_uint8 (self->reader, &tmp.u8);
			liscr_args_seti_int (args, tmp.u8);
		}
		else if (!strcmp (type, "uint16"))
		{
			liarc_reader_get_uint16 (self->reader, &tmp.u16);
			liscr_args_seti_int (args, tmp.u16);
		}
		else if (!strcmp (type, "uint32"))
		{
			liarc_reader_get_uint32 (self->reader, &tmp.u32);
			liscr_args_seti_float (args, tmp.u32);
		}
	}
}

static void private_write (
	LIArcPacket* self,
	LIScrArgs*   args,
	int          start)
{
	int i;
	int bol;
	float flt;
	const char* str;
	const char* type;

	for (i = start ; liscr_args_geti_string (args, i, &type) ; i++)
	{
		i++;
		if (!strcmp (type, "bool"))
		{
			bol = 0;
			liscr_args_geti_bool_convert (args, i, &bol);
			liarc_writer_append_uint8 (self->writer, bol);
		}
		else if (!strcmp (type, "float"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_float (self->writer, flt);
		}
		else if (!strcmp (type, "int8"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int8 (self->writer, (int8_t) flt);
		}
		else if (!strcmp (type, "int16"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int16 (self->writer, (int16_t) flt);
		}
		else if (!strcmp (type, "int32"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int32 (self->writer, (int32_t) flt);
		}
		else if (!strcmp (type, "string"))
		{
			str = "";
			liscr_args_geti_string (args, i, &str);
			liarc_writer_append_string (self->writer, str);
			liarc_writer_append_nul (self->writer);
		}
		else if (!strcmp (type, "uint8"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int8 (self->writer, (uint8_t) flt);
		}
		else if (!strcmp (type, "uint16"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int16 (self->writer, (uint16_t) flt);
		}
		else if (!strcmp (type, "uint32"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int32 (self->writer, (uint32_t) flt);
		}
	}
}

/** @} */
/** @} */
