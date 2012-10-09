/**
 * @file dxfreader.hpp
 */

#ifndef DXFREADER_HPP_
#define DXFREADER_HPP_

#include <dl_creationadapter.h>
#include "virtualenvironment.hpp"


/**
 * This class takes care of the entities read from the file.
 *
 * @author Andrew Mustun
 */
class DxfReader : public DL_CreationAdapter
{
public:
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

#endif  // DXFREADER_HPP_
