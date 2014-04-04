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

#include <stk/FileWvIn.h>
#include <cmath>

#include "soundsource.hpp"
#include "avrsexception.hpp"

namespace avrs
{

SoundSource::SoundSource(std::string filename)
	: _filename(filename)
{
	;  // nothing to do
}

SoundSource::~SoundSource()
{
	;  // nothing to do
}

SoundSource::ptr_t SoundSource::create(std::string filename)
{
	ptr_t p_tmp(new SoundSource(filename));

	if (!p_tmp->_init())
	{
		p_tmp.reset();
		throw AvrsException("Error creating SoundSource");
	}

	return p_tmp;
}

bool SoundSource::_init()
{
	stk::FileWvIn in_wv(1000000, 1024);
	bool retval = true;

	try {
		in_wv.openFile(_filename, false, true);

		if (in_wv.channelsOut() > 1) // must be mono
		{
			ERROR("Number of channels greater than 1 for mono input");
			retval = false;
		}

		_ir.resize(in_wv.getSize(), 0.0f);

		avrs::sample_t max_value = 0.0f, abs_value;

		// read and normalize
		for (uint i = 0; i < in_wv.getSize(); i++)
		{
			_ir[i] = (avrs::sample_t) in_wv.tick();
			abs_value = fabs(_ir[i]);

			if (abs_value > max_value)
				max_value = abs_value;
		}

		for (uint i = 0; i < in_wv.getSize(); i++)
		{
			_ir[i] /= max_value;  // normalize
		}
	}
	catch (stk::StkError &ex) {
		DPRINT("%s", ex.getMessageCString());
		retval = false;
	}

	return retval;
}

}  // namespace avrs
