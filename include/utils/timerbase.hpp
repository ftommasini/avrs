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

#ifndef TIMERBASE_HPP_
#define TIMERBASE_HPP_

namespace avrs
{

typedef enum
{
	second,
	millisecond,
	microsecond,
	nanosecond
} timer_unit_t;

class TimerBase
{
public:
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual double elapsed_time(timer_unit_t u) = 0;
	virtual void print_elapsed_time() = 0;

protected:
	double _diff;
};

}  // namespace avrs

#endif /* TIMERBASE_HPP_ */
