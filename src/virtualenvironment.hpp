/*
 * Copyright (C) 2009-2013 Fabián C. Tommasini
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

#ifndef VIRTUALENVIRONMENT_HPP_
#define VIRTUALENVIRONMENT_HPP_

#include <memory>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sys/time.h>
#include <cstdio>
#include <stddef.h>

#include <rtai_mbx.h>


#include <Iir.h>
#include <Delay.h>

#include "tree.hpp"

#include "rttools.hpp"
#include "mathtools.hpp"
#include "tracker.hpp"
#include "trackerwiimote.hpp"
#include "convolver.hpp"
#include "surface.hpp"
#include "soundsource.hpp"
#include "listener.hpp"
#include "headfilter.hpp"
#include "fdn.hpp"
#include "common.hpp"
#include "configuration.hpp"

typedef struct VirtualSource
{
	unsigned int id;
	point3d_t pos;
	unsigned int order;
	float dist_listener;
	float time_abs_ms;
	float time_rel_ms;
	int surface_index;
	point3d_t inter_point;
	bool vis_test_1;
	bool vis_test_2;
	bool visible;
	point3d_t ref_listener_pos; // to listener
	orientation_angles_t ref_listener_orientation; // referenced to listener
	orientation_angles_t initial_orientation; // initial orientation

	VirtualSource()
	{
		id = 0;
		order = 0;
		dist_listener = 0.0f;
		surface_index = -1;
		vis_test_1 = false;
		vis_test_2 = false;
		visible = false;
	}
} virtualsource_t;

/**
 * Class for generation of virtual environment Binaural Impulse Response (BIR)
 */
class VirtualEnvironment
{
public:
	typedef std::auto_ptr<VirtualEnvironment> ptr_t;

	virtual ~VirtualEnvironment();
	/// Static factory function for VirtualEnvironment objects
	static ptr_t create(avrs::config_sim_t *cs, TrackerBase::ptr_t tracker);

	// Room methods

	float get_room_area();
	float get_room_volume() const;
	unsigned int n_surfaces() const;
	void add_surface(Surface *s);
	void update_surfaces_data();  // TODO private?

	// ISM methods

	void calc_ISM();
	unsigned int n_vs() const;
	unsigned int n_visible_vs();
	// for debug!!!
	void print_vis();
	//void check_vis();

	void calc_late_reverberation();

	// Generic methods

	void start_simulation();
	void stop_simulation();

	void calibrate_tracker();

	/**
	 * Update current position/orientation value used for real-time process by mean of the tracker
	 */
	bool update_listener_orientation();

	// BIR methods

	/**
	 * Render the binaural impulse response (BIR) in real-time process by using current tracker data
	 */
	void renderize();
	/**
	 * Get the current BIR
	 * @return the current BIR
	 */
	binauraldata_t &get_BIR();
	bool new_BIR() const;

private:
	VirtualEnvironment(avrs::config_sim_t *cs, TrackerBase::ptr_t tracker);

	bool _init();

	config_sim_t *_config_sim;
//	config_t *_config;

	data_t _input_buffer;

	binauraldata_t _render_buffer;  // keep the complete BIR

	unsigned long _length_bir;

	data_t _zeros;
	bool _new_bir;

	TrackerBase::ptr_t _tracker;

	MBX *_mbx_tracker;
	trackerdata_t _tracker_data;
	trackerdata_t _prev_tracker_data;

	std::vector<Surface *> _surfaces;
	typedef std::vector<Surface *>::iterator surfaces_it_t;
	float _area;
	float _volume;
	volatile bool _new_data; // flag that indicates new surfaces data

	// TODO: many sound sources
	SoundSource::ptr_t _sound_source;
	Listener::ptr_t _listener;

	// VS propagation
	unsigned int _max_order;
	float _max_dist;
	tree<virtualsource_t *> _tree; // VSs tree
	typedef tree<virtualsource_t *>::iterator tree_it_t;
	tree_it_t _root_it;
	volatile unsigned int _count_vs;
	float _time_ref_ms;

	// visible VS vector
	std::vector<virtualsource_t *> _vis;
	typedef std::vector<virtualsource_t *>::iterator vis_it_t;

	typedef struct CompareVSDistance
	{
		bool operator()(virtualsource_t *i, virtualsource_t *j)
		{
			return (i->dist_listener < j->dist_listener);
		}
	} cmpvsdistance_t;

	void _propagate_ISM(virtualsource_t *vs, tree_it_t vs_node,
			const unsigned int order);
	bool _check_vis_1(Surface *s, virtualsource_t *vs);
	bool _check_vis_2(virtualsource_t *vs, const tree_it_t vs_node);
	void _calc_vs_orientation(virtualsource_t *vs);
	void _update_vs_orientations();  // todo private
	void _sort_vis();
	void _update_vis();

	// Late reverberation
	Fdn::ptr_t _fdn;
	data_t _late_buffer;

	// renderer
#ifndef HRTF_IIR
	HrtfSet::ptr_t _hrtfdb;
	hrtf_t _hrtf;
	HrtfConvolver::ptr_t _hrtf_conv_l;
	HrtfConvolver::ptr_t _hrtf_conv_r;
#else
	HrtfCoeffSet::ptr_t _hcdb;
	hrtfcoeff_t _hc;
	stk::Iir _filter_l;
	stk::Iir _filter_r;
	stk::Delay _delay;
#endif

	stk::Iir _filter_surfaces;;

	// Filter methods

#ifndef HRTF_IIR
	binauraldata_t _hrtf_filter(data_t &input, const orientation_t &ori);
#else
	binauraldata_t _hrtf_iir_filter(data_t &input, const orientation_angles_t &ori);
#endif

	data_t _surfaces_filter(data_t &input, const tree_it_t node);

	bool _listener_is_moved();

    // thread for VS chain filter
    static void *_vs_filter_wrapper(void *arg);
    void *_vs_filter_thread(tree_it_t it, uint index);
	std::vector<binauraldata_t> _outputs;  // keep the VS chain filter IR

	typedef struct {
		VirtualEnvironment *ve;
		tree_it_t it;
		uint index;
	} threaddata_t;
};


inline void VirtualEnvironment::start_simulation()
{

	if (_tracker.get() != NULL) {
		_tracker->start();

		//_tracker->calibrate();
	}
}

inline void VirtualEnvironment::calibrate_tracker()
{
	if (_tracker.get() != NULL)
		_tracker->calibrate();
}

inline void VirtualEnvironment::stop_simulation()
{
	if (_tracker.get() != NULL)
		_tracker->stop();
}

inline binauraldata_t &VirtualEnvironment::get_BIR()
{
	return _render_buffer;
}

inline bool VirtualEnvironment::new_BIR() const
{
	return _new_bir;
}

inline float VirtualEnvironment::get_room_area()
{
	return _area;
}

inline float VirtualEnvironment::get_room_volume() const
{
	return _volume;
}

inline unsigned int VirtualEnvironment::n_surfaces() const
{
	return (unsigned int) _surfaces.size();
}

inline unsigned int VirtualEnvironment::n_vs() const
{
	return (unsigned int) _tree.size(_root_it);
}

inline unsigned int VirtualEnvironment::n_visible_vs()
{
	return (unsigned int) _vis.size();
}

// respecto al oyente (en el sistema de coordenadas L)
inline void VirtualEnvironment::_calc_vs_orientation(virtualsource_t *vs)
{
	// azimuth calculus
	vs->ref_listener_pos = vs->pos - _listener->get_position();
	vs->initial_orientation.az =
			-((atan2(vs->ref_listener_pos(Y), vs->ref_listener_pos(X)) * mathtools::PIdiv180_inverse) - 90.0f); // in degrees

	// elevation calculus
	float r = sqrt(vs->ref_listener_pos(X) * vs->ref_listener_pos(X)
			+ vs->ref_listener_pos(Y) * vs->ref_listener_pos(Y));
	vs->initial_orientation.el =
			-((atan2(r, vs->ref_listener_pos(Z)) * mathtools::PIdiv180_inverse) - 90.0f); // in degrees

	vs->initial_orientation = vs->initial_orientation - _listener->get_orientation();

//	arma::rowvec vs_pos_r(4);
//	vs_pos_r << vs->pos(0) << vs->pos(1) << vs->pos(2) << 0 << endr;
//	arma::rowvec vs_pos_l(4);
//	vs_pos_l = vs_pos_r * _listener->get_rotation_matrix();
//	vs->ref_listener_pos << vs_pos_l(0) << vs_pos_l(1) << vs_pos_l(2) << endr;
//	// azimuth
//	vs->initial_orientation.az =
//			-((atan2(vs->ref_listener_pos(Y), vs->ref_listener_pos(X)) * mathtools::PIdiv180_inverse) - 90.0f); // in degrees
//	// elevation
//	float r = sqrt(vs->ref_listener_pos(X) * vs->ref_listener_pos(X)
//			+ vs->ref_listener_pos(Y) * vs->ref_listener_pos(Y));
//	vs->initial_orientation.el =
//			-((atan2(r, vs->ref_listener_pos(Z)) * mathtools::PIdiv180_inverse) - 90.0f); // in degrees

}

inline bool VirtualEnvironment::_listener_is_moved()
{
	if (fabs(_prev_tracker_data.ori.az - _tracker_data.ori.az) >= _config_sim->angle_threshold ||
		fabs(_prev_tracker_data.ori.el - _tracker_data.ori.el) >= _config_sim->angle_threshold)
	{
		_prev_tracker_data = _tracker_data;  // the listener exceeded the threshold, so save the new data
		return true;
	}

	return false;
}

#endif  // VIRTUALENVIRONMENT_HPP_
