#pragma once
#include "Geometry.h"
#include <iostream>

using namespace glm;
class Circle :
	public Geometry
{
public:
	Circle();
	Circle(const vec3 & original_point, double radius_distance, const vec3 & normal_vector);
	// public api
	void setOrigin(vec3 original_point);
	void setRadius(double radius_distance);
	void setNormal(vec3 normal_vector);
	const vec3 getOrigin();
	const double getRadius();
	const vec3 getNormal();
private:
	// original point
	vec3 origin;
	// the radius
	double radius;	
	// the normal of the circle plane
	vec3 normal;
};

