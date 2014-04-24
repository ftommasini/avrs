/*
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

#include "avrsexception.hpp"
#include "room.hpp"

namespace avrs
{

Room::Room(configuration_t::ptr_t config)
{
	_config = config;
	_volume = _config->volume;
	_area = 0.0f;
	_new_surface = true;
	load_dxf();
}

Room::~Room()
{
	;
}

void Room::load_dxf()
{
	DxfReader::ptr_t reader(new DxfReader());
	boost::shared_ptr<DL_Dxf> dxf(new DL_Dxf());
	const char *filename = _config->dxf_file.c_str();

	if (!dxf->in(filename, reader.get()))  // if file open failed
		throw AvrsException("Error loading DXF file");

	_surfaces = reader->get_surfaces();
	_update_data();
}

float Room::total_area() const
{
	return _area;
}

float Room::volume() const
{
	return _volume;
}

unsigned int Room::n_surfaces() const
{
	return (unsigned int) _surfaces.size();
}

void Room::add_surface(Surface::ptr_t s)
{
	_surfaces.push_back(s);
	_new_surface = true;
}

Surface::ptr_t Room::get_surface(int i)
{
	return _surfaces[i];
}

// Private functions

// Update room area and filter coefficients for each surface
void Room::_update_data()
{
	if (_new_surface)
	{
		_update_area();

		#pragma omp parallel for
		for (unsigned int i = 0; i < _surfaces.size(); i++)
		{
			Surface::ptr_t s = _surfaces[i];

			if (_config->b_coeff.size() > 0)
				s->set_b_filter_coeff(_config->b_coeff[i]);

			if (_config->a_coeff.size() > 0)
				s->set_a_filter_coeff(_config->a_coeff[i]);
		}

		_new_surface = false;
	}
}

void Room::_update_area()
{
	_area = 0.0f;

	for (unsigned int i = 0; i < _surfaces.size(); i++)
	{
		Surface::ptr_t s = _surfaces[i];
		_area += s->get_area(); // sum areas of all surfaces
	}
}

}  // namespace avrs

