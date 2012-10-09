/**
 * \file input.cpp
 * \brief The input classes (implementation)
 */

#include "input.hpp"

/*
 * InputWave
 */

InputWave::InputWave(std::string filename)
	: _filename(filename)
{
	_in_wv = new stk::FileWvIn(1000000, 1024);
}

InputWave::~InputWave()
{
	_in_wv->closeFile();
	delete _in_wv;
}

InputWave::ptr_t InputWave::create(std::string filename)
{
	ptr_t p_tmp(new InputWave(filename));

	if (!p_tmp->_init())
	{
		p_tmp.release();  // NULL-like pointer
	}

	return p_tmp;
}

bool InputWave::_init()
{
	bool retval = true;

	try {
		_in_wv->openFile(_filename, false, true);

		if (_in_wv->channelsOut() > 1) {  // must be mono
			ERROR("Number of channels greater than 1 for mono input");
			retval = false;
		}
	}
	catch (stk::StkError &ex) {
		DPRINT("%s", ex.getMessageCString());
		retval = false;
	}

	return retval;
}


/*
 * InputWaveLoop
 */

InputWaveLoop::InputWaveLoop(std::string filename)
	: _filename(filename)
{
	_in_loop = new stk::FileLoop(1000000, 1024);
}

InputWaveLoop::~InputWaveLoop()
{
	_in_loop->closeFile();
	delete _in_loop;
}

InputWaveLoop::ptr_t InputWaveLoop::create(std::string filename)
{
	ptr_t p_tmp(new InputWaveLoop(filename));

	if (!p_tmp->_init())
	{
		p_tmp.release();  // NULL-like pointer
	}

	return p_tmp;
}

bool InputWaveLoop::_init()
{
	bool retval = true;

	try {
		_in_loop->openFile(_filename, false, true);

		if (_in_loop->channelsOut() > 1) {  // must be mono
			ERROR("Number of channels greater than 1 for mono input");
			retval = false;
		}
	}
	catch (stk::StkError &ex) {
		DPRINT("%s", ex.getMessageCString());
		retval = false;
	}

	return retval;
}


/*
 * InputNoise
 */

InputNoise::InputNoise()
{
	;  // nothing to do
}

InputNoise::~InputNoise()
{
	;  // nothing to do
}

InputNoise::ptr_t InputNoise::create()
{
	ptr_t p_tmp(new InputNoise());
	return p_tmp;
}

