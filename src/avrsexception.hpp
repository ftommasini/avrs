/*
 * Copyright (C) 2013 Fabián C. Tommasini <fabian@tommasini.com.ar>
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

#ifndef AVRSEXCEPTION_HPP_
#define AVRSEXCEPTION_HPP_

#include <string>
#include <exception>

namespace avrs
{

class AvrsException: public std::exception
{
public:
	AvrsException(const std::string m = "AVRS exception")
			:_msg(m)
	{
		;
	}

	virtual ~AvrsException() throw()
	{
		;
	}

	virtual const char* what() const throw()
	{
		return _msg.c_str();
	}

private:
	std::string _msg;
};

} // namespace

#endif /* AVRSEXCEPTION_HPP_ */
