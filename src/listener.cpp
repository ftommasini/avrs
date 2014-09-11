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

#include "listener.hpp"

namespace avrs
{

Listener::Listener()
{
	;  // nothing to do
}

Listener::~Listener()
{
	;  // nothing to do
}

Listener::ptr_t Listener::create()
{
	ptr_t p_tmp(new Listener());
	return p_tmp;
}

void Listener::set_initial_POV(const orientationangles_t &o, const point3_t &p)
{
    // Initial rotation
    _R0 = avrs::math::angles_2_rotation_matrix(o);
	 // Initial position
    _pos0 = p;
}

}  // namespace avrs
