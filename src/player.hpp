/**
 * \file output.hpp
 * \brief
 * The output classes (description)
 */

#ifndef OUTPUT_HPP_
#define OUTPUT_HPP_

#include <cassert>
#include <memory>

// STK headers
#include <RtAudio.h>

// AVRS headers
#include "common.hpp"

using namespace avrs;

/**
 * Plays the output data trough the sound card
 */
class Player
{
public:
	typedef std::auto_ptr<Player> ptr_t;

	virtual ~Player();
	/// Static factory function for InputWave objects
	static ptr_t create(float gain_factor = 1.0);

	void set_gain_factor(float gain_factor);
	float get_gain_factor() const;
	bool start();
	bool stop();
	bool is_running();
	void mute();
	void unmute();

	static int callback(void *output_buffer, void *input_buffer,
			uint n_buffer_frames, double stream_time, RtAudioStreamStatus status,
			void *data_pointer);

private:
	Player(float gain_factor);

	bool _init();

	bool _running;
	float _gain_factor;
	float _gain_factor_tmp;  // save the current gain factor, when muting
	bool _muted;
	RtAudio _audio_dev;
	uint _count_empty;
	int _fifo_id;

    // thread related stuff
    pthread_t _thread_id;
    static void* _thread_wrapper(void*);
    void* _thread(void*);
};

inline void Player::set_gain_factor(float gain_factor)
{
	assert(gain_factor >= 0.0f);
	_gain_factor = gain_factor;
}

inline float Player::get_gain_factor() const
{
	return _gain_factor;
}

inline void Player::mute()
{
	if (!_muted) {
		_gain_factor_tmp = _gain_factor;
		_gain_factor = 0.0f;
		_muted = true;
	}
}

inline void Player::unmute()
{
	if (_muted) {
		_gain_factor = _gain_factor_tmp;
		_muted = false;
	}
}

inline bool Player::is_running()
{
	return _running;
}

#endif  // OUTPUT_HPP_

