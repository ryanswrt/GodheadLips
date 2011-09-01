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
 * \addtogroup LISnd Sound 
 * @{
 * \addtogroup LISndSample Sample
 * @{
 */

#ifndef LI_DISABLE_SOUND

#include <stream_decoder.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "lipsofsuna/archive.h"
#include "lipsofsuna/system.h"
#include "sound-sample.h"

static void private_flac_error_callback (
	const FLAC__StreamDecoder*     decoder,
	FLAC__StreamDecoderErrorStatus status,
	void*                          data);

static FLAC__StreamDecoderWriteStatus private_flac_stream_callback (
	const FLAC__StreamDecoder* decoder,
	const FLAC__Frame*         frame,
	const FLAC__int32* const   buffer[],
	void*                      data);

static int private_load_flac (
	LISndSample* self,
	const char*  file);

static int private_load_vorbis (
	LISndSample* self,
	const char*  file);

static void private_load_raw (
	LISndSample* self,
	ALuint       format,
	void*        buffer,
	int          len,
	int          freq);

static void private_worker_thread (
	LISysAsyncCall* call,
	void*           data);

/*****************************************************************************/

/**
 * \brief Creates a new sample.
 *
 * The sample is loaded from an Ogg Vorbis or Flac file.
 *
 * \param system Sound system.
 * \param file File name.
 * \return New sample or NULL.
 */
LISndSample* lisnd_sample_new (
	LISndSystem* system,
	const char*  file)
{
	LISndSample* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LISndSample));
	if (self == NULL)
		return NULL;

	/* Store the filename. */
	self->file = lisys_string_dup (file);
	if (self->file == NULL)
	{
		lisnd_sample_free (self);
		return NULL;
	}

	/* Allocate a buffer. */
	alGenBuffers (2, self->buffers);
	if (alGetError() != AL_NO_ERROR)
	{
		lisnd_sample_free (self);
		return NULL;
	}

	/* Start loading the sample. */
	self->worker = lisys_async_call_new (private_worker_thread, NULL, self);
	if (self->worker == NULL)
	{
		lisnd_sample_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the sample.
 * \param self Sample.
 */
void lisnd_sample_free (
	LISndSample* self)
{
	/* Stop the worker thread. */
	if (self->worker != NULL)
	{
		lisys_async_call_stop (self->worker);
		lisys_async_call_free (self->worker);
	}

	alDeleteBuffers (2, self->buffers);
	lisys_free (self->file);
	lisys_free (self);
}

/*****************************************************************************/

static void private_flac_error_callback (
	const FLAC__StreamDecoder*     decoder,
	FLAC__StreamDecoderErrorStatus status,
	void*                          data)
{
}

static FLAC__StreamDecoderWriteStatus private_flac_stream_callback (
	const FLAC__StreamDecoder* decoder,
	const FLAC__Frame*         frame,
	const FLAC__int32* const   buffer[],
	void*                      data)
{
	FLAC__uint32 i;
	FLAC__uint32 j;
	FLAC__uint8 sample8;
	FLAC__uint16 sample16;
	LIArcWriter* writer = data;

	if (frame->header.bits_per_sample == 8)
	{
		for (j = 0 ; j < frame->header.blocksize ; j++)
		for (i = 0 ; i < frame->header.channels ; i++)
		{
			sample8 = buffer[i][j];
			if (!liarc_writer_append_raw (writer, &sample8, 1))
				return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
		}
	}
	else if (frame->header.bits_per_sample == 16)
	{
		for (j = 0 ; j < frame->header.blocksize ; j++)
		for (i = 0 ; i < frame->header.channels ; i++)
		{
			sample16 = buffer[i][j];
			if (!liarc_writer_append_raw (writer, &sample16, 2))
				return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
		}
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static int private_load_flac (
	LISndSample* self,
	const char*  file)
{
	void* buffer;
	ALenum format;
	FLAC__uint32 bps;
	FLAC__uint32 channels;
	FLAC__uint32 length;
	FLAC__uint32 rate;
	FLAC__StreamDecoder* decoder;
	FLAC__StreamDecoderInitStatus status;
	LIArcWriter* writer;

	/* Create decoder. */
	decoder = FLAC__stream_decoder_new ();
	if (decoder == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	/* Allocate temporary storage. */
	writer = liarc_writer_new ();
	if (writer == NULL)
	{
		FLAC__stream_decoder_delete (decoder);
		return 0;
	}

	/* Decode file. */
	status = FLAC__stream_decoder_init_file (decoder, file,
		private_flac_stream_callback, NULL, private_flac_error_callback, writer);
	if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
	{
		lisys_error_set (EINVAL, "FLAC decoder cannot open file `%s'", file);
		liarc_writer_free (writer);
		FLAC__stream_decoder_delete (decoder);
		return 0;
	}
	if (!FLAC__stream_decoder_process_until_end_of_stream (decoder))
	{
		lisys_error_set (EINVAL, "FLAC decoder failed to process file `%s'", file);
		liarc_writer_free (writer);
		FLAC__stream_decoder_finish (decoder);
		FLAC__stream_decoder_delete (decoder);
		return 0;
	}

	/* Stream format. */
	bps = FLAC__stream_decoder_get_bits_per_sample (decoder);
	channels = FLAC__stream_decoder_get_channels (decoder);
	length = liarc_writer_get_length (writer);
	rate = FLAC__stream_decoder_get_sample_rate (decoder);

	/* Upload format. */
	if (channels == 1 && bps == 8)
		format = AL_FORMAT_MONO8;
	else if (channels == 1 && bps == 16)
		format = AL_FORMAT_MONO16;
	else if (channels == 1 && bps == 8)
		format = AL_FORMAT_STEREO8;
	else if (channels == 1 && bps == 16)
		format = AL_FORMAT_STEREO16;
	else
	{
		lisys_error_set (EINVAL, "FLAC stream format unsupported in file `%s'", file);
		liarc_writer_free (writer);
		FLAC__stream_decoder_finish (decoder);
		FLAC__stream_decoder_delete (decoder);
		return 0;
	}

	/* Upload to OpenAL. */
	buffer = lisys_calloc (length, sizeof (uint8_t));
	if (buffer != NULL)
	{
		memcpy (buffer, liarc_writer_get_buffer (writer), length);
		private_load_raw (self, format, buffer, length, rate);
		lisys_free (buffer);
	}

	/* Finish reading. */
	liarc_writer_free (writer);
	FLAC__stream_decoder_finish (decoder);
	FLAC__stream_decoder_delete (decoder);

	return 1;
}

static int private_load_vorbis (
	LISndSample* self,
	const char*  file)
{
	int bs = -1;
	int freq;
	long num;
	void* buffer;
	ogg_int64_t pos;
	ogg_int64_t len;
	vorbis_info* info;
	OggVorbis_File vorbis;

	/* Initialize the decoder. */
	num = ov_fopen ((char*) file, &vorbis);
	if (num != 0)
	{
		if (num == OV_EREAD)
			lisys_error_set (EIO, "cannot open `%s'", file);
		else
			lisys_error_set (EINVAL, "cannot read `%s'", file);
		return 0;
	}

	/* Get stream information. */
	info = ov_info (&vorbis, -1);
	freq = info->rate;
	len = 2 * info->channels * ov_pcm_total (&vorbis, -1);
	pos = 0;

	/* Allocate the output buffer. */
	buffer = lisys_malloc (len);
	if (buffer == NULL)
	{
		ov_clear (&vorbis);
		return 0;
	}

	/* Decode the stream. */
	while (1)
	{
		num = ov_read (&vorbis, buffer + pos, len - pos, 0, 2, 1, &bs);
		if (num <= 0)
			break;
		pos += num;
	}

	/* Upload to OpenAL. */
	if (info->channels == 1)
		private_load_raw (self, AL_FORMAT_MONO16, buffer, len, freq);
	else
		private_load_raw (self, AL_FORMAT_STEREO16, buffer, len, freq);
	ov_clear (&vorbis);
	lisys_free (buffer);

	return 1;
}

static void private_load_raw (
	LISndSample* self,
	ALuint       format,
	void*        buffer,
	int          len,
	int          freq)
{
	int i;
	uint8_t* src8;
	uint8_t* dst8;
	uint16_t* src16;
	uint16_t* dst16;

	/* Upload stereo data. */
	if (format == AL_FORMAT_STEREO8 || AL_FORMAT_STEREO16)
	{
		self->stereo = 1;
		alBufferData (self->buffers[1], format, buffer, len, freq);
	}

	/* Convert to mono. */
	if (format == AL_FORMAT_STEREO8)
	{
		src8 = buffer;
		dst8 = buffer;
		for (i = 0 ; i < len ; i += 2)
		{
			*dst8 = src8[(i % 4) != 0];
			dst8++;
			src8 += 2;
		}
		len /= 2;
		format = AL_FORMAT_MONO8;
	}
	else if (format == AL_FORMAT_STEREO16)
	{
		src16 = buffer;
		dst16 = buffer;
		for (i = 0 ; i < len ; i += 4)
		{
			*dst16 = src16[(i % 8) != 0];
			dst16++;
			src16 += 2;
		}
		len /= 2;
		format = AL_FORMAT_MONO16;
	}

	/* Upload mono data. */
	alBufferData (self->buffers[0], format, buffer, len, freq);
}

static void private_worker_thread (
	LISysAsyncCall* call,
	void*           data)
{
	LISndSample* self = data;

	if (lisys_path_check_ext (self->file, "flac"))
	{
		if (!private_load_flac (self, self->file))
			lisys_error_report ();
	}
	else if (lisys_path_check_ext (self->file, "ogg"))
	{
		if (!private_load_vorbis (self, self->file))
			lisys_error_report ();
	}
	self->loaded = 1;
}

#endif

/** @} */
/** @} */

