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

/*
 * Based on JotReverb of "REALSIMPLE Lab on Reverberation"
 * by Julius O. Smith III and Nelson Lee
 * see: https://ccrma.stanford.edu/realsimple/reverb/JotReverb_STK.html
 *
 * 8-line, single input, single output feedback delay network,
 * with each delay line cascaded by a one-pole lowpass filter.
 * There is also a tone correction filter.
 *
 * This class implements an order 8 SISO FDN with
 * b = [1,1,1,1,1,1,1,1]^T, c = [1,1,1,1,1,1,1,1]^T,
 * and feedback matrix A = A_8,
 * where A_N = gA*(I_N - (2/N)*u_N*(u_N)^T).
 * (I_N denotes the N by N identity matrix, and
 * u_N = [1,...,1]^T, an N-vector of ones.) Each delay is
 * cascaded by a lowpass filter according to the
 * difference equation:
 * y_i(n) = g_i(1-a_i)*x_i(n) + a_i*y_i(n-1)
 * where the subscript i = {1,2,3,4,5,6,7,8} denotes each of the
 * 8 delay lines. A tone correction filter is applied
 * to the FDN output and summed with the input sample
 * to produce the final system output.
 *
 * You set the reverberation time at DC and at half the
 * sampling rate, the gain coefficient gA of the A matrix,
 * and the delay line lengths of each of the 4 delay lines
 * during instantiation. The default constructor sets up a
 * 0-sample length for each delay line with gA = 0.0, g_i = 1.0,
 * and a_i = 0.0 for all i and no tone correction (resulting in
 * no delay, feedback, or filtering).
 */


#ifndef _FDN_HPP_
#define _FDN_HPP_

#include <memory>
#include <stk/Delay.h>
#include <stk/Iir.h>
#include <stk/OneZero.h>
#include <stk/OnePole.h>
#include <armadillo>
#include <boost/shared_ptr.hpp>

namespace avrs
{

using namespace stk;
using namespace arma;

class Fdn
{
public:
	typedef boost::shared_ptr<Fdn> ptr_t;

	//! Class destructor.
	~Fdn();

	static ptr_t create(unsigned int N, double gain_A, const double *b, const double *c,
			double d, const long *m, double RTatDC, double RTatPI,
			std::string b_coeff_file, std::string a_coeff_file);

	//! Clears the internal states of the FDN delay lines and filters.
	void clear();

	//! Input one sample to the FDN and return one output.
	StkFloat tick(StkFloat sample);

private:
	//! Constructor which sets the t60 reverb times at DC and at half
	//! the sampling rate, the gain coefficient of the feedback matrix,
	//! and the FDN delay line lengths
	Fdn(unsigned int N, double gain_A, const double *b, const double *c,
			double d, const long *m, double RTatDC, double RTatPI,
			std::string b_coeff_file, std::string a_coeff_file);

	void _init();
	void _stabilize(long n_ticks);

	unsigned int _N;
	StkFloat _gA; // gain coefficient gA for the A feedback matrix
	mat _A;
	colvec _b;
	colvec _c;
	StkFloat _d;
	colvec _s; // delay line inputs
	colvec _s_delayed; // delay line outputs
	colvec _s_filtered; // delay line outputs

	double _t60_0;
	double _t60_pi;

	mat _b_coeff;
	mat _a_coeff;

	std::vector<unsigned long> _m; // length of delay lines
	std::vector< boost::shared_ptr<Delay> > _delayline; // array of pointers to delay line objects
	std::vector< boost::shared_ptr<Iir> > _filter; // array of pointers to filter objects
	std::vector< boost::shared_ptr<OnePole> > _lp_filter;  // array of pointers to low-pass filter objects
	boost::shared_ptr<OneZero> _tc; // pointer to tone correction filter
};

}  // namespace avrs

#endif  // _FDN_HPP_

