#include "z_buffer_culling.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>
#include <iostream>


ZCulling::ZCulling(unsigned short width, unsigned short height, std::string obj_file) : TriangleRasterization(width, height, obj_file)
{
	depth_buffer.reserve(width * height);
	for (int i = 0; i < width * height; i++) {
		depth_buffer[i] = std::numeric_limits<float>::max();
	}
}

ZCulling::~ZCulling()
{
	depth_buffer.clear();
}

void ZCulling::DrawScene()
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
		using namespace linalg::ostream_overloads;
		float4 translated[3];
		for (int i = 0; i < 3; i++)
		{
			translated[i] = mul(translateMatrix, face.vertexes[i]);
			translated[i] /= translated[i].w;
			translated[i].x = x_center + x_center * translated[i].x;
			translated[i].y = y_center + y_center * translated[i].y;
			std::cout << translated[i] << std::endl;
		}
		DrawTriangle(translated);
		std::cout << std::endl;
	}
}

void ZCulling::Clear()
{

}

void ZCulling::DrawTriangle(float4 triangle[3])
{
	//bug not in z buffering but in draw scene I guess, as it returns triangles where z coordinate of one of them is always bigger than another
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
				double z = (w2 * triangle[2].z + w0 * triangle[0].z + w1 * triangle[1].z) / area;
				SetPixelWithDepth(x, y, z, color(255 * w0 / area, 255 * w1 / area, 255 * w2 / area));
				drawn = true;
			}
		}
	}
}



void ZCulling::SetPixelWithDepth(unsigned short x, unsigned short y, double depth, color c)
{
	if (DepthTest(x, y, depth))
	{
		frame_buffer[y * width + x] = c;
		depth_buffer[y * width + x] = depth;
	}
}

bool ZCulling::DepthTest(unsigned short x, unsigned short y, double z)
{
	auto z_prev = depth_buffer[y * width + x];
	return z < z_prev;
}