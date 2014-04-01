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

#ifndef TIMER_HPP_
#define TIMER_HPP_

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

namespace avrs
{

class Timer
{
public:
	void start();
	void stop();

	double get_elapsed_us();
	double get_elapsed_ms();
	double get_elapsed_s();

	void print_elapsed_time();

private:
	double _t0;
	double _t1;
	double _diff;

	/*
	 * Author:  David Robert Nadeau
	 * Site:    http://NadeauSoftware.com/
	 * License: Creative Commons Attribution 3.0 Unported License
	 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
	 */
	double _get_CPU_time();
};

}  // namespace avrs

#endif /* TIMER_HPP_ */
