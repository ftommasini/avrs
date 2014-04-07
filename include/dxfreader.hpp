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

/**
 * @file dxfreader.hpp
 */

#ifndef DXFREADER_HPP_
#define DXFREADER_HPP_

#include <dxflib/dl_creationadapter.h>
#include <boost/shared_ptr.hpp>

#include "virtualenvironment.hpp"

namespace avrs
{

/**
 * This class takes care of the entities read from the file.
 *
 * @author Andrew Mustun
 */
class DxfReader : public DL_CreationAdapter
{
public:
	typedef boost::shared_ptr<DxfReader> ptr_t;

	DxfReader(VirtualEnvironment *ve);

    virtual void addLayer(const DL_LayerData& data);
    virtual void addPoint(const DL_PointData& data);
    virtual void addLine(const DL_LineData& data);
    virtual void addArc(const DL_ArcData& data);
    virtual void addCircle(const DL_CircleData& data);
    virtual void addPolyline(const DL_PolylineData& data);
    virtual void addVertex(const DL_VertexData& data);
    virtual void add3dFace(const DL_3dFaceData& data);

    void print_attributes();

private:
    VirtualEnvironment *_ve;
};

}  // namespace avrs

#endif  // DXFREADER_HPP_
