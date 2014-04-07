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

#include <iostream>
#include <boost/format.hpp>

#include "utils/timercpu.hpp"

namespace avrs
{

void TimerCpu::start()
{
	_t0 = _get_CPU_time();
}

void TimerCpu::stop()
{
	_t1 = _get_CPU_time();
	_diff = _t1 - _t0;
}

double TimerCpu::elapsed_time(timer_unit_t u)
{
	switch (u)
	{
	case second:
		return _diff;

	case millisecond:
		return (_diff / 1E-3);

	case microsecond:
		return (_diff / 1E-6);

	case nanosecond:
		return (_diff / 1E-9);

	default:
		return -1.0;
	}
}

void TimerCpu::print_elapsed_time()
{
	 boost::format fmter("Time [s]:\t%.9f\nTime [ms]:\t%.6f\nTime [us]:\t%.3f\nTime [ns]:\t%.0f\n");
	 std::cout << boost::format(fmter)
	    		% _diff
	    		% (_diff / 1E-3)
	    		% (_diff / 1E-6)
	    		% (_diff / 1E-9);
}

// Private functions

/**
 * Returns the amount of CPU time used by the current process,
 * in seconds, or -1.0 if an error occurred.
 */
double TimerCpu::_get_CPU_time( )
{
#if defined(_WIN32)
	/* Windows -------------------------------------------------- */
	FILETIME createTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;
	if ( GetProcessTimes( GetCurrentProcess( ),
		&createTime, &exitTime, &kernelTime, &userTime ) != -1 )
	{
		SYSTEMTIME userSystemTime;
		if ( FileTimeToSystemTime( &userTime, &userSystemTime ) != -1 )
			return (double)userSystemTime.wHour * 3600.0 +
				(double)userSystemTime.wMinute * 60.0 +
				(double)userSystemTime.wSecond +
				(double)userSystemTime.wMilliseconds / 1000.0;
	}

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
	/* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if _POSIX_TIMERS > 0
	/* Prefer high-res POSIX timers, when available. */
	{
		clockid_t id;
		struct timespec ts;
#if _POSIX_CPUTIME > 0
		/* Clock ids vary by OS.  Query the id, if possible. */
		if ( clock_getcpuclockid( 0, &id ) == -1 )
#endif
#if defined(CLOCK_MONOTONIC)
			/* Use known clock id for AIX, Linux, or Solaris. */
			id = CLOCK_MONOTONIC;  // absolute elapsed wall-clock time
#elif defined(CLOCK_VIRTUAL)
			/* Use known clock id for BSD or HP-UX. */
			id = CLOCK_VIRTUAL;
#else
			id = (clockid_t)-1;
#endif

		if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
			return (double)ts.tv_sec +
				(double)ts.tv_nsec / 1000000000.0;
	}
#endif

#if defined(RUSAGE_SELF)
	{
		struct rusage rusage;
		if ( getrusage( RUSAGE_SELF, &rusage ) != -1 )
			return (double)rusage.ru_utime.tv_sec +
				(double)rusage.ru_utime.tv_usec / 1000000.0;
	}
#endif

#if defined(_SC_CLK_TCK)
	{
		const double ticks = (double)sysconf( _SC_CLK_TCK );
		struct tms tms;
		if ( times( &tms ) != (clock_t)-1 )
			return (double)tms.tms_utime / ticks;
	}
#endif

#if defined(CLOCKS_PER_SEC)
	{
		clock_t cl = clock( );
		if ( cl != (clock_t)-1 )
			return (double)cl / (double)CLOCKS_PER_SEC;
	}
#endif

#endif

	return -1.0;		/* Failed. */
}

}  // namespace avrs
