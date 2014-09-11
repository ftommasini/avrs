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

#include <cstring>
#include <fstream>

#include "headfilter.hpp"
#include "avrsexception.hpp"
#include "utils/math.hpp"

namespace avrs
{

HrtfCoeffSet::HrtfCoeffSet(std::string filename)
	: _filename(filename)
{
	;
}

HrtfCoeffSet::~HrtfCoeffSet()
{
	delete _kd_tree;
	_deallocate_memory();
	annClose();
}

HrtfCoeffSet::ptr_t HrtfCoeffSet::create(std::string filename)
{
	ptr_t p_tmp(new HrtfCoeffSet(filename));

	if (!p_tmp->_load())
		throw avrs::AvrsException("Initialization fail");

	// build kd-tree
	p_tmp->_build_kd_tree();

	return p_tmp;
}

void HrtfCoeffSet::get_HRTF_coeff(hrtfcoeff_t *val, point3_t point_L)
{
	assert(val != NULL);

	// vertical-polar to rectangular conversion
	double point[3];
	point[X] = point_L(X);
	point[Y] = point_L(Y);
	point[Z] = point_L(Z);

	// search in kd-tree
	const int k = 1;
	ANNidxArray nnIdx = new ANNidx[k];
	ANNdistArray dists = new ANNdist[k];

	_kd_tree->annkSearch(
			point,
			k,
			nnIdx,
			dists);

	int idx = nnIdx[0];  // index of nearest neighbor

	// copy data
	val->itd = _itd[idx];
	memcpy(&val->b_left[0], &_b_left[idx][0], sizeof(double) * _n_coeff);
	memcpy(&val->a_left[0], &_a_left[idx][0], sizeof(double) * _n_coeff);
	memcpy(&val->b_right[0], &_b_right[idx][0], sizeof(double) * _n_coeff);
	memcpy(&val->a_right[0], &_a_right[idx][0], sizeof(double) * _n_coeff);

	float az, el;
	avrs::math::rectangular_2_polarAVRS(point, &az, &el);
	DPRINT("\tAz: %+1.3f [%+1.3f]\t El: %+1.3f [%+1.3f]\t ITD: %s %d samples",
			az, _az[idx], el, _el[idx],
			val->itd >= 0 ? "L" : "R", val->itd >= 0 ? val->itd : -(val->itd));
}

bool HrtfCoeffSet::_load()
{
	bool ok = true;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		file.open(_filename.c_str(), std::ios::in | std::ios::binary);

		// AVRS format
		char format[4 + 1];
		file.read(format, 4);
		format[4] = '\0';
		std::string sformat(format);

		if (sformat.compare("AVRS") != 0)
			throw std::string("Wrong file format");

		// Version of AVRS format
		unsigned short version;
		file.read(reinterpret_cast<char *>(&version), sizeof(unsigned short));

		if (version != 4)  // format version 4
			throw std::string("Wrong format version");

		// Type of data
		unsigned short data_type;
		file.read(reinterpret_cast<char *>(&data_type), sizeof(unsigned short));
		// nothing to do...

		// Name of data
		unsigned int name_length;
		file.read(reinterpret_cast<char *>(&name_length), sizeof(unsigned int));
		char name[name_length + 1];
		file.read(name, name_length);
		name[name_length] = '\0';
		std::string sname(name);

		file.read(reinterpret_cast<char *>(&_n_az), sizeof(unsigned int));
		file.read(reinterpret_cast<char *>(&_n_el), sizeof(unsigned int));
		file.read(reinterpret_cast<char *>(&_n_hrtf), sizeof(unsigned int));
		file.read(reinterpret_cast<char *>(&_order), sizeof(unsigned int));
		_n_coeff = _order + 1;

		// Allocate memory for this data
		_allocate_memory();

		std::vector<float>::iterator it;

		for (uint i = 0; i < _n_hrtf; i++)
		{
			// Azimuth value
			float az;
			file.read(reinterpret_cast<char *>(&az), sizeof(float));
			_az[i] = az;
			it = std::find(_az_values.begin(), _az_values.end(), az);

			if (it == _az_values.end())  // not found
				_az_values.push_back(az);  // added

			// Elevation value
			float el;
			file.read(reinterpret_cast<char *>(&el), sizeof(float));
			_el[i] = el;
			it = std::find(_el_values.begin(), _el_values.end(), el);

			if (it == _el_values.end())  // not found
				_el_values.push_back(el);  // added

			// Coordinate of points in space
			file.read(reinterpret_cast<char *>(&_points[i][0]), sizeof(double) * 3);  // 3D point

			// ITD value
			file.read(reinterpret_cast<char *>(&_itd[i]), sizeof(int));

			// Left ear
			file.read(reinterpret_cast<char *>(&_b_left[i][0]), sizeof(double) * _n_coeff);
			file.read(reinterpret_cast<char *>(&_a_left[i][0]), sizeof(double) * _n_coeff);

			// Right ear
			file.read(reinterpret_cast<char *>(&_b_right[i][0]), sizeof(double) * _n_coeff);
			file.read(reinterpret_cast<char *>(&_a_right[i][0]), sizeof(double) * _n_coeff);
		}

		file.close();
		ok = true;
	}
	catch (std::ifstream::failure ex)
	{
		std::cout << ex.what() << std::endl;
		ok = false;
	}
	catch (std::string ex)
	{
		std::cout << ex << std:: endl;
		ok = false;
	}

	return ok;
}

void HrtfCoeffSet::_build_kd_tree()
{
	_kd_tree = new ANNkd_tree(	// build search structure
				_points,		// the data points
				_n_hrtf,		// number of points
				3);				// dimensions of space
}

void HrtfCoeffSet::_allocate_memory()
{
	uint i, j;

	// Allocate memory
	_b_left = (double **) malloc(sizeof(double) * _n_hrtf);
	_a_left = (double **) malloc(sizeof(double) * _n_hrtf);
	_b_right = (double **) malloc(sizeof(double) * _n_hrtf);
	_a_right = (double **) malloc(sizeof(double) * _n_hrtf);

	for (i = 0; i < _n_hrtf; i++)
	{
		_b_left[i] = (double *) malloc(sizeof(double) * _n_coeff);
		_a_left[i] = (double *) malloc(sizeof(double) * _n_coeff);
		_b_right[i] = (double *) malloc(sizeof(double) * _n_coeff);
		_a_right[i] = (double *) malloc(sizeof(double) * _n_coeff);
	}

	_points = (double **) malloc(sizeof(double) * _n_hrtf);

	for (i = 0; i < _n_hrtf; i++)
		_points[i] = (double *) malloc(sizeof(double) * 3);  // 3D point

	_az = (float *) malloc(sizeof(float) * _n_hrtf);
	_el = (float *) malloc(sizeof(float) * _n_hrtf);
	_itd = (int *) malloc(sizeof(int) * _n_hrtf);

	// Clear arrays
	for (i = 0; i < _n_hrtf; i++)
	{
		for (j = 0; j < _n_coeff; j++)
		{
				// left
				_b_left[i][j] = 0.0;
				_a_left[i][j] = 0.0;
				// right
				_b_right[i][j] = 0.0;
				_a_right[i][j] = 0.0;
		}
	}

	for (i = 0; i < _n_hrtf; i++)
	{
		_points[i][0] = 0.0;
		_points[i][1] = 0.0;
		_points[i][2] = 0.0;

		_az[i] = 0.0f;
		_el[i] = 0.0f;
		_itd[i] = 0;
	}
}

void HrtfCoeffSet::_deallocate_memory()
{
	uint i;

	// De-allocate memory
	for (i = 0; i < _n_hrtf; i++)
	{
		free(_b_left[i]);
		free(_a_left[i]);
		free(_b_right[i]);
		free(_a_right[i]);
	}

	free(_b_left);
	free(_a_left);
	free(_b_right);
	free(_a_right);

	for (i = 0; i < _n_hrtf; i++)
		free(_points[i]);

	free(_points);

	free(_az);
	free(_el);
	free(_itd);
}

}  // namespace avrs
