/*
 * Copyright (C) 2009-2013 Fabián C. Tommasini <fabian@tommasini.com.ar>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 *
 */

#include <string.h>
#include <sys/mman.h>

// RTAI headers
#include <rtai_lxrt.h>
#include <rtai_mbx.h>
#include <rtai_fifos.h>

#include "common.hpp"
#include "rttools.hpp"
#include "player.hpp"

// Player
Player::Player(float gain_factor)
{
	set_gain_factor(gain_factor);
	_gain_factor_tmp = 0.0f;
	_muted = false;
	_running = false;
	_count_empty = 0;
}

Player::~Player()
{
	stop();
}

Player::ptr_t Player::create(float gain_factor)
{
	ptr_t p_tmp(new Player(gain_factor));

	if (!p_tmp->_init())
		p_tmp.reset(); // NULL-like pointer

	return p_tmp;
}

bool Player::_init()
{
	// Setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	parameters.deviceId = _audio_dev.getDefaultOutputDevice();
	parameters.nChannels = N_CHANNELS; // TODO global constant?
	RtAudioFormat format = (sizeof(sample_t) == 8) ? RTAUDIO_FLOAT64
			: RTAUDIO_FLOAT32;
	RtAudio::StreamOptions options;
	options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_NONINTERLEAVED;
	uint buffer_frames = BUFFER_SAMPLES; // TODO global constant?

	try
	{
		_audio_dev.openStream(&parameters, NULL, format, SAMPLE_RATE,
				&buffer_frames, &Player::callback, this, &options);
	}
	catch (RtError &ex)
	{
		DPRINT("%s", ex.what());
		return false;
	}

	return true;
}

bool Player::start()
{
	if (_running)
		return false; // already is running

	_fifo_id = open(RTF_OUT_DEV, O_RDONLY | O_NONBLOCK);

	if (_fifo_id < 0)
	{
		DPRINT("Error opening %s", RTF_OUT_DEV);
		return false;
	}

	try
	{
		_audio_dev.startStream();
	}
	catch (RtError &ex)
	{
		DPRINT("%s", ex.what());
		close(_fifo_id);
		return false;
	}

	_running = true;

	return true;
}

bool Player::stop()
{
	if (!_running)
		return false; // already is not running

	_running = false;

	close(_fifo_id);

	try
	{
		_audio_dev.stopStream();
		_audio_dev.closeStream();
	}
	catch (RtError &ex)
	{
		DPRINT("%s", ex.what());
		close(_fifo_id);
		return false;
	}
	catch (...)
	{
		DPRINT("Rare error...");
	}

	return true;
}

// FIXME TIENE UN DELAY DE UN BLOQUE AL RECIBIR LOS PAQUETES!!!
// static callback function
int Player::callback(void *output_buffer, void *input_buffer,
		uint n_buffer_frames, double stream_time, RtAudioStreamStatus status,
		void *data_pointer)
{
	static ssize_t bytes_to_read = RTF_OUT_BLOCK * sizeof(sample_t);
	Player *player = (Player *) data_pointer;
	register sample_t *samples = (sample_t *) output_buffer;

	if (bytes_to_read == read(player->_fifo_id, samples, bytes_to_read))
	{
		for (uint i = 0; i < RTF_OUT_BLOCK; i++)
			samples[i] = player->_gain_factor * samples[i];
	}
	else
	{
		player->_count_empty++; // count empty readings
		//DPRINT("empty reading %d", op->_count_empty);
	}

	return 0; // 1 for stop
}
