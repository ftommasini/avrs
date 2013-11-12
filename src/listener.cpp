/*
 * Copyright (C) 2009-2013 Fabián C. Tommasini <fabian@tommasini.com.ar>
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
 * @file listener.cpp
 */

#include "listener.hpp"

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

	if (!p_tmp->_init())
		p_tmp.reset();

	return p_tmp;
}

bool Listener::_init()
{
	return true;
}

void Listener::set_initial_point_of_view(const avrs::orientation_angles_t &o, const avrs::point3d_t &p)
{
    // Initial Rotation matrix
    _R0 = _calculate_rotation_matrix(o);

	 // Initial Translation matrix
	_T0 << 1 << 0 << 0 << p(0) << endr
	    << 0 << 1 << 0 << p(1) << endr
	    << 0 << 0 << 1 << p(2) << endr
	    << 0 << 0 << 0 << 1    << endr;

	// Transformation matrix
	_Tr = _R0 * _T0;
}

void Listener::set_orientation_reference(const avrs::orientation_angles_t &o)
{
	_ori_ref = o;
	_ori = _ori_ref;
}

void Listener::set_position_reference(const avrs::point3d_t &p)
{
	_pos_ref = p;
	_pos = _pos_ref;
}
