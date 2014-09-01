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

#include <fstream>

#include "avrsexception.hpp"
#include "airabsorption.hpp"

namespace avrs
{

AirAbsorption::AirAbsorption(std::string filename)
	: _filename(filename)
{
	;
}

AirAbsorption::~AirAbsorption()
{
	_deallocate_memory();;
}

AirAbsorption::ptr_t AirAbsorption::create(std::string filename)
{
	ptr_t p_tmp(new AirAbsorption(filename));

	if (!p_tmp->_load())
		throw avrs::AvrsException("Initialization fail");

	return p_tmp;
}

bool AirAbsorption::_load()
{
	bool ok = true;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit );

	try {
		file.open(_filename.c_str(), std::ios::in | std::ios::binary);

		file.read(reinterpret_cast<char *>(&_n_distances), sizeof(unsigned int));
		file.read(reinterpret_cast<char *>(&_order), sizeof(unsigned int));
		_n_coeff = _order + 1;

		// Allocate memory for this data
		_allocate_memory();

		std::vector<float>::iterator it;

		for (uint i = 0; i < _n_distances; i++)
		{
			// Azimuth value
			float dist;
			file.read(reinterpret_cast<char *>(&dist), sizeof(float));
			_map_index[dist] = i;

			// Filter coefficients
			file.read(reinterpret_cast<char *>(&_b[i][0]), sizeof(double) * _n_coeff);
			file.read(reinterpret_cast<char *>(&_a[i][0]), sizeof(double) * _n_coeff);
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

void AirAbsorption::_allocate_memory()
{
	uint i, j;

	// Allocate memory
	_b = (double **) malloc(sizeof(double) * _n_distances);
	_a = (double **) malloc(sizeof(double) * _n_distances);

	for (i = 0; i < _n_distances; i++)
	{
		_b[i] = (double *) malloc(sizeof(double) * _n_coeff);
		_a[i] = (double *) malloc(sizeof(double) * _n_coeff);
	}

	// Clear arrays
	for (i = 0; i < _n_distances; i++)
	{
		for (j = 0; j < _n_coeff; j++)
		{
			_b[i][j] = 0.0;
			_a[i][j] = 0.0;
		}
	}
}

void AirAbsorption::_deallocate_memory()
{
	uint i;

	// De-allocate memory
	for (i = 0; i < _n_distances; i++)
	{
		free(_b[i]);
		free(_a[i]);
	}

	free(_b);
	free(_a);
}

} /* namespace avrs */
