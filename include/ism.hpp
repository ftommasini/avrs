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

#ifndef ISM_HPP_
#define ISM_HPP_

#include <vector>
#include <algorithm>
#include <boost/shared_ptr.hpp>

#include "utils/tree.hpp"
#include "common.hpp"
#include "room.hpp"
#include "virtualsource.hpp"
#include "configuration.hpp"

namespace avrs
{

class Ism
{
public:
	typedef boost::shared_ptr<Ism> ptr_t;

	Ism(configuration_t *config, const Room::ptr_t &r);
	virtual ~Ism();

	void calculate(bool discard_nodes);

	unsigned long get_count_vs();
	unsigned long get_count_visible_vs();
	unsigned long get_bytes_vs();
	unsigned long get_bytes_visible_vs();

	void print_list();
	void print_summary();

private:
	configuration_t *_config;
	Room::ptr_t _room;

	// tree for VSs
	tree<VirtualSource::ptr_t> _tree; // VSs tree
	typedef tree<VirtualSource::ptr_t>::iterator tree_it_t;
	tree_it_t _root_it;  // root of tree
	unsigned long _count_vs;

	// audible VSs
	std::vector<VirtualSource::ptr_t> _aud;
	typedef std::vector<VirtualSource::ptr_t>::iterator aud_it_t;

	void _propagate(VirtualSource::ptr_t vs, const tree_it_t node_parent,
			const unsigned int order, const bool discard_nodes);
	bool _check_audibility_1(const VirtualSource::ptr_t &vs);
	bool _check_audibility_2(const VirtualSource::ptr_t &vs, const tree_it_t node);
	void _sort_aud();

	typedef struct CompareVSDistance
	{
		bool operator()(VirtualSource::ptr_t i, VirtualSource::ptr_t j)
		{
			return (i->dist_listener < j->dist_listener);
		}
	} comparevsdistance_t;
};

}  // namespace avrs

#endif /* ISM_HPP_ */
