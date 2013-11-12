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

#ifndef HEADFILTER_HPP_
#define HEADFILTER_HPP_

#include <map>
#include <algorithm>
#include <cmath>
#include <memory>

#include "common.hpp"

// Find HRIR implementation in previous versions

typedef struct
{
	avrs::complex_t *left;
	avrs::complex_t *right;
	uint length;
} hrtf_t;

/**
 * HRTF Set for listener model
 */
class HrtfSet
{
public:
	typedef std::auto_ptr<HrtfSet> ptr_t;

	~HrtfSet();
	/// Static factory function for HrtfSet objects
	static ptr_t create(std::string filename);

	void get_HRTF(hrtf_t *val, float az, float el);

private:
	typedef std::map<float, uint> map_t;

	HrtfSet(std::string filename);

	/// Initialize the HRTF Set
	/// \return \b true if successful, \b false otherwise
	bool _init();

	typedef struct CompareDistance
	{
		CompareDistance(float val) :
			_val(val)
		{
			;
		}

		bool operator()(float lhs, float rhs)
		{
			return std::abs(_val - lhs) < std::abs(_val - rhs);
		}

	private:
		float _val;
	} cmpdistance_t;

	uint _get_closest(float val, float *array, uint n)
	{
		float *result = std::min_element(array, array + n, cmpdistance_t(val));
		return (result - array); // returns the index
	}

	void _allocate_memory();
	void _deallocate_memory();

	std::string _filename; ///< Filename of HRTF set in AVRS format

	map_t _az_map; ///< map of azimuths (azimuth, index)
	map_t _el_map; ///< map of elevations (elevation, index)

	float *_az_values; ///< array with the azimuth values
	uint _n_az; ///< number of azimuth values

	float *_el_values; ///< array with the elevation values
	uint _n_el; ///< number of elevation values

	avrs::complex_t ***_hrtf_l; ///< cube of all HRTFs for left ear
	avrs::complex_t ***_hrtf_r; ///< cube of all HRTFs for right ear
	uint _n_sc; ///< number of spectral components of HRTFs

//	float _previous_az; ///< Previous azimuth value
//	float _previous_el; ///< Previous elevation value
};

inline void HrtfSet::get_HRTF(hrtf_t *h, float az, float el)
{
	assert(h != NULL);

//	DPRINT("\tAz: %+1.4f [%+1.4f]\tEl: %+1.4f [%+1.4f]",
//			_az_values[az_idx], az, _el_values[el_idx], el);

	// if is in-range (1 degree), use the previous values
//	if (abs(_previous_az - az) < 1.0f && abs(_previous_el - el) < 1.0f)
//		return;

	uint az_idx = _get_closest(az, _az_values, _n_az);
	uint el_idx = _get_closest(el, _el_values, _n_el);

//	DPRINT("\tAz: %+1.3f [%+1.3f] e: %+1.3f\tEl: %+1.3f [%+1.3f] e: %+1.3f",
//			_az_values[az_idx], az, (az - _az_values[az_idx]),
//			_el_values[el_idx], el, (el - _el_values[el_idx]));

	h->left = &_hrtf_l[az_idx][el_idx][0];
	h->right = &_hrtf_r[az_idx][el_idx][0];
	h->length = _n_sc;

//	_previous_az = az;
//	_previous_el = el;
}



typedef struct HrtfCoeff
{
	std::vector<double> b_left;
	std::vector<double> a_left;
	std::vector<double> b_right;
	std::vector<double> a_right;
	uint length;
	int itd;

	HrtfCoeff(uint len = 21)
	{
		length = len;
		itd = 0;
		b_left.resize(length);
		a_left.resize(length);
		b_right.resize(length);
		a_right.resize(length);
	}
} hrtfcoeff_t;

/**
 * HRTF coefficients set for listener model
 * For use with IIR filters (Steiglitz-McBride method)
 */
class HrtfCoeffSet  // TODO inherit class for both class
{
public:
	typedef std::auto_ptr<HrtfCoeffSet> ptr_t;
	~HrtfCoeffSet();

	/// Static factory function for HrtfSet objects
	static ptr_t create(std::string filename);

	void get_HRTF_coeff(hrtfcoeff_t *val, float az, float el);

private:
	typedef std::map<float, uint> map_t;

	HrtfCoeffSet(std::string filename);
	bool _init();
	void _allocate_memory();
	void _deallocate_memory();

	typedef struct CompareDistance
	{
		CompareDistance(float val) :
			_val(val)
		{
			;
		}

		bool operator()(float lhs, float rhs)
		{
			return std::abs(_val - lhs) < std::abs(_val - rhs);
		}

	private:
		float _val;
	} cmpdistance_t;

	uint get_closest(float val, float *array, uint n)
	{
		float *result = std::min_element(array, array + n, cmpdistance_t(val));
		return (result - array); // returns the index
	}

	std::string _filename;

	map_t _az_map;  ///< map of azimuths (azimuth, index)
	map_t _el_map;  ///< map of elevations (elevation, index)

	// left ear
	double ***_b_left; // cube
	double ***_a_left; // cube
	// right ear
	double ***_b_right; // cube
	double ***_a_right; // cube

	int *_itd;

	float *_az_values;  ///< array with the azimuth values
	uint _n_az;  ///< number of azimuth values

	float *_el_values;  ///< array with the elevation values
	uint _n_el;   ///< number of elevation values

	uint _order;
	uint _n_coeff;  // must be: order + 1

//	float _previous_az; ///< Previous azimuth value
//	float _previous_el; ///< Previous elevation value
};

inline void HrtfCoeffSet::get_HRTF_coeff(hrtfcoeff_t *val, float az, float el)
{
	assert(val != NULL);

	// if not changed, use the previous values
//	if (az == _previous_az && el == _previous_el)
//		return;

	uint az_index = get_closest(az, _az_values, _n_az);
	uint el_index = get_closest(el, _el_values, _n_el);
	int itd = _itd[az_index * _n_el + el_index];

//	DPRINT("\tAz: %+1.4f [%+1.4f]\tEl: %+1.4f [%+1.4f]\t ITD: add to %s %d samples",
//			_az_values[azIndex], az, _el_values[elIndex], el,
//			itd >= 0 ? "L" : "R", itd >= 0 ? itd : -itd);

	memcpy(&val->b_left[0], &_b_left[az_index][el_index][0], sizeof(double) * _n_coeff);
	memcpy(&val->a_left[0], &_a_left[az_index][el_index][0], sizeof(double) * _n_coeff);
	memcpy(&val->b_right[0], &_b_right[az_index][el_index][0], sizeof(double) * _n_coeff);
	memcpy(&val->a_right[0], &_a_right[az_index][el_index][0], sizeof(double) * _n_coeff);
	val->itd = itd;

//	_previous_az = az;
//	_previous_el = el;
}


#endif // HEADFILTER_HPP_
