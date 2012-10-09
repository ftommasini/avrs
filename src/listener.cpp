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
