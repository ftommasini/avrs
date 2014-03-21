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

#ifndef VERSION_HPP_
#define VERSION_HPP_

#include <string>
#include <boost/lexical_cast.hpp>

// the configured options and settings
#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 0

inline std::string get_version()
{
	std::string major = boost::lexical_cast<std::string>(VERSION_MAJOR);
	std::string minor = "." + boost::lexical_cast<std::string>(VERSION_MINOR);
	std::string patch = (VERSION_PATCH != 0 ? ("." + boost::lexical_cast<std::string>(VERSION_PATCH)) : "");
	return (major + minor + patch);
}

#endif /* VERSION_HPP_ */
