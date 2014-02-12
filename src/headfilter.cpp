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

/*
 * HrtfSet
 */

HrtfSet::HrtfSet(std::string filename)
	: _filename(filename)
{
//	_previous_az = -500.0f;  // azimuth that doesn't exist
//	_previous_el = -500.0f;  // elevation that doesn't exist
}

HrtfSet::~HrtfSet()
{
	_deallocate_memory();
}

HrtfSet::ptr_t HrtfSet::create(std::string filename)
{
	ptr_t p_tmp(new HrtfSet(filename));

	if (!p_tmp->_init())
		p_tmp.release();

	return p_tmp;
}

bool HrtfSet::_init()
{
	uint i, j, k;
	FILE *p_file;
	size_t n;

	p_file = fopen(_filename.c_str(), "rb");

	if (!p_file) {
		ERROR("Error opening the HRTF database");
		return false;
	}

	char format[4 + 1];  // AVRS + null-character

	n = fread(format, sizeof(char), 4, p_file);

	if (n != 4)
		goto error;

	format[4] = '\0';  // null-character added

	if (strcmp(format, "AVRS") != 0)
	{
		ERROR("Wrong file format");
		goto error;
	}

	DPRINT("Loading HRTF database");

	// Number of azimuths to read
	n = fread(&_n_az, sizeof(uint), 1, p_file);

	if (n != 1)
		goto error;

	// Number of elevations to read
	n = fread(&_n_el, sizeof(uint), 1, p_file);

	if (n != 1)
		goto error;

	// Number of spectral components to read
	n = fread(&_n_sc, sizeof(uint), 1, p_file);

	if (n != 1)
		goto error;

	DPRINT("Nº Az: %d, Nº El: %d, Nº SC: %d", _n_az, _n_el, _n_sc);

	// Allocate memory for this
	_allocate_memory();

	n = fread(_az_values, sizeof(float), _n_az, p_file);

	if (n != _n_az)
		goto error;

	n = fread(_el_values, sizeof(float), _n_el, p_file);

	if (n != _n_el)
		goto error;

	float buffer[_n_sc * 2];  // temporal buffer for real and imaginary part

	for (i = 0; i < _n_az; i++) {
		_az_map[_az_values[i]] = i;

		for (j = 0; j < _n_el; j++) {
			if (i == 0)  // only the first azimuth cycle
				_el_map[_el_values[j]] = j;

			n = fread(buffer, sizeof(float), _n_sc * 2, p_file);  // real and imaginary part

			if (n != _n_sc * 2)
				goto error;

			// Assign left ear
			for (k = 0; k < _n_sc; k++) {
				_hrtf_l[i][j][k][0] = buffer[2 * k];  // real
				_hrtf_l[i][j][k][1] = buffer[2 * k + 1];  // imaginary
			}

			n = fread(buffer, sizeof(float), _n_sc * 2, p_file);   // real and imaginary part

			if (n != _n_sc * 2)
				goto error;

			// Assign right ear
			for (k = 0; k < _n_sc; k++) {
				_hrtf_r[i][j][k][0] = buffer[2 * k];  // real
				_hrtf_r[i][j][k][1] = buffer[2* k + 1];  // imaginary
			}
		}
	}

	fclose(p_file);

	return true;

error:   // read error management
	DPRINT("Error loading HRTF database");
	fclose(p_file);
	_deallocate_memory();
	return false;
}

void HrtfSet::_allocate_memory()
{
	uint i, j, k;

	// Allocate memory
	_hrtf_l = (avrs::complex_t ***) malloc(sizeof(avrs::complex_t) * _n_az);
	_hrtf_r = (avrs::complex_t ***) malloc(sizeof(avrs::complex_t) * _n_az);

	for (i = 0; i < _n_az; i++)  // for each elevation
	{
		_hrtf_l[i] = (avrs::complex_t **) malloc(sizeof(avrs::complex_t) * _n_el);
		_hrtf_r[i] = (avrs::complex_t **) malloc(sizeof(avrs::complex_t) * _n_el);

		for (j = 0; j < _n_el; j++)  // for each elevation
		{
			_hrtf_l[i][j] = (avrs::complex_t *) malloc(sizeof(avrs::complex_t) * _n_sc);
			_hrtf_r[i][j] = (avrs::complex_t *) malloc(sizeof(avrs::complex_t) * _n_sc);;
		}
	}

	// Clear arrays
	for (i = 0; i < _n_az; i++) {
		for (j = 0; j < _n_el; j++) {
			for (k = 0; k < _n_sc; k++) {
				// left
				_hrtf_l[i][j][k][0] = 0.0f;  // real
				_hrtf_r[i][j][k][1] = 0.0f;  // imaginary
				// right
				_hrtf_l[i][j][k][0] = 0.0f;  // real
				_hrtf_r[i][j][k][1] = 0.0f;  // imaginary
			}
		}
	}

	_az_values = (float *) malloc(sizeof(float) * _n_az);
	_el_values = (float *) malloc(sizeof(float) * _n_el);
}


void HrtfSet::_deallocate_memory()
{
	uint i, j;

	if (!_hrtf_l)  // verify previous allocation
		return;

	// De-allocate memory
	for (i = 0; i < _n_az; i++) {
		for (j = 0; j < _n_el; j++) {
			free(_hrtf_l[i][j]);
			free(_hrtf_r[i][j]);
		}

		free(_hrtf_l[i]);
		free(_hrtf_r[i]);
	}

	free(_hrtf_l);
	free(_hrtf_r);

	free(_az_values);
	free(_el_values);
}


/*
 * HrtfCoeffSet
 */

HrtfCoeffSet::HrtfCoeffSet(std::string filename)
	: _filename(filename)
{
	;
}

HrtfCoeffSet::~HrtfCoeffSet()
{
	_deallocate_memory();
}

HrtfCoeffSet::ptr_t HrtfCoeffSet::create(std::string filename)
{
	ptr_t p_tmp(new HrtfCoeffSet(filename));

	if (!p_tmp->_init())
		p_tmp.release();

	return p_tmp;
}

void HrtfCoeffSet::get_HRTF_coeff(hrtfcoeff_t *val, float az, float el)
{
	assert(val != NULL);

	// get closest azimuth value
	uint az_vector_index = get_closest(az, &_az_values[0], _n_az);
	uint az_index = _az_map[_az_values[az_vector_index]];
	// get closest elevation value
	uint el_vector_index = get_closest(el, &_el_values[0], _n_el);
	uint el_index = _el_map[_el_values[el_vector_index]];

//	bool change_el = true;
//
//	while (_a_left[az_index][el_index][0] != 1.0 || _a_right[az_index][el_index][0] != 1.0)
//	{
//		if (az_vector_index < _az_values.size() - 1)
//			az_vector_index++;
//		else
//			az_vector_index--;
//
//		if (el_vector_index < _el_values.size() - 1)
//			el_vector_index++;
//		else
//			el_vector_index--;
//
//		uint az_index = _az_map[_az_values[az_vector_index]];
//		uint el_index = _el_map[_el_values[el_vector_index]];
//	}

//	DPRINT("a[0] left: %f\t a[0] right: %f",
//			_a_left[az_index][el_index][0],
//			_a_right[az_index][el_index][0]);

	// TODO check if coefficients are not null!!!

	val->itd = _itd_map[az_index * _n_el + el_index];
	memcpy(&val->b_left[0], &_b_left[az_index][el_index][0], sizeof(double) * _n_coeff);
	memcpy(&val->a_left[0], &_a_left[az_index][el_index][0], sizeof(double) * _n_coeff);
	memcpy(&val->b_right[0], &_b_right[az_index][el_index][0], sizeof(double) * _n_coeff);
	memcpy(&val->a_right[0], &_a_right[az_index][el_index][0], sizeof(double) * _n_coeff);

	DPRINT("\tAz: %+1.3f [%+1.3f]\t El: %+1.3f [%+1.3f]\t ITD: %s %d samples",
			az, _az_values[az_index], el, _el_values[el_index],
			val->itd >= 0 ? "L" : "R", val->itd >= 0 ? val->itd : -(val->itd));
}

bool HrtfCoeffSet::_init()
{
	return _load();
}

bool HrtfCoeffSet::_load()
{
	bool ok = true;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit );

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

		if (version != 3)
			throw std::string("Wrong format version");

		// Type of data
		unsigned short data_type;
		file.read(reinterpret_cast<char *>(&data_type), sizeof(unsigned short));
		// nothing to do...

		file.read(reinterpret_cast<char *>(&_n_az), sizeof(unsigned int));
		file.read(reinterpret_cast<char *>(&_n_el), sizeof(unsigned int));
		file.read(reinterpret_cast<char *>(&_n_filters), sizeof(unsigned int));
		file.read(reinterpret_cast<char *>(&_order), sizeof(unsigned int));
		_n_coeff = _order + 1;

		// Allocate memory for this data
		_allocate_memory();

		int az_index = 0;  // index of azimuth row
		int el_index = 0;  // index of elevation column

		int az_next_index = 0;
		int el_next_index = 0;

		map_orientation_t::iterator it;

		for (unsigned int i = 0; i < _n_filters; i++)
		{
			// Azimuth value
			float az;
			file.read(reinterpret_cast<char *>(&az), sizeof(float));

			it = _az_map.find(az);

			if (it == _az_map.end())  // not found
			{
				_az_map[az] = az_next_index;
				_az_values.push_back(az);
				az_index = az_next_index;
				az_next_index++;
			}
			else  // found
				az_index = it->second;

			// Elevation value
			float el;
			file.read(reinterpret_cast<char *>(&el), sizeof(float));

			it = _el_map.find(el);

			if (it == _el_map.end())  // not found
			{
				_el_map[el] = el_next_index;
				_el_values.push_back(el);
				el_index = el_next_index;
				el_next_index++;
			}
			else  // found
				el_index = it->second;

			// ITD value
			int itd;
			file.read(reinterpret_cast<char *>(&itd), sizeof(int));
			_itd_map[az_index * _n_el + el_index] = itd;

			// Left ear
			file.read(reinterpret_cast<char *>(&_b_left[az_index][el_index][0]), sizeof(double) * _n_coeff);
			file.read(reinterpret_cast<char *>(&_a_left[az_index][el_index][0]), sizeof(double) * _n_coeff);

			// Right ear
			file.read(reinterpret_cast<char *>(&_b_right[az_index][el_index][0]), sizeof(double) * _n_coeff);
			file.read(reinterpret_cast<char *>(&_a_right[az_index][el_index][0]), sizeof(double) * _n_coeff);
		}

		file.close();
//		std::sort(_az_values.begin(), _az_values.end());
//		std::sort(_el_values.begin(), _el_values.end());
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

void HrtfCoeffSet::_allocate_memory()
{
	uint i, j, k;

	// Allocate memory
	_b_left = (double ***) malloc(sizeof(double) * _n_az);
	_a_left = (double ***) malloc(sizeof(double) * _n_az);
	_b_right = (double ***) malloc(sizeof(double) * _n_az);
	_a_right = (double ***) malloc(sizeof(double) * _n_az);

	for (i = 0; i < _n_az; i++) {
		_b_left[i] = (double **) malloc(sizeof(double) * _n_el);
		_a_left[i] = (double **) malloc(sizeof(double) * _n_el);
		_b_right[i] = (double **) malloc(sizeof(double) * _n_el);
		_a_right[i] = (double **) malloc(sizeof(double) * _n_el);

		for (j = 0; j < _n_el; j++) {
			_b_left[i][j] = (double *) malloc(sizeof(double) * _n_coeff);
			_a_left[i][j] = (double *) malloc(sizeof(double) * _n_coeff);
			_b_right[i][j] = (double *) malloc(sizeof(double) * _n_coeff);
			_a_right[i][j] = (double *) malloc(sizeof(double) * _n_coeff);
		}
	}

//	_itd = (int *) malloc(sizeof(int) * _n_az * _n_el);

	// Clear arrays
	for (i = 0; i < _n_az; i++) {
		for (j = 0; j < _n_el; j++) {
			for (k = 0; k < _n_coeff; k++) {
				// left
				_b_left[i][j][k] = 0.0;
				_a_left[i][j][k] = 0.0;
				// right
				_b_right[i][j][k] = 0.0;
				_a_right[i][j][k] = 0.0;
			}
		}
	}

//	for (i = 0; i < _n_az * _n_el; i++)
//		_itd[i] = 0;
}

void HrtfCoeffSet::_deallocate_memory()
{
	// De-allocate memory
	for (uint i = 0; i < _n_az; i++) {
		for (uint j = 0; j < _n_el; j++) {
			free(_b_left[i][j]);
			free(_a_left[i][j]);
			free(_b_right[i][j]);
			free(_a_right[i][j]);
		}

		free(_b_left[i]);
		free(_a_left[i]);
		free(_b_right[i]);
		free(_a_right[i]);
	}

	free(_b_left);
	free(_a_left);
	free(_b_right);
	free(_a_right);

//	free(_itd);
}
