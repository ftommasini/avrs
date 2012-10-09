/**
 * \file soundsource.hpp
 */

#ifndef SOUNDSOURCE_HPP_
#define SOUNDSOURCE_HPP_

#include "common.hpp"

class SoundSource
{
public:
	typedef std::auto_ptr<SoundSource> ptr_t;

	virtual ~SoundSource();
    static ptr_t create(std::string filename);
    // todo Can return reference???
	avrs::data_t get_IR(avrs::orientation_angles_t &ori);

	avrs::point3d_t pos;

private:
	SoundSource(std::string filename);
	bool _init();

	std::string _filename;
	avrs::data_t _ir;
};

inline avrs::data_t SoundSource::get_IR(avrs::orientation_angles_t &ori)
{
	return _ir;
}

#endif // SOUNDSOURCE_HPP_
