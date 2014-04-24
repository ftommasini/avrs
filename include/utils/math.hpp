/*
 * Copyright (c) 2006-2010 Quality & Usability Lab
 *                         Deutsche Telekom Laboratories, TU Berlin
 *                         Ernst-Reuter-Platz 7, 10587 Berlin, Germany
 *                         <SoundScapeRenderer@telekom.de>
 * Copyright (C) 2011-2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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
 * @file math.hpp
 * @brief
 * Stores mathematical constants and helper functions.
 **/

#ifndef MATH_HPP_
#define MATH_HPP_

#include <limits>
#include <cmath>
#include <armadillo>

#include "common.hpp"

#define _USE_MATH_DEFINES

namespace avrs
{

/// Contains mathematical variables, constants and helper functions
namespace math
{

/// \f$\pi\f$ (@b float representation).
/// @warning if you need @b double precision, you should change this!
// 9 digits are needed for float, 17 digits for double
const float PI = M_PI;
const float PIdiv180 = PI / 180.0f; ///< \f$\pi\f$ divided by 180
const float PIdiv180_inverse = 180.0f / PI; ///< 180 divided by \f$\pi\f$

/// @name dB2linear()
/// three overloaded versions for float, double and long double.
//@{
/** Convert a level in decibel to a linear gain factor.
 * @param L level
 * @param power if @b true, a factor of 10 is used, if @b false (default), the
 * factor is 20.
 * @return the linear counterpart to @a L.
 **/
inline float dB2linear(float L, bool power = false)
{
	float f = 0.05f;

	if (power)
		f = 0.1f;

	return pow(10.0f, L * f);
}

inline double dB2linear(double L, bool power = false)
{
	double f = 0.05;

	if (power)
		f = 0.1;

	return pow(10.0, L * f);
}

inline long double dB2linear(long double L, bool power = false)
{
	long double f = 0.05l;

	if (power)
		f = 0.1l;

	return pow(10.0l, L * f);
}
//@}

/// @name linear2dB()
/// three overloaded versions for float, double and long double.
//@{
/** Convert a linear gain factor to a level in dB.
 * @param x gain factor
 * @param power if @b true, a factor of 10 is used, if @b false (default), the
 * factor is 20.
 * @return the logarithmic counterpart to @a x.
 * @attention returns -Inf for x=0 and NaN for x<0.
 **/
inline float linear2dB(float x, bool power = false)
{
	float f = 20.0f;

	if (power)
		f = 10.0f;

	return f * log10(x);
}

inline double linear2dB(double x, bool power = false)
{
	double f = 20.0;

	if (power)
		f = 10.0;

	return f * log10(x);
}

inline long double linear2dB(long double x, bool power = false)
{
	long double f = 20.0l;

	if (power)
		f = 10.0l;

	return f * log10(x);
}

/// @name deg2rad()
/// three overloaded versions for float, double and long double.
//@{
/** Convert an angle in degrees to an angle in radians.
 * @param angle dito
 * @return angle in radians.
 **/
inline float deg2rad(float angle)
{
	return angle * PIdiv180;
}

inline double deg2rad(double angle)
{
	return angle * PIdiv180;
}

inline long double deg2rad(long double angle)
{
	return angle * PIdiv180;
}
//@}

/// @name rad2deg()
/// three overloaded versions for float, double and long double.
//@{
/** Convert an angle in radians to an angle in degrees.
 * @param angle dito
 * @return angle in degrees.
 **/
inline float rad2deg(float angle)
{
	return angle * PIdiv180_inverse;
}

inline double rad2deg(double angle)
{
	return angle * PIdiv180_inverse;
}

inline long double rad2deg(long double angle)
{
	return angle * PIdiv180_inverse;
}
//@}

/// @name wrap_angle()
/// three overloaded versions for float, double and long double.
//@{
/** Wrap an angle into the interval from 0 deg to 360- deg.
 * @param angle dito in degreess
 * @return wrapped angle in degrees.
 **/
inline float wrap_angle(float angle)
{
	angle = std::fmod(angle, 360.0f);

	if (angle < 0.0f)
		angle += 360.0f;

	return angle;
}

inline double wrap_angle(double angle)
{
	angle = std::fmod(angle, 360.0);

	if (angle < 0.0)
		angle += 360.0;

	return angle;
}

inline long double wrap_angle(long double angle)
{
	angle = std::fmod(angle, 360.0l);

	if (angle < 0.0l)
		angle += 360.0l;

	return angle;
}
//@}

/**
 * Rounds each element of X to the nearest multiple of 10^N.
 * N must be scalar, and integer-valued.
 * @see Matlab version of this function
 */
inline float roundn(float x, int n)
{
	// Compute the power of 10 to which input will be rounded.
	float p = pow(10, n);

	// Round x to the nearest multiple of p.
	return (float) (p * round(x / p));
}

inline float speed_of_sound(float temp)
{
	return 331.4 * sqrt(1 + (temp / 273));
}

inline arma::mat::fixed<4,4> rotation_matrix_from_angles(const avrs::orientation_angles_t &o)
{
    // To radians
    double az = (o.az * PI) / 180.0;
    double el = (o.el * PI) / 180.0;

	// Rotation matrix ZXZ convention
	arma::mat::fixed<4,4> R;  // 4 x 4 matrix
	double sin_az = sin(az);
	double cos_az = cos(az);
	double sin_el = sin(el);
	double cos_el = cos(el);
	R << cos_az           << sin_az           << 0      << 0 << arma::endr
	  << -sin_az * cos_el << cos_az * cos_el  << sin_el << 0 << arma::endr
	  << sin_az * sin_el  << -cos_az * sin_el << cos_el << 0 << arma::endr
	  << 0                << 0                << 0      << 1 << arma::endr;

	return R;
}

inline void polar2rectangular(float az_deg, float el_deg, double *point)
{
	// vertical-polar to rectangular conversion
	float az_r = deg2rad(az_deg);
	float el_r = deg2rad(el_deg);
	point[X] = cos(az_r) * cos(el_r);
	point[Y] = sin(az_r) * cos(el_r);
	point[Z] = sin(el_r);
}

inline void rectangular2polar(double *point, float *az_deg, float *el_deg)
{
	// rectangular to vertical-polar conversion
	float az_r = (float)atan2(point[Y], point[X]);
	float el_r = (float)atan2(point[Z], sqrt(point[X] * point[X] + point[Y] * point[Y]));
	*az_deg = rad2deg(az_r);
	*el_deg = rad2deg(el_r);
}

// normalized sinc function
inline double sinc(double x)
{
	if (x == 0.0)
		return 1;

	return sin(PI * x) / (PI * x);
}

}  // namespace math

}  // namespace avrs

#endif  // MATH_HPP_
