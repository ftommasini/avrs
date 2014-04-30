/*
 * Copyright (C) 2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
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

#ifndef AIRABSORPTION_HPP_
#define AIRABSORPTION_HPP_

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace avrs
{

class AirAbsorption
{
public:
	typedef boost::shared_ptr<AirAbsorption> ptr_t;

	virtual ~AirAbsorption();
	/// Static factory function for HrtfSet objects
	static ptr_t create(std::string filename);

	std::vector<double> &get_b_coeff(float distance);
	std::vector<double> &get_a_coeff(float distance);

private:
	AirAbsorption(std::string filename);

	boost::unordered_map<float, int> _map_index;  // map for <distance, index of distance>
	double **_b;
	double **_a;
	uint _n_distances;
	uint _n_coeff;
	uint _order;

	std::string _filename;

	bool _load();
	void _allocate_memory();
	void _deallocate_memory();
};

//inline std::vector<double> &AirAbsorption::get_b_coeff(float distance)
//{
//	int idx = _map_index[distance];
//	return _b_coeff;
//}
//
//inline std::vector<double> &AirAbsorption::get_a_coeff(float distance)
//{
//	return _a_coeff;
//}

} /* namespace avrs */

#endif /* AIRABSORPTION_HPP_ */
