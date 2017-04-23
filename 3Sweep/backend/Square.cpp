#include "Square.h"



Square::Square()
{
}

Square::Square(const vec3 & original_point, double length_distance, double width_distance, const vec3 & normal_vector)
{
	origin = vec3(original_point);
	length = length_distance;
	width = width_distance;
	normal = normalize(vec3(normal_vector));
}

Square::Square(const vec3 & original_point, double length_distance, double width_distance, const vec3 & normal_vector, std::vector<vec3>& vertices)
{
	origin = vec3(original_point);
	length = length_distance;
	width = width_distance;
	normal = normalize(vec3(normal_vector));
	for (auto vertex : vertices) vertex_list.push_back(vertex);
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

void Square::addVertex(vec3 vertex_point)
{
	vertex_list.push_back(vertex_point);
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

const std::vector<vec3> Square::getVertices()
{
	return vertex_list;
}

const bool Square::isOriginal()
{
	return vertex_list.size() != 0;
}
