/*
 * tokenizer.cpp
 *
 *  Created on: Jun 2, 2011
 *      Author: fabian
 */

#include "tokenizer.hpp"

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
