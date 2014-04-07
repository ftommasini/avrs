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
 * @file listener.hpp
 */

#ifndef LISTENER_HPP_
#define LISTENER_HPP_

#include <cmath>
#include <armadillo>

#include "utils/math.hpp"
#include "common.hpp"

namespace avrs
{

using namespace arma;

class Listener
{
public:
	typedef std::auto_ptr<Listener> ptr_t;

	virtual ~Listener();
    static ptr_t create();

    void set_orientation_reference(const avrs::orientation_angles_t &o);

    avrs::orientation_angles_t &get_orientation();
    void rotate(const avrs::orientation_angles_t &o);

    void set_position_reference(const avrs::point3d_t &p);

    void set_initial_point_of_view(const avrs::orientation_angles_t &o, const avrs::point3d_t &p);

    //avrs::point3d_t &get_position();  // TODO position_t
    avrs::point3d_t pos;  // in room reference system

    void translate(const avrs::point3d_t &p);  // from reference position

    mat::fixed<4,4> &get_rotation_matrix();

private:
	Listener();

	avrs::point3d_t _pos_ref;
	avrs::orientation_angles_t _ori;
	avrs::orientation_angles_t _ori_ref;  // TODO deprecated?

	arma::mat::fixed<4,4> _R0;  // Initial Rotation matrix
	arma::mat::fixed<4,4> _T0;  // Initial Rotation matrix
	arma::mat::fixed<4,4> _Tr;  // Transformation matrix

	arma::mat::fixed<4,4> _Rc;  // Current Rotation matrix
	arma::mat::fixed<4,4> _Tc;  // Current Translation matrix
};

inline void Listener::rotate(const avrs::orientation_angles_t &o)
{
	arma::mat::fixed<4,4> Ri = avrs::math::rotation_matrix_from_angles(o);  // ZXZ
	_Rc = Ri * _Tr;
    //_Rc.print();

	// Euler angles ZXZ (in degrees)
	int sign1 = (_Rc(0,1) >= 0 ? 1 : -1);
	_ori.az = sign1 * (acos(_Rc(0,0)) * 180.0) / M_PI;
	int sign2 = (_Rc(1,2) >= 0 ? 1 : -1);
	_ori.el = sign2 * (acos(_Rc(2,2)) * 180.0) / M_PI;
	_ori.ro = 0;  // always zero
}

inline void Listener::translate(const avrs::point3d_t &p)
{
    mat::fixed<4,4> Ti;
	Ti << 1 << 0 << 0 << p(0) << endr
	   << 0 << 1 << 0 << p(1) << endr
	   << 0 << 0 << 1 << p(2) << endr
	   << 0 << 0 << 0 << 1    << endr;
	_Tc = Ti * _Tr;

    // TODO test!
	// http://www.fastgraph.com/makegames/3drotation/
}

inline avrs::orientation_angles_t &Listener::get_orientation()
{
	return _ori;
}

//inline avrs::point3d_t &Listener::get_position()  // TODO position_t
//{
//	return _pos;
//}

inline mat::fixed<4,4> &Listener::get_rotation_matrix()
{
	return _Rc;
}

}  // namespace avrs

#endif // LISTENER_HPP_
