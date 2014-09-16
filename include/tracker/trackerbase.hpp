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

#ifndef TRACKERBASE_HPP_
#define TRACKERBASE_HPP_

#include <boost/shared_ptr.hpp>

#include "common.hpp"

namespace avrs
{

typedef struct TrackerData
{
	position_t pos;
	orientationangles_t ori;
	unsigned long timestamp;

	matrix33_t R;  // rotation matrix
	matrix33_t T;  // traslation matrix

	TrackerData()
	{
		timestamp = 0;
	}

//	TrackerData operator-(const TrackerData &val) const
//	{
//		TrackerData res;
//
//		res.pos = pos + val.pos;
//		res.ori = ori - val.ori;
//
//		return res;
//	}
//
//	TrackerData operator+(const TrackerData &val) const
//	{
//		TrackerData res;
//
//		res.pos = pos + val.pos;
//		res.ori = ori + val.ori;
//
//		return res;
//	}

} trackerdata_t;

class TrackerBase
{
public:
	typedef boost::shared_ptr<TrackerBase> ptr_t;

	// pure virtual functions
	virtual void calibrate() = 0;
	virtual trackerdata_t get_data() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

}  // namespace avrs

#endif /* TRACKERBASE_HPP_ */
