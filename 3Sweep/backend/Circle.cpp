#include "Circle.h"

using namespace glm;

Circle::Circle()
{
	// unit circle in maya
	Circle(vec3(0.0, 0.0, 0.0), 1.0, vec3(0.0, 0.0, 1.0));
}

Circle::Circle(const vec3 & original_point, double radius_distance, const vec3 & normal_vector)
{
	origin = vec3(original_point);
	radius = radius_distance;
	normal = normalize(vec3(normal_vector));
}

void Circle::setOrigin(vec3 original_point)
{
	origin = vec3(original_point);
}

void Circle::setNormal(vec3 normal_vector)
{
	normal = normalize(vec3(normal_vector));
}

void Circle::setRadius(double radius_distance)
{
	radius = radius_distance;
}

const vec3 Circle::getOrigin()
{
	return origin;
}

const vec3 Circle::getNormal()
{
	return normal;
}

const double Circle::getRadius()
{
	return radius;
}
