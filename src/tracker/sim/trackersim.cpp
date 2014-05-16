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

/**
 * \file tracker.cpp
 */

#include <rtai_lxrt.h>

#include "tracker/sim/trackersim.hpp"
#include "utils/rttools.hpp"
#include "utils/configfilereader.hpp"
#include "avrsexception.hpp"

namespace avrs
{

TrackerSim::TrackerSim(sim_t sim, unsigned int read_interval_ms, std::string filename)
	: _running(false)
{
	_sim_type = sim;
	_read_interval_ms = read_interval_ms;
	_filename = filename;
	_init();
}

TrackerSim::~TrackerSim()
{
	stop();
}

TrackerSim::ptr_t TrackerSim::create(sim_t sim, unsigned int read_interval_ms, std::string filename)
{
	ptr_t p_tmp(new TrackerSim(sim, read_interval_ms, filename));
	return p_tmp;
}

void TrackerSim::_init()
{
	if (_sim_type == from_file)
	{
		_file = fopen(_filename.c_str(), "rb");

		if (!_file)
			throw AvrsException("Error creating VirtualEnvironment");
	}
	else if (_sim_type == constant)
	{
		load_sim_constant_file();
	}
}

void TrackerSim::start()
{
	if (_running)
		return;

	_running = true;
	pthread_create(&_thread_id, NULL, TrackerSim::_threadWrapper, this); // create thread
}

void TrackerSim::stop()
{
	if (!_running)
		return;

	void *thread_exit_status;
	_running = false;
	pthread_join(_thread_id, &thread_exit_status);

	if (_sim_type == from_file)
		fclose(_file);
}

inline void TrackerSim::calibrate()
{
	;  // nothing to do
}


void *TrackerSim::_threadWrapper(void *arg)
{
	return reinterpret_cast<TrackerSim*> (arg)->_thread(NULL);
}

void* TrackerSim::_thread(void *arg)
{
	RT_TASK *rt_task;
	RTIME interval = nano2count(_read_interval_ms * 1E6);
	MBX *mbx_tracker;
	int val;

	rt_allow_nonroot_hrt();
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// create task
	rt_task = rt_task_init_schmod(nam2num("TSKTRA"), 2, 0, 0, SCHED_FIFO, rttools::cpu_id(0));

	if (!rt_task)
	{
		ERROR("Cannot init TRACKER task");
		pthread_exit(NULL);
	}

	mbx_tracker = rttools::get_mbx(MBX_TRACKER_NAME, MBX_TRACKER_BLOCK * sizeof(trackerdata_t));

	if (!mbx_tracker)
	{
		ERROR("Cannot init TRACKER mailbox");
		rt_task_delete(rt_task);
		pthread_exit(NULL);
	}

	while (_running)
	{
		// simulate the current position
		switch (_sim_type)
		{
		case constant:
			sim_constant();
			break;

		case calculated:
			sim_calculated_az();
			//sim_calculated_el();
			break;

		case from_file:
			sim_from_file();
			break;

		case cipic_angles:
			sim_cipic_angles();
			break;
		}

		_data.timestamp = (unsigned long) rt_get_time_ns();

		// send message (non-blocking)
		val = rt_mbx_send_if(mbx_tracker, &_data, sizeof(trackerdata_t));

		if (-EINVAL == val)
		{
			ERROR("Mailbox is invalid");
			break;
		}

		rt_sleep(interval);
	}

	rt_task_delete(rt_task);
	rttools::del_mbx(MBX_TRACKER_NAME);

	return arg;
}

void TrackerSim::sim_cipic_angles()
{
	// Azimuth values
	static float az[] =
	{ -80, -65, -55, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15,
			20, 25, 30, 35, 40, 45, 55, 65, 80 };
	static uint az_idx = 0;
	static uint az_size = (uint) sizeof(az) / sizeof(az[0]);

	// Elevation values
	static float el[] =
	{ -45, -39.375, -33.75, -28.125, -22.5, -16.875, -11.2500, -5.6250, 0,
			5.625, 11.25, 16.875, 22.5, 28.125, 33.75, 39.375, 45, 50.625,
			56.25, 61.875, 67.5, 73.125, 78.75, 84.375, 90, 95.625, 101.25,
			106.875, 112.5, 118.125, 123.75, 129.375, 135, 140.625, 146.25,
			151.875, 157.5, 163.125, 168.75, 174.375, 180, 185.625, 191.251,
			196.875, 202.5, 208.125, 213.75, 219.375, 225, 230.625 };
	static uint el_idx = 0;
	static uint el_size = (uint) sizeof(el) / sizeof(el[0]);

	_data.ori.az = az[6]; //az[az_idx];
	_data.ori.el = el[el_idx];
	az_idx = (az_idx + 1) % az_size;
	el_idx = (el_idx + 1) % el_size;
}

void TrackerSim::sim_calculated_az()
{
	static orientation_angles_t ori_tmp(80.0f, 0.0f, 0.0f);
	static float step = 0.5f;

	if (step > 0) // positive step
	{
		if (ori_tmp.az >= 80.0f)
			step = -step;
	}
	else if (step < 0) // negative step
	{
		if (ori_tmp.az <= -80.0f)
			step = -step;
	}

	ori_tmp.az += step;
	_data.ori = ori_tmp;
}

void TrackerSim::sim_calculated_el()
{
	static orientation_angles_t ori_tmp(0.0f, 0.0f, 0.0f);
	static float step = 1.0f;

	if (step > 0) // positive step
	{
		if (ori_tmp.el >= 360.0f)
			step = -step;
	}
	else if (step < 0) // negative step
	{
		if (ori_tmp.el <= -360.0f)
			step = -step;
	}

	ori_tmp.el += step;
	_data.ori = ori_tmp;
}

void TrackerSim::sim_from_file()
{
	static float data[6]; // data to be read
	bool ok;

	do
	{
		ok = (fread(data, sizeof(float), 6, _file) == 6);

		if (feof(_file))
		{
			DPRINT("Rewind tracker file");
			fseek(_file, 0, SEEK_SET); // at origin again
		}
	} while (!ok);

	_data.pos.x = data[0];
	_data.pos.y = data[1];
	_data.pos.z = data[2];
	_data.ori.az = data[3];
	_data.ori.el = data[4];
	_data.ori.ro = data[5];

//	DPRINT("Az: %+1.3f\tEl: %+1.3f", _data.ori.az, _data.ori.el);
}

void TrackerSim::sim_constant()
{
	;
//	DPRINT("X: %+1.3f\tY: %+1.3f\tZ: %+1.3f", _data.pos.x, _data.pos.y, _data.pos.z);
}

void TrackerSim::load_sim_constant_file()
{
	std::string filename = "tracker_sim_constant.dat";
	ConfigFileReader cfr(filename);

	if (!cfr.readInto(_data.pos.x, "X"))
		throw AvrsException("Error in tracker file: X is missing");

	if (!cfr.readInto(_data.pos.y, "Y"))
		throw AvrsException("Error in tracker file: Y is missing");

	if (!cfr.readInto(_data.pos.z, "Z"))
		throw AvrsException("Error in tracker file: Z is missing");

	if (!cfr.readInto(_data.ori.az, "AZ"))
		throw AvrsException("Error in tracker file: AZ is missing");

	if (!cfr.readInto(_data.ori.el, "EL"))
		throw AvrsException("Error in tracker file: EL is missing");

	if (!cfr.readInto(_data.ori.ro, "RO"))
		throw AvrsException("Error in tracker file: RO is missing");
}

}  // namespace avrs
