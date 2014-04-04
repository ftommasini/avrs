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

Room::Room()
{
	_area = 0.0f;
}

Room::~Room()
{
//	for (surfaces_it_t it = _surfaces.begin(); it != _surfaces.end(); it++)
//	{
//		Surface::ptr_t s = *it;
//		delete s;
//	}
}

void Room::load_dxf(std::string dxf_filename)
{
	DxfReader::ptr_t reader(new DxfReader());
	boost::shared_ptr<DL_Dxf> dxf(new DL_Dxf());
	const char *filename = dxf_filename.c_str();

	if (!dxf->in(filename, reader.get()))  // if file open failed
		throw AvrsException("Error loading DXF file");

	_surfaces = reader->get_surfaces();
	_update_area();
}

float Room::get_total_area() const
{
	return _area;
}

unsigned int Room::n_surfaces() const
{
	return (unsigned int) _surfaces.size();
}

void Room::add_surface(Surface::ptr_t s)
{
	_surfaces.push_back(s);
	_area += s->get_area();
}

Surface::ptr_t Room::get_surface(int i)
{
	return _surfaces[i];
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

}  // namespace ismx
//
