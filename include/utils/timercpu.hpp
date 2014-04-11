/*
 * Copyright (C) 2013 Fabián C. Tommasini <fabian@tommasini.com.ar>
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

#ifndef TIMERCPU_HPP_
#define TIMERCPU_HPP_

#if defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <ctime>

#else
#error "Unable to define get_CPU_time() for an unknown OS."
#endif

#include "timerbase.hpp"

namespace avrs
{

class TimerCpu : public TimerBase
{
public:
	virtual void start();
	virtual void stop();
	virtual double elapsed_time(timer_unit_t u);

private:
	double _t0;
	double _t1;

	/*
	 * Author:  David Robert Nadeau
	 * Site:    http://nadeausoftware.com/articles/2012/03/c_c_tip_how_measure_cpu_time_benchmarking
	 * License: Creative Commons Attribution 3.0 Unported License
	 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
	 */
	double _get_CPU_time();
};

}  // namespace avrs

#endif /* TIMER_HPP_ */
