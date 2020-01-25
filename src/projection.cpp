#include "projection.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>


Projection::Projection(unsigned short width, unsigned short height, std::string obj_file) : ReadObj(width, height, obj_file)
{
}

Projection::~Projection()
{
}

void Projection::DrawTriangle(float4 triangle[3], color color)
{
	float r = 100;
	DrawLine(static_cast<unsigned short>(x_center + x_center*triangle[0].x),
		static_cast<unsigned short>(y_center + y_center *triangle[0].y),
		static_cast<unsigned short>(x_center + x_center * triangle[1].x),
		static_cast<unsigned short>(y_center + y_center * triangle[1].y),
		color);

	DrawLine(static_cast<unsigned short>(x_center + x_center * triangle[1].x),
		static_cast<unsigned short>(y_center + y_center * triangle[1].y),
		static_cast<unsigned short>(x_center + x_center * triangle[2].x),
		static_cast<unsigned short>(y_center + y_center * triangle[2].y),
		color);

	DrawLine(static_cast<unsigned short>(x_center + x_center * triangle[2].x),
		static_cast<unsigned short>(y_center + y_center * triangle[2].y),
		static_cast<unsigned short>(x_center + x_center * triangle[0].x),
		static_cast<unsigned short>(y_center + y_center * triangle[0].y),
		color);
}

void Projection::DrawScene()
{
	parser->Parse();

	float alpha = 0;
	float3 eye{ 0,0,0 };
	float3 look_at{ 0,0,2 };
	float3 up{ 0,1,0 };

	// first row in code = first column in matrix
	float4x4 rotation
	{
		{1, 0, 0, 0},
		{0, cos(alpha), -sin(alpha), 0},
		{0, sin(alpha), cos(alpha), 0},
		{0, 0, 2, 1}
	};

	float4x4 translte
	{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 2, 1}
	};

	float4x4 world = mul(translte, rotation);

	float3 zaxis = normalize(eye - look_at);
	float3 xaxis = normalize(cross(up, zaxis));
	float3 yaxis = normalize(cross(zaxis, xaxis));

	float4x4 view{
		{xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eye)},
		{yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eye)},
		{zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eye)},
		{0,0,0,1}
	};

	float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

	float fov_y_angle = M_PI / 3;
	float yScale = 1 / tan(fov_y_angle / 2);
	float xScale = yScale / aspect_ratio;
	float zFar = 10;
	float zNear = 1;

	float4x4 projection
	{
		{xScale, 0, 0, 0},
		{0, yScale, 0, 0},
		{0, 0, zFar/(zNear-zFar), zNear*zFar / (zNear - zFar)},
		{0, 0, 1, 0}
	};

	float4x4 translateMatrix = mul(projection, view, world);

	for (auto face : parser->GetFaces())
	{
		float4 translated[3];
		for (int i = 0; i < 3; i++)
		{
			translated[i] = mul(translateMatrix, face.vertexes[i]);
			translated[i] /= translated[i].w;
		}
		DrawTriangle(translated, color(255, 255, 255));
	}
}


