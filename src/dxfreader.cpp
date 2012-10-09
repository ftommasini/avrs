/**
 * @file dxfreader.cpp
 */

#include <iostream>
#include <stdio.h>

#include "dxfreader.hpp"

DxfReader::DxfReader(VirtualEnvironment *ve)
{
	assert(ve != 0);
	_ve = ve;
}

void DxfReader::addLayer(const DL_LayerData& data)
{
	;
}

void DxfReader::addPoint(const DL_PointData& data)
{
	;
}

void DxfReader::addLine(const DL_LineData& data)
{
	;
}

void DxfReader::addArc(const DL_ArcData& data)
{
	;
}

void DxfReader::addCircle(const DL_CircleData& data)
{
	;
}

void DxfReader::addPolyline(const DL_PolylineData& data)
{
	;
}

void DxfReader::addVertex(const DL_VertexData& data)
{
	;
}

void DxfReader::add3dFace(const DL_3dFaceData& data)
{
	static unsigned int id = 0;
	_ve->add_surface(new Surface(++id, data.x, data.y, data.z, 4));
}

void DxfReader::print_attributes()
{
	printf("  Attributes: Layer: %s, ", attributes.getLayer().c_str());
	printf(" Color: ");

	if (attributes.getColor() == 256)
	{
		printf("BYLAYER");
	}
	else if (attributes.getColor() == 0)
	{
		printf("BYBLOCK");
	}
	else
	{
		printf("%d", attributes.getColor());
	}
	printf(" Width: ");
	if (attributes.getWidth() == -1)
	{
		printf("BYLAYER");
	}
	else if (attributes.getWidth() == -2)
	{
		printf("BYBLOCK");
	}
	else if (attributes.getWidth() == -3)
	{
		printf("DEFAULT");
	}
	else
	{
		printf("%d", attributes.getWidth());
	}

	printf(" Type: %s\n", attributes.getLineType().c_str());
}

