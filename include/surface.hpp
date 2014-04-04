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


#ifndef SURFACE_HPP_
#define SURFACE_HPP_

#include <memory>
#include <map>
#include <boost/shared_ptr.hpp>

#include "common.hpp"

namespace avrs
{

class Surface
{
public:
	typedef boost::shared_ptr<Surface> ptr_t;

	Surface(unsigned int id, const float *x_vert, const float *y_vert,
			const float *z_vert, int n_vert);
	Surface(unsigned int id, const double *x_vert, const double *y_vert,
			const double *z_vert, int n_vert);
	virtual ~Surface();

	unsigned int get_id() const;
	float get_area() const;
	bool is_point_inside(avrs::point3d_t point);
	float get_dist_origin() const;
	avrs::point3d_t &get_normal();
	arma::frowvec4 &get_plane_coeff();

	void set_b_filter_coeff(std::vector<double> &b_coeff);
	std::vector<double> &get_b_filter_coeff();
	void set_a_filter_coeff(std::vector<double> &a_coeff);
	std::vector<double> &get_a_filter_coeff();

private:
	bool _init();

	unsigned int _id;
	arma::fmat _vert;  // 4x3 matrix (four xyz points for now)
	avrs::point3d_t _center;  ///< Geometric center of the plane
	avrs::point3d_t _normal;  // normal to plane
	float _dist_origin;  // distance to origin
	float _area;
	arma::frowvec4 _plane_coeff;  // (a, b, c, d) coefficients of plane equation [ax + by + cz + d = 0]

	// material filter coefficients
	std::vector<double> _b_filter_coeff;
	std::vector<double> _a_filter_coeff;

	void _calc_center();
	void _calc_plane_coeff();
	void _calc_normal();
	void _calc_dist_origin();
	void _calc_area();

#ifndef BORISH
	arma::fmat _vert_proj;  // 4x2 matrix with the projection to 2D points of _vert matrix
	unsigned int _coord_to_remove;

	arma::frowvec2 _project_to_2d(arma::frowvec3 p);
	int _pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);
#endif
};

inline unsigned int Surface::get_id() const
{
	return _id;
}

inline float Surface::get_area() const
{
	return _area;
}

inline float Surface::get_dist_origin() const
{
	return _dist_origin;
}

inline avrs::point3d_t &Surface::get_normal()
{
	return _normal;
}

inline arma::frowvec4 &Surface::get_plane_coeff()
{
	return _plane_coeff;
}

inline void Surface::set_b_filter_coeff(std::vector<double> &b_coeff)
{
	_b_filter_coeff = b_coeff;
}

inline std::vector<double> &Surface::get_b_filter_coeff()
{
	return _b_filter_coeff;
}

inline void Surface::set_a_filter_coeff(std::vector<double> &a_coeff)
{
	_a_filter_coeff = a_coeff;
}

inline std::vector<double> &Surface::get_a_filter_coeff()
{
	return _a_filter_coeff;
}

}  // namespace avrs

#endif  // SURFACE_HPP_
