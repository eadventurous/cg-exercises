#pragma once


#include "triangle_rasterization.h"


class ZCulling: public TriangleRasterization
{
public:
	ZCulling(unsigned short width, unsigned short height, std::string obj_file);
	virtual ~ZCulling();

	void DrawScene();
	void Clear();

protected:
	void DrawTriangle(float4 triangle[3]);
	void SetPixelWithDepth(unsigned short x, unsigned short y, double depth, color c);
	bool DepthTest(unsigned short x, unsigned short y, double z);
	double GetZ(float4 triangle[3], unsigned short x, unsigned short y);
	std::vector<double> depth_buffer;
};
