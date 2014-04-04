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

#define PRECISION (1e-3f)

/*
 * Surface
 */

Surface::Surface(unsigned int id, const float *x_vert, const float *y_vert,
		const float *z_vert, int n_vert)
{
	assert(n_vert == 4);  // restricted to 4 vertices for now

	_id = id;
	_vert.set_size(4, 3);

	for (arma::u32 i = 0; i < (arma::u32) n_vert; i++)
	{
		_vert(i,X) = x_vert[i];
		_vert(i,Y) = y_vert[i];
		_vert(i,Z) = z_vert[i];
	}

	_init();
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

//Surface::ptr_t Surface::create(unsigned int id, const float *x_vert, const float *y_vert,
//		const float *z_vert, int n_vert)
//{
//	ptr_t p_tmp(new Surface(id, x_vert, y_vert, z_vert, n_vert));
//
//	if (!p_tmp->_init())
//		p_tmp.reset();
//
//	return p_tmp;
//}
//
//Surface::ptr_t Surface::create(unsigned int id, const double *x_vert, const double *y_vert,
//		const double *z_vert, int n_vert)
//{
//	ptr_t p_tmp(new Surface(id, x_vert, y_vert, z_vert, n_vert));
//
//	if (!p_tmp->_init())
//		p_tmp.reset();
//
//	return p_tmp;
//}

bool Surface::_init()
{
	_calc_center();
	_calc_plane_coeff();
	_calc_normal();
	_calc_dist_origin();
	_calc_area();

	return true;
}

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

/**
 * @see "Extension of the image model to arbitrary polyhedra".
 * Jeffrey Borish, J. Acoust. Soc. Am. 75, 1827 (1984), DOI:10.1121/1.390983
 */
bool Surface::is_point_inside(avrs::point3d_t point)
{
	static avrs::point3d_t dir_cos;
	static avrs::point3d_t dir_cos_ref;
	//static umat ineq;
	static avrs::point3d_t v1;
	static avrs::point3d_t v2;
	static avrs::point3d_t c;

	// test for first vertex
	v1 = _vert.row(3) - point;
	v2 = _vert.row(0) - point;
	c = cross(v1, v2);

	// the checking for parallelism is omitted
	// norm(c, 2) == 0

	dir_cos_ref = c / norm(c, 2);  // direction cosines (reference)

	// the rest of vertices
	for (arma::u32 i = 0; i < _vert.n_rows - 1; i++)
	{
		v1 = _vert.row(i) - point;
		v2 = _vert.row(i + 1) - point;
		c = cross(v1, v2);

		// the checking for parallelism is omitted
		// norm(c, 2) == 0

		dir_cos = c / norm(c, 2);  // direction cosines

		for (arma::u32 j = 0; j < 3; j++)
		{
			if (fabs(dir_cos[j] - dir_cos_ref[j]) >= PRECISION)
				return false;
		}
//		ineq = find(abs(dir_cos - dir_cos_ref) > 0);  // save inequalities
//		if (!ineq.is_empty())
//			return false;
	}

	return true;

	/*
vert = get(plano, 'Vertices');

if sobreBorde(vert, pCorte) == 1
    ok = 2;
    return;
end

% test para verificar si el punto de corte est� dentro de los l�mites
v1 = vert(4,:) - pCorte;
v2 = vert(1,:) - pCorte;
c = cross(v1, v2);

if norm(c) == 0
    ok = 0;
    return;
end

cosDir1 = roundn(c / norm(c), -5);  % cosenos directores para el producto cruz

for i = 1:3  % para el resto de los v�rtices
    v1 = vert(i,:) - pCorte;
    v2 = vert(i + 1,:) - pCorte;
    c = cross(v1, v2);

    if norm(c) == 0
        ok = 0;
        return;
    end

    cosDir2 = roundn(c / norm(c), -5);  % cosenos directores para el producto cruz

    if isequal(cosDir1, cosDir2)
        ok = 1;
    else
        ok = 0;
        return;
    end
end
	 */

/*
// PARECE QUE NO FUNCIONA ESTE METODO!!

	// adapted from: http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
	// firstly, the point must be in the same plane (not checked!)
	// then...
	u32 row_idx, col_idx;
	float min_value;
	min_value = _vert.min(row_idx, col_idx);  // search for minimum value

	fmat v_test(4,2);
	frowvec2 p_test;

	col_idx=2;

	switch (col_idx)
	{
	case X:
		v_test = _vert.cols(1,2);
		p_test << point(1) << point(2);
		std::cout << "X" << std::endl;
		break;

	case Y:
		v_test = join_rows(_vert.col(0), _vert.col(2));
		p_test << point(0) << point(2);
		std::cout << "Y" << std::endl;
		break;

	case Z:
		v_test = _vert.cols(0,1);
		p_test << point(0) << point(1);
		std::cout << "Z" << std::endl;
		break;
	}

	// now, verify if it is inside
	u32 i, j;
	bool c = false;

	for (i = 0, j = v_test.n_rows - 1; i < v_test.n_rows; j = i++)  // for each vertex
	{
		if ( ((v_test(i,Y) > p_test(Y)) != (v_test(j,Y) > p_test(Y)))  &&
			 (p_test(X) < (v_test(j,X) - v_test(i,X)) * (p_test(Y) - v_test(i,Y)) / (v_test(j,Y) - v_test(i,Y)) + v_test(i,X)) )
		{
			c = !c;
		}
	}

	return c;
	*/
/*
int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((verty[i]>testy) != (verty[j]>testy)) &&
	 (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
       c = !c;
  }
  return c;
}
*/
	/*
	3D Polygons

    By this, I mean a flat polygon embedded in a plane in 3D.

    Simply project the polygon's vertices, and the test point, into 2D.

    Do the projection by deleting one coordinate, e.g., project (x,y,z) to (x,z).

    For numerical stability, delete the coordinate with the smallest range.
	 */
}

}  // namespace avrs
