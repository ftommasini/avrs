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

#ifndef ROOM_HPP_
#define ROOM_HPP_

#include <exception>
#include <boost/shared_ptr.hpp>
#include <dxflib/dl_dxf.h>

#include "surface.hpp"
#include "dxfreader.hpp"
#include "configuration.hpp"

namespace avrs
{

class Room
{
public:
	typedef boost::shared_ptr<Room> ptr_t;

	Room(configuration_t::ptr_t config);
	virtual ~Room();

	float total_area() const;
	float volume() const;
	unsigned int n_surfaces() const;
	void add_surface(Surface::ptr_t s);
	void load_dxf();
	Surface::ptr_t get_surface(int i);

private:
	typedef std::vector<Surface::ptr_t>::iterator surfaces_it_t;

	configuration_t::ptr_t _config;
	std::vector<Surface::ptr_t> _surfaces;
	bool _new_surface;  // flag that indicates new surface data
	float _area;
	float _volume;

	void _update_data();
	void _update_area();
};

}  // namespace avrs

#endif /* ROOM_HPP_ */
