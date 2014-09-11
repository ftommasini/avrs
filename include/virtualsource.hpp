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

#ifndef VIRTUALSOURCE_HPP_
#define VIRTUALSOURCE_HPP_

#include "surface.hpp"
#include "common.hpp"

namespace avrs
{

typedef struct VirtualSource
{
	typedef boost::shared_ptr<VirtualSource> ptr_t;

	unsigned long id;

	point3_t pos_R;  // in Room coordinates system
	point3_t pos_L;  // in Listener coordinates system
	matrix44_t rotation;

	short order;
	float dist_listener;
	float time_abs_ms;
	float time_rel_ms;
	Surface::ptr_t surface_ptr;
	point3_t intersection_point;
	bool audible;
	ptr_t parent_ptr;

	orientationangles_t orientation_L; // referenced to Listener
	orientationangles_t orientation_0; // initial orientation

} virtualsource_t;

}  // namespace avrs

#endif /* VIRTUALSOURCE_HPP_ */
