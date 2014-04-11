/*
 * Copyright (C) 2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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

#include "utils/timerrtai.hpp"

namespace avrs
{

void TimerRtai::start()
{
	_t0 = rt_get_time_ns();
}

void TimerRtai::stop()
{
	_t1 = rt_get_time_ns();
	_diff = _t1 - _t0;
}

double TimerRtai::elapsed_time(timer_unit_t u)
{
	switch (u)
	{
	case second:
		return (_diff / 1E9);

	case millisecond:
		return (_diff / 1E6);

	case microsecond:
		return (_diff / 1E3);

	case nanosecond:
		return _diff;

	default:
		return -1.0;
	}
}

}  // namespace avrs
