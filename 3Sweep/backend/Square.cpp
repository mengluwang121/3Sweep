#include "Square.h"



Square::Square()
{
	Square(vec3(0.0, 0.0, 0.0), 1.0, 1.0, vec3(0.0, 0.0, 1.0));
}

Square::Square(const vec3 & original_point, double length_distance, double width_distance, const vec3 & normal_vector)
{
	origin = vec3(original_point);
	length = length_distance;
	width = width_distance;
	normal = normalize(vec3(normal_vector));
}


Square::~Square()
{
}

void Square::setOrigin(vec3 original_point)
{
	origin = vec3(original_point);
}

void Square::setLength(double length_distance)
{
	length = length_distance;
}

void Square::setWidth(double width_distance)
{
	width = width_distance;
}

void Square::setNormal(vec3 normal_vector)
{
	normal = normal_vector;
}

const vec3 Square::getOrigin()
{
	return origin;
}

const double Square::getLength()
{
	return length;
}

const double Square::getWidth()
{
	return width;
}

const vec3 Square::getNormal()
{
	return normal;
}