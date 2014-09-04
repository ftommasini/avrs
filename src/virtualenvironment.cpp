/*
 * Copyright (C) 2009-2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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

#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <stk/FileWvOut.h>
#include <stk/Noise.h>

#include "utils/timerrtai.hpp"
#include "dxfreader.hpp"
#include "avrsexception.hpp"
#include "virtualenvironment.hpp"


namespace avrs
{

VirtualEnvironment::VirtualEnvironment(configuration_t::ptr_t cs, TrackerBase::ptr_t tracker)
{
	assert(cs.get() != NULL);
	_config = cs;

	// Tracker
	_tracker = tracker;
	// inexistent orientations
	_prev_tracker_data.ori.az = -500.0f;
	_prev_tracker_data.ori.el = -500.0f;
	// initialize tracker
	_mbx_tracker = rttools::get_mbx(MBX_TRACKER_NAME, MBX_TRACKER_BLOCK * sizeof(trackerdata_t));

	if (!_mbx_tracker)
	{
		ERROR("Cannot init TRACKER mailbox");
		throw AvrsException("Error creating VirtualEnvironment");
	}

	// Sound source
	_sound_source = cs->sound_source;
	assert(_sound_source.get() != NULL);

	// Listener
	_listener = cs->listener;
	assert(_listener.get() != NULL);

	// FDN
	const unsigned int N = 8;  // lines of FDN
	long m[N] = { 601, 691, 773, 839, 919, 997, 1061, 1129 };
	double gA = 1.0; // gain coefficient for the A feedback matrix
	double b[N] = { 1, 1, 1, 1, 1, 1, 1, 1 };
	double c[N] = { 1, -1, 1, -1, 1, -1, 1, -1 };
	double d = 0.0;

	_fdn = Fdn::create(N, gA, b, c, d, m, _config->rt60_0, _config->rt60_pi,
			_config->fdn_b_coeff, _config->fdn_a_coeff);

	// BIR length
	_length_bir = _config->bir_length_samples;
	// resize data vectors
	_render_buffer.left.resize(_length_bir, 0.0f);
	_render_buffer.right.resize(_length_bir, 0.0f);
	_zeros.resize(_length_bir, 0.0f);
	_late_buffer.resize(_length_bir, 0.0f);
	_new_bir = false;

	// create and load HRTF DB
	_hcdb = HrtfCoeffSet::create(_config->hrtf_file);
	_delay.setMaximumDelay(BUFFER_SAMPLES);

	// Room
	_room = boost::make_shared<Room>(cs);
	std::cout << "Room loaded" << std::endl;
	std::cout << "Volume: " << _room->volume() << " - Total area: " << _room->total_area() << std::endl;

	// ISM
	_ism = boost::make_shared<Ism>(cs, _room);

	TimerRtai t;
	t.start();
	_ism->calculate(false);
	t.stop();
	t.print_elapsed_time(millisecond, "ISM");
	_ism->print_summary();

	_outputs.resize(_ism->get_count_visible_vs());  // output per visible VS
	_air_absorption = AirAbsorption::create(_config->air_absorption_file);

	// Late reverberation
	_calc_late_reverberation();
}

VirtualEnvironment::~VirtualEnvironment()
{
	rttools::del_mbx(MBX_TRACKER_NAME);
}

VirtualEnvironment::ptr_t VirtualEnvironment::create(configuration_t::ptr_t cs, TrackerBase::ptr_t tracker)
{
	ptr_t p_tmp(new VirtualEnvironment(cs, tracker));
	return p_tmp;
}


bool VirtualEnvironment::update_listener_orientation()
{
	trackerdata_t tmp_data;

	// receive message from TRACKER mailbox
	// non-blocking
	int val = rt_mbx_receive_if(_mbx_tracker, &tmp_data, sizeof(trackerdata_t));

	if (-EINVAL == val)
	{
		ERROR("Mailbox is invalid");
		return false;
	}

	// check for new data
	if (_tracker_data.timestamp != tmp_data.timestamp)
	{
		if (tmp_data.timestamp != 0)
		{
			_tracker_data = tmp_data;  // save the current tracker data
			_listener->rotate(tmp_data.ori);  // update listener orientation
			//_listener->move(tmp_data.pos.to_point3d());  // update listener position
			_ism->update_vs_orientations(_listener->get_orientation());  // update VS orientations

//			DPRINT("%+1.3f %+1.3f \t %+1.3f %+1.3f",
//					_tracker_data.ori.az,
//					_listener->get_orientation().az,
//					_tracker_data.ori.el,
//					_listener->get_orientation().el);
//			DPRINT("az: %3.1f, el: %3.1f", _listener->get_orientation().az, _listener->get_orientation().el);
		}
	}

	return true;
}

void VirtualEnvironment::renderize()
{
	// check if the listener is moved
	if (!_listener_is_moved())
	{
		_new_bir = false;
		return;
	}

	TimerRtai t;
	unsigned long i, j;
	data_t input;
	data_t image;
	binauraldata_t output(BUFFER_SAMPLES);

//	memcpy(&_render_buffer.left[0], &_late_buffer[0], sample_mix_time() * sizeof(sample_t));
//	memcpy(&_render_buffer.right[0], &_late_buffer[0], sample_mix_time() * sizeof(sample_t));

//	memcpy(&_render_buffer.left[0], &_zeros[0], sample_mix_time() * sizeof(sample_t));
//	memcpy(&_render_buffer.right[0], &_zeros[0], sample_mix_time() * sizeof(sample_t));

	memcpy(&_render_buffer.left[0], &_zeros[0], _length_bir * sizeof(sample_t));
	memcpy(&_render_buffer.right[0], &_zeros[0], _length_bir * sizeof(sample_t));

	// TODO RECORRER SOLO AUDIBLES
	for (Ism::tree_vs_t::iterator it = _ism->tree_vs.begin(); it != _ism->tree_vs.end(); it++)
	{
		VirtualSource::ptr_t vs = *it;

		if (!vs->audible)  	// only for audible VSs
			continue;

//		int n = 64;
//		std::vector<double> x = math::linspace(-PI, PI, n);
		//
		//	for (int i = 0; i < n; i++)
		//		input[i] = math::sinc(x[i]);

#ifdef APPLY_DIRECTIVITY_FILTERING
//		t.start();
		// directivity filtering
		input = _sound_source->get_IR(vs->orientation_ref_listener);
		assert(input.size() <= VS_SAMPLES);  // TODO REVISAR LONGITUD DE EARLY REFLECTIONS
		input.resize(VS_SAMPLES, 0.0f);
//		t.stop();
//		DPRINT("Directivity - time %.3f", t.elapsed_time(microsecond));
#else
		data_t input(32);
		input[0] = 1.0f;  // delta dirac
#endif

#ifdef APPLY_SURFACE_FILTERING
		// surface filtering
		input = _surfaces_filter(input, it);
#endif

#ifdef APPLY_AIR_FILTERING
//		t.start();
		// distance attenuation
		float attenuation_factor = 1.0f / vs->dist_listener;

		for (i = 0; i < input.size(); i++)
			input[i] *= attenuation_factor;
//		t.stop();
//		DPRINT("Distance - time %.3f", t.elapsed_time(microsecond));
#endif

#ifdef APPLY_HRTF_FILTERING
		// HRTF filtering
		output = _hrtf_iir_filter(input, vs->pos_R);
#else
		// Non HRTF filtering
		memcpy(&output.left[0], &input[0], input.size() * sizeof(sample_t));
		memcpy(&output.right[0], &input[0], input.size() * sizeof(sample_t));
#endif

		// Buffer accumulation
//		t.start();
		// calculate the sample from reflectogram where starts this reflection
		unsigned long sample = (unsigned long) round((vs->time_rel_ms * SAMPLE_RATE) / 1000.0f);

		// add filter reflection to reflectogram
		for (i = sample, j = 0; j < output.size(); i++, j++)
		{
			_render_buffer.left[i] += output.left[j];
			_render_buffer.right[i] += output.right[j];
		}

//		t.stop ();
//		DPRINT("Buffer - time %.3f", t.elapsed_time(microsecond));
	}

	// add delay from source to listener
	unsigned long samples_source_listener =
			(unsigned long) ceil((_ism->dist_source_listener() / _config->speed_of_sound) * SAMPLE_RATE);
	Delay delay_l, delay_r;
	delay_l.setDelay(samples_source_listener);
	delay_r.setDelay(samples_source_listener);

	for (i = 0; i < _length_bir; i++)
	{
		_render_buffer.left[i] = delay_l.tick(_render_buffer.left[i]);
		_render_buffer.right[i] = delay_r.tick(_render_buffer.right[i]);
	}

	_new_bir = true;

	// FOR DEBUG!!!
	static long flag = 0;
	flag++;

	if (flag == 1)
	{
//		stk::FileWvOut out1_l("early_l.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
//		stk::FileWvOut out1_r("early_r.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
//
//		for (i = 0; i < sample_mix_time(); i++)
//		{
//			out1_l.tick(_render_buffer.left[i]);
//			out1_r.tick(_render_buffer.right[i]);
//		}

		stk::FileWvOut out2_l("bir_l.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
		stk::FileWvOut out2_r("bir_r.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);

		for (i = 0; i < _length_bir; i++)
		{
			out2_l.tick(_render_buffer.left[i]);
			out2_r.tick(_render_buffer.right[i]);
		}
	}
}

data_t VirtualEnvironment::_surfaces_filter(data_t &input, const Ism::tree_vs_t::iterator node)
{
//	TimerRtai t;
 	data_t values = input;
 	Ism::tree_vs_t::iterator root_it = _ism->root_tree_vs;
 	Ism::tree_vs_t::iterator current_node = node;

	while (current_node != root_it)  // while the current node is not the root node
	{
		VirtualSource::ptr_t vs = *current_node;
		assert(vs.get() != NULL);
		Surface::ptr_t s = vs->surface_ptr;
		assert(s.get() != NULL);

//		t.start();
		//_set coefficients and clear previous filter state
		_filter_surfaces.setCoefficients(s->get_b_filter_coeff(), s->get_a_filter_coeff(), true);

		// filter for the current surface
		for (uint i = 0; i < input.size(); i++)
			values[i] = (sample_t) _filter_surfaces.tick(values[i]);

//		t.stop();
//		DPRINT("surface - time %.3f", t.elapsed_time(microsecond));

		current_node = _ism->tree_vs.parent(current_node);  // get the parent
	}

	return values;
}

// IIR filter for single reflection
binauraldata_t VirtualEnvironment::_hrtf_iir_filter(data_t &input, const point3d_t &vs_pos_R)
{
//	TimerRtai t;
	binauraldata_t output(BUFFER_SAMPLES);
	stk::StkFrames out_l(input.size(), 1);  // one channel
	stk::StkFrames out_r(input.size(), 1);  // one channel


	point3d_t vs_pos_L = vs_pos_R * _listener->get_rotation_matrix().submat(0, 0, 2, 2);
	vs_pos_L.print();
	_hcdb->get_HRTF_coeff(&_hc, vs_pos_L);
//	t.start();
	// get the best-fit HRTF for both ears
//	_hcdb->get_HRTF_coeff(&_hc, ori.az, ori.el);
//	t.stop();
//	DPRINT("Kd-tree - time %.3f", t.elapsed_time(microsecond));

//	t.start();
	_filter_l.setCoefficients(_hc.b_left, _hc.a_left, true);
	_filter_r.setCoefficients(_hc.b_right, _hc.a_right, true);

	// HRTF filtering
	#pragma omp for
	for (uint i = 0; i < input.size(); i++)
	{
		out_l[i] = _filter_l.tick(input[i]);
		out_r[i] = _filter_r.tick(input[i]);
	}

	_delay.clear();

	// ITD
	if (_hc.itd > 0)  // left is delayed
	{
		_delay.setDelay(_hc.itd);
		_delay.tick(out_l);
	}
	else if (_hc.itd < 0)  // right is delayed
	{
		_delay.setDelay((-1) * _hc.itd);  // change the sign
		_delay.tick(out_r);
	}

	#pragma omp for
	for (uint i = 0; i < out_l.size(); i++)
	{
		output.left[i] = (sample_t) out_l[i];
		output.right[i] = (sample_t) out_r[i];
	}

//	t.stop();
//	DPRINT("HRTF - time %.3f", t.elapsed_time(microsecond));

	return output;
}

void VirtualEnvironment::_calc_late_reverberation()
{
	uint i;

	data_t input(_length_bir);
	input[0] = 1.0;  // delta dirac

	std::vector<double> output(_length_bir);  // temporary

	for (i = 0; i < _length_bir; i++)
		output[i] = _fdn->tick(input[i]);

	// mix time (converted to sample)
	unsigned long sample_mix = sample_mix_time();
	double val_mix = output[sample_mix];

	// scaling
	for (i = 0; i< _length_bir; i++)
		output[i] /= val_mix;


#ifdef FDN_SCALING_DISTANCE
	double max_value = 0.0;
	double abs_value;

	for (i = 0; i < _length_bir; i++)
	{
		abs_value = fabs(output[i]);

		if (abs_value > max_value)
			max_value = abs_value;
	}

	for (i = 0; i < _length_bir; i++)
	{
		float d = (i * _config->speed_of_sound) / SAMPLE_RATE;  // calculation of distance travel by sound
		float scaling_factor = (d > 1.0f ? d : 1);
		output[i] /= (max_value * scaling_factor);  // scaling (normalize and 1/r attenuation)
	}

	// attenuation function for early part
	for (i = 0; i < sample_mix; i++)
		_late_buffer[i] = (sample_t) (output[i] * (1.0 - pow(0.01, (i + 1.0) / sample_mix)));

	for (i = sample_mix; i < _length_bir; i++)
		_late_buffer[i] = (sample_t) output[i];
#else
	float dist_mix = (float) (sample_mix * _config->speed_of_sound) / SAMPLE_RATE;
	float scaling_factor = (1 / dist_mix) / fabs(output[sample_mix]);  // for scale properly

	// attenuation function for early part
	for (i = 0; i < sample_mix; i++)
		_late_buffer[i] = (sample_t) (output[i] * (1.0 - pow(0.01, (i + 1.0) / sample_mix)) * scaling_factor);

	// normalization for late part
	for (i = sample_mix; i < _length_bir; i++)
		_late_buffer[i] = (sample_t) (output[i] * scaling_factor);
#endif

	// add late part to render buffer
	#pragma omp for
	for (i = 0; i < _length_bir; i++)
	{
		_render_buffer.left[i] += _late_buffer[i];
		_render_buffer.right[i] += _late_buffer[i];
	}

//	// FOR DEBUG!!!
//	stk::FileWvOut out("late.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
//
//	for (i = 0; i < _length_bir; i++)
//		out.tick(_late_buffer[i]);
}

}  // namespace avrs
