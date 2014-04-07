/*
 * Copyright (C) 2013-2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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

#include <boost/format.hpp>

#include "ism.hpp"

namespace avrs
{

Ism::Ism(configuration_t *config, const Room::ptr_t &r)
{
	_config = config;
	_room = r;
}

Ism::~Ism()
{
	_aud.clear();
	_tree.clear();
}

void Ism::calculate(bool discard_nodes)
{
	_count_vs = 0;

	// create VS from "real" source (order 0)
	VirtualSource::ptr_t vs(new VirtualSource);
	vs->id = ++_count_vs; // must be 1 because is a "real" source
	vs->audible = true;
	vs->pos = _config->sound_source->pos;
	vs->dist_listener = arma::norm(vs->pos - _config->listener->pos, 2);
	vs->pos_ref_listener = vs->pos - _config->listener->pos;

	// append to top of tree
	_tree.clear();
	_root_it = _tree.insert(_tree.begin(), vs);
	_aud.clear();
	_aud.push_back(vs);

	// propagate first order... and then run recursively
	_propagate(vs, _root_it, 1, discard_nodes);
}

// recursive function (depth-first traversal, pre-order)
void Ism::_propagate(VirtualSource::ptr_t vs_parent, const tree_it_t node_parent,
		const unsigned int order, const bool discard_nodes)
{
	DPRINT("Order %d", order);

	// for each surface
	for (unsigned int i = 0; i < _room->n_surfaces(); i++)
	{
		DPRINT("Surface %d", i);

		Surface::ptr_t s = _room->get_surface(i);

		// do the reflection
		// (normal to the surface, already calculated)

		// distance from virtual source (VS) to surface
		float dist_vs_s = s->get_dist_origin() - arma::dot(vs_parent->pos, s->get_normal());

		// validity test (if VS fails, is discarded)
		if (dist_vs_s > 0.0f)
		{
			// progeny VS position
			arma::frowvec3 pos = vs_parent->pos + 2 * dist_vs_s * s->get_normal();
			// distance from VS to listener
			float dist_listener = arma::norm(pos - _config->listener->pos, 2);

			// proximity test (if it fails, is discarded)
			if (dist_listener <= _config->max_distance)
			{
				// create the new progeny VS
				VirtualSource::ptr_t vs_progeny(new VirtualSource);

				// update values for valid VS
				vs_progeny->pos = pos;
				vs_progeny->dist_listener = dist_listener;
				vs_progeny->order = order;
				vs_progeny->surface_ptr = s;
				vs_progeny->id = ++_count_vs;

				// calculate the position referenced to listener of progeny VS
				vs_progeny->pos_ref_listener = vs_progeny->pos - _config->listener->pos;

				// calculate the orientation of VS (when available)

				// append the progeny VS to the tree (because is not discarded)
				tree_it_t node_progeny = _tree.append_child(node_parent, vs_progeny);

				// first audibility test
				bool aud_test_1 = _check_audibility_1(vs_progeny); // audibility 1
				bool aud_test_2 = true;

				// second audibility test
				// order greater than 1, first visibility test must be passed
				if (order > 1 && aud_test_1)
					aud_test_2 = _check_audibility_2(vs_progeny, node_progeny); // audibility 2

				vs_progeny->audible = (aud_test_1 && aud_test_2); // reduction of truth table

				if (vs_progeny->audible)
					_aud.push_back(vs_progeny); // add progeny VS to the vector that contains visible VSs

				// next order
				if (static_cast<short>(order + 1) <= _config->max_order)
					_propagate(vs_progeny, node_progeny, order + 1, discard_nodes); // propagate the next order
			}
		}

		// TODO lock_guard
		if (discard_nodes)
			_tree.erase_children(node_parent); // release memory
	}
}

unsigned long Ism::get_count_vs()
{
	return (unsigned long) (_tree.size() - 1);
}

unsigned long Ism::get_count_visible_vs()
{
	return (unsigned long) (_aud.size() - 1);
}

unsigned long Ism::get_bytes_vs()
{
	return get_count_vs() * sizeof(VirtualSource);
}

unsigned long Ism::get_bytes_visible_vs()
{
	return ((unsigned long) _aud.size()) * sizeof(VirtualSource);
}

void Ism::print_list()
{
	_sort_aud();

	std::cout << "List of visible VSs\n" << std::endl;
	boost::format fmter_title("%-15s\t%-15s\t%-15s\t%-7s\t%-7s\t%+10s\t%+10s\t%+10s\n");
	boost::format fmter_content("%-15.3f\t%-15.3f\t%-15.3f\t%-7i\t%-7i\t%+10.3f\t%+10.3f\t%+10.3f\n");
	std::cout << boost::format(fmter_title) % "Relative [ms]"
					% "Absolute [ms]" % "Distance [m]" % "Order" % "Id"
					% "X" % "Y" % "Z";
	float time_ref_ms;

	for (aud_it_t it = _aud.begin(); it != _aud.end(); it++)
	{
		VirtualSource::ptr_t vs = *it;
		float time_abs_ms = (vs->dist_listener / _config->speed_of_sound) * 1000.0f;

		if (it == _aud.begin())
			time_ref_ms = time_abs_ms;

		float time_rel_ms = time_abs_ms - time_ref_ms;

		std::cout << boost::format(fmter_content)
			% time_rel_ms
			% time_abs_ms
			% vs->dist_listener
			% vs->order
			% vs->id
			% vs->pos(X)
			% vs->pos(Y)
			% vs->pos(Z);
	}

	std::cout << std::endl;
}

void Ism::print_summary()
{
	std::cout << "Total VSs:\t" << get_count_vs() << std::endl;
	std::cout << "Total MB:\t" << boost::format("%.3f\n") % (get_bytes_vs() / (1024.0 * 1024.0));
	std::cout << "Audible VSs:\t" << get_count_visible_vs() << std::endl;
	std::cout << std::endl;
}

bool Ism::_check_audibility_1(const VirtualSource::ptr_t &vs)
{
	// first find the intersection point between a line and a plane in 3D
	// see: http://softsurfer.com/Archive/algorithm_0104/algorithm_0104B.htm

	Surface::ptr_t s = vs->surface_ptr;
	arma::frowvec4 plane_coeff = s->get_plane_coeff();
	// n . (P1 - P0) where n is the normal of the plane
	float denom = plane_coeff(0) * vs->pos_ref_listener(X)
			+ plane_coeff(1) * vs->pos_ref_listener(Y)
			+ plane_coeff(2) * vs->pos_ref_listener(Z);

	// check if line and plane are parallel (value near to zero)
	if (fabs(denom) <= PRECISION)
		return false;

	// calculate the parameter for the parametric equation of the line
	float t = -(plane_coeff(0) * (_config->listener->pos)(X)
			+ plane_coeff(1) * (_config->listener->pos)(Y)
			+ plane_coeff(2) * (_config->listener->pos)(Z) + plane_coeff(3)) / denom;
	vs->intersection_point = _config->listener->pos + vs->pos_ref_listener * t;  // calculate the intersection point

	// finally, check if the intersection point is inside of surface
	return s->is_point_inside(vs->intersection_point);
}

bool Ism::_check_audibility_2(const VirtualSource::ptr_t &vs, const tree_it_t node)
{
	// intersection point is our virtual listener position
	arma::frowvec3 pos_vl = vs->intersection_point; // already calculated in visibility test 1 (position of "virtual listener")
	// get node of VS parent
	tree_it_t node_parent = _tree.parent(node);

	while (node_parent != _root_it) // while the parent node is not the root node
	{
		VirtualSource::ptr_t vs_parent = *node_parent;
		assert(vs_parent != NULL);
		Surface::ptr_t s = vs_parent->surface_ptr;  // or _r->get_surface(vs_parent->surface_index);

		// check for visibility
		arma::frowvec3 xyz_vs = vs_parent->pos - pos_vl;  // VS position referenced to virtual listener
		arma::frowvec4 plane_coeff = s->get_plane_coeff();
		// dot product
		float denom = plane_coeff(0) * xyz_vs(X)
				+ plane_coeff(1) * xyz_vs(Y)
				+ plane_coeff(2) * xyz_vs(Z);

		if (fabs(denom) <= PRECISION)
			return false;

		float t = -(plane_coeff(0) * pos_vl(X)
				+ plane_coeff(1) * pos_vl(Y)
				+ plane_coeff(2) * pos_vl(Z)
				+ plane_coeff(3)) / denom;
		arma::frowvec3 inter_point = pos_vl + xyz_vs * t;

		if (!(s->is_point_inside(inter_point)))
			return false;

		pos_vl = inter_point; // the "new" virtual listener position
		node_parent = _tree.parent(node_parent); // get node of VS parent
	}

	return true;
}

void Ism::_sort_aud()
{
	std::sort(_aud.begin(), _aud.end(), comparevsdistance_t()); // sort by distance to the listener
}


} /* namespace avrs */
