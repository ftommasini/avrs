/**
 * \file tokenizer.hpp
 */

#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <string>

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

#endif  // TOKENIZER_H_
