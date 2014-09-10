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


/**
 * @file listener.hpp
 */

#ifndef LISTENER_HPP_
#define LISTENER_HPP_

#include <cmath>
#include <armadillo>
#include <boost/shared_ptr.hpp>

#include "utils/math.hpp"
#include "common.hpp"

namespace avrs
{

using namespace arma;

class Listener
{
public:
	typedef boost::shared_ptr<Listener> ptr_t;

	virtual ~Listener();
    static ptr_t create();

    void set_initial_POV(const avrs::orientationangles_t &o, const avrs::point3_t &p);
    avrs::orientationangles_t &get_orientation();
    void rotate(const avrs::orientationangles_t &o);
    avrs::point3_t &get_position();  // TODO position_t
    void translate(const avrs::point3_t &p);  // from reference position

    matrix4_t &get_rotation_matrix();
    //matrix4_t &get_translation_matrix();
//    matrix4_t &get_transformation_matrix();

private:
	Listener();

	avrs::orientationangles_t _ori;
	avrs::point3_t _pos;  // in room reference system

	matrix4_t _R0;  // Initial Rotation matrix
	matrix4_t _T0;  // Initial Translation matrix
	matrix4_t _Tr0;  // Initial Transformation matrix

	matrix4_t _Rc;  // Current Rotation matrix
	matrix4_t _Tc;  // Current Translation matrix
};

inline void Listener::rotate(const avrs::orientationangles_t &o)
{
	matrix4_t Ri = avrs::math::rotation_matrix_from_angles(o);  // ZXZ
	_Rc = Ri * _Tr0;

	// Euler angles ZXZ (in degrees)
	int sign1 = (_Rc(0,1) >= 0 ? 1 : -1);
	_ori.az = sign1 * (acos(_Rc(0,0)) * 180.0) / M_PI;
	int sign2 = (_Rc(1,2) >= 0 ? 1 : -1);
	_ori.el = sign2 * (acos(_Rc(2,2)) * 180.0) / M_PI;
	_ori.ro = 0;  // always zero
}

inline void Listener::translate(const avrs::point3_t &p)
{
	matrix4_t Ti;
	Ti = avrs::math::translation_matrix_from_vector(p);
	_Tc = Ti * _Tr0;

	_pos = p;
}

inline avrs::orientationangles_t &Listener::get_orientation()
{
	return _ori;
}

inline avrs::point3_t &Listener::get_position()
{
	return _pos;
}

inline matrix4_t &Listener::get_rotation_matrix()
{
	return _Rc;
}

//inline matrix4_t &Listener::get_translation_matrix()
//{
//	return _Tc;
//}

//inline matrix4_t &Listener::get_transformation_matrix()
//{
//	return _Rc * _Tc;
//}

}  // namespace avrs

#endif // LISTENER_HPP_
