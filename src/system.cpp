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

#include <sys/mman.h>
#include <sys/io.h>
#include <pthread.h>
#include <csignal>
#include <cstring>
#include <rtai_lxrt.h>
#include <rtai_mbx.h>
#include <rtai_fifos.h>

#include "common.hpp"
#include "avrsexception.hpp"
#include "configuration.hpp"
#include "utils/rttools.hpp"
#include "utils/math.hpp"
#include "system.hpp"

namespace avrs
{

System::System(std::string config_filename, bool show_config)
{
	_config_manager.load_configuration(config_filename);
	_config_sim = _config_manager.get_configuration();

	if (show_config)
		_config_manager.show_configuration();

	if (!_init())
		throw AvrsException("Error creating System");
}

System::~System()
{
	;
}

// Singleton (static ptr_t)
System::ptr_t System::get_instance(std::string config_filename, bool show_config)
{
	static System::ptr_t p_instance(new System(config_filename, show_config));  // instance
	return p_instance;
}

bool System::_init()
{
	_in = InputWaveLoop::create(_config_sim->anechoic_file);
	assert(_in.get() != NULL);
	_out = Player::create(avrs::math::dB2linear(_config_sim->master_gain_db));
	assert(_out.get() != NULL);

	_conv_l = Convolver::create(BUFFER_SAMPLES);
	_conv_r = Convolver::create(BUFFER_SAMPLES);

	uint read_interval_ms = 10;  // ms (100 Hz)

#ifdef WIIMOTE_TRACKER
	std::cout << "Starting Wiimote tracker\n";
	_tracker = TrackerWiimote::create(
			"tracker_points",
			_config_sim->tracker_params,
			read_interval_ms);
#else
	std::cout << "Starting simulated tracker\n";
	_tracker = TrackerSim::create(
			TrackerSim::from_file,
			read_interval_ms,
			_config_sim->tracker_sim_file);
#endif

	assert(_tracker.get() != NULL);

	_ve = VirtualEnvironment::create(_config_sim, _tracker);
	assert(_ve.get() != NULL);

	//_ve->print_vis();  // for debug only
	// Print information of ISM
	std::cout << "Total VSs calculated: " << _ve->n_vs()  << std::endl;
	std::cout << "Audible VSs: " << _ve->n_visible_vs() << std::endl;

	return true;
}

// SRT main task
bool System::run()
{
	signal(SIGINT, end_system);
	signal(SIGTERM, end_system);

	RT_TASK *wait_task;

	rt_allow_nonroot_hrt(); // allow hard real-time from non-root users

	if (rt_is_hard_timer_running())
	{
		// Skip hard real time timer setting...
		_sampling_interval = nano2count(TICK_TIME);
	}
	else
	{
		// Starting real time timer...
		rt_set_oneshot_mode();
		start_rt_timer(0);
		_sampling_interval = nano2count(TICK_TIME);
	}

	mlockall(MCL_CURRENT | MCL_FUTURE);

	// create WAIT task
	wait_task = rt_task_init_schmod(nam2num("TSKWAI"), 2, 0, 0, SCHED_FIFO, 0xFF);

	if (!wait_task)
	{
		ERROR("Cannot init WAIT task");
		return false;
	}

	_is_running = true;
	pthread_create(&_thread_id, NULL, System::_rt_wrapper, this); // run simulation (in another thread)

	printf(">> ");

	while (!g_end_system)
	{
		switch (getchar())
		{
			case 'Q':
			case 'q':
				g_end_system = 1;
				break;

			case 'C':
			case 'c':
				_ve->calibrate_tracker();
				printf("Calibrating done\n");
				printf(">> ");
				break;

			case '\n':
				printf(">> ");
				break;
		}
	}

	pthread_join(_thread_id, NULL); // wait until run thread finish...

	// Why error? (segmentation fault)
	//stop_rt_timer();
	rt_task_delete(wait_task);

	printf("Quit\n");

	return true;
}

void *System::_rt_wrapper(void *arg)
{
	return reinterpret_cast<System *> (arg)->_rt_thread(NULL);
}

// HRT Task
void *System::_rt_thread(void *arg)
{
	RT_TASK *sys_task;
	RTIME start_time;
	int *retval = NULL;
	// for time measurements
	RTIME start_loop, end_loop;
	RTIME start_render, end_render;
	RTIME start_conv, end_conv;
	float elapsed_loop, elapsed_render, elapsed_conv;
	// other variables
	int val;
	bool ok;
	uint i;

	rt_allow_nonroot_hrt();
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// create SYS task
	sys_task = rt_task_init_schmod(nam2num("TSKSYS"), 0, 0, 0, SCHED_FIFO, 0xFF); // use CPU 1????

	if (!sys_task)
	{
		DPRINT("Cannot init SYS task\n");
		*retval = 1;
		pthread_exit(retval);
	}

	// create the output RT-FIFO
	if (rtf_create(RTF_OUT_NUM, RTF_OUT_BLOCK * sizeof(sample_t) * 4) < 0) // capacity x 4
	{
		ERROR("Cannot init rt-fifo");
		*retval = 1;
		pthread_exit(retval);
	}

	rtf_reset(RTF_OUT_NUM); // clear it out

	_input.resize(BUFFER_SAMPLES);
	int n_bytes = RTF_OUT_BLOCK * sizeof(sample_t);  // both ears
	sample_t *output_player = (sample_t *) malloc(RTF_OUT_BLOCK * sizeof(sample_t));  // sending by RT-FIFO
	sample_t *output_l = (sample_t *) malloc(BUFFER_SAMPLES * sizeof(sample_t));
	sample_t *output_r = (sample_t *) malloc(BUFFER_SAMPLES * sizeof(sample_t));

	for (i = 0; i < BUFFER_SAMPLES; i++)
		output_l[i] = output_r[i] = 0.0f;

#ifdef RTCONV_THREADS
	pthread_t t_l;
	pthread_t t_r;
#endif

	_ve->calc_late_reverberation();

	_out->start(); // start the output
	_ve->start_simulation();

	start_time = rt_get_time() + 50 * _sampling_interval;
	rt_task_make_periodic(sys_task, start_time, _sampling_interval); // make the task periodic
	rt_make_hard_real_time(); // execute the task in hard real-time

	while (!g_end_system)
	{
		start_loop = rt_get_time_ns();

		// get input (anechoic signal)
		for (i = 0; i < BUFFER_SAMPLES; i++)
			_input[i] = _in->tick();

		// update the position
		ok = _ve->update_listener_orientation();

		if (!ok)
			end_system(-1);

		// renderize BIR
		start_render = rt_get_time_ns();
		_ve->renderize();
		end_render = rt_get_time_ns();

		// get the new BIR
		_bir = _ve->get_BIR();

		// update the BIR in the real-time convolver
		start_conv = rt_get_time_ns();

#ifndef RTCONV_THREADS
		if (_ve->new_BIR())
		{
			_conv_l->set_filter_t(_bir.left);
			_conv_r->set_filter_t(_bir.right);
		}

		// convolve with anechoic signal
		output_l = _conv_l->convolve_signal(_input.data());
		output_r = _conv_r->convolve_signal(_input.data());
#else
		pthread_create(&t_l, NULL, System::_convolve_left_wrapper, this);
		pthread_create(&t_r, NULL, System::_convolve_right_wrapper, this);
		pthread_join(t_l,  (void **) &output_l);
		pthread_join(t_r, (void **) &output_r);
#endif

		end_conv = rt_get_time_ns();

		// preparing output for RT-FIFO
		memcpy(output_player, output_l, BUFFER_SAMPLES * sizeof(sample_t));
		memcpy(&output_player[BUFFER_SAMPLES], output_r, BUFFER_SAMPLES * sizeof(sample_t));

		// send to output player
		val = rtf_put(RTF_OUT_NUM, output_player, n_bytes); // both ears [left right]

		if (val != n_bytes)
		{
			DPRINT("ERROR sending through RT-FIFO, %d bytes instead of %d bytes", val, n_bytes * 2);
			//rtf_reset(RTF_OUT_NUM);
		}

		end_loop = rt_get_time_ns();
		elapsed_loop = (float) (end_loop - start_loop) / 1E6; // in ms
		elapsed_render = (float) (end_render - start_render) / 1E6; // in ms
		elapsed_conv = (float) (end_conv - start_conv) / 1E6; // in ms
//		DPRINT("Render: %6.3f - RT Convolution: %6.3f - Loop: %6.3f - Tick: %6.3f ms",
//				elapsed_render, elapsed_conv, elapsed_loop, TICK_TIME / 1.0e+6f);

		rt_task_wait_period();
	}

	free(output_player);

	//_out->stop(); // stop the output
	_ve->stop_simulation();

	rt_make_soft_real_time();
	rt_task_delete(sys_task);
	rtf_destroy(RTF_OUT_NUM);

	return 0;
}

void *System::_convolve_left_wrapper(void *arg)
{
	return reinterpret_cast<System *> (arg)->_convolve_left_thread(NULL);
}

void *System::_convolve_left_thread(void *arg)
{
	if (_ve->new_BIR())
		_conv_l->set_filter_t(_bir.left);

	return _conv_l->convolve_signal(_input.data());
}

void *System::_convolve_right_wrapper(void *arg)
{
	return reinterpret_cast<System *> (arg)->_convolve_right_thread(NULL);
}

void *System::_convolve_right_thread(void *arg)
{
	if (_ve->new_BIR())
		_conv_r->set_filter_t(_bir.right);

	return _conv_r->convolve_signal(_input.data());
}

}  // namespace avrs
