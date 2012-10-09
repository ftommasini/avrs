/**
 * \file input.h
 * \brief The input classes (description)
 */

#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <cstdlib>
#include <string>

// STK headers
#include <FileWvIn.h>
#include <FileLoop.h>

// AVRS headers
#include "common.hpp"
#include "convolver.hpp"

using namespace avrs;

/**
 * Input base class (abstract)
 */
class InputBase
{
public:
	typedef std::auto_ptr<InputBase> ptr_t;

	virtual sample_t tick() = 0; // pure virtual
	virtual void tick(sample_t *val, uint n) = 0; // pure virtual
};


/**
 * Input from wave file
 */
class InputWave: public InputBase
{
public:
	typedef std::auto_ptr<InputWave> ptr_t;

	virtual ~InputWave();
	/// Static factory function for InputWave objects
	static ptr_t create(std::string filename);

	sample_t tick(); // virtual
	void tick(sample_t *val, uint n); // virtual

private:
	// Private constructor
	InputWave(std::string filename);
	bool _init();

	std::string _filename;
	stk::FileWvIn *_in_wv;
};

inline sample_t InputWave::tick()
{
	return (sample_t) _in_wv->tick();
}

inline void InputWave::tick(sample_t *val, uint n)
{
	stk::StkFrames tmp(n, 1); // mono
	_in_wv->tick(tmp);

	for (uint i = 0; i < n; i++)
		val[i] = (sample_t) tmp[i];
}

/**
 * Input from loop wave file
 */
class InputWaveLoop: public InputBase
{
public:
	typedef std::auto_ptr<InputWaveLoop> ptr_t;

	virtual ~InputWaveLoop();
	/// Static factory function for InputWaveLoop objects
	static ptr_t create(std::string filename);

	sample_t tick(); // virtual
	void tick(sample_t *val, uint n); // virtual

private:
	InputWaveLoop(std::string filename);
	bool _init();

	std::string _filename;
	stk::FileLoop *_in_loop;
};

inline sample_t InputWaveLoop::tick()
{
	return (sample_t) _in_loop->tick();
}

inline void InputWaveLoop::tick(sample_t *val, uint n)
{
	stk::StkFrames tmp(n, 1); // mono
	_in_loop->tick(tmp);

	for (uint i = 0; i < n; i++)
		val[i] = (sample_t) tmp[i];
}


/**
 * Input from noise generator
 */
class InputNoise: public InputBase
{
public:
	typedef std::auto_ptr<InputNoise> ptr_t;

	virtual ~InputNoise();
	/// Static factory function for InputNoise objects
	static ptr_t create();

	sample_t tick(); // virtual
	void tick(sample_t *val, uint n); // virtual

private:
	InputNoise();
};

inline sample_t InputNoise::tick()
{
	return (sample_t) (2.0 * rand() / (RAND_MAX + 1.0) - 1.0);
}

inline void InputNoise::tick(sample_t *val, uint n)
{
	for (uint i = 0; i < n; i++)
		val[i] = (sample_t) (2.0 * rand() / (RAND_MAX + 1.0) - 1.0);
}

#endif  // INPUT_HPP_
