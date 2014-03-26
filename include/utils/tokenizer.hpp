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

#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <string>

namespace avrs
{

// Adapted from:
// http://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
class Tokenizer
{
public:
	static const std::string DELIMITERS;

	Tokenizer(const std::string& str);
	Tokenizer(const std::string& str, const std::string& delimiters);

	bool next_token();
	bool next_token(const std::string& delimiters);
	const std::string get_token() const;
	void reset();

protected:
	const std::string _string;
	size_t _offset;
	std::string _token;
	std::string _delimiters;
};

}  // namespace avrs

#endif  // TOKENIZER_H_
