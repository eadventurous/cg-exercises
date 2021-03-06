#include "triangle_rasterization.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>


TriangleRasterization::TriangleRasterization(unsigned short width, unsigned short height, std::string obj_file) : Projection(width, height, obj_file)
{
}

TriangleRasterization::~TriangleRasterization()
{
}

void TriangleRasterization::DrawScene()
{
	parser->Parse();

	float alpha = M_PI * 30 / 180;
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
		{0, 0, zFar / (zNear - zFar), zNear*zFar / (zNear - zFar)},
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
			translated[i].x = x_center + x_center * translated[i].x;
			translated[i].y = y_center + y_center * translated[i].y;
		}
		DrawTriangle(translated);
	}
}

void TriangleRasterization::DrawTriangle(float4 triangle[3])
{
	color c(255, 255, 255);
	float2 bb_begin = max(float2(0, 0), min(min(triangle[0].xy(), triangle[1].xy()), triangle[2].xy()));
	float2 bb_end = min(float2(static_cast<float>(width - 1), static_cast<float>(height - 1)), max(max(triangle[0].xy(), triangle[1].xy()), triangle[2].xy()));

	double area = EdgeFunction(triangle[2].xy(), triangle[0].xy(), triangle[1].xy());
	bool drawn = false;
	for (int x = bb_begin.x; x < bb_end.x; x++) {
		for (int y = bb_begin.y; y < bb_end.y; y++) {
			float2 point(x, y);
			float w2 = EdgeFunction(point, triangle[0].xy(), triangle[1].xy());
			float w0 = EdgeFunction(point, triangle[1].xy(), triangle[2].xy());
			float w1 = EdgeFunction(point, triangle[2].xy(), triangle[0].xy());
			if (w0 > 0 && w1 > 0 && w2 > 0) {
				SetPixel(x, y, color(255 * w0 / area, 255 * w1 / area, 255 * w2 / area));
				drawn = true;
			}
		}
	}
	if (drawn) {
		DrawLine(static_cast<unsigned short>(triangle[0].x),
			static_cast<unsigned short>(triangle[0].y),
			static_cast<unsigned short>(triangle[1].x),
			static_cast<unsigned short>(triangle[1].y),
			c);

		DrawLine(static_cast<unsigned short>(triangle[1].x),
			static_cast<unsigned short>(triangle[1].y),
			static_cast<unsigned short>(triangle[2].x),
			static_cast<unsigned short>(triangle[2].y),
			c);

		DrawLine(static_cast<unsigned short>(triangle[2].x),
			static_cast<unsigned short>(triangle[2].y),
			static_cast<unsigned short>(triangle[0].x),
			static_cast<unsigned short>(triangle[0].y),
			c);
	}
}

float TriangleRasterization::EdgeFunction(float2 a, float2 b, float2 c)
{
	return cross(a - b, a - c);
}


