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

#include <dxflib/dl_dxf.h>
#include <boost/format.hpp>

#include "virtualenvironment.hpp"
#include "dxfreader.hpp"

namespace avrs
{

VirtualEnvironment::VirtualEnvironment(configuration_t *cs, TrackerBase::ptr_t tracker)
{
	assert(cs != NULL);

	_config_sim = cs;
	_tracker = tracker;
	_sound_source = cs->sound_source;
	assert(_sound_source.get() != NULL);
	_listener = cs->listener;
	assert(_listener.get() != NULL);
	_new_data = false;
	_new_bir = false;
	_volume = 0.0f;
	_area = 0.0f;
	_count_vs = 0;

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

	_length_bir = _config_sim->bir_length_samples;

	// resize data vectors
	_render_buffer.left.resize(_length_bir, 0.0f);
	_render_buffer.right.resize(_length_bir, 0.0f);
	_zeros.resize(_length_bir, 0.0f);
	_late_buffer.resize(_length_bir, 0.0f);

	// create and load HRTF DB into program
//#ifndef HRTF_IIR
//	_hrtfdb = HrtfSet::create(_config_sim->hrtf_file);
//	// for left ear
//	_hrtf_conv_l = HrtfConvolver::create(N_FFT);
//	_hrtf_conv_l->setKernelLength(KERNEL_LENGTH);
//	_hrtf_conv_l->setSegmentLength(SEGMENT_LENGTH);
//	// for right ear
//	_hrtf_conv_r = HrtfConvolver::create(N_FFT);
//	_hrtf_conv_r->setKernelLength(KERNEL_LENGTH);
//	_hrtf_conv_r->setSegmentLength(SEGMENT_LENGTH);
//#else
	_hcdb = HrtfCoeffSet::create(_config_sim->hrtf_file);
	_delay.setMaximumDelay(BUFFER_SAMPLES);
//#endif
}

VirtualEnvironment::~VirtualEnvironment()
{
	rttools::del_mbx(MBX_TRACKER_NAME);

	// deallocate memory
	for (surfaces_it_t i = _surfaces.begin(); i != _surfaces.end(); i++)
	{
		Surface *s = *i;
		delete s;
	}

	// deallocate memory
	// tree pre-order traversal
	for (tree_it_t i = _tree.begin(); i != _tree.end(); i++)
	{
		virtualsource_t *vs = *i;
		delete vs;
	}
}

VirtualEnvironment::ptr_t VirtualEnvironment::create(configuration_t *cs, TrackerBase::ptr_t tracker)
{
	ptr_t p_tmp(new VirtualEnvironment(cs, tracker));

	if (!p_tmp->_init())
		p_tmp.reset(); // set to NULL

	return p_tmp;
}

bool VirtualEnvironment::_init()
{
	// initialize tracker
	_mbx_tracker = rttools::get_mbx(MBX_TRACKER_NAME, MBX_TRACKER_BLOCK * sizeof(trackerdata_t));

	if (!_mbx_tracker)
	{
		ERROR("Cannot init TRACKER mailbox");
		return false;
	}

	// Initialize room model
	DxfReader *reader = new DxfReader(this);
	DL_Dxf *dxf = new DL_Dxf();
	const char *filename = _config_sim->dxf_file.c_str();

	if (!dxf->in(filename, reader))  // if file open failed
	{
		DPRINT("%s could not be opened.\n", filename);  // todo error
	    delete dxf;
	    delete reader;
	    return false;
	}

	delete dxf;
	delete reader;

	// updates and calculations
	update_surfaces_data();
	calc_ISM();  // calculate VSs by ISM

	_update_vis();
	_update_vs_orientations();

	// for debug only
	_sort_vis();
	print_vis();

	return true;
}

void VirtualEnvironment::add_surface(Surface *s)
{
	_surfaces.push_back(s);
	_new_data = true;
}

void VirtualEnvironment::calc_ISM()
{
//	RTIME startt, endt;
//	float elapsedt;

	//startt = rt_get_time_ns();

	_max_dist = _config_sim->max_distance;
	_max_order = _config_sim->max_order;

	// create VS from "real" source
	// order 0
	virtualsource_t *vs = new virtualsource_t;
	vs->id = ++_count_vs;  // must be 1 because is a "real" source
	vs->visible = true;
	vs->pos = _sound_source->pos;
	vs->dist_listener = norm(vs->pos - _listener->get_position(), 2);
	_time_ref_ms = (vs->dist_listener / _config_sim->speed_of_sound) * 1000.0f;
	vs->time_abs_ms = _time_ref_ms;
	vs->time_rel_ms = 0.0f;

	_calc_vs_orientation(vs);

	// append to top of the tree
	_root_it = _tree.insert(_tree.begin(), vs);

	_vis.clear();
	_vis.push_back(vs);

	// propagate first order... and then run recursively
	_propagate_ISM(vs, _root_it, 1);

	//endt = rt_get_time_ns();

	//elapsedt = (float) (endt - startt) / 1E6; // in ms
	//DPRINT("ISM calculation time: %2.4f ms", elapsedt);
	//DPRINT("Max distance: %3.2f - Max order: %d", _max_dist, _max_order);
}

// recursive function
void VirtualEnvironment::_propagate_ISM(virtualsource_t *vs, tree_it_t node, const unsigned int order)
{
	if (order > _max_order)  // break condition for recursive function (for safety)
		return;

	// for each surface
	for (unsigned int i = 0; i < _surfaces.size(); i++)
	{
		Surface *s = _surfaces[i];

		// do the reflection
		// (normal to the surface, already calculated)

		// distance from virtual source (VS) to surface
		float dist_vs_s = s->get_dist_origin() - dot(vs->pos, s->get_normal());

		// validity test (if VS fails, is discarded)
		if (dist_vs_s  > 0.0f)
		{
			// create the new progeny VS
			virtualsource_t *vs_progeny = new virtualsource_t;

			vs_progeny->pos = vs->pos + 2 * dist_vs_s * s->get_normal();  // progeny vs position
			vs_progeny->dist_listener =  norm(vs_progeny->pos - _listener->get_position(), 2);  // distance from VS to listener

			// proximity test
			if (vs_progeny->dist_listener > _max_dist)
				break;  // if it fails, is discarded

			vs_progeny->time_abs_ms = (vs_progeny->dist_listener / 343.0f) * 1000.0f;;
			vs_progeny->time_rel_ms = vs_progeny->time_abs_ms - _time_ref_ms;
			vs_progeny->order = order;
			vs_progeny->surface_index = i;

			// append the progeny VS to the tree (because is not discarded)
			tree_it_t node_progeny = _tree.append_child(node, vs_progeny);
			vs_progeny->id = ++_count_vs;  // update the id, because is a valid VS
			// calculate the orientation of VS
			_calc_vs_orientation(vs_progeny);

			// first visibility test
			vs_progeny->vis_test_1 = _check_vis_1(s, vs_progeny);  // update the visibility 1

			// second visibility test
			// order greater than 1, and must be passed first visibility test
			if (order > 1 && vs_progeny->vis_test_1)
			{
				vs_progeny->vis_test_2 = _check_vis_2(vs_progeny, node_progeny);  // update the visibility 2
				vs_progeny->visible = (vs_progeny->vis_test_1 && vs_progeny->vis_test_2);  // both test must be passed
			}
			else
			{
				vs_progeny->visible = vs_progeny->vis_test_1;
			}

			// TODO for now, avrs call update_vis instead of do this
//			if (vs_progeny->visible)
//				_vis.push_back(vs_progeny);  // add progeny VS to the vector that contains visible VSs

			_propagate_ISM(vs_progeny, node_progeny, order + 1);  // propagate the next order
		}
	}
}

bool VirtualEnvironment::_check_vis_1(Surface *s, VirtualSource *vs)
{
	// first find the intersection point between a line and a plane in 3D
	// see: http://softsurfer.com/Archive/algorithm_0104/algorithm_0104B.htm

//	Surface *s = _surfaces[vs->surface_index];
//	point3d_t xyz_vs = vs->ref_listener_pos;// vs->pos - _listener->pos;  // (P1 - P0) where P1, P2 are elements of line
	frowvec4 plane_coeff = s->get_plane_coeff();
	// n . (P1 - P0) where n is the normal of the plane
	float denom = plane_coeff(0) * vs->ref_listener_pos(X) +
			plane_coeff(1) * vs->ref_listener_pos(Y) +
			plane_coeff(2) * vs->ref_listener_pos(Z);

	// check if line and plane are parallel
	if (denom == 0.0f)
		return false;

	// calculate the parameter for the parametric equation of the line
	float t = -(plane_coeff(0) * (_listener->get_position())(X) +
			plane_coeff(1) * (_listener->get_position())(Y) +
			plane_coeff(2) * (_listener->get_position())(Z) +
			plane_coeff(3)) / denom;
	vs->inter_point = _listener->get_position() + vs->ref_listener_pos * t;;  // calculate the intersection point

	// finally, check if the intersection point is inside of surface
	return s->is_point_inside(vs->inter_point);   // TODO border checking!!!
}

/*
 * Calcula si la fuente es visible o no para el receptor
 * Realiza el segundo test
 * Argumentos de entrada:
 * vs: pointer to the current VS
 * node: iterator of current tree node
 * Argumentos de salida:
 * true: la fuente es visible, false: la fuente no es visible
 */
bool VirtualEnvironment::_check_vis_2(virtualsource_t *vs, const tree_it_t node)
{
	bool ok = false;
	frowvec3 pos_vl = vs->inter_point;  // already calculated in visibility test 1 (position of "virtual listener")
	virtualsource_t *vs_parent;

	// get parent VS
	tree_it_t node_parent = _tree.parent(node);

	while (node_parent != _root_it)  // while the current node is not the root node
	{
		vs_parent = *node_parent;
		assert(vs_parent != NULL);

		Surface *s = _surfaces[vs_parent->surface_index];  // get the surface that generated the parent VS

		// check for visibility
		frowvec3 xyz_vs = vs_parent->pos - pos_vl;
		frowvec4 plane_coeff = s->get_plane_coeff();
		// dot product
		float denom = plane_coeff(0) * xyz_vs(0) +
				plane_coeff(1) * xyz_vs(1) +
				plane_coeff(2) * xyz_vs(2);

		if (denom == 0.0f)
			return false;

		float t = -(plane_coeff(0) * pos_vl(0) +
				plane_coeff(1) * pos_vl(1) +
				plane_coeff(2) * pos_vl(2) +
				plane_coeff(3)) / denom;
		frowvec3 inter_point = pos_vl + xyz_vs * t;
		ok =  s->is_point_inside(inter_point);

		if (!ok)
			return false;

		pos_vl = inter_point;  // the "new" artificial listener
		node_parent = _tree.parent(node_parent);  // get the parent
	}

	return true;
}

// TODO chequear solo visibilidad
/**
 * Check the visibility of all VSs
 */
//void VirtualEnvironment::check_vis()
//{
//	// tree pre-order traversal
//	for (tree_it_t i = _tree.begin(); i != _tree.end(); i++)
//	{
//		virtualsource_t *vs = (virtualsource_t *) *i;
//
//		// first visibility test
//		vs->vis_test_1 = _check_vis_1(vs);  // update the visibility 1
//
//		// second visibility test
//		// order greater than 1, and must be passed first visibility test
//		if (vs->order > 1 && vs->vis_test_1)
//		{
//			vs->vis_test_2 = _check_vis_2(vs, i, vs->order);  // update the visibility 2
//			vs->visible = (vs->vis_test_1 && vs->vis_test_2);  // both test must be passed
//		}
//		else
//		{
//			vs->visible = vs->vis_test_1;
//		}
//	}
//}

// Update room area and filter coefficients for each surface
void VirtualEnvironment::update_surfaces_data()
{
	if (_new_data)
	{
		_area = 0.0f;

		for (unsigned int i = 0; i < _surfaces.size(); i++)
		{
			Surface *s = _surfaces[i];
			_area += s->get_area(); // sum areas of all surfaces

			if (_config_sim->b_coeff.size() > 0)
				s->set_b_filter_coeff(_config_sim->b_coeff[i]);

			if (_config_sim->a_coeff.size() > 0)
				s->set_a_filter_coeff(_config_sim->a_coeff[i]);
		}

		_new_data = false;
	}
}

void VirtualEnvironment::_update_vis()
{
	_vis.clear();  // clear vector

	for (tree_it_t it = _tree.begin(); it != _tree.end(); it++)
	{
		virtualsource_t *vs = *it;

		if (vs->visible)
			_vis.push_back(vs);
	}

	_outputs.resize(_vis.size());  // output per visible VS

	//DPRINT("Total VSs: %d - Visibles VSs: %d", (int) _tree.size(_root_it), (int) _vis.size());
}

// todo maybe in a thread
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
			_update_vs_orientations();  // and update VS orientations

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

void VirtualEnvironment::_update_vs_orientations()
{
	// only for visible VSs
	for (vis_it_t it = _vis.begin(); it != _vis.end(); it++)
	{
		virtualsource_t *vs = *it;
		vs->ref_listener_orientation = vs->initial_orientation - _listener->get_orientation();
	}

//	vis_it_t tmp = _vis.begin();
//	virtualsource_t *v = *tmp;
//	DPRINT("az: %3.1f, el: %3.1f", v->ref_listener_orientation.az, v->ref_listener_orientation.el);
}

void VirtualEnvironment::_sort_vis()
{
	sort(_vis.begin(), _vis.end(), cmpvsdistance_t());  // sort by distance to the listener
}

void VirtualEnvironment::print_vis()
{
	std::cout << "List of visible VSs\n" << std::endl;
	boost::format fmter_title("%-15s\t%-15s\t%-15s\t%-7s\t%-7s\t%+10s\t%+10s\t%+10s\n");
	boost::format fmter_content("%-15.3f\t%-15.3f\t%-15.3f\t%-7i\t%-7i\t%+10.3f\t%+10.3f\t%+10.3f\n");
	std::cout << boost::format(fmter_title) % "Relative [ms]"
					% "Absolute [ms]" % "Distance [m]" % "Order" % "Id"
					% "X" % "Y" % "Z";
	float time_ref_ms;

	for (vis_it_t it = _vis.begin(); it != _vis.end(); it++)
	{
		virtualsource_t *vs = *it;
		float time_abs_ms = (vs->dist_listener / _config_sim->speed_of_sound) * 1000.0f;

		if (it == _vis.begin())
			time_ref_ms = time_abs_ms;

		float time_rel_ms = time_abs_ms - time_ref_ms;

		std::cout << boost::format(fmter_content)
			% time_rel_ms
			% time_abs_ms
			% vs->dist_listener
			% vs->order
			% vs->id
			% vs->pos(X)
			% vs->pos(Y)
			% vs->pos(Z);
	}

	std::cout << std::endl;


//	float ref_time = (_vis[0]->dist_listener / _config_sim->speed_of_sound) * 1000.0f;
//
//	printf("\n  Abs\t  Rel\t  Az\t  El\tOrder\t  ID\t  Vis1\t  Vis2\n");
//
//	for (vis_it_t it = _vis.begin(); it != _vis.end(); it++)
//	{
//		virtualsource_t *vs = *it;
//
//		float abs_time = (vs->dist_listener / _config_sim->speed_of_sound) * 1000.0f;
//		printf("%7.2f\t", abs_time);
//		printf("%7.2f\t", abs_time - ref_time);
//		printf("%+7.2f\t", vs->ref_listener_orientation.az);
//		printf("%+7.2f\t", vs->ref_listener_orientation.el);
//		printf("%d\t", vs->order);
//		printf("%d\t", vs->id);
//		printf("%d\t", (int) vs->vis_test_1);
//		printf("%d\t", (int) vs->vis_test_2);
//		printf("\n");
//	}
//
//	printf("\n");
}

#ifndef VSFILTER_THREADS

void VirtualEnvironment::renderize()
{
	float elapsed;

	// check if the listener is moved
	if (!_listener_is_moved())
	{
		_new_bir = false;
		return;
	}

	unsigned long i, j;
	binauraldata_t output;

	memcpy(&_render_buffer.left[0], &_zeros[0], _config_sim->bir_length_samples * sizeof(sample_t));
	memcpy(&_render_buffer.right[0], &_zeros[0], _config_sim->bir_length_samples * sizeof(sample_t));

	// TODO SE PODRÍA MEJORAR SI SE GUARDAR EL ITERATOR EN CADA VS, ASI SE RECORRE SOLAMENTE LAS VISIBLES

	// only for visible VSs
	for (tree_it_t it = _tree.begin(); it != _tree.end(); it++)
	{
		virtualsource_t *vs = *it;

		if (!vs->visible)
			continue;

		// directivity filtering
		data_t input = _sound_source->get_IR(vs->ref_listener_orientation);
		assert(input.size() <= VS_SAMPLES);  // TODO REVISAR LONGITUDE DE EARLY REFLECTIONS
		input.resize(VS_SAMPLES, 0.0f);

		// surface filtering
		input = _surfaces_filter(input, it);

		// distance attenuation
		float attenuation_factor = 1.0f / vs->dist_listener;

		for (i = 0; i < input.size(); i++)
			input[i] *= attenuation_factor;

		// HRTF filtering
#ifndef HRTF_IIR
		//output = _hrtf_filter(input, vs->ref_listener_orientation);  // HRTF spectrums

#else
		output = _hrtf_iir_filter(input, vs->ref_listener_orientation);
#endif

		// calculate the sample from reflectogram where starts this reflection
		// TODO delay menor a una muestra
		unsigned long sample = (unsigned long) round((vs->time_rel_ms * SAMPLE_RATE) / 1000.0f);

		// add filter reflection to reflectogram
		for (i = sample, j = 0; j < output.size(); i++, j++)
		{
			assert(i <= _render_buffer.left.size());
			_render_buffer.left[i] += output.left[j];
			_render_buffer.right[i] += output.right[j];
		}

		_new_bir = true;
	}
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

	memcpy(&_render_buffer.left[0], &_zeros[0], _config_sim->bir_length_samples * sizeof(sample_t));
	memcpy(&_render_buffer.right[0], &_zeros[0], _config_sim->bir_length_samples * sizeof(sample_t));

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


//	DPRINT("************\nSTART\n");
//	for (uint i = 0; i < input.size(); i++)
//	{
//		DPRINT("%f", input[i]);
//	}
//
//	DPRINT("************\nEND\n");

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
	binauraldata_t output(BUFFER_SAMPLES);
	stk::StkFrames out_l(input.size(), 1);  // one channel
	stk::StkFrames out_r(input.size(), 1);  // one channel

	// get the best-fit HRTF for both ears
	_hcdb->get_HRTF_coeff(&_hc, ori.az, ori.el);

	_filter_l.setCoefficients(_hc.b_left, _hc.a_left, true);
	_filter_r.setCoefficients(_hc.b_right, _hc.a_right, true);

	// HRTF filtering
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

	for (uint i = 0; i < out_l.size(); i++)
	{
		output.left[i] = (sample_t) out_l[i];
		output.right[i] = (sample_t) out_r[i];
	}

	return output;
}

#endif

data_t VirtualEnvironment::_surfaces_filter(data_t &input, const tree_it_t node)
{
//	RTIME start, end;
//	float elapsed;

 	data_t values = input;

	// get parent VS
	tree_it_t current_node = node;

	while (current_node != _root_it)  // while the current node is not the root node
	{
		virtualsource_t *vs = *current_node;
		assert(vs != NULL);

//		start = rt_get_time_ns();

		Surface *s = _surfaces[vs->surface_index];
		//_set coefficients and clear previous filter state
		_filter_surfaces.setCoefficients(s->get_b_filter_coeff(), s->get_a_filter_coeff(), true);

		// filter for the current surface
		for (uint i = 0; i < input.size(); i++)
			values[i] = (sample_t) _filter_surfaces.tick(values[i]);

		current_node = _tree.parent(current_node);  // get the parent

//		end = rt_get_time_ns();
//		elapsed = (float) (end - start) / 1E3; // in us
//		DPRINT("Time: %6.3f us",  elapsed);
	}

	return values;
}

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
		d = (i * _config_sim->speed_of_sound) / SAMPLE_RATE;  // calculation of distance travel by sound
		scaling_factor = (d > 1.0f ? d : 1);
		output[i] /= (max_value * scaling_factor);  // scaling (normalize and 1/r attenuation)
	}

	unsigned long sample_mix = (unsigned long)
			((_config_sim->max_distance / _config_sim->speed_of_sound) * SAMPLE_RATE);
	double k = sample_mix / -log(0.01);

	//DPRINT("k = %f", k);

	for (i = 0; i < sample_mix; i++)
	{
		_late_buffer[i] = (sample_t) (output[i] * (1.0 - exp(-(i + 1.0) / k)));  // attenuation function for early part
	}

	for (i = sample_mix; i < _length_bir; i++)
	{
		_late_buffer[i] = (sample_t) output[i];
	}
}

}  // namespace avrs
