#pragma once


#include "read_obj.h"


class Projection : public ReadObj
{
public:
	Projection(unsigned short width, unsigned short height, std::string obj_file);
	virtual ~Projection();

	void DrawScene();

protected:
	void DrawTriangle(float4 triangle[3], color color);

	float x_center = width / 2;
	float y_center = height / 2;
	float radius = std::min(x_center, y_center) - 40.0f;

};
