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

#include "utils/timerbase.hpp"

namespace avrs
{

void TimerBase::print_elapsed_time(timer_unit_t u, std::string header)
{
	if (header.length() > 0)
		std::cout << header << " - ";

	std::string precision;

	if (u == nanosecond)
		precision = "%.0f";
	else
		precision = "%.3f";

	boost::format fmter("Elapsed time: " + precision + " " + _get_unit(u) + "\n");
	std::cout << boost::format(fmter) % elapsed_time(u);
}

void TimerBase::print_elapsed_time(std::string header)
{
	if (header.length() > 0)
		std::cout << header << " - ";

	boost::format fmter(
			"\nElapsed time:\t%.3f " + _get_unit(second) +
			"\nElapsed time:\t%.3f " + _get_unit(millisecond) +
			"\nElapsed time:\t%.3f " + _get_unit(microsecond) +
			"\nElapsed time:\t%.0f " + _get_unit(nanosecond) + "\n"
			);
	std::cout << boost::format(fmter)
		% elapsed_time(second)
		% elapsed_time(millisecond)
		% elapsed_time(microsecond)
		% elapsed_time(nanosecond);
}

std::string TimerBase::_get_unit(timer_unit_t u)
{
	switch (u)
	{
	case second:
		return "s";

	case millisecond:
		return "ms";

	case microsecond:
		return "us";

	case nanosecond:
		return "ns";

	default:
		return "";
	}
}

}  // namespace avrs


