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

#include "surface.hpp"

#include <cassert>

namespace avrs
{

Surface::Surface(unsigned int id, const float *x_vert, const float *y_vert,
		const float *z_vert, int n_vert)
{
	assert(n_vert == 4);  // restricted to 4 vertices

	_id = id;
	_vert.set_size(4, 3);

	for (arma::u32 i = 0; i < (arma::u32) n_vert; i++)
	{
		_vert(i,X) = x_vert[i];
		_vert(i,Y) = y_vert[i];
		_vert(i,Z) = z_vert[i];
	}

#ifndef BORISH
	// calculate range of coordinates
	arma::frowvec3 vert_max = max(_vert);
	arma::frowvec3 vert_min = min(_vert);
	arma::frowvec3 vert_range = vert_max - vert_min;  // range
	vert_range.min(_coord_to_remove);  // smallest range

	_vert_proj.set_size(4, 2);

	for (arma::u32 i = 0; i < (arma::u32) _vert_proj.n_rows; i++)
	{
		arma::frowvec2 r = _project_to_2d(_vert.row(i));
		_vert_proj.row(i) = r;
	}
#endif

	_calc_center();
	_calc_plane_coeff();
	_calc_normal();
	_calc_dist_origin();
	_calc_area();
}

Surface::Surface(unsigned int id, const double *x_vert, const double *y_vert,
		const double *z_vert, int n_vert)
{
	assert(n_vert == 4);  // restricted to 4 vertices for now

	_id = id;
	_vert.set_size(4, 3);

	for (arma::u32 i = 0; i < (arma::u32) n_vert; i++)
	{
		_vert(i,X) = (float) x_vert[i];
		_vert(i,Y) = (float) y_vert[i];
		_vert(i,Z) = (float) z_vert[i];
	}

	_init();
}

Surface::~Surface()
{
	;
}

#ifdef BORISH

/**
 * @see
 * "Extension of the image model to arbitrary polyhedra".
 * Jeffrey Borish, J. Acoust. Soc. Am. 75, 1827 (1984), DOI:10.1121/1.390983
 */
bool Surface::is_point_inside(arma::frowvec3 point)
{
	static arma::frowvec3 dir_cos;
	static arma::frowvec3 dir_cos_ref;
	static arma::frowvec3 v1;
	static arma::frowvec3 v2;
	static arma::frowvec3 c;

	// test for first vertex
	v1 = _vert.row(3) - point;
	v2 = _vert.row(0) - point;
	c = cross(v1, v2);

	// the checking for parallelism is omitted
	// norm(c, 2) == 0

	dir_cos_ref = c / norm(c, 2); // direction cosines (reference)

	// the rest of vertices
	for (arma::u32 i = 0; i < _vert.n_rows - 1; i++)
	{
		v1 = _vert.row(i) - point;
		v2 = _vert.row(i + 1) - point;
		c = cross(v1, v2);

		// the checking for parallelism is omitted
		// norm(c, 2) == 0

		dir_cos = c / norm(c, 2); // direction cosines

		for (arma::u32 j = 0; j < 3; j++)
		{
			if (fabs(dir_cos[j] - dir_cos_ref[j]) >= PRECISION)
				return false;
		}
	}

	return true;
}

#else

bool Surface::is_point_inside(arma::frowvec3 point)
{
	// projection to 2D
	arma::frowvec2 p = _project_to_2d(point);
	float testx = p(X);
	float testy = p(Y);
	int nvert = _vert_proj.n_rows;
	float *vertx = _vert_proj.colptr(X);
	float *verty = _vert_proj.colptr(Y);

	return (_pnpoly(nvert, vertx, verty, testx, testy) % 2);
}

arma::frowvec2 Surface::_project_to_2d(arma::frowvec3 p)
{
	unsigned int j = 0;
	arma::frowvec2 projection;

	for (unsigned int i = 0; i < 3; i++)
	{
		if (i == _coord_to_remove)
			continue;

		projection(j++) = p(i);
	}

	return projection;
}

// http://www.ecse.rpi.edu/~wrf/Research/Short_Notes/pnpoly.html
int Surface::_pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
  int i, j, c = 0;

  for (i = 0, j = nvert-1; i < nvert; j = i++)
  {
    if ( ((verty[i] > testy) != (verty[j] > testy)) &&
	     (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]) )
       c = !c;
  }

  return c;
}

#endif

// Private functions

void Surface::_calc_center()
{
	arma::frowvec3 xyz_min = min(_vert, 0);  // across the points
	arma::frowvec3 xyz_max = max(_vert, 0);  // across the points
	_center = ((xyz_max - xyz_min) / 2) + xyz_min;  // (x, y, z) center
}

void Surface::_calc_plane_coeff()
{
	arma::fmat A, B, C;

	A << _vert(1,1) - _vert(0,1) << _vert(1,2) - _vert(0,2) << arma::endr
	  << _vert(2,1) - _vert(0,1) << _vert(2,2) - _vert(0,2) << arma::endr;

	B << _vert(1,2) - _vert(0,2) << _vert(1,0) - _vert(0,0) << arma::endr
	  << _vert(2,2) - _vert(0,2) << _vert(2,0) - _vert(0,0) << arma::endr;

	C << _vert(1,0) - _vert(0,0) << _vert(1,1) - _vert(0,1) << arma::endr
	  << _vert(2,0) - _vert(0,0) << _vert(2,1) - _vert(0,1) << arma::endr;

	float a = det(A);
	float b = det(B);
	float c = det(C);
	float d = -1.0f * (a * _vert(0,0) + b * _vert(0,1) + c * _vert(0,2));

	_plane_coeff << a << b << c << d << arma::endr;
}

void Surface::_calc_normal()
{
	_normal = _plane_coeff.subvec(0,2);  // (a, b, c) is the normal of plane
	_normal /= -1 * norm(_normal, 2);  // unitary normal (negative for better calculation later)
}

void Surface::_calc_dist_origin()
{
	float a = _plane_coeff(0);
	float b = _plane_coeff(1);
	float c = _plane_coeff(2);
	float d = _plane_coeff(3);

	// the origin is (x, y, z) = (0, 0, 0), so ...
	_dist_origin = fabs(d) / sqrt(a * a + b * b + c * c);
}

void Surface::_calc_area()
{
	avrs::point3d_t ac = _vert.row(0) - _vert.row(2);  // ac vector
	avrs::point3d_t bd = _vert.row(1) - _vert.row(3);  // bd vector
	_area = 0.5f * norm(cross(ac, bd), 2);
}

}  // namespace avrs
