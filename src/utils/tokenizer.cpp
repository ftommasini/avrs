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

#include "tokenizer.hpp"

namespace avrs
{

const std::string Tokenizer::DELIMITERS(" \t\n\r");

Tokenizer::Tokenizer(const std::string &s) :
	_string(s),
	_offset(0),
	_delimiters(DELIMITERS)
{
	;  // nothing to do
}

Tokenizer::Tokenizer(const std::string &s, const std::string &delimiters) :
	_string(s),
	_offset(0),
	_delimiters(delimiters)
{
	;  // nothing to do
}

bool Tokenizer::next_token()
{
	return next_token(_delimiters);
}

bool Tokenizer::next_token(const std::string &delimiters)
{
	size_t i = _string.find_first_not_of(delimiters, _offset);

	if (std::string::npos == i)
	{
		_offset = _string.length();
		return false;
	}

	size_t j = _string.find_first_of(delimiters, i);

	if (std::string::npos == j)
	{
		_token = _string.substr(i);
		_offset = _string.length();
		return true;
	}

	_token = _string.substr(i, j - i);
	_offset = j;

	return true;
}

const std::string Tokenizer::get_token() const
{
	return _token;
}

void Tokenizer::reset()
{
	_offset = 0;
}

}  // namespace avrs
