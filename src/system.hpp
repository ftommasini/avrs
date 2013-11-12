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

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <memory>  // auto_ptr

// RTAI headers
#include <rtai_lxrt.h>

// AVRS headers
#include "configuration.hpp"
#include "input.hpp"
#include "player.hpp"
#include "tracker.hpp"
#include "headfilter.hpp"
#include "virtualenvironment.hpp"

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
	typedef std::auto_ptr<System> ptr_t; ///< auto_ptr to System

	virtual ~System();
	/// Static factory function for System objects
	static ptr_t create(avrs::config_sim_t *config_sim);

	/**
	 * Run the system. Sets up RT process and runs through loop
	 * @return
	 */
	bool run();

private:
	System(avrs::config_sim_t *config_sim);  ///< Private constructor
	System(const System &); ///< Prevent copy-construction
	System &operator=(const System &);  ///< Prevent assignment

	bool _init();

	volatile bool _running;

	RTIME _sampling_interval;

	// one for now, maybe more in the future
	VirtualEnvironment::ptr_t _ve;

//	avrs::config_t *_config;
	avrs::config_sim_t *_config_sim;
	InputWaveLoop::ptr_t _in;
	Player::ptr_t _out;

	data_t _input;
	binauraldata_t _bir;
	Convolver::ptr_t _conv_l;
	Convolver::ptr_t _conv_r;

	TrackerBase::ptr_t _tracker;

	HrtfSet::ptr_t _hf;

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

#endif  // SYSTEM_HPP_
