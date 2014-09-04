/*
 * Copyright (C) 2009-2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <ANN/ANN.h>

#include "common.hpp"

namespace avrs {

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
class HrtfCoeffSet
{
public:
	typedef boost::shared_ptr<HrtfCoeffSet> ptr_t;

	~HrtfCoeffSet();

	/// Static factory function for HrtfSet objects
	static ptr_t create(std::string filename);

	//void get_HRTF_coeff(hrtfcoeff_t *val, float az, float el);
	void get_HRTF_coeff(hrtfcoeff_t *val, point3d_t point_L);

private:
	HrtfCoeffSet(std::string filename);

	bool _load();
	void _build_kd_tree();
	void _allocate_memory();
	void _deallocate_memory();

	std::string _filename;

	uint _n_hrtf;  // number of HRTFs
	float *_az;  // array with azimuth values
	float *_el;  // array with elevation values
	int *_itd;  // array with ITD values
	double **_points;  // matrix with coordinates of points in space where HRTF is loaded

	uint _order;  // order of filters
	uint _n_coeff;  // must be: order + 1
	// left ear coefficients
	double **_b_left;
	double **_a_left;
	// right ear coefficients
	double **_b_right;
	double **_a_right;

	uint _n_az;  ///< number of unique azimuth values
	std::vector<float> _az_values;  ///< array with unique azimuth values

	uint _n_el;   ///< number of unique elevation values
	std::vector<float> _el_values;  ///< array with unique elevation values

	// kd-tree for nearest neighbor search
	ANNkd_tree* _kd_tree;
};

}  // namespace avrs

#endif // HEADFILTER_HPP_
