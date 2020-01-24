#include "draw_line.h"


#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>


LineDrawing::LineDrawing(unsigned short width, unsigned short height): ColorSpace(width, height)
{

}

LineDrawing::~LineDrawing()
{
}

void LineDrawing::DrawLine(unsigned short x_begin, unsigned short y_begin, unsigned short x_end, unsigned short y_end, color color)
{
	bool steep = (abs(y_end - y_begin) > abs(x_end - x_begin));

	if (steep) {
		std::swap(x_begin, y_begin);
		std::swap(x_end, y_end);
	}

	if (x_begin > x_end) {
		std::swap(x_begin, x_end);
		std::swap(y_begin, y_end);
	}

	unsigned short y = y_begin;
	float dx = static_cast<float>(x_end - x_begin);
	float dy = fabs(static_cast<float>(y_end - y_begin));

	float error = dx / 2.0;
	float value = y_begin > y_end ? -1 : 1;

	for (unsigned short x = x_begin; x <= x_end; x++) {
		if (steep) {
			SetPixel(y, x, color);
		}
		else {
			SetPixel(x, y, color);
		}

		error -= dy;

		if (error < 0) {
			y += value;
			error += dx;
		}
	}
}

void LineDrawing::DrawScene()
{
	float x_center = width / 2.0;
	float y_center = height / 2.0;
	unsigned short radius = std::min(x_center, y_center) - 40.0;
	for (float angle = 0.0; angle < 360.0; angle += 5.0) {
		DrawLine(x_center, y_center, 
			x_center + radius*cos(angle * M_PI / 180),
			y_center + radius * sin(angle * M_PI / 180),
			color(255*sin(angle * M_PI / 180),
				255 * cos(angle * M_PI / 180), 255));
	}
}

