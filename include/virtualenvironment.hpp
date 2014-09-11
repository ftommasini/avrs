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

#ifndef VIRTUALENVIRONMENT_HPP_
#define VIRTUALENVIRONMENT_HPP_

#include <memory>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sys/time.h>
#include <cstdio>
#include <stddef.h>
#include <stk/Iir.h>
#include <stk/Fir.h>
#include <stk/Delay.h>
#include <stk/DelayA.h>
#include <boost/shared_ptr.hpp>

extern "C" {
#include <rtai_mbx.h>
}

#include "utils/tree.hpp"
#include "utils/rttools.hpp"
#include "utils/math.hpp"
#include "tracker/sim/trackersim.hpp"
#include "tracker/wiimote/trackerwiimote.hpp"
#include "convolver.hpp"
#include "room.hpp"
#include "soundsource.hpp"
#include "listener.hpp"
#include "ism.hpp"
#include "headfilter.hpp"
#include "fdn.hpp"
#include "common.hpp"
#include "configuration.hpp"
#include "airabsorption.hpp"
#include "virtualsource.hpp"

namespace avrs
{

/**
 * Class for generation of virtual environment Binaural Impulse Response (BIR)
 */
class VirtualEnvironment
{
public:
	typedef boost::shared_ptr<VirtualEnvironment> ptr_t;

	virtual ~VirtualEnvironment();
	/// Static factory function for VirtualEnvironment objects
	static ptr_t create(configuration_t::ptr_t cs, TrackerBase::ptr_t tracker);

	// Info
	float get_room_area() const;
	unsigned int n_surfaces() const;
	unsigned int n_vs() const;
	unsigned int n_visible_vs();

	// Commands
	void start_simulation();
	void stop_simulation();
	void calibrate_tracker();

	/// Update current position/orientation value used for real-time process by mean of the tracker
	bool update_listener_orientation();

	// BIR methods

	/// Render the binaural impulse response (BIR) in real-time process by using current tracker data
	void renderize();

	unsigned long sample_mix_time();

	/**
	 * Get the current BIR
	 * @return the current BIR
	 */
	binauraldata_t &get_BIR();

	bool is_new_BIR() const;

private:
	VirtualEnvironment(configuration_t::ptr_t cs, TrackerBase::ptr_t tracker);

	configuration_t::ptr_t _config;

	// Buffers
	data_t _early_buffer;  // early reflections
	data_t _late_buffer;  // diffusion + late reverberation
	binauraldata_t _render_buffer;  // complete BIR

	unsigned long _length_bir;
	data_t _zeros;
	bool _new_bir;  // flag indicates new BIR

	// Tracker
	TrackerBase::ptr_t _tracker;
	MBX *_mbx_tracker;
	trackerdata_t _tracker_data;
	trackerdata_t _prev_tracker_data;

	// Room
	Room::ptr_t _room;
	// Sound source
	SoundSource::ptr_t _sound_source;
	// Listener
	Listener::ptr_t _listener;
	// Early reflections
	Ism::ptr_t _ism;
	// Late reverberation
	Fdn::ptr_t _fdn;
	// Air absorption
	AirAbsorption::ptr_t _air_absorption;
	// Surface material filters
	stk::Iir _filter_surfaces;
	// Renderer
	HrtfCoeffSet::ptr_t _hcdb;
	hrtfcoeff_t _hc;
	stk::Iir _filter_l;
	stk::Iir _filter_r;
	stk::Delay _delay;

	// Private methods
	void _calc_late_reverberation();
	binauraldata_t _hrtf_iir_filter(data_t &input, const point3_t &vs_pos_R);
	data_t _surfaces_filter(data_t &input, const Ism::tree_vs_t::iterator node);
	bool _listener_is_moved();

    // Thread for VS chain filter
    static void *_vs_filter_wrapper(void *arg);
    void *_vs_filter_thread(Ism::tree_vs_t::iterator it, uint index);
	std::vector<binauraldata_t> _outputs;  // keep the VS chain filter IR

	typedef struct {
		VirtualEnvironment *ve;
		Ism::tree_vs_t::iterator it;
		uint index;
	} threaddata_t;
};


inline void VirtualEnvironment::start_simulation()
{
	if (_tracker.get() != NULL)
		_tracker->start();
}

inline void VirtualEnvironment::stop_simulation()
{
	if (_tracker.get() != NULL)
		_tracker->stop();
}

inline void VirtualEnvironment::calibrate_tracker()
{
	if (_tracker.get() != NULL)
		_tracker->calibrate();
}

inline binauraldata_t &VirtualEnvironment::get_BIR()
{
	return _render_buffer;
}

inline bool VirtualEnvironment::is_new_BIR() const
{
	return _new_bir;
}

inline float VirtualEnvironment::get_room_area() const
{
	return _room->total_area();
}

inline unsigned int VirtualEnvironment::n_surfaces() const
{
	return _room->n_surfaces();
}

inline unsigned int VirtualEnvironment::n_vs() const
{
	return _ism->get_count_vs();
}

inline unsigned int VirtualEnvironment::n_visible_vs()
{
	return _ism->get_count_visible_vs();
}

inline unsigned long VirtualEnvironment::sample_mix_time()
{
	return (unsigned long)((_config->transition_time / _config->speed_of_sound) * SAMPLE_RATE);  // transition time

	// alternatives
	// return (unsigned long)((sqrt(_room->volume()) * SAMPLE_RATE) / 1000);  // mixing time
	// return (unsigned long)((_config->max_distance / _config->speed_of_sound) * SAMPLE_RATE);  // max distance
}

inline bool VirtualEnvironment::_listener_is_moved()
{
	if (fabs(_prev_tracker_data.ori.az - _tracker_data.ori.az) >= _config->angle_threshold ||
		fabs(_prev_tracker_data.ori.el - _tracker_data.ori.el) >= _config->angle_threshold)
	{
		_prev_tracker_data = _tracker_data;  // the listener exceeded the threshold, so save the new data
		return true;
	}

	return false;
}

}  // namespace avrs

#endif  // VIRTUALENVIRONMENT_HPP_
