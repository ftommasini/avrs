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

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <boost/shared_ptr.hpp>

// RTAI headers
extern "C" {
#include <rtai_lxrt.h>
}

// AVRS headers
#include "configuration.hpp"
#include "input.hpp"
#include "player.hpp"
#include "headfilter.hpp"
#include "virtualenvironment.hpp"
#include "tracker/sim/trackersim.hpp"
#include "tracker/wiimote/trackerwiimote.hpp"

namespace avrs
{

// Ending system stuff
static int g_end_system = 0;
static int g_waiting = 1;
static inline void end_system(int sig)
{
	g_end_system = 1;
}

/**
 * System class (singleton)
 */
class System
{
public:
	typedef boost::shared_ptr<System> ptr_t; ///< auto_ptr to System

	virtual ~System();
	static ptr_t get_instance(std::string config_filename, bool show_config);

	/**
	 * Run the system. Sets up RT process and runs through loop
	 * @return
	 */
	bool run();

private:
	System(std::string config_filename, bool show_config);  ///< Private constructor
	System(const System &); ///< Prevent copy-construction
	System &operator=(const System &);  ///< Prevent assignment

	void _init();

	volatile bool _is_running;

	RTIME _sampling_interval;

	// one for now, maybe more in the future
	VirtualEnvironment::ptr_t _ve;

	ConfigurationManager _config_manager;
	configuration_t::ptr_t _config_sim;

	InputWaveLoop::ptr_t _in;
	Player::ptr_t _out;

	data_t _input;
	binauraldata_t _bir;
	Convolver::ptr_t _conv_l;
	Convolver::ptr_t _conv_r;

	TrackerBase::ptr_t _tracker;

    // thread related stuff
    pthread_t _thread_id;
    static void *_rt_wrapper(void *arg);
	/**
	 * Hard real-time function in user space (RTAI-LXRT)
	 * @param arg
	 */
    void *_rt_thread(void *arg);

    // threads for real-time convolution
    static void *_convolve_left_wrapper(void *arg);
    void *_convolve_left_thread(void *arg);
    static void *_convolve_right_wrapper(void *arg);
    void *_convolve_right_thread(void *arg);
};

}  // namespace avrs

#endif  // SYSTEM_HPP_

