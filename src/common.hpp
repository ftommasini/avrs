/*
 * Copyright (C) 2009-2012 Fabián C. Tommasini
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

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <cassert>
#include <cstdio>
#include <vector>
#include <iostream>
#include <memory>  // for auto_ptr
#include <fftw3.h>
#include <armadillo>

namespace avrs
{

// typedefs
typedef fftwf_complex complex_t;  // for spectral data

typedef float sample_t;  // for time data

typedef std::vector<sample_t> data_t;

typedef arma::frowvec3 point3d_t;

typedef struct BinauralData
{
	data_t left;
	data_t right;

	BinauralData(unsigned long size = 0)
	{
		left.resize(size);
		right.resize(size);
	}

	unsigned long size()
	{
		return left.size();
	}
} binauraldata_t;

typedef struct Position
{
	float x;	///< X position (in cm)
	float y;	///< Y position (in cm)
	float z;	///z Z position (in cm)

	Position()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Position operator+(const Position &val) const
	{
		Position res;

		res.x = x + val.x;
		res.y = y + val.y;
		res.z = z + val.z;

	    return res;
	}

	Position operator-(const Position &val) const
	{
		Position res;

		res.x = x - val.x;
		res.y = y - val.y;
		res.z = z - val.z;

	    return res;
	}

	point3d_t to_point3d() const
	{
		point3d_t p;

		p << x << y << z << arma::endr;

		return p;
	}

} position_t, orientation_vector_t;

typedef struct OrientationAngles
{
	float az;	///< Azimuth angle (in degrees)
	float el;	///< Elevation angle (in degrees)
	float ro;	///< Roll angle (in degrees)

	OrientationAngles()
	{
		az = 0.0f;
		el = 0.0f;
		ro = 0.0f;
	}

	OrientationAngles(float az_val, float el_val, float ro_val = 0)
	{
		az = az_val;
		el = el_val;
		ro = ro_val;
	}

	OrientationAngles operator+(const OrientationAngles &val) const
	{
		OrientationAngles res;

		// azimuth (-180, +180]
		float az2 = az + val.az;

		if (az2 > 180)
			res.az = az2 - 360;
		else if (az < -180)
			res.az = az2 + 360;
		else
			res.az = az2;

		// elevation [-90, +90]
		float el2 = el + val.el;

		if (el2 > 90)
			res.el = 180 - el2;
		else if (el < -90)
			res.el = -180 - el2;
		else
			res.el = el2;

	    return res;
	}

	OrientationAngles operator-(const OrientationAngles &val) const
	{

		OrientationAngles res;

		// azimuth (-180, +180]
		float az2 = az - val.az;

		if (az2 > 180)
			res.az = az2 - 360;
		else if (az2 < -180)
			res.az = az2 + 360;
		else
			res.az = az2;

		// elevation [-90, +90]
		float el2 = el - val.el;

		if (el2 > 90)
			res.el = 180 - el2;
		else if (el2 < -90)
			res.el = -180 - el2;
		else
			res.el = el2;

	    return res;
	}
} orientation_angles_t;

typedef struct TrackerData
{
	position_t pos;
	orientation_angles_t ori;
	unsigned long timestamp;

	TrackerData()
	{
		timestamp = 0;
	}

	TrackerData operator-(const TrackerData &val) const
	{
		TrackerData res;

//		res.pos = pos - val.pos;
//		res.ori = ori - val.ori;

		// azimuth (-180, +180]
		float az = ori.az - val.ori.az;

		if (az > 180)
			res.ori.az = az - 360;
		else if (az < -180)
			res.ori.az = az + 360;
		else
			res.ori.az = az;

		// elevation [-90, +90]
		float el = ori.el - val.ori.el;

		if (el > 90)
			res.ori.el = 180 - el;
		else if (el < -90)
			res.ori.el = -180 - el;
		else
			res.ori.el = el;

		return res;
	}

	TrackerData operator+(const TrackerData &val) const
	{
		TrackerData res;

		res.pos = pos + val.pos;
		res.ori = ori + val.ori;

		return res;
	}

} trackerdata_t;

}  // namespace


// Definitions for all the system

// for linear algebra
#define X	0
#define Y	1
#define Z	2

/// Sample rate
#define SAMPLE_RATE			44100

/// Number of output channels
#define N_CHANNELS			2

/// Samples of system processing buffer
#define BUFFER_SAMPLES		512

#define VS_SAMPLES			256  // 256 samples per VS

// Preprocessor macros

/// Write a verbose message to stderr.
#define VERBOSE(format, ...) \
	fprintf(stderr, "VERBOSE [%s:%d]: " format "\n", \
							__FILE__, __LINE__, ## __VA_ARGS__)

/// Write a warning message to stderr.
#define WARNING(format, ...) \
	fprintf(stderr, "WARNING [%s:%d]: " format "\n", \
							__FILE__, __LINE__, ## __VA_ARGS__)

/// Write an error message to stderr.
#define ERROR(format, ...) \
	fprintf(stderr, "ERROR [%s:%d]: " format "\n", \
							__FILE__, __LINE__, ## __VA_ARGS__)

/// Write debug message to stderr
#ifndef NDEBUG
#define DPRINT(format, ...)	\
	fprintf(stderr, "DEBUG [%s:%d]: " format "\n", \
								__FILE__, __LINE__, ## __VA_ARGS__)
#else
#define DPRINT(format, ...)  // nothing
#endif


#define HRTF_IIR
//#define RTCONV_THREADS
//#define VSFILTER_THREADS

#endif // COMMON_HPP_
