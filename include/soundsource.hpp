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

#ifndef SOUNDSOURCE_HPP_
#define SOUNDSOURCE_HPP_

#include <boost/shared_ptr.hpp>

#include "common.hpp"

namespace avrs
{

class SoundSource
{
public:
	typedef boost::shared_ptr<SoundSource> ptr_t;

	virtual ~SoundSource();
    static ptr_t create(std::string filename);

	avrs::data_t get_IR(avrs::point3_t &p);
	avrs::point3_t pos;

private:
	SoundSource(std::string filename);
	bool _init();

	std::string _filename;
	avrs::data_t _ir;
};

inline avrs::data_t SoundSource::get_IR(avrs::point3_t &p)
{
	return _ir;
}

}  // namespace avrs

#endif // SOUNDSOURCE_HPP_
