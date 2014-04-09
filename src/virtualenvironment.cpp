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
	_tracker = tracker;
	_sound_source = cs->sound_source;
	assert(_sound_source.get() != NULL);
	_listener = cs->listener;
	assert(_listener.get() != NULL);

	_new_bir = false;

	// Room
	_room = boost::make_shared<Room>(cs);
	_room->load_dxf();

	// ISM
	_ism = boost::make_shared<Ism>(cs, _room);

	// FDN
	const unsigned int N = 8;
	long m[N] = { 601, 691, 773, 839, 919, 997, 1061, 1129 };
	double gA = 1.0; // gain coefficient for the A feedback matrix
	double b[N] = { 1, 1, 1, 1, 1, 1, 1, 1 };
	double c[N] = { 1, -1, 1, -1, 1, -1, 1, -1};
	double d = 0.0;
	double t60_DC = 2.5675; // reverberation time at DC
	double t60_pi = 1.1527; // reverberation time at half the sampling rate
	_fdn = Fdn::create(N, gA, b, c, d, m, t60_DC, t60_pi);

	// inexistent orientations
	_prev_tracker_data.ori.az = -500.0f;
	_prev_tracker_data.ori.el = -500.0f;

	_length_bir = _config->bir_length_samples;

	// resize data vectors
	_render_buffer.left.resize(_length_bir, 0.0f);
	_render_buffer.right.resize(_length_bir, 0.0f);
	_zeros.resize(_length_bir, 0.0f);
	_late_buffer.resize(_length_bir, 0.0f);

	// create and load HRTF DB into program
#ifndef HRTF_IIR
	_hrtfdb = HrtfSet::create(_config->hrtf_file);
	// for left ear
	_hrtf_conv_l = HrtfConvolver::create(N_FFT);
	_hrtf_conv_l->setKernelLength(KERNEL_LENGTH);
	_hrtf_conv_l->setSegmentLength(SEGMENT_LENGTH);
	// for right ear
	_hrtf_conv_r = HrtfConvolver::create(N_FFT);
	_hrtf_conv_r->setKernelLength(KERNEL_LENGTH);
	_hrtf_conv_r->setSegmentLength(SEGMENT_LENGTH);
#else
	_hcdb = HrtfCoeffSet::create(_config->hrtf_file);
	_delay.setMaximumDelay(BUFFER_SAMPLES);
#endif

	_delay_vs_l.setMaximumDelay(BUFFER_SAMPLES);
	_delay_vs_l.setMaximumDelay(BUFFER_SAMPLES);

	// initialize tracker
	_mbx_tracker = rttools::get_mbx(MBX_TRACKER_NAME, MBX_TRACKER_BLOCK * sizeof(trackerdata_t));

	if (!_mbx_tracker)
	{
		ERROR("Cannot init TRACKER mailbox");
		throw AvrsException("Error creating VirtualEnvironment");
	}

//	TimerRtai t;
//	t.start();
	_ism->calculate(false);
//	t.stop();
//	DPRINT("ISM - time %.3f", t.elapsed_time(millisecond));

	_outputs.resize(_ism->get_count_visible_vs());  // output per visible VS
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

#ifndef VSFILTER_THREADS

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
	binauraldata_t output;

	memcpy(&_render_buffer.left[0], &_zeros[0], _config->bir_length_samples * sizeof(sample_t));
	memcpy(&_render_buffer.right[0], &_zeros[0], _config->bir_length_samples * sizeof(sample_t));

	// TODO GUARDAR EL ITERATOR EN CADA VS, ASI SE RECORRE SOLAMENTE LAS VISIBLES
	for (Ism::tree_vs_t::iterator it = _ism->tree_vs.begin(); it != _ism->tree_vs.end(); it++)
	{
		VirtualSource::ptr_t vs = *it;

		if (!vs->audible)  	// only for audible VSs
			continue;

//		t.start();
		// directivity filtering
		input = _sound_source->get_IR(vs->orientation_ref_listener);
		assert(input.size() <= VS_SAMPLES);  // TODO REVISAR LONGITUD DE EARLY REFLECTIONS
		input.resize(VS_SAMPLES, 0.0f);
//		t.stop();
//		DPRINT("Directivity - time %.3f", t.elapsed_time(microsecond));

		// surface filtering
		input = _surfaces_filter(input, it);

//		t.start();
		// distance attenuation
		float attenuation_factor = 1.0f / vs->dist_listener;

		for (i = 0; i < input.size(); i++)
			input[i] *= attenuation_factor;
//		t.stop();
//		DPRINT("Distance - time %.3f", t.elapsed_time(microsecond));

		// HRTF filtering
#ifndef HRTF_IIR
		output = _hrtf_filter(input, vs->orientation_ref_listener);  // HRTF spectrums

#else
		output = _hrtf_iir_filter(input, vs->orientation_ref_listener);
#endif

		// TODO delay menor a una muestra
//		_delay_vs_l.clear();
//		_delay_vs_r.clear();
//		float delay = (vs->time_rel_ms * SAMPLE_RATE) / 1000.0f;
//		DPRINT("delay %f", delay);
//
//		if (delay >= 0.5f)
//		{
//		_delay_vs_l.setDelay(delay);
//		_delay_vs_r.setDelay(delay);
//		unsigned long sample = (unsigned long) round(delay);
//
//		for (i = 0; i < _length_bir; i++)
//		{
//			_render_buffer.left[i] += _delay_vs_l.tick(output.left[i]);
//			_render_buffer.right[i] += _delay_vs_r.tick(output.right[i]);
//		}
//		}
//		else
//		{
//			//
//		}

//		t.start();
		// calculate the sample from reflectogram where starts this reflection
		unsigned long sample = (unsigned long) round((vs->time_rel_ms * SAMPLE_RATE) / 1000.0f);

		// add filter reflection to reflectogram
		for (i = sample, j = 0; j < output.size(); i++, j++)
		{
			//assert(i <= _render_buffer.left.size());
			_render_buffer.left[i] += output.left[j];
			_render_buffer.right[i] += output.right[j];
		}
//		t.stop ();
//		DPRINT("Buffer - time %.3f", t.elapsed_time(microsecond));
	}


//	static long flag = 0;
//	flag++;
//
//	if (flag == 100)
//	{
//	stk::FileWvOut out_l("early_l.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
//	stk::FileWvOut out_r("early_r.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
//
//	for (i = 0; i < _length_bir; i++)
//	{
//		out_l.tick(_render_buffer.left[i]);
//		out_r.tick(_render_buffer.right[i]);
//	}
//	}

//	t.start();
	// add late part to render buffer
	#pragma omp for
	for (i = 0; i < _length_bir; i++)
	{
		_render_buffer.left[i] += _late_buffer[i];
		_render_buffer.right[i] += _late_buffer[i];

	}
//	t.stop();
//	DPRINT("Union - time %.3f", t.elapsed_time(microsecond));

	_new_bir = true;


//	if (flag == 100)
//	{
//	stk::FileWvOut out_l("bir_l.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
//	stk::FileWvOut out_r("bir_r.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
//
//	for (i = 0; i < _length_bir; i++)
//	{
//		out_l.tick(_render_buffer.left[i]);
//		out_r.tick(_render_buffer.right[i]);
//	}
//	}
}

#else

void VirtualEnvironment::renderize()
{
	// check if the listener is moved
	if (!_listener_is_moved())
	{
		_new_bir = false;
		return;
	}

	pthread_t t_id[_vis.size()];
	unsigned int t_index = 0;
	unsigned int vs_sample[_vis.size()];
	threaddata_t td[_vis.size()];
	unsigned int i, j;
	binauraldata_t output;

	memcpy(&_render_buffer.left[0], &_zeros[0], _config->bir_length_samples * sizeof(sample_t));
	memcpy(&_render_buffer.right[0], &_zeros[0], _config->bir_length_samples * sizeof(sample_t));

	// TODO SE PODRÍA MEJORAR SI SE GUARDAR EL ITERATOR EN CADA VS, ASI SE RECORRE SOLAMENTE LAS VISIBLES

	// only for visible VSs
	for (tree_it_t it = _tree.begin(); it != _tree.end(); it++)
	{
		virtualsource_t *vs = *it;

		if (!vs->visible)
			continue;

		td[t_index].ve = this;
		td[t_index].it = it;
		td[t_index].index = t_index;
		vs_sample[t_index] = (unsigned int) round((vs->time_rel_ms * SAMPLE_RATE) / 1000.0f);
		pthread_create(&t_id[t_index], NULL, VirtualEnvironment::_vs_filter_wrapper, &td[t_index]);
		t_index++;
	}

	for (t_index = 0; t_index < _outputs.size(); t_index++)
	{
		pthread_join(t_id[t_index], NULL);

		// add filter reflection to reflectogram
		for (i = vs_sample[t_index], j = 0; j < _outputs[t_index].left.size(); i++, j++)
		{
			assert(i <= _render_buffer.left.size());
			_render_buffer.left[i] += _outputs[t_index].left[j];
			_render_buffer.right[i] += _outputs[t_index].right[j];
		}
	}
}

#endif

#ifndef HRTF_IIR

void *VirtualEnvironment::_vs_filter_wrapper(void *arg)
{
	threaddata_t *td = (threaddata_t *) arg;
//	VirtualEnvironment *ve = td->ve;
//	virtualsource_t *vs = *(td->it);

	return td->ve->_vs_filter_thread(td->it, td->index);
}

void *VirtualEnvironment::_vs_filter_thread(tree_it_t it, uint index)
{
	virtualsource_t *vs = *it;
	unsigned int i;

	// directivity filtering
	data_t input = _sound_source->get_IR(vs->ref_listener_orientation);
	assert(input.size() <= BUFFER_SAMPLES);
	input.resize(BUFFER_SAMPLES, 0.0f);

	// surface filtering
	input = _surfaces_filter(input, it);

	// distance attenuation
	float attenuation_factor = 1.0f / vs->dist_listener;

	for (i = 0; i < input.size(); i++)
		input[i] *= attenuation_factor;

	// HRTF filtering
	_outputs[index] = _hrtf_filter(input, vs->ref_listener_orientation);

	return 0;  // TODO maybe the index
}

// filter for single reflection
binauraldata_t VirtualEnvironment::_hrtf_filter(data_t &input, const orientation_angles_t &ori)
{
	binauraldata_t output(BUFFER_SAMPLES);

	_hrtfdb->get_HRTF(&_hrtf, ori.az, ori.el);  // get the best-fit HRTF for both ears

	// convolution with 1 image-source
	_hrtf_conv_l->setSKernel(_hrtf.left, N_FFT);
	_hrtf_conv_r->setSKernel(_hrtf.right, N_FFT);

	_hrtf_conv_l->setTSegment(&input[0], SEGMENT_LENGTH);
	_hrtf_conv_r->setTSegment(&input[0], SEGMENT_LENGTH);

	_hrtf_conv_l->convolve(&output.left[0]);
	_hrtf_conv_r->convolve(&output.right[0]);

//	_hrtf_conv_l->convolve(&_left[0]);
//	_hrtf_conv_r->convolve(&_right[0]);

//	l = _left;
//	r = _right;
	return output;
}

// filter for single reflection
binauraldata_t VirtualEnvironment::_hrtf_fir_filter(data_t &input, const orientation_angles_t &ori)
{
	binauraldata_t output(BUFFER_SAMPLES);
	stk::StkFrames out_l(input.size(), 1);  // one channel
	stk::StkFrames out_r(input.size(), 1);  // one channel

	// get the best-fit HRTF for both ears
	_hrtfdb->get_HRTF(&_hrtf, ori.az, ori.el);

	_fir_l.setCoefficients(_hc.left, true);
	_fir_r.setCoefficients(_hc.rigth, true);

	// HRIR filtering
	for (uint i = 0; i < input.size(); i++)
	{
		out_l[i] = _fir_l.tick(input[i]);
		out_r[i] = _fir_r.tick(input[i]);
	}

	for (uint i = 0; i < out_l.size(); i++)
	{
		output.left[i] = (sample_t) out_l[i];
		output.right[i] = (sample_t) out_r[i];
	}

	return output;
}


#else

// IIR filter for single reflection
binauraldata_t VirtualEnvironment::_hrtf_iir_filter(data_t &input, const orientation_angles_t &ori)
{
//	TimerRtai t;
	binauraldata_t output(BUFFER_SAMPLES);
	stk::StkFrames out_l(input.size(), 1);  // one channel
	stk::StkFrames out_r(input.size(), 1);  // one channel

//	t.start();
	// get the best-fit HRTF for both ears
	_hcdb->get_HRTF_coeff(&_hc, ori.az, ori.el);
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

#endif

void VirtualEnvironment::calc_late_reverberation()
{
	orientation_angles_t ori;  // dummy
	data_t input = _sound_source->get_IR(ori);
	std::vector<double> output(_length_bir);  // temporary

	double val;
	unsigned long i;
	double max_value = 0.0f, abs_value;

	for (i = 0; i < _length_bir; i++)
	{
		val = (i >= input.size() ? 0.0 : (double) input[i]);
		output[i] = _fdn->tick(val);
		abs_value = fabs(output[i]);

		if (abs_value > max_value)
			max_value = abs_value;
	}

	float d, scaling_factor;

	// scale properly
	for (i = 0; i < _length_bir; i++)
	{
		d = (i * _config->speed_of_sound) / SAMPLE_RATE;  // calculation of distance travel by sound
		scaling_factor = (d > 1.0f ? d : 1);
		output[i] /= (max_value * scaling_factor);  // scaling (normalize and 1/r attenuation)
	}

	unsigned long sample_mix = (unsigned long) ((_config->max_distance / _config->speed_of_sound) * SAMPLE_RATE);
	double k = sample_mix / -log(0.01);

	for (i = 0; i < sample_mix; i++)
	{
		_late_buffer[i] = (sample_t) (output[i] * (1.0 - exp(-(i + 1.0) / k)));  // attenuation function for early part
	}

	for (i = sample_mix; i < _length_bir; i++)
	{
		_late_buffer[i] = (sample_t) output[i];
	}


//	stk::FileWvOut out("late.wav", 1, stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16);
//
//	for (i = 0; i < _length_bir; i++)
//		out.tick(_late_buffer[i]);
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

}  // namespace avrs
