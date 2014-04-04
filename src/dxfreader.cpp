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
 * @file dxfreader.cpp
 */

#include <iostream>
#include <cstdio>

#include "dxfreader.hpp"

namespace avrs
{

DxfReader::DxfReader(VirtualEnvironment::ptr_t ve)
{
	assert(ve.get() != NULL);
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

}  // namespace avrs
