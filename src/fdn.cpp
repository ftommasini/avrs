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

#include "fdn.hpp"

#include <stk/Stk.h>
#include <math.h> // for basic math functions

Fdn::Fdn(unsigned int N, double gain_A, const double *b, const double *c,
		double d, const long *m, double RTatDC, double RTatPI) :
	_N(N), _gA(gain_A), _d(d), _t60_0(RTatDC), _t60_pi(RTatPI)
{
	_m.resize(_N);
	_delayline.resize(_N);
	_lp_filter.resize(_N);
//	_filter.resize(_N);

	_b.reshape(_N, 1);
	_c.reshape(_N, 1);

	for (unsigned int i = 0; i < _N; i++)
	{
		_m[i] = m[i]; // m delays
		_b[i] = b[i]; // b gains
		_c[i] = c[i]; // c gains
	}

	_s.reshape(_N, 1);
	_s_delayed.reshape(_N, 1);
	_s_filtered.reshape(_N, 1);

//	_b_coeff.load("data/filters_num.txt", raw_ascii);
//	_a_coeff.load("data/filters_den.txt", raw_ascii);
}

Fdn::~Fdn()
{
	for (unsigned int i = 0; i < _N; i++)
	{
		delete _delayline[i];
		delete _lp_filter[i];
//		delete _filter[i];
	}

	delete _tc;
}

Fdn::ptr_t Fdn::create(unsigned int N, double gain_A, const double *b,
		const double *c, double d, const long *m, double RTatDC, double RTatPI)
{
	ptr_t p_tmp(new Fdn(N, gain_A, b, c, d, m, RTatDC, RTatPI));

	if (!p_tmp->_init())
		p_tmp.reset();

	return p_tmp;
}

bool Fdn::_init()
{
	// Setup filters

	// Tone control filter
	_tc = new OneZero();
	_tc->setB0(1.2462);
	_tc->setB1(-0.2462);

	// some variables need for calculating lowpass filter coefficients
	StkFloat T, alpha;

	T = 1.0 / (Stk::sampleRate());

	if (_t60_0 == 0.0)
	{
		alpha = 1.0; // to prevent dividing by 0
	}
	else
	{
		alpha = _t60_pi / _t60_0;
	}

	double tmp_val = (log(10) / 4) * (1 - (1 / (alpha * alpha)));
	std::vector<StkFloat> g(_N);
	std::vector<StkFloat> a(_N);

	// Absorption filters
	for (unsigned int i = 0; i < _N; i++)
	{
		_delayline[i] = new Delay(_m[i], _m[i]);

		float exponent = -(3.0 * _m[i] * T) / _t60_0;
		g[i] = pow(10, exponent);
		a[i] = log10(g[i]) * tmp_val;
		_lp_filter[i] = new OnePole();
		_lp_filter[i]->setB0(g[i] * (1 - a[i]));
		_lp_filter[i]->setA1(a[i]);

//		std::vector<StkFloat> b;
//		mat::row_iterator bfrom = _b_coeff.begin_row(i); // start of row i
//		mat::row_iterator bto = _b_coeff.end_row(i); // end of row i
//
//		for (mat::row_iterator ri = bfrom; ri != bto; ri++)
//		{
//			b.push_back(*ri);
//		}
//
//		std::vector<StkFloat> a;
//		mat::row_iterator afrom = _a_coeff.begin_row(i); // start of row i
//		mat::row_iterator ato = _a_coeff.end_row(i); // end of row i
//
//		for (mat::row_iterator ri = afrom; ri != ato; ri++)
//		{
//			a.push_back(*ri);
//		}
//
//		_filter[i] = new Iir(b, a);
	}

	// Feedback matrix (A) calculation
	colvec u = ones<colvec> (_N);
	mat I = eye<mat> (_N, _N);
	_A = _gA * ((2.0 / _N) * u * trans(u) - I);

	clear();

	// stabilize the FDN
	// eliminate the first part of the output
	long n_ticks = _m[_N - 1] * 2;
	_stabilize(n_ticks);

	return true;
}

void Fdn::clear(void)
{
	for (unsigned int i = 0; i < _N; i++)
	{
		_delayline[i]->clear();
		_lp_filter[i]->clear();
//		_filter[i]->clear();

		_s[i] = 0.0;
		_s_delayed[i] = 0.0;
		_s_filtered[i] = 0.0;
	}

	_tc->clear();
}

StkFloat Fdn::tick(StkFloat sample)
{
	// for each line
	for (unsigned int i = 0; i < _N; i++)
	{
		_s_delayed[i] = _delayline[i]->tick(_s[i]);

		//_s_filtered[i] = _s_delayed[i];  // wihtout attenuation

		_s_filtered[i] = _lp_filter[i]->tick(_s_delayed[i]);

		//_s_filtered[i] = _filter[i]->tick(_s_delayed[i]);
	}

	_s = (sample * _b) + (_A * _s_filtered);

	return (_tc->tick(dot(_c, _s_filtered)) + (_d * sample));
}

void Fdn::_stabilize(long n_ticks)
{
	// ticks without attenuation
	// for initialization purposes

	for (long i = 0; i < n_ticks; i++)
	{
		// for each line
		for (unsigned int i = 0; i < _N; i++)
		{
			_s_delayed[i] = _delayline[i]->tick(_s[i]);
			_s_filtered[i] = _s_delayed[i];
		}

		_s = (1.0 * _b) + (_A * _s_filtered);
	}
}
